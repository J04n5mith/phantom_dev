using UnityEngine;
using UnityEditor;
using System.Threading;
using System;
using System.Net.Sockets;
using System.Net;
using System.Linq;

public class PhantomTalker
{
    private int portToTalk = 50005;
    private volatile bool talking;
    UdpClient talker;
    IPEndPoint groupEP;
    IPAddress address;
    Logger logger;
    PhantomData data;

    

    public double ScaleFactor{get; set;}
    //constructor
    public PhantomTalker(string ip_address, int port, PhantomData data, Logger logger)
    {
        portToTalk = port;
        address = System.Net.IPAddress.Parse(ip_address);
        this.logger = logger;
        this.talking = false;
        this.data = data;
        Init();
    }

    private void Init()
    {
        talker = null;
        try
        {
            talker = new UdpClient(portToTalk);
        }
        catch (SocketException)
        {
            //do nothing
        }

        if (talker != null)
        {
            groupEP = new IPEndPoint(address, portToTalk);
        }
    }

    public void SendForceVector(float[] force, float[] torque)
    {
    
	    byte[] msg = new byte[100];
	    ushort op = (ushort)Operation.FORCE_TORQUE_ON;
	    msg[0] = (byte)(op & 0x0f);
        msg[1] = (byte)((op << 8) & 0x0f);
	    
	    msg[2] = 1;
	    
	    byte[] tmp = new byte[4];
	    
	    /*******Force********/
	    //force X
	    tmp = BitConverter.GetBytes((float)force[0]);
        msg[6] = tmp[0];
	    msg[5] = tmp[1];
	    msg[4] = tmp[2];
	    msg[3] = tmp[3];
	    
	    //force Y
	    tmp = BitConverter.GetBytes((float)force[1]);
        msg[10] = tmp[0];
	    msg[9] = tmp[1];
	    msg[8] = tmp[2];
	    msg[7] = tmp[3];
	    
	    //force Z
	    tmp = BitConverter.GetBytes((float)force[2]);
        msg[14] = tmp[0];
	    msg[13] = tmp[1];
	    msg[12] = tmp[2];
	    msg[11] = tmp[3];
	    
	    /*******Torque********/
	    //torque X
	    tmp = BitConverter.GetBytes((float)torque[0]);
        msg[18] = tmp[0];
	    msg[17] = tmp[1];
	    msg[16] = tmp[2];
	    msg[15] = tmp[3];
	    
	    //Y
	    tmp = BitConverter.GetBytes((float)torque[1]);
        msg[22] = tmp[0];
	    msg[21] = tmp[1];
	    msg[20] = tmp[2];
	    msg[19] = tmp[3];
	    
	    //Z
	    tmp = BitConverter.GetBytes((float)torque[2]);
        msg[26] = tmp[0];
	    msg[25] = tmp[1];
	    msg[24] = tmp[2];
	    msg[23] = tmp[3];
	    
	    talker.Send(msg, msg.Length, groupEP);
	}
	
	public void turnForcesOff()
    {
        
	    byte[] msg = new byte[100];
	    ushort op = (ushort)Operation.FORCE_TORQUE_ON;
	    msg[0] = (byte)(op & 0x0f);
        msg[1] = (byte)((op << 8) & 0x0f);
        
        msg[2] = 0;
        
        talker.Send(msg, msg.Length, groupEP);
	}
	
	

    static byte[] tmp = new byte[4];
    public void SendLocationOfGameObjects(int id, Vector3 position)
    {
        byte[] msg = new byte[100];
        
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
	    
        talker.Send(msg, msg.Length, groupEP);
    }
    

}
