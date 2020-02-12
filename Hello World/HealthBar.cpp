#include "HealthBar.h"
#include <iostream>
#include <string>

HealthBar::HealthBar()
{
}

HealthBar::HealthBar(float health)
{
	m_health = health;
}

HealthBar::~HealthBar()
{
}

void HealthBar::DisplayHealth()
{
	std::cout << "Health: " << std::to_string((m_health * 100.f)) << "%" << std::endl;
}

float HealthBar::GetHealth() const
{
	return m_health;
}

void HealthBar::SetHealth(float health)
{
	m_health = health;
}
