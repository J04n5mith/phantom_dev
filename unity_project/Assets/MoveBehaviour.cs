using System.Collections;
using System.Collections.Generic;
using System.Threading;
using UnityEngine;
using UnityEngine.SceneManagement;
using System.IO;
using System;

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
    float totalDepth = 0.0f;
    float depth = 0.0f; 
    //When collision happens, set the force direction 
	void OnCollisionEnter(Collision other)
	{	 
	    //Get the contact point and normalize it
		forceDirection = Vector3.zero;
		ContactPoint contactPoint = other.contacts[0];
		Vector3 normal = contactPoint.normal;
		Vector3 center = other.collider.bounds.center;
		
		totalDepth = (transform.position - center).magnitude;
        
        //unity has a left directional coordinate system, so the z direction is inverted for PHANToM
		forceDirection.x = normal.x;
		forceDirection.y = normal.y;
		forceDirection.z = -normal.z;  
        //Debug.Log("Direction: " + forceDirection.ToString());
	}
	
	//torque is not needed in a stiff wall
    float[] torque = {0.0f, 0.0f, 0.0f}; 
    RaycastHit hit;
    
    float ratio = 0.0f;
    //When the haptic point is in the object
	void OnCollisionStay(Collision other)
	{
	    //Debug.Log("Name: " + other.gameObject.name);
	    //Get Collider of the PHANToM pointer
		var thisCollider = GetComponent<Collider>();
		ContactPoint contactPoint = other.contacts[0];
		Vector3 center = other.collider.bounds.center;
        
        //For penetration computation
        Vector3 otherPosition = other.gameObject.transform.position;
        Quaternion otherRotation = other.gameObject.transform.rotation;
        
        Vector3 direction;
        float distance;
        
        //Compute depth of penetration
        bool overlapped = Physics.ComputePenetration(
                other.collider, otherPosition, otherRotation,
                thisCollider, transform.position, transform.rotation,
                out direction, out distance
            );
        
         
        //if the colliders are overlapped compute the force 
        //should be all the time when we are in the OnCollisionStay method
        if(overlapped)
        {                
            depth = (transform.position - center).magnitude;
            ratio = 1-((1/totalDepth)*depth);
            //Debug.Log( "Depth ratio: " + ratio);
            force[0] = forceDirection.x*computeForce(Mathf.Abs(ratio));  
            force[1] = forceDirection.y*computeForce(Mathf.Abs(ratio));
            force[2] = forceDirection.z*computeForce(Mathf.Abs(ratio));            
        }
        //Debug.Log("force: " + force[0] + ", " + force[1] + ", " + force[2]);
        talker.SendForceVector(force, torque);
	}
	
	//how stiff should the wall be?
	public float rigidyFactor;
	public float maximumForce;
	//Compute the actual force output in here
	float computeForce(float ratio)
	{
	    float force = rigidyFactor*ratio;
	    //We don't want to exceed the force so we limit it to some force 
	    return (force < maximumForce) ? force : maximumForce;
	}
	
	float[] zeroForce = {0.0f, 0.0f, 0.0f};
	//If the collision ends, set force to zero 
	void OnCollisionExit(Collision other)
	{		
	    talker.SendForceVector(zeroForce, torque);
	}


    // Start is called before the first frame update
    void Start()
    {
        logger = new Logger(new MyLogHandler());

        logger.Log("Starting application");
        data = new PhantomData(20);
        listener = new PhantomListener("192.168.122.199", 50005, data, logger);
        talker = new PhantomTalker("192.168.122.199", 50000, data, logger);
        listener.StartListener(100);
        logger.Log("UDP Listener initialized");
        
        mouse = GameObject.FindGameObjectWithTag("Player");
        
        //Set the physics rendering to 1000 Hz
        Time.fixedDeltaTime = 0.001f;
    }



    // Update is called once per frame
    void Update()
    {        
        //Do something that is not needed more often
    }
    
    void FixedUpdate()
    {
        //Do haptic rendering in here
        mouse.transform.position = data.MousePosition;        
    }

    //Clean up
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
