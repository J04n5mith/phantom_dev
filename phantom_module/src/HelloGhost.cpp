//=============================================================================
//     Filename : HelloGhost.cpp
//   Written by : Brandon Itkowitz
//     Contains : Example GHOST Application
// ----------------------------------------------------------------------------
// This program demonstrates how to create a GHOST scene graph, add a primitive
// to it and then feel the scene.
//
// Send your questions, comments or bugs to:
//    support@sensable.com
//    http://www.sensable.com
//
// Copyright (c) 1996-1999 SensAble Technologies, Inc. All rights reserved.
//=============================================================================
#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <gstBasic.h>
#include <gstScene.h>
#include <gstVRML.h>
#include <ghostGLUTManager.h>
#include <gstSphere.h>
#include <gstSeparator.h>
#include <iostream.h>

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <signal.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BULEN 1024
#define SERVER "192.168.122.1"
#define PORT 8080

struct sockaddr_in si_other;
int connectSocket, slen=sizeof(si_other), iResult;
WSADATA wsaData;



int recvbuflen;
char *sendbuf;

volatile sig_atomic_t stop;

void inthand(int signum) {
    stop = 1;
}

int initNetwork();

int main(int argc, char *argv[])
{

	if(initNetwork() == 1)
	{
		cout << "Network init failed, exiting..." << endl;
		return 1;
	}

	gstScene scene;
	gstSeparator *root = new gstSeparator();
	
	gstSeparator *vrlmSep = gstReadVRMLFile("scene.wrl");
	vrlmSep->scale(20.0);

	cout << "Place the PHANToM in its reset position and press <ENTER>." << endl;
	cin.get();

	gstPHANToM *phantom = new gstPHANToM("Default PHANToM");
	if(!phantom || !phantom->getValidConstruction()) {
		cerr << "Unable to initialize Phantom device!" << endl;
		exit(-1);
	}


	scene.setRoot(root);
	
	root->addChild(vrlmSep);
	root->addChild(phantom);

	while (gstVRMLGetNumErrors() > 0) {
		gstVRMLError err = gstVRMLPopEarliestError();
		cout << "Error in VRML file ";
		cout << gstVRMLGetErrorTypeName(err.GetError()) << " ";
		cout << err.GetMSG() << " ";
		cout << "on line " << err.GetLine() << endl;
	}
	scene.startServoLoop();
	// create an instance of the GLUT OpenGL Manager
	ghostGLUTManager *glutManager = ghostGLUTManager::CreateInstance(argc, argv, "gstTriPolyMesh Example");

	glutManager->loadScene(&scene);

	

	bool endFlag = false;

	gstPoint currentPoint;

	//glutManager->startMainloop();
	
	
	char msg[1024];
	char *input;
	const double * vector3;
	signal(SIGINT, inthand);
	while(!scene.getDoneServoLoop()) {

		currentPoint = phantom->getPosition_WC();
		vector3 = currentPoint.getValue();
		memcpy(msg, vector3, (sizeof(double)*3));
		if(sendto(connectSocket, msg, 1024, 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
			gets(input);
			
			exit(EXIT_FAILURE);
		}
		if(stop)	
		{
			scene.stopServoLoop();
			break;
		}
	}

	scene.stopServoLoop();

    return 0;
}


int initNetwork()
{
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	char *res;

	if(iResult != NO_ERROR)
	{
		cout << "WSASTartup failed with error " << iResult << endl;
		gets(res);
		return 1;
	}

	connectSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(connectSocket == SOCKET_ERROR)
	{
		cout << "NO connection to host" << endl;
		gets(res);
		return 1;
	}

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
	return 0;
}
