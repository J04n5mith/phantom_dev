// UnityListener.h: interface for the UnityListener class.
//
//////////////////////////////////////////////////////////////////////


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

enum Operation {MOUSE_POS, GAMEOBJECT_POS, FORCE_TORQUE_ON, FORCE_USHORT=0xFFFF};


class UnityListener  
{
private:
	bool forcesOn;
	double phantom_force[3];
	double phantom_torque[3];

	HANDLE receiverHandle;
	DWORD threadId;
	HANDLE mutex;

	struct sockaddr_in si_other_recv;
	int connectSocketRecv, slen_recv, iResultRecv;
	WSADATA wsaDataRecv;
	char *SERVER;
	unsigned short RECEIVER_PORT;

	void deserializeForceAndTorque(char *recv_msg);
	void dezerializeData(char* recv_msg);
	int receiveProcessingData();
	static unsigned int __stdcall receiverThread(void* p_this);
	int Init();
public:
	UnityListener(char *server_ip, unsigned short port);
	virtual ~UnityListener();
	void startListening();
	bool turnForcesOn();
	double* getTorque();
	double* getForce();

};

#endif // !defined(AFX_UNITYLISTENER_H__0690F1BF_2DA6_4391_99DC_21764C78BED1__INCLUDED_)
