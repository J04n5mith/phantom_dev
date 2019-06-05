using UnityEngine;
using UnityEditor;
using System.Threading;
using System;
using System.Net.Sockets;
using System.Net;
using System.Linq;

public class PhantomData
{
    public Vector3 MousePosition {get {return mousePostition;} }
    private Vector3 mousePostition;

    public double ScaleFactor { get {return scale;} }
    private double scale;
    public PhantomData(int scaleFactor)
    {
        this.scale = scaleFactor;
    }

    public void SetPhantomMousePoint(double x, double y, double z)
    {
        //Set the new Vector (x,y,z) - 
        //z need to be inverted TODO: why?
        //All doubles needs to be scaled down --> resolution of phantom is lower
        mousePostition.Set((float)(x/scale),(float)(y/scale),(float)(-(z/scale)));        
    }
}