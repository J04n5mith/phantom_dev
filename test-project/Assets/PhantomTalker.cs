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

    static byte[] tmp = new byte[4];
    public void SendLocationOfGameObjects(int id, Vector3 position)
    {
        
        
        byte[] msg = new byte[1024];
        
        //Operation type Position of GameObject
        ushort op = (ushort)Operation.GAMEOBJECT_POS;
        msg[0] = (byte)(op & 0x0f);
        msg[1] = (byte)((op << 8) & 0x0f);

        //ID of GameObject
        tmp = BitConverter.GetBytes((int)id);
        msg[5] = tmp[0];
	    msg[4] = tmp[1];
	    msg[3] = tmp[2];
	    msg[2] = tmp[3]; 
        
        tmp = BitConverter.GetBytes((float)position.x);
        msg[9] = tmp[0];
	    msg[8] = tmp[1];
	    msg[7] = tmp[2];
	    msg[6] = tmp[3];
        
        tmp = BitConverter.GetBytes((float)position.y);
        msg[13] = tmp[0];
	    msg[12] = tmp[1];
	    msg[11] = tmp[2];
	    msg[10] = tmp[3];

        tmp = BitConverter.GetBytes((float)position.z);
        msg[17] = tmp[0];
	    msg[16] = tmp[1];
	    msg[15] = tmp[2];
	    msg[14] = tmp[3];
        // msg[4] = (byte)(position.x >> 24);
	    // msg[5] = (byte)(position.x >> 16);
	    // msg[6] = (byte)(position.x >> 8);
	    // msg[7] = (byte)(position.x); 

        // msg[8] = (byte)(position.y >> 24);
	    // msg[9] = (byte)(position.y >> 16);
	    // msg[10] = (byte)(position.y >> 8);
	    // msg[11] = (byte)(position.y); 

        // msg[12] = (byte)(position.z >> 24);
	    // msg[13] = (byte)(position.z >> 16);
	    // msg[14] = (byte)(position.z >> 8);
	    // msg[15] = (byte)(position.z); 
        talker.Send(msg, msg.Length, groupEP);
    }

}
