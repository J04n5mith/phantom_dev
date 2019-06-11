using UnityEngine;
using UnityEditor;
using System.Threading;
using System;
using System.Net.Sockets;
using System.Net;
using System.Linq;

public class PhantomListener
{
    private int m_portToListen = 8080;
    private volatile bool listening;
    Thread m_ListeningThread;
    UdpClient listener;
    Logger logger;
    PhantomData data;


    public double ScaleFactor{get; set;}
    //constructor
    public PhantomListener(PhantomData data, Logger logger)
    {
        this.logger = logger;
        this.listening = false;
        
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
            IPAddress address = System.Net.IPAddress.Parse("192.168.122.199");
            IPEndPoint groupEP = new IPEndPoint(address, m_portToListen);

            try
            {
                double[] receivedDoubles = new double[3];
                while (this.listening)
                {

                    byte[] bytes = listener.Receive(ref groupEP);

                    receivedDoubles = ParseVector(bytes);
                    
                    data.SetPhantomMousePoint(receivedDoubles[0], receivedDoubles[1], receivedDoubles[2]);
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

    public double[] ParseVector(byte[] bytes)
    {
        double[] receivedDoubles = new double[3];

        for (int i = 0; i < receivedDoubles.Length; i++)
        {
            receivedDoubles[i] = BitConverter.ToDouble(bytes, i*8);
        }
	return receivedDoubles;
    }

    
}


