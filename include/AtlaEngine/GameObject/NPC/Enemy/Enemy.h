#pragma once

#include "../NPC.h"
#include "EnemyAI.h"

class Enemy : public NPC
{
protected:
private:
public:

	enum STATES
	{
		IDLE,
		ATTACK,
		WAIT,
		PATROL
	};

	Enemy()
	{
		brain = new StateManager(this);
		pos.x = 0;
		pos.y = 0;
		Tag = "Enemy";
	};
	Enemy(int _x, int _y)
	{
		brain = new StateManager(this);
		pos.x = _x;
		pos.y = _y;
		Tag = "Enemy";
	};
	Enemy(int _x, int _y, STATES _state)
	{
		brain = new StateManager(this);
		pos.x = _x;
		pos.y = _y;
		Tag = "Enemy";
		switch (_state)
		{
			default:
			case WAIT:
			case IDLE:
				brain->SwitchState(new EnemyAI::Idle());
				break;
			case ATTACK:
				brain->SwitchState(new EnemyAI::Attack());
				break;
			case PATROL:
				brain->SwitchState(new EnemyAI::Patrol());
				break;
		}
	};


	/*~Enemy()
	{

	}*/
};