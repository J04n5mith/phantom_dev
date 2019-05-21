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
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BULEN 512
#define DEFAULT_PORT "8080"

int main(int argc, char *argv[])
{
	gstScene scene;
	gstSeparator *root = new gstSeparator();
	
	gstSeparator *vrlmSep = gstReadVRMLFile("dinoborg.wrl");

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

	

/*	bool endFlag = false;

	while(!scene->getDoneServoLoop()) {
		//blalbalbalblabla
		
		if(endFlag)	
		{
			scene->stopServoLoop();
		}
	}*/

	glutManager->startMainloop();

    return 0;
}
