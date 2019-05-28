using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using System.IO;
using System;
using UnityX3D;

public class MoveBehaviour : MonoBehaviour
{
    UDPListener listener;
    GameObject player;
    Logger logger;

    // Start is called before the first frame update
    void Start()
    {
        logger = new Logger(new MyLogHandler());

        logger.Log("Starting application");
        
        listener = new UDPListener(logger);
        listener.StartListener(1024);
        logger.Log("UDP Listener initialized");
        

        player = GameObject.FindGameObjectWithTag("Player");
    }



    // Update is called once per frame
    void Update()
    {
        player.transform.position = listener.ReceivedVector;
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