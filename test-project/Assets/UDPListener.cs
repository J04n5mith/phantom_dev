using UnityEngine;
using UnityEditor;
using System.Threading;
using System;
using System.Net.Sockets;
using System.Net;
using System.Linq;

public class UDPListener
{
    private int m_portToListen = 8080;
    private volatile bool listening;
    Thread m_ListeningThread;
    UdpClient listener;
    Logger logger;

    public Vector3 ReceivedVector { get { return receivedVector; } }
    private Vector3 receivedVector;

    //constructor
    public UDPListener(Logger logger)
    {
        this.logger = logger;
        this.listening = false;
        receivedVector = new Vector3();
        receivedVector.Set(0, 0, 0);
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
        catch (SocketException)
        {
            //do nothing
        }

        if (listener != null)
        {
            IPAddress address = System.Net.IPAddress.Parse("192.168.122.199");
            IPEndPoint groupEP = new IPEndPoint(address, m_portToListen);

            try
            {
                //string s = "";
                double[] receivedDoubles = new double[3];
                while (this.listening)
                {

                    byte[] bytes = listener.Receive(ref groupEP);


                    for (int i = 0; i < receivedDoubles.Length; i++)
                    {
                        receivedDoubles[i] = BitConverter.ToDouble(bytes, i*8);
                    }
                    
                    //Set the new Vector (x,y,z) - z need to be inverted
                    receivedVector.Set((float)(receivedDoubles[0]/20), (float)(receivedDoubles[1]/20), (float)(-(receivedDoubles[2]/20)));
                    
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
            finally
            {
                listener.Close();
                Console.WriteLine("Done listening for UDP broadcast");
            }
        }
    }

    
}

