// UnityListener.cpp: implementation of the UnityListener class.
//
//////////////////////////////////////////////////////////////////////

#include "UnityListener.h"

struct ObjectPositions {
	int id;
	double *position;
	bool hasChanged;
}*pos;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



//Calls Init method
UnityListener::UnityListener(char *server_ip, unsigned short port)
{
	RECEIVER_PORT = port;
	SERVER = server_ip;
	slen_recv = sizeof(si_other_recv);
	forcesOn = FALSE;
	Init();
}

UnityListener::~UnityListener()
{

}

int UnityListener::Init()
{
	
	iResultRecv = WSAStartup(MAKEWORD(2,2), &wsaDataRecv);
	char *res;

	if(iResultRecv != NO_ERROR)
	{
		cout << "WSASTartup failed with error " << iResultRecv << endl;
		gets(res);
		exit(EXIT_FAILURE);
	}

	connectSocketRecv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(connectSocketRecv == SOCKET_ERROR)
	{
		cout << "NO connection to host" << endl;
		gets(res);
		exit(EXIT_FAILURE);
	}

	memset((char *) &si_other_recv, 0, sizeof(si_other_recv));
	si_other_recv.sin_family = AF_INET;
	si_other_recv.sin_port = htons(RECEIVER_PORT);
	si_other_recv.sin_addr.S_un.S_addr = INADDR_ANY;
	
	iResultRecv = bind(connectSocketRecv, (SOCKADDR *) &si_other_recv, sizeof(si_other_recv));
    if (iResultRecv != 0) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        return 1;
    }
	return 0;
}

void UnityListener::startListening()
{
	cout << "Start Listening... " << endl;
	mutex = CreateMutex(NULL, false, NULL);
	UnityListener::receiverHandle = (HANDLE)_beginthreadex(0,0, &UnityListener::receiverThread, this, 0, 0);
}

unsigned int __stdcall UnityListener::receiverThread(void* p_this)
{
	cout << "Trying to create thread..." << endl;
	UnityListener *p_listener = static_cast<UnityListener*>(p_this);
	p_listener->receiveProcessingData(); // Non-static member function!
    return 0;
}



void UnityListener::initPositions(int number_of_objects, vector<double*> objectPositions)
{
	pos = new ObjectPositions[number_of_objects];
	for(int i = 0; i < objectPositions.size(); i++)
	{
		pos[i].id = i;
		pos[i].position = objectPositions[i];
		pos[i].hasChanged = false;
		cout << "init position " << i << ": " 
			<< pos[i].position[0] << ", " << pos[i].position[0] << ", " <<pos[i].position[0] << endl;
	}
}

double* UnityListener::getPosition(int id)
{
	return pos[id].position;
}

bool UnityListener::positionChanged(int id)
{
	return pos[id].hasChanged;
}

int UnityListener::receiveProcessingData()
{
	char buffer[100];
	short buffer_size;
	
	while(1)
	{		
		if((buffer_size = recvfrom(connectSocketRecv, buffer, 100, 0, (struct sockaddr *) &si_other_recv, &slen_recv)) >= 0)
		{
			printf("Buffersize: %d\n", buffer_size);
			dezerializeData(buffer);
		}
	}

	iResultRecv = closesocket(connectSocketRecv);
    if (iResultRecv == SOCKET_ERROR) {
        wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
        return 1;
    }

	return 0;
}

bool UnityListener::turnForcesOn()
{
	return forcesOn;
}

double* UnityListener::getTorque()
{
	return phantom_torque;
}

double* UnityListener::getForce()
{
	return phantom_force;
}

void UnityListener::dezerializeData(char* recv_msg)
{
	WORD oper = recv_msg[0] | (recv_msg[1] << 8);
	Operation op = (Operation)oper;
	switch(op)
	{
		case GAMEOBJECT_POS:
			dezerializeGameObjectPos(recv_msg);
			break;
		case FORCE_TORQUE_ON:
			deserializeForceAndTorque(recv_msg);
			break;
		default:
			printf("Incompatible Operation, Number: %d \n", oper);
	}
}


//union
union position_converter
{
   unsigned char buf[4];
   float number;
}position_converter[3];


