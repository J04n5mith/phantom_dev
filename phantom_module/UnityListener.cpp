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
UnityListener::UnityListener()
{
	RECEIVER_PORT = 8085;
	SERVER = "192.168.122.1";
	slen_recv = sizeof(si_other_recv);
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
	WaitForSingleObject(mutex, 1);
	return pos[id].position;
}

bool UnityListener::positionChanged(int id)
{
	WaitForSingleObject(mutex, 1);
	return pos[id].hasChanged;
}

int UnityListener::receiveProcessingData()
{
	char recv_msg[1024];

	ObjectPositions *tmpPos = pos;
	
	while(1)
	{
		
		if(recvfrom(connectSocketRecv, recv_msg, 1024, 0, (struct sockaddr *) &si_other_recv, &slen_recv) != -1)
		{
			dezerializeData(recv_msg);
		}

	}

	iResultRecv = closesocket(connectSocketRecv);
    if (iResultRecv == SOCKET_ERROR) {
        wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
        return 1;
    }

	return 0;
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
		default:
			printf("Incompatible Operation");
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

	WaitForSingleObject(mutex, INFINITE);


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


	if( (pos[id].position[0] == (double)position_converter[0].number) &&
		(pos[id].position[1] == (double)position_converter[1].number) &&
		(pos[id].position[2] == (double)position_converter[2].number) )
	{
		cout << id << " all false" << endl;
		pos[id].hasChanged = false;
		return;
	}

	if(pos[id].position[0] != (double)position_converter[0].number){
		cout << id << " x true" << endl;
		pos[id].position[0] = (double)position_converter[0].number;
		pos[id].hasChanged = true;
	}


	if(pos[id].position[1] != (double)position_converter[1].number){
		cout << id << " y true" << endl;
		pos[id].position[1] = (double)position_converter[1].number;
		pos[id].hasChanged = true;
	}

	if(pos[id].position[2] != (double)position_converter[2].number){
		cout << id << " z true" << endl;
		pos[id].position[2] = (double)position_converter[2].number;
		pos[id].hasChanged = true;
	}

	
	ReleaseMutex(mutex);
	Sleep(10);
	//printf("ID: %d, Positions: (x, y, z) : (%f, %f, %f) \n", id, pos[id].position[0], pos[id].position[1], pos[id].position[2]);
}