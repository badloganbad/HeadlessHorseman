using UnityEngine;
using System.Collections;

public class PriestScript : MonoBehaviour {




	void OnTriggerStay(Collider other)
	{
		if(other.tag == "Player")
		{
			other.GetComponent<PlayerScript>().currentHealth-=  .05f;
			other.GetComponent<PlayerScript> ().H_middle.fillAmount = other.GetComponent<PlayerScript> ().currentHealth / 100;
		}
	}

	//variable holding the transform to look at - this will be the player
	public Transform target;

	//variable to track the robot health
	public int Health;

	public float speed;

	//variables for minimum and maxHealth
	public int minHealth, maxHealth;



	// Use this for initialization
	void Start () {


		target = GameObject.FindWithTag ("Player").transform;


	}

	// Update is called once per frame
	void Update () {

		// rotate this enemy to constantly "look at" the information in the variable target - the player's transform.position
		transform.LookAt (target);


		//get access to this character's Character Controller Component
		CharacterController controller = GetComponent<CharacterController> ();

		//(local axis) and not the world's z axis
		Vector3 forward = transform.TransformDirection (Vector3.forward);

		// talk to the Character Controller and tell this enemy to move
		//based on the value given in robotspeed
		controller.SimpleMove (forward * speed);

	}
		
}
