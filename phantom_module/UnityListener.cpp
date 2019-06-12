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
	int id, ret;
	float tmp[3];
	double vector[3];

	while(1)
	{
		if(recvfrom(connectSocketRecv, recv_msg, 1024, 0, (struct sockaddr *) &si_other_recv, &slen_recv) != -1)
		{
			id = (recv_msg[0] << 24) | (recv_msg[1] << 16) | (recv_msg[2] << 8) | (recv_msg[3]);
			tmp[0] = (recv_msg[4] << 24) | (recv_msg[5] << 16) | (recv_msg[6] << 8) | (recv_msg[7]);
			tmp[1] = (recv_msg[8] << 24) | (recv_msg[9] << 16) | (recv_msg[10] << 8) | (recv_msg[11]);
			tmp[2] = (recv_msg[12] << 24) | (recv_msg[13] << 16) | (recv_msg[14] << 8) | (recv_msg[15]);
			vector[0] = (double)tmp[0]; //x
			vector[1] = (double)tmp[1]; //y
			vector[2] = (double)-(tmp[2]); //z
			objects[id] = vector;
			printf("ID: %i (x, y, z) (%f, %f, %f) \n", id, vector[0], vector[1], vector[2]); 
			//cout << "ID: " << id << "(x, y, z) (" << vector[0]*20 << ", " 
			//	<< vector[1]*20 << ", " << vector[2]*20 << ")" << endl;
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