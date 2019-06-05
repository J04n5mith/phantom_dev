using UnityEngine;
using UnityEditor;
using System.Threading;
using System;
using System.Net.Sockets;
using System.Net;
using System.Linq;

public class PhantomTalker
{
    private int m_portToTalk = 8085;
    private volatile bool talking;
    Thread m_TalkingThread;
    UdpClient talker;
    IPEndPoint groupEP;
    IPAddress address;
    Logger logger;
    PhantomData data;

    public double ScaleFactor{get; set;}
    //constructor
    public PhantomTalker(PhantomData data, Logger logger)
    {
        this.logger = logger;
        this.talking = false;
        this.data = data;
        Init();
    }

    public void Init()
    {
        talker = null;
        try
        {
            talker = new UdpClient(m_portToTalk);
        }
        catch (SocketException)
        {
            //do nothing
        }

        if (talker != null)
        {
            address = System.Net.IPAddress.Parse("192.168.122.199");
            groupEP = new IPEndPoint(address, m_portToTalk);
        }
    }

    public void SendLocationOfGameObjects(int id, Vector3 position)
    {
        double[] pos = {position.x, position.y, position.z};
        byte[] msg = new byte[1024];
        msg[0] = (byte)(id >> 24);
	msg[1] = (byte)(id >> 16);
	msg[2] = (byte)(id >> 8);
	msg[3] = (byte)(id); 
        Buffer.BlockCopy(pos, 0, msg, 4, pos.Length);
        talker.Send(msg, msg.Length, groupEP);
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
