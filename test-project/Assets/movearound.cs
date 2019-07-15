using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class movearound : MonoBehaviour {

	// Adjust the speed for the application.
    public float speed = 1.0f;

    // The target (cylinder) position.
    private Transform target;

	// Use this for initialization
	void Start () {
        /*GameObject[] cube = GameObject.FindGameObjectsWithTag("Objects");
		target =  cube[0].transform;
        target.transform.position = new Vector3(2.0f, 0.0f, 2.0f);*/
	}
	
	// Update is called once per frame
	void Update () {
		/*// Move our position a step closer to the target.
        float step =  speed * Time.deltaTime; // calculate distance to move
        transform.position = Vector3.MoveTowards(transform.position, target.position, step);

        // Check if the position of the cube and sphere are approximately equal.
        if (Vector3.Distance(transform.position, target.position) < 0.001f)
        {
            // Swap the position of the cylinder.
            target.position *= -1.0f;
        }*/
		
	}
}
