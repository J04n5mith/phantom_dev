// UnityListener.cpp: implementation of the UnityListener class.
//
//////////////////////////////////////////////////////////////////////

#include "UnityListener.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Calls Init method
UnityListener::UnityListener()
{
	RECEIVER_PORT = 8085;
	SERVER = "192.168.122.1";
	Init();
	slen_recv = sizeof(si_other_recv);
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
	UnityListener::receiverHandle = (HANDLE)_beginthreadex(0,0, &UnityListener::receiverThread, this, 0, 0);
}

unsigned int __stdcall UnityListener::receiverThread(void* p_this)
{
	cout << "Trying to create thread..." << endl;
	UnityListener *p_listener = static_cast<UnityListener*>(p_this);
	p_listener->receiveProcessingData(); // Non-static member function!
    return 0;
}


int UnityListener::receiveProcessingData()
{
	char recv_msg[1024];
	
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

void UnityListener::initPositions(vector<double*> objectPositions)
{
	objects = objectPositions;
}

double* UnityListener::getPosition(int id)
{
	return objects[id];
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
union test
{
   unsigned char buf[4];
   float number;
}test;

void UnityListener::dezerializeGameObjectPos(char *recv_msg)
{
	int id;

	id = (recv_msg[2] << 24) | (recv_msg[3] << 16) | (recv_msg[4] << 8) | (recv_msg[5]);
	test.buf[0] = recv_msg[9];
	test.buf[1] = recv_msg[8];
	test.buf[2] = recv_msg[7];
	test.buf[3] = recv_msg[6];
	if(this->objects[id][0] != (double)test.number){this->objects[id][0] = (double)test.number;}


	test.buf[0] = recv_msg[13];
	test.buf[1] = recv_msg[12];
	test.buf[2] = recv_msg[11];
	test.buf[3] = recv_msg[10];
	if(this->objects[id][1] != (double)test.number){this->objects[id][1] = (double)test.number;}


	test.buf[0] = recv_msg[17];
	test.buf[1] = recv_msg[16];
	test.buf[2] = recv_msg[15];
	test.buf[3] = recv_msg[14];
	if(this->objects[id][2] != (double)test.number){this->objects[id][2] = (double)test.number;}


	Sleep(0);
	printf("ID: %d, Positions: (x, y, z) : (%f, %f, %f) \n", id, objects[id][0], objects[id][1], objects[id][2]);
}