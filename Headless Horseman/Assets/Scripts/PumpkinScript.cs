using UnityEngine;
using System.Collections;

public class PumpkinScript : MonoBehaviour {


	void OnTriggerEnter(Collider other)
	{
		if (other.tag == "Player") {
			other.SendMessage("GotHealth");
		}
	}

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
