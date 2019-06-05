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
	this->RECEIVER_PORT = 8085;
	this->SERVER = "192.168.122.1";
	this->slen_recv = sizeof(si_other_recv);
	Init();
}

UnityListener::~UnityListener()
{

}

int UnityListener::Init()
{
	
	this->iResultRecv = WSAStartup(MAKEWORD(2,2), &this->wsaDataRecv);
	char *res;

	if(this->iResultRecv != NO_ERROR)
	{
		cout << "WSASTartup failed with error " << this->iResultRecv << endl;
		gets(res);
		exit(EXIT_FAILURE);
	}

	this->connectSocketRecv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(this->connectSocketRecv == SOCKET_ERROR)
	{
		cout << "NO connection to host" << endl;
		gets(res);
		exit(EXIT_FAILURE);
	}

	memset((char *) &this->si_other_recv, 0, sizeof(this->si_other_recv));
	this->si_other_recv.sin_family = AF_INET;
	this->si_other_recv.sin_port = htons(RECEIVER_PORT);
	this->si_other_recv.sin_addr.S_un.S_addr = inet_addr(SERVER);
	return 0;
}

void UnityListener::startListening()
{
	UnityListener::receiverHandle = (HANDLE)_beginthreadex(0,0, &UnityListener::receiverThread, this, 0, 0);
}

unsigned int __stdcall UnityListener::receiverThread(void* p_this)
{
	UnityListener *p_listener = static_cast<UnityListener*>(p_this);
	p_listener->receiveProcessingData(); // Non-static member function!
    return 0;
}


int UnityListener::receiveProcessingData()
{
	char *recv_msg;
	int id;
	double object[3];
	while(1)
	{
		recvfrom(connectSocketRecv, recv_msg, 1024, 0, (struct sockaddr *) &si_other_recv, &slen_recv);
		
		id = (recv_msg[0] << 24) | (recv_msg[1] << 16) | (recv_msg[2] << 8) | (recv_msg[3]);

	}

	return 0;
}

void UnityListener::initPositions(vector<double*> objectPositions)
{
	objects = objectPositions;
}

void UnityListener::getPosition(int id)
{

}