using UnityEngine;
using System.Collections;

public class PriestScript : MonoBehaviour {





	void OnTriggerEnter(Collider other)
	{
		if (other.tag == "Player") {
			other.SendMessage("GotHit");
		}
	}

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
