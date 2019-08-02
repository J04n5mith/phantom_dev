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
#include <gstSphere.h>
#include <gstSeparator.h>
#include <iostream.h>
#include <gstDynamic.h>

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <signal.h>

#include <time.h>

#include <process.h>

#include "../UnityListener.h"
#include "../UnityTalker.h"

#pragma comment (lib, "Ws2_32.lib")

/*********************************************/
//Defines
/*********************************************/

/*********************************************/
//Global data
/*********************************************/
struct sockaddr_in si_other_send;

UnityListener *listener;
UnityTalker *talker;

volatile sig_atomic_t stop;

double scale = 20.0;

/*********************************************/
//Function Prototypes
/*********************************************/
void inthand(int signum) {
    stop = 1;
}
void printHelp();

int main(int argc, const char *argv[])
{

	if(argc < 7)
	{
		cout << "Invalid Arguments, should be minimal 7 arguments" << endl;
		printHelp();
		return 1;
	}
	if(strcmp(argv[1], "-h") == 0)
	{
		printHelp();
		return 1;
	}

	char *path_to_vrml;
	bool contains_vrml = false;
	bool force_mode = false;
	char *target_ip = "no";
	unsigned short target_port_in;
	unsigned short target_port_out;
	
	printf("Parsing %d Arguments ...\n", argc);

	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i],"-ip") == 0)
		{
			target_ip = (char *) malloc(strlen(argv[i+1])+1);
			strcpy(target_ip, argv[i+1]);
		}
		if(strcmp(argv[i], "-p") == 0)
		{
			target_port_in = atoi(argv[i+1]);
			target_port_out = atoi(argv[i+2]);
		}
		if(strcmp(argv[i], "-f") == 0)
		{
			path_to_vrml = (char *) malloc(strlen(argv[i+1])+1);
			strcpy(path_to_vrml, argv[i+1]);
			contains_vrml = true;
		}
		if(strcmp(argv[i], "-m") == 0)
		{
			if(strcmp(argv[i+1], "force") == 0)
			{
				force_mode = true;
			}
			else if(strcmp(argv[i+1], "vrml") == 0)
			{
				force_mode = false;
			}
			else
			{
				cout << "Invalid Mode " << argv[i+1] << endl;
				printHelp();
				return 1;
			}
		}

		if(strcmp(argv[i], "-s") == 0)
		{
			scale = atof(argv[i+1]);
		}
	}

	if((strcmp(target_ip, "no") == 0)
	|| (target_port_in == NULL)
	|| (target_port_out == NULL))
	{
		cout << "Invalid Arguments..." << endl;
		printHelp();
		return 1;
	}

	cout << "Set parameter: " << endl;
	cout << "Target IP: " << target_ip << endl;
	printf("Target talking port: %d\n", target_port_out);
	printf("Target listening port: %d\n", target_port_in);
	cout << "Target VRML file: " << path_to_vrml << endl;
	printf("Scale factor: %f\n", scale);
	if(force_mode)
	{
		cout << "Mode: force" << endl;
	}
	else
	{
		cout << "Mode: vrml" << endl;
	}


	listener = new UnityListener(target_ip, target_port_in);
	talker = new UnityTalker(target_ip, target_port_out);
	
	if(talker->initSendNetwork() == 1)
	{
		cout << "Network init failed, exiting..." << endl;
		return 1;
	}

	gstScene scene;
	gstSeparator *root = new gstSeparator();
	gstSeparator *vrmlScene;

	const char* myvrml = path_to_vrml;
	if(contains_vrml)
	{
		vrmlScene = gstReadVRMLFile(path_to_vrml);
		while (gstVRMLGetNumErrors() > 0) {
			gstVRMLError err = gstVRMLPopEarliestError();
			cout << "Error in VRML file ";
			cout << gstVRMLGetErrorTypeName(err.GetError()) << " ";
			cout << err.GetMSG() << " ";
			cout << "on line " << err.GetLine() << endl;
		}
		vrmlScene->scale(scale);
		
	}

	
	cout << "Place the PHANToM in its reset position and press <ENTER>." << endl;
	cin.get();

	gstPHANToM *phantom = new gstPHANToM("Default PHANToM");
	if(!phantom || !phantom->getValidConstruction()) {
		cerr << "Unable to initialize Phantom device!" << endl;
		exit(-1);
	}

	scene.setRoot(root);
	if(contains_vrml)
	{
		cout << "Adding vrml scene to root separator..." << endl;
		root->addChild(vrmlScene);
	}
	root->addChild(phantom);

	scene.startServoLoop();
	
	listener->startListening();

	signal(SIGINT, inthand);
	gstPoint tmpPoint;
	const double *vector;
	cout << "Start main loop... Press crtl+c to stop" << endl;

	if(force_mode)
	{
		while(!stop) {

			tmpPoint = phantom->getPosition_WC();
			vector = tmpPoint.getValue();
			talker->SendMousePosition((double*)vector);

			
			if(listener->turnForcesOn())
			{

				phantom->setForceOutput(TRUE);

				double* force = listener->getForce();
				gstVector forceVec = new gstVector(force[0], force[1], force[2]);

				double* torque = listener->getTorque();
				gstVector torqueVec = new gstVector(torque[0], torque[1], torque[2]);
				phantom->setForce(forceVec, torqueVec);
			}
			else
			{
				clock_t tStart = clock();
				phantom->setForceOutput(FALSE);
			}
			
				
			if(stop)	
			{
				scene.stopServoLoop();
				break;
			}

		}
	}
	else
	{
		while(!stop) {

			tmpPoint = phantom->getPosition_WC();
			vector = tmpPoint.getValue();
			talker->SendMousePosition((double*)vector);
			if(stop)	
			{
				scene.stopServoLoop();
				break;
			}
		}
	}

	scene.stopServoLoop();
	cout << "Entered ctrl+c, press any key to get out!" << endl;
	cin.get();
    return 0;
}

void printHelp()
{
	cout << "Printing Help for tool:" << endl;

	cout << "Input should be as follow: ./unityexchange.exe -i <targetIP> -p <targetPortIn> <targetPortOut> -f /path/to/vrmlFile.wrl\n\n" << endl;

	cout << "-ip <targetIP> - IP of system where Unity is running on." << endl;
	cout << "		- f.e.: -i 192.168.0.1\n\n" << endl;

	cout << "-p <listen_port> <talking_port> - Port where unity and Phantom exchanging data." << endl;
	cout << "		- f.e.: -p 50000 50005\n\n" << endl;

	cout << "-m <mode> - application mode, either 'vrml' or 'force'" << endl;
	cout << "force: haptic logic in Unity" << endl;
	cout << "vrml: haptic logic in GHOST with vrml\n\n" << endl;
	
	cout << "-s <scale_in_double> - scale factor to scale the vrml Object if needed, because the size is in mm" << endl;
	cout << "		- f.e.: -s 20.0 (20 is default)\n\n" << endl;

	cout << "-f </path/to/vrmlFile.wrl> - (Optional) Haptic objects as VRML-Scene to add.\n\n" << endl;


	
	cout << "-h - Print this help message" << endl;
}
