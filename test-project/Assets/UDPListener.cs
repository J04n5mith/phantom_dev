using UnityEngine;
using UnityEditor;
using System.Threading;
using System.Diagnostics;
using System;
using System.Net.Sockets;
using System.Net;
using System.Linq;

public class PhantomListener
{
    private int m_portToListen;
    IPAddress address;
    
    private volatile bool listening;
    Thread m_ListeningThread;
    UdpClient listener;
    Logger logger;
    PhantomData data;


    public double ScaleFactor{get; set;}
    //constructor
    public PhantomListener(string target_ip, int port, PhantomData data, Logger logger)
    {
        this.logger = logger;
        this.listening = false;
        m_portToListen = port;
        address = System.Net.IPAddress.Parse(target_ip);
        data.SetPhantomMousePoint(1, 1, 1);
        this.ScaleFactor = 20.0;
        this.data = data;
    }

    public void StartListener(int exceptedMessageLength)
    {
        if (listening == false)
        {
            logger.Log("Init and Start Logger");
            m_ListeningThread = new Thread(ListenForUDPPackages);
            m_ListeningThread.IsBackground = true;
            this.listening = true;
            m_ListeningThread.Start();
            logger.Log("Logger started");
        }
    }

    public void StopListener()
    {
        this.listening = false;
        this.listener.Close();
    }



   readonly double _microSecPerTick = 1000000D / System.Diagnostics.Stopwatch.Frequency;
    public void ListenForUDPPackages()
    {
        listener = null;
        try
        {
            listener = new UdpClient(m_portToListen);
        }
        catch (SocketException sex)
        {
            logger.LogException(sex);
        }

        if (listener != null)
        {
            IPEndPoint groupEP = new IPEndPoint(address, m_portToListen);

            try
            {
                
                while (this.listening)
                {
                    byte[] bytes = listener.Receive(ref groupEP);
                    
                    /*Stopwatch stopWatch = new Stopwatch();
                    stopWatch.Start();*/
                    DeserializeMessage(bytes);
                    /*stopWatch.Stop();
                    long ts = (long)(stopWatch.ElapsedTicks * _microSecPerTick);
                    logger.Log("Microseconds: " + ts);*/
                    
                }
            }
            catch (Exception e)
            {
                logger.LogException(e);
            }
            finally
            {
                listener.Close();
            }
        }
    }

    private void DeserializeMessage(byte[] msg)
    {
        Operation op = (Operation)(msg[0] | (msg[1] << 8));
        switch(op)
        {
            case Operation.MOUSE_POS:
                SaveMousePositionFromBuffer(msg);
                break;

            default:
                logger.Log("Not supported Operation");
                break;

        }

    }

    private void SaveMousePositionFromBuffer(byte[] msg)
    {
        double[] receivedDoubles = new double[3];
        receivedDoubles = ParseVector(msg);
        data.SetPhantomMousePoint(receivedDoubles[0], receivedDoubles[1], receivedDoubles[2]);
    }

    private double[] ParseVector(byte[] bytes)
    {
        double[] receivedDoubles = new double[3];

        for (int i = 0; i < receivedDoubles.Length; i++)
        {
            receivedDoubles[i] = BitConverter.ToDouble(bytes, (i*8)+2);
        }
	    return receivedDoubles;
    }

    
}


