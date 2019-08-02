using System.Collections;
using System.Collections.Generic;
using System.Threading;
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


    
    float[] force = new float[3];
    Vector3 forceDirection;

	void OnCollisionEnter(Collision other)
	{	    
		forceDirection = Vector3.zero;
		ContactPoint contactPoint = other.contacts[0];
		Vector3 normal = contactPoint.normal;
        
        //unity has a left directional coordinate system
		forceDirection.x = normal.x;
		forceDirection.y = normal.y;
		forceDirection.z = -normal.z;  
        //Debug.Log("Direction: " + dir.ToString());
	}
	
	float rigidyFactor = 0.1f;
    float[] torque = {0.0f, 0.0f, 0.0f};

	void OnCollisionStay(Collision other)
	{
	    //Debug.Log("Name: " + other.gameObject.name);
		Vector3 center = other.collider.bounds.center;
	
		ContactPoint contactPoint = other.contacts[0];
        Vector3 normal = contactPoint.normal;

        RaycastHit hit;
        //Debug.DrawRay(center, normal, Color.green);
        if (Physics.Raycast(center, normal, out hit))
        {
            float penetrationDepth = hit.distance;
            force[0] = forceDirection.x*rigidyFactor/(penetrationDepth);  
            force[1] = forceDirection.y*rigidyFactor/(penetrationDepth);
            force[2] = forceDirection.z*rigidyFactor/(penetrationDepth);
            Debug.Log( "Depth: " + penetrationDepth );
        }
        
        /*force[0] = dir.x*rigidyFactor;  
        force[1] = dir.y*rigidyFactor;
        force[2] = dir.z*rigidyFactor;*/
        //Debug.Log("force: " + force[0] + ", " + force[1] + ", " + force[2]);
        talker.SendForceVector(force, torque);
	}
	
	void OnCollisionExit(Collision other)
	{
	    float[] zeroTorque = {0.0f, 0.0f, 0.0f};
		float[] zeroForce = {0.0f, 0.0f, 0.0f};
		
	    talker.SendForceVector(zeroForce, zeroTorque);
	}


    // Start is called before the first frame update
    void Start()
    {
        logger = new Logger(new MyLogHandler());
        
        Application.targetFrameRate = 1000;

        logger.Log("Starting application");
        data = new PhantomData(20);
        listener = new PhantomListener("192.168.122.199", 50005, data, logger);
        talker = new PhantomTalker("192.168.122.199", 50000, data, logger);
        listener.StartListener(100);
        logger.Log("UDP Listener initialized");
        
        mouse = GameObject.FindGameObjectWithTag("Player");

   
    }



    // Update is called once per frame
    void Update()
    {        
        mouse.transform.position = data.MousePosition;        
    }

    private void OnApplicationQuit()
    {
        listener.StopListener();
    }
    
    /*private Vector3 screenPoint;
	private Vector3 offset;
		
	void OnMouseDown(){
		screenPoint = Camera.main.WorldToScreenPoint(gameObject.transform.position);
		offset = gameObject.transform.position - Camera.main.ScreenToWorldPoint(new Vector3(Input.mousePosition.x, Input.mousePosition.y, screenPoint.z));
	}
		
	void OnMouseDrag(){
		Vector3 cursorPoint = new Vector3(Input.mousePosition.x, Input.mousePosition.y, screenPoint.z);
		Vector3 cursorPosition = Camera.main.ScreenToWorldPoint(cursorPoint) + offset;
		transform.position = cursorPosition;
	}*/
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
