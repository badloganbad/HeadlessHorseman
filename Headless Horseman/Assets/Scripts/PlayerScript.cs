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

	//boosts/ horseshoes
	int currentBoosts = 0;

	public Image H_front; // for health front
	public Image H_middle;
	public Image H_back;

	public Image T_front; // for tomb front
	public Image T_middle;
	public Image T_back;

	public Image[] horseshoe;

	void GotVengeance()
	{
		currentTakeDowns += 2;
		T_middle.fillAmount = currentTakeDowns / MaxTakeDowns;
	}

	void GotPickup()
	{
		if (currentBoosts < 4) {
			currentBoosts += 1;
			for (int i = 0; i < currentBoosts; i++) {
				if (horseshoe [i].enabled == false)
					horseshoe [i].enabled = true;
			}
		}
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
		
		if (Input.GetKeyDown(KeyCode.Space)) {
			if (currentBoosts > 0) {
					horseshoe [currentBoosts - 1].enabled = false;
					currentBoosts -= 1;
			}
		}
	
	}
}
