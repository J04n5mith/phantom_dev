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

#include <process.h>

#include "../UnityListener.h"

#pragma comment (lib, "Ws2_32.lib")

/*********************************************/
//Defines
/*********************************************/
#define DEFAULT_BULEN 1024
#define SERVER "192.168.122.1"
#define SENDER_PORT 8080

/*********************************************/
//Global data
/*********************************************/
struct sockaddr_in si_other_send;
int connectSocketSend, slen_send=sizeof(si_other_send), iResultSend;
WSADATA wsaDataSend;

UnityListener *listener;

int recvbuflen;
char *sendbuf;

volatile sig_atomic_t stop;

double scale = 20.0;

/*********************************************/
//Function Prototypes
/*********************************************/
void inthand(int signum) {
    stop = 1;
}
int initSendNetwork();
int receiveProcessingData();
void SendMousePosition(gstPoint mousePosition);


int main(int argc, char *argv[])
{
	listener = new UnityListener();
	
	if(initSendNetwork() == 1)
	{
		cout << "Network init failed, exiting..." << endl;
		return 1;
	}



	gstScene scene;
	gstSeparator *root = new gstSeparator();


	vector<gstSeparator *> vrmlObjs;
	
	gstSeparator *cube = gstReadVRMLFile("cube.wrl");
	cube->scale(scale);
	vrmlObjs.push_back(cube);
	gstSeparator *capsule = gstReadVRMLFile("capsule.wrl");
	capsule->scale(scale);
	vrmlObjs.push_back(capsule);
	gstSeparator *cylinder = gstReadVRMLFile("cylinder.wrl");
	cylinder->scale(scale);
	vrmlObjs.push_back(cylinder);
	gstSeparator *plane = gstReadVRMLFile("plane.wrl");
	
	plane->scale(scale);
	vrmlObjs.push_back(plane);

	

	cout << "Place the PHANToM in its reset position and press <ENTER>." << endl;
	cin.get();

	gstPHANToM *phantom = new gstPHANToM("Default PHANToM");
	if(!phantom || !phantom->getValidConstruction()) {
		cerr << "Unable to initialize Phantom device!" << endl;
		exit(-1);
	}

	scene.setRoot(root);
	
	root->addChild(cube);
	root->addChild(capsule);
	root->addChild(cylinder);
	root->addChild(plane);
	root->addChild(phantom);

	while (gstVRMLGetNumErrors() > 0) {
		gstVRMLError err = gstVRMLPopEarliestError();
		cout << "Error in VRML file ";
		cout << gstVRMLGetErrorTypeName(err.GetError()) << " ";
		cout << err.GetMSG() << " ";
		cout << "on line " << err.GetLine() << endl;
	}
	scene.startServoLoop();
	
	vector<double *> objectPositions;
	for(int i = 0; i < vrmlObjs.size(); i++)
	{
		root->getChild(i)->getPosition_WC().printSelf();
		objectPositions.push_back((double *)vrmlObjs[i]->getPosition());
	}
	

	//const gstTransform *phantomtrans = mani->getNode();

	//phantomtrans->setCenter(newCenter);
	
	bool endFlag = false;

	gstPoint currentPoint;

	//glutManager->startMainloop();
	
	listener->initPositions(objectPositions.size(), objectPositions);
	listener->startListening();

	signal(SIGINT, inthand);
	gstPoint *tmpPoint;
	double *vector;
	while(!stop) {
		

		SendMousePosition(phantom->getPosition_WC());

		for(int i = 0; i < vrmlObjs.size(); i++)
		{
			if(listener->positionChanged(i))
			{
				vector = listener->getPosition(i);
				cout << "ID: " << i << ", Position: " << vector[0]*scale << ", " << vector[1]*scale << ", " << vector[2]*scale << endl;
				tmpPoint = new gstPoint((vector[0]*scale), (vector[1]*scale), (vector[2]*scale));
				vrmlObjs[i]->setPosition_WC(tmpPoint);
			}		

		}

		if(stop)	
		{
			scene.stopServoLoop();
			break;
		}

	}

	scene.stopServoLoop();
	cin.get();
    return 0;
}




const double * vector3;
char msg[1024];
char *input;
/*********************************************/
//Sending mouse position
/*********************************************/
void SendMousePosition(gstPoint mousePosition)
{
	vector3 = mousePosition.getValue();
	Operation mousePos = MOUSE_POS;
	char op[2];
	op[0] = (mousePos & 0x0F);
	op[1] = ((mousePos << 8) & 0x0F);

	memcpy(msg, op, sizeof(op));
	memcpy(msg+2, vector3, (sizeof(double)*3));
	if(sendto(connectSocketSend, msg, 1024, 0, (struct sockaddr *) &si_other_send, slen_send) == SOCKET_ERROR)
	{
		cout << "sendto() failed with error code : " << WSAGetLastError() << endl;
		gets(input);
		
		exit(EXIT_FAILURE);
	}
}


/*********************************************/
//Init Sending-Socket function
/*********************************************/
int initSendNetwork()
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
