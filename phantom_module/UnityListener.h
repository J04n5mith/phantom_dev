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


class UnityListener  
{
	vector<double *> objects;

	HANDLE receiverHandle;
	DWORD threadId;

	struct sockaddr_in si_other_recv;
	int connectSocketRecv, slen_recv, iResultRecv;
	WSADATA wsaDataRecv;
	char *SERVER;
	unsigned short RECEIVER_PORT;

public:
	UnityListener();
	virtual ~UnityListener();
	void startListening();
	void initPositions(vector<double*> objectPositions);
	double* getPosition(int id);

private:
	int Init();
	int receiveProcessingData();
	static unsigned int __stdcall receiverThread(void* p_this);

};

#endif // !defined(AFX_UNITYLISTENER_H__0690F1BF_2DA6_4391_99DC_21764C78BED1__INCLUDED_)
