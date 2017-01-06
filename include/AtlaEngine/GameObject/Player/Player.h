#pragma once

#include "../GameObject.h"

class Player : public GameObject
{
protected:
private:
	std::vector<GameObject*> m_collectedTreasure;
	bool m_isMoving = false;
public:
	int32_t Layer = 1;
	int16_t lives = 3;
	bool Move(glm::ivec2 _movement);

	bool isMoving()
	{
		return m_isMoving;
	}

	void Draw();

	size_t GetCollectedTreasureAmount()
	{
		return m_collectedTreasure.size();
	}

	Player(Level* _level)
	{
		this->parent_level = _level;
		this->Tag = "Player";
	}

	void Update();
};