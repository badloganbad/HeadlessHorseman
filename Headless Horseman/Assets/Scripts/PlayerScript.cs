using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class PlayerScript: MonoBehaviour {

	const float MAXSPEED = 20.0f;
	const float MINSPEED = -4.0f;
	const float PLAYERSPEED = 10.0f;

	public float velocity = 0.0f;

	//health
	float MaxHealth = 100.0f;
	public float currentHealth = 100.0f;

	//takedowns
	float MaxTakeDowns = 10;
	public float currentTakeDowns = 0;

	public Image H_front; // for health front
	public Image H_middle;
	public Image H_back;

	public Image T_front; // for tomb front
	public Image T_middle;
	public Image T_back;

	void GotVengeance()
	{
		currentTakeDowns += 1;
		T_middle.fillAmount = currentTakeDowns / MaxTakeDowns;
	}

	void GotPickup()
	{
		
	}

	void GotHit()
	{
		currentHealth -= 20;
		H_middle.fillAmount = currentHealth / MaxHealth;
	}

	void GettingHit()
	{

	}

	void GotHealth()
	{
		currentHealth += 25;
		if (currentHealth > MaxHealth)
			currentHealth = MaxHealth;
		H_middle.fillAmount = currentHealth / MaxHealth;
	}

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {

		//check for player inputs
		if (Input.GetKey (KeyCode.W))
			velocity += PLAYERSPEED * Time.deltaTime;

		if (Input.GetKey (KeyCode.A))
			transform.Rotate (0, -100*Time.deltaTime, 0);

		if (Input.GetKey(KeyCode.S))
			velocity -= PLAYERSPEED/2 * Time.deltaTime;
		
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
