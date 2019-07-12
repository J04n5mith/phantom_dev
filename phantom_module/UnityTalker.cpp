// UnityTalker.cpp: implementation of the UnityTalker class.
//
//////////////////////////////////////////////////////////////////////

#include "UnityTalker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UnityTalker::UnityTalker(char* server_ip, unsigned short port)
{
	SENDER_PORT = port;
	SERVER = server_ip;
	slen_send = sizeof(si_other_send);
	initSendNetwork();
}

UnityTalker::~UnityTalker()
{

}


int UnityTalker::initSendNetwork()
{
	iResultSend = WSAStartup(MAKEWORD(2,2), &wsaDataSend);
	char *res;

	if(iResultSend != NO_ERROR)
	{
		cout << "WSASTartup failed with error " << iResultSend << endl;
		gets(res);
		return 1;
	}

	connectSocketSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(connectSocketSend == SOCKET_ERROR)
	{
		cout << "NO connection to host" << endl;
		gets(res);
		return 1;
	}

	memset((char *) &si_other_send, 0, sizeof(si_other_send));
	si_other_send.sin_family = AF_INET;
	si_other_send.sin_port = htons(SENDER_PORT);
	si_other_send.sin_addr.S_un.S_addr = inet_addr(SERVER);
	return 0;
}

void UnityTalker::SendMousePosition(double* vector3)
{
	char msg[100];
	char *input;

	Operation mousePos = MOUSE_POS;
	char op[2];
	op[0] = (mousePos & 0x0F);
	op[1] = ((mousePos << 8) & 0x0F);

	memcpy(msg, op, sizeof(op));
	memcpy(msg+2, vector3, (sizeof(double)*3));
	if(sendto(connectSocketSend, msg, 100, 0, (struct sockaddr *) &si_other_send, slen_send) == SOCKET_ERROR)
	{
		cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
		gets(input);
		
		exit(EXIT_FAILURE);
	}
}