using UnityEngine;
using System.Collections;

public class PeasantScript : MonoBehaviour {




	void OnTriggerEnter(Collider other)
	{
		if (other.tag == "Player") {
			other.SendMessage("GotVengeance");
		}
	}

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
