using UnityEngine;
using System.Collections;

public class HorseshoeScript : MonoBehaviour {



	void OnTriggerEnter(Collider other)
	{
		if (other.tag == "Player") {
			other.SendMessage("GotPickup");
		}
	}

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
