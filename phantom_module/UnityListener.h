// UnityListener.h: interface for the UnityListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNITYLISTENER_H__0690F1BF_2DA6_4391_99DC_21764C78BED1__INCLUDED_)
#define AFX_UNITYLISTENER_H__0690F1BF_2DA6_4391_99DC_21764C78BED1__INCLUDED_

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
	vector<double *> objects;
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
public:
	UnityListener(char *server_ip, unsigned short port);
	virtual ~UnityListener();
	void startListening();
	void initPositions(int number_of_objects, vector<double*> objectPositions);
	double* getPosition(int id);
	bool UnityListener::positionChanged(int id);
	int Init();
	int receiveProcessingData();
	static unsigned int __stdcall receiverThread(void* p_this);
	void dezerializeData(char* recv_msg);
	void dezerializeGameObjectPos(char* recv_msg);
	bool turnForcesOn();
	double* getTorque();
	double* getForce();

};

#endif // !defined(AFX_UNITYLISTENER_H__0690F1BF_2DA6_4391_99DC_21764C78BED1__INCLUDED_)
