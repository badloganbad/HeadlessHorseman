using UnityEngine;
using System.Collections;

public class MovementScript : MonoBehaviour {

	const float MAXSPEED = 20.0f;
	const float MINSPEED = -4.0f;

	float velocity = 0.0f;

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {

		//check for player inputs
		if (Input.GetKey (KeyCode.W))
			velocity += 10.0f * Time.deltaTime;

		if (Input.GetKey (KeyCode.A))
			transform.Rotate (0, -100*Time.deltaTime, 0);

		if (Input.GetKey(KeyCode.S))
			velocity -= 20.0f * Time.deltaTime;
		
		if (Input.GetKey(KeyCode.D))
			transform.Rotate (0, 100*Time.deltaTime, 0);

		//update variables
		if (velocity >= MAXSPEED)
			velocity = MAXSPEED;
		if (velocity <= MINSPEED)
			velocity = MINSPEED;

		//decay for slowing down - player lets off the keys
		velocity *= .975f;

		//update player position based on velocity
		transform.Translate(velocity*Vector3.forward * Time.deltaTime);
	
	}
}
