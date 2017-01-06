/*! \file EnemyAI.h

Header file housing Enemy object states
*/

#pragma once

#include "../NPC.h"
#include "../../../Atla.h"

namespace EnemyAI
{

	class Idle;
	class SmartWait;
	class Wait;
	class SeekPlayer;
	class Patrol;
	class Explode;

	/// Idle class
	class Idle : public AIState
	{
		protected:
		private:
		public:
			void Update()
			{
				// do nothing cus idle
			}

	};

	class SmartWait : public AIState
	{
	protected:
	private:
		uint32_t timer;
		uint32_t length;
	public:

		SmartWait(uint32_t len)
		{
			length = len;
		}

		void Start()
		{
			timer = 0;
		}

		void Update();

	};

	/// Count up one tick per frame, and if the timer exceeds the specified length, switch back to previous state in queue
	class Wait : public AIState
	{
	protected:
	private:
		uint32_t timer;
		uint32_t length;
	public:

		Wait(uint32_t len)
		{
			length = len;
		}

		void Start()
		{
			timer = 0;
		}

		void Update()
		{
			timer += 1;
			if (timer >= length)
			{
				this->GetManager()->SwitchState(this->GetManager()->GetQueue()->front());
				this->GetManager()->GetQueue()->pop();
				delete this;
			}
		}

	};

	/// Find path, follow path, repeat until told otherwise.
	class Patrol : public AIState
	{
	protected:
	private:
		size_t path_pos;
	public:

		void Start()
		{
			path_pos = 0;
		}

		void Update();

	};

	class Attack : public AIState
	{
	protected:
	private:
	public:
		void Update()
		{
			
		}

	};

	class SeekPlayer : public AIState
	{
	protected:
		GameObject* m_player;
	private:
	public:

		void Start()
		{

		}

		void Update();

		SeekPlayer(GameObject* _player)
		{
			m_player = _player;
		}

	};

	class Explode : public AIState
	{
	protected:
		GameObject* m_player;
	private:
	public:

		void Update();

		//void Update();

		Explode(GameObject* _player, StateManager* _parentstateman);

	};

};