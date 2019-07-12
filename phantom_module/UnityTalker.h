// UnityTalker.h: interface for the UnityTalker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNITYTALKER_H__59C2F0B8_CB48_4C80_813F_8096442D2F43__INCLUDED_)
#define AFX_UNITYTALKER_H__59C2F0B8_CB48_4C80_813F_8096442D2F43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <signal.h>
#include <process.h>
#include <iostream.h>
#include <stdlib.h>
#include <vector.h>
#include <gstBasic.h>

#include "UnityListener.h"

class UnityTalker  
{
private:
	int recvbuflen;
	char *sendbuf;
	
	struct sockaddr_in si_other_send;
	int connectSocketSend, slen_send, iResultSend;
	WSADATA wsaDataSend;

	char *SERVER;
	unsigned short SENDER_PORT;

public:
	UnityTalker(char *server_ip, unsigned short port);
	virtual ~UnityTalker();

	int initSendNetwork();
	void SendMousePosition(double * vector3);

};

#endif // !defined(AFX_UNITYTALKER_H__59C2F0B8_CB48_4C80_813F_8096442D2F43__INCLUDED_)
