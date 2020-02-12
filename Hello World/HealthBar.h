#pragma once
#include "JSON.h"

class HealthBar
{
public:
	//Creates the health bar
	HealthBar();
	//Creates the health bar with this amount of health
	HealthBar(float health);
	//Destroys the health bar
	~HealthBar();

	//Display the current health
	void DisplayHealth();
	//Getters
	float GetHealth() const;
	//Setters
	void SetHealth(float health);
private:
	//100% Health
	float m_health = 1.f;
};

inline void to_json(nlohmann::json& j, const HealthBar& bar)
{
	j["Health"] = bar.GetHealth();
}
inline void from_json(const nlohmann::json& j, HealthBar& bar)
{
	bar.SetHealth(j["Health"]);
}
