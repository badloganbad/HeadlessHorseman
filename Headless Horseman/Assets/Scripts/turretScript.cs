using UnityEngine;
using System.Collections;

public class turretScript : MonoBehaviour {

	public Transform player;

	public GameObject turretRotate;

	public float attackDist = 7.0f;

	public bool activated;

	public Transform firePoint;

	public Rigidbody bullet;

	public float bulletForce;

	public float fireRate;

	float fireTime = 0.0f;

	//public AudioClip hit, explode;

	public int turretHealth, minDamage, maxDamage;

	//public GameObject explosion;


	//public Rigidbody pickup;

	void OnCollisionEnter(Collision other){



		}

	// Use this for initialization
	void Start () {

	}
	
	// Update is called once per frame
	void Update ()
	{
	
		float dist = Vector3.Distance (player.transform.position, transform.position);

		if (dist <= attackDist) {


			activated = true;

			if (Time.time > fireTime) {

				Rigidbody tempBullet = Instantiate (bullet, firePoint.position, firePoint.rotation) as Rigidbody;
		
				Vector3 fwd = firePoint.TransformDirection (Vector3.forward);

				tempBullet.velocity = fwd * bulletForce;

				fireTime = Time.time + fireRate;
			}
				
		}
	
	}
}