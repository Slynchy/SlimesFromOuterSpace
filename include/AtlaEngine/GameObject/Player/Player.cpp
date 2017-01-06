#include "Player.h"
#include "../../Level/Level.h"
#include "../../InputManager/InputManager.h"
#include "../../Atla.h"
#include "../NPC/Enemy/Enemy.h"

// return true if successfully moved
bool Player::Move(glm::ivec2 _movement)
{
	glm::ivec2 newpos = this->pos + _movement;
	direction = GetDirectionFromDelta(_movement);
	if (parent_level->map.at(newpos.y).at(newpos.x) != 1)
	{
		this->pos += _movement;
		this->m_isMoving = true;

		for each (GameObject* var in GameObject::SceneGraph)
		{
			if (var->pos == this->pos && var->Tag == "Treasure")
			{
				m_collectedTreasure.push_back(var);
				GameObject::RemoveFromSceneGraph(var);
				this->parent_level->UpdateNodeMap();
				this->parent_level->engine->PlaySFX(this->parent_level->engine->RMANAGER->GetSFX("./sfx/blip.mp3"));
				break;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
};

void Player::Draw()
{
	if (this->m_hidden) return;
	this->draw_pos = Atla::interpolate(this->draw_pos, this->pos, 0.03f);
	SDL_Rect temprectsrc = {
		0,
		16 * this->direction,
		16,
		16
	};
	SDL_Rect temprect = {
		int((int(16 * Atla::zoomFactor) * this->draw_pos.x) - (int)this->parent_level->engine->GetViewport()->pos.x),
		int((int(16 * Atla::zoomFactor) * this->draw_pos.y) - (int)this->parent_level->engine->GetViewport()->pos.y),
		int(16 * Atla::zoomFactor),
		int(16 * Atla::zoomFactor)
	};
	SDL_RenderCopy(this->parent_level->engine->RENDERER, this->GetActiveSprite(), &temprectsrc, &temprect);
	if (
		(draw_pos.x > (float)((float)pos.x - 0.0005f) && draw_pos.x < (float)((float)pos.x + 0.0005f)) &&
		(draw_pos.y > (float)((float)pos.y - 0.0005f) && draw_pos.y < (float)((float)pos.y + 0.0005f))
		)
	{
		this->m_isMoving = false;
	}
}

void Player::Update()
{
	if (this->isMoving() == true) return;
	InputManager* _input = this->parent_level->engine->INPUTMANAGER;

	bool input = false;

	if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_F1])
	{
		this->parent_level->engine->TOGGLE_DEBUG_MODE();
		this->isInvisible(!this->isInvisible());
		SDL_Delay(200);
		return;
	}

	if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_W])
	{
		input = this->Move(glm::ivec2(0, -1));
	}
	else if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_S])
	{
		input = this->Move(glm::ivec2(0, 1));
	}
	else if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_A])
	{
		input = this->Move(glm::ivec2(-1, 0));
	}
	else if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_D])
	{
		input = this->Move(glm::ivec2(1, 0));
	}

	if (this->parent_level->engine->IS_DEBUG_MODE())
	{
		if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_EQUALS])
		{
			input = true;
		}

		if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_0])
		{
			for each (GameObject* var in GameObject::SceneGraph)
			{
				Enemy* temp;
				if(var->Tag == "Enemy")
					temp = dynamic_cast<Enemy*>(var);
				else continue;

				if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::ASTAR)
				{
					temp->GetBrain()->defaultPathfinder = StateManager::ALGORITHMS::DEPTH_FIRST;
				}
				else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::DEPTH_FIRST)
				{
					temp->GetBrain()->defaultPathfinder = StateManager::ALGORITHMS::BREADTH_FIRST;
				}
				else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::BREADTH_FIRST)
				{
					temp->GetBrain()->defaultPathfinder = StateManager::ALGORITHMS::BEST_FIRST;
				}
				else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::BEST_FIRST)
				{
					temp->GetBrain()->defaultPathfinder = StateManager::ALGORITHMS::ASTAR_EPSILON;
				}
				else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::ASTAR_EPSILON)
				{
					temp->GetBrain()->defaultPathfinder = StateManager::ALGORITHMS::DJ_KASTRAS;
				}
				else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::DJ_KASTRAS)
				{
					temp->GetBrain()->defaultPathfinder = StateManager::ALGORITHMS::ASTAR;
				}
				temp->GetBrain()->SwitchPathAlgorithm(temp->GetBrain()->defaultPathfinder);
			}
			SDL_Delay(100);
		}

		if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_N])
		{
			//Atla::zoomFactor -= 0.025f;
			Atla::zoomFactor = Atla::interpolate(Atla::zoomFactor, Atla::zoomFactor - 0.05f, 0.03f);
		}
		else if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_M])
		{
			//Atla::zoomFactor += 0.025f;
			Atla::zoomFactor = Atla::interpolate(Atla::zoomFactor, Atla::zoomFactor + 0.05f, 0.03f);
		}

		if (_input->SDL_KEYBOARDSTATE[SDL_SCANCODE_MINUS])
		{
			this->parent_level->DumpNMAP();
		}
	}

	if (input)
	{
		this->parent_level->engine->UpdateAI();
	}
}