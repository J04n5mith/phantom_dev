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
	char *recv_msg;
	int id;
	float tmp[3];
	double vector[3];
	while(1)
	{
		recvfrom(connectSocketRecv, recv_msg, 1024, 0, (struct sockaddr *) &si_other_recv, &slen_recv);
		id = (recv_msg[0] << 24) | (recv_msg[1] << 16) | (recv_msg[2] << 8) | (recv_msg[3]);
		tmp[0] = (recv_msg[4] << 24) | (recv_msg[5] << 16) | (recv_msg[6] << 8) | (recv_msg[7]);
		tmp[1] = (recv_msg[8] << 24) | (recv_msg[9] << 16) | (recv_msg[10] << 8) | (recv_msg[11]);
		tmp[2] = (recv_msg[12] << 24) | (recv_msg[13] << 16) | (recv_msg[14] << 8) | (recv_msg[15]);
		vector[0] = (double)tmp[0]; //x
		vector[1] = (double)tmp[1]; //y
		vector[2] = (double)-(tmp[2]); //z
		objects[id] = vector;
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