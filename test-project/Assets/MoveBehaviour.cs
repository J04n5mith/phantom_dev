using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using System.IO;
using System;
using UnityX3D;

public class MoveBehaviour : MonoBehaviour
{
    PhantomListener listener;
    PhantomTalker talker;
    PhantomData data;
    GameObject mouse;
    GameObject[] objects;
    Logger logger;

    // Start is called before the first frame update
    void Start()
    {
        logger = new Logger(new MyLogHandler());

        logger.Log("Starting application");
        data = new PhantomData();
        listener = new PhantomListener(data, logger);
        talker = new PhantomTalker(data, logger);
        listener.StartListener(1024);
        logger.Log("UDP Listener initialized");
        

        mouse = GameObject.FindGameObjectWithTag("Player");

        objects = GameObject.FindGameObjectWithTag("Untagged");
    }



    // Update is called once per frame
    void Update()
    {
        mouse.transform.position = data.MousePosition;
        
        for (int i = 0; i < objects.Length; i++)
        {
            if(objects[i].transform.hasChanged)
            {
                talker.SendLocationOfGameObjects(i, objects[i].transform.position);
            }
        }
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
