﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using System.IO;
using System;
using UnityX3D;


public enum Operation {MOUSE_POS, GAMEOBJECT_POS, FORCE_TORQUE_ON, FORCE_USHORT = 0xFF}

public class MoveBehaviour : MonoBehaviour
{
    PhantomListener listener;
    PhantomTalker talker;
    PhantomData data;
    GameObject mouse;
    GameObject player;
    GameObject[] objects;
    Vector3[] lastPosition;
    Logger logger;

    public float hoverForce;

	void OnCollisionEnter(Collision col)
	{
	    
		Debug.Log("We hit something");
		foreach(ContactPoint contact in col.contacts)
		{
		    float[] bla = {0.0f, 0.0f, 0.0f};
	        Vector3 dir = (contact.point - transform.position);
	        dir = -dir.normalized;
	        float[] force = {dir.x, dir.y, dir.z};
	        talker.SendForceVector(force, bla);
			Debug.Log(contact.thisCollider.name + " hit " + contact.otherCollider.name);
		}	
		
		
	}

	void OnCollisionStay(Collision other)
	{
		Debug.Log("Stay Here!!!");
		foreach(ContactPoint contact in other.contacts)
		{
		    float[] bla = {0.0f, 0.0f, 0.0f};
	        Vector3 dir = (contact.point - transform.position);
	        dir = -dir.normalized;
	        float[] force = {dir.x, dir.y, dir.z};
			Debug.Log(contact.point);
			//Debug.DrawRay(contact.point, contact.normal, Color.black);
			//float dist = Vector3.Distance(contact.point, transform.position);
		}
		//other.GetComponent<Rigidbody>().AddForce(Vector3.up * hoverForce, ForceMode.Acceleration);
	}

	void OnCollisionExit(Collision other)
	{
	    talker.turnForcesOff();
		Debug.Log("We are out");
	}

    // Start is called before the first frame update
    void Start()
    {
        logger = new Logger(new MyLogHandler());

        logger.Log("Starting application");
        data = new PhantomData(20);
        listener = new PhantomListener(data, logger);
        talker = new PhantomTalker(data, logger);
        listener.StartListener(1024);
        logger.Log("UDP Listener initialized");
        

        mouse = GameObject.FindGameObjectWithTag("Player");

        player = GameObject.FindGameObjectWithTag("GameController");
        
        objects = GameObject.FindGameObjectsWithTag("Objects");

        lastPosition = new Vector3[objects.Length];

        for (int i = 0; i < objects.Length; i++)
        {
            logger.Log(objects[i].name);
	    lastPosition[i] = objects[i].transform.position;
        }
    }



    // Update is called once per frame
    void Update()
    {
        
        mouse.transform.position = data.MousePosition;
        
        /*for (int i = 0; i < objects.Length; i++)
        {
            if(objects[i].transform.position != lastPosition[i])
            {
		lastPosition[i] = objects[i].transform.position;
                talker.SendLocationOfGameObjects(i, objects[i].transform.position);
            }
        }*/
    }

    private void OnApplicationQuit()
    {
        listener.StopListener();
    }
}

public class MyLogHandler : ILogHandler
{
    public void LogFormat(LogType logType, UnityEngine.Object context, string format, params object[] args)
    {
        Debug.unityLogger.logHandler.LogFormat(logType, context, format, args);
    }

    public void LogException(Exception exception, UnityEngine.Object context)
    {
        Debug.unityLogger.LogException(exception, context);
    }
}