void UnityListener::dezerializeGameObjectPos(char *recv_msg)
{
	int id;

	id = (recv_msg[2] << 24) | (recv_msg[3] << 16) | (recv_msg[4] << 8) | (recv_msg[5]);
	position_converter[0].buf[0] = recv_msg[9];
	position_converter[0].buf[1] = recv_msg[8];
	position_converter[0].buf[2] = recv_msg[7];
	position_converter[0].buf[3] = recv_msg[6];

	position_converter[1].buf[0] = recv_msg[13];
	position_converter[1].buf[1] = recv_msg[12];
	position_converter[1].buf[2] = recv_msg[11];
	position_converter[1].buf[3] = recv_msg[10];
	
	position_converter[2].buf[0] = recv_msg[17];
	position_converter[2].buf[1] = recv_msg[16];
	position_converter[2].buf[2] = recv_msg[15];
	position_converter[2].buf[3] = recv_msg[14];

	//printf("ID: %d, Positions: (x, y, z) : (%f, %f, %f) \n", id, position_converter[0].number, position_converter[1].number, position_converter[2].number);

	if( (pos[id].position[0] == (double)position_converter[0].number) &&
		(pos[id].position[1] == (double)position_converter[1].number) &&
		(pos[id].position[2] == (double)position_converter[2].number) )
	{
		//cout << id << " all false" << endl;
		pos[id].hasChanged = false;
		return;
	}

	if(pos[id].position[0] != (double)position_converter[0].number){
		//cout << id << " x true" << endl;
		pos[id].position[0] = (double)position_converter[0].number;
		pos[id].hasChanged = true;
	}


	if(pos[id].position[1] != (double)position_converter[1].number){
		//cout << id << " y true" << endl;
		pos[id].position[1] = (double)position_converter[1].number;
		pos[id].hasChanged = true;
	}

	if(pos[id].position[2] != (double)(-position_converter[2].number)){
		//cout << id << " z true" << endl;
		pos[id].position[2] = (double)(-position_converter[2].number);
		pos[id].hasChanged = true;
	}
	//printf("ID: %d, Positions: (x, y, z) : (%f, %f, %f) \n", id, pos[id].position[0], pos[id].position[1], pos[id].position[2]);
}

union force
{
   unsigned char buf[4];
   float number;
}force[3];

union torque
{
	unsigned char buf[4];
	float number;
}torque[3];

void UnityListener::deserializeForceAndTorque(char *recv_msg)
{
	forcesOn = (bool)recv_msg[2];
	
	if(forcesOn == TRUE)
	{
		/************Force************/
		//X
		force[0].buf[0] = recv_msg[6];
		force[0].buf[1] = recv_msg[5];
		force[0].buf[2] = recv_msg[4];
		force[0].buf[3] = recv_msg[3];

		//Y
		force[1].buf[0] = recv_msg[10];
		force[1].buf[1] = recv_msg[9];
		force[1].buf[2] = recv_msg[8];
		force[1].buf[3] = recv_msg[7];
		
		//Z
		force[2].buf[0] = recv_msg[14];
		force[2].buf[1] = recv_msg[13];
		force[2].buf[2] = recv_msg[12];
		force[2].buf[3] = recv_msg[11];
		
		phantom_force[0] = (double)force[0].number;
		phantom_force[1] = (double)force[1].number;
		phantom_force[2] = (double)force[2].number;
		
		//printf("FORCE: (x, y, z) : (%f, %f, %f) \n", phantom_force[0], phantom_force[1], phantom_force[2]);

		/************Torque************/
		//X
		torque[0].buf[0] = recv_msg[18];
		torque[0].buf[1] = recv_msg[17];
		torque[0].buf[2] = recv_msg[16];
		torque[0].buf[3] = recv_msg[15];

		//Y
		torque[1].buf[0] = recv_msg[22];
		torque[1].buf[1] = recv_msg[21];
		torque[1].buf[2] = recv_msg[20];
		torque[1].buf[3] = recv_msg[19];
		
		//Z
		torque[2].buf[0] = recv_msg[26];
		torque[2].buf[1] = recv_msg[25];
		torque[2].buf[2] = recv_msg[24];
		torque[2].buf[3] = recv_msg[23];
		
		phantom_torque[0] = (double)torque[0].number;
		phantom_torque[1] = (double)torque[1].number;
		phantom_torque[2] = (double)torque[2].number;

		//printf("FORCE: (x, y, z) : (%f, %f, %f) \n", phantom_torque[0], phantom_torque[1], phantom_torque[2]);
	}
}