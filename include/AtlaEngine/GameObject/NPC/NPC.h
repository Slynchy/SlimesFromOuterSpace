#pragma once

#include "../GameObject.h"
#include "../../AI/State.h"

class Level;

class NPC : public GameObject
{
protected:
private:
	int m_health;
public:
	StateManager* brain;

	void SetHealth(int _health)
	{
		m_health = _health;
	}
	int GetHealth(int _health)
	{
		return m_health;
	}

	virtual StateManager* GetBrain()
	{
		return brain;
	}

	virtual void Update()
	{
		brain->Update();
	}

	~NPC()
	{
		if(brain != nullptr)
			delete brain;
		delete this;
	}
};