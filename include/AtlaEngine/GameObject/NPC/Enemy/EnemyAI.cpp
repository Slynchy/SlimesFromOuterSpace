#include "EnemyAI.h"

#include "../../../Level/Level.h"
#include "../../../../ModularPathFinding/node.h"

#include "../../../FiniteAnimation.h"

#include "../../Player/Player.h"

#include "../../../../ModularPathFinding/pathalgo.h"

void EnemyAI::Patrol::Update()
{
	if (this->GetManager()->parent == nullptr)
	{
		return;
	}
	else if (path_pos == SIZE_MAX)
	{
		int randomnum = rand() % this->parent_manager->parent->parent_level->rooms.size();
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		parent_manager->SwitchPathAlgorithm(this->parent_manager->defaultPathfinder);
		this->parent_manager->DoPath(
			this->parent_manager->parent->pos,
			this->parent_manager->parent->parent_level->rooms.at(randomnum).pos + glm::ivec2(
				this->parent_manager->parent->parent_level->rooms.at(randomnum).size.x * r,
				this->parent_manager->parent->parent_level->rooms.at(randomnum).size.y * r2
			),
			this->parent_manager->parent->parent_level->nmap,
			false);
		path_pos = 0;
		return;
	}
	else if ((path_pos) >= this->parent_manager->GetPath()->size())
	{
		path_pos = SIZE_MAX;
		this->parent_manager->QueueState(new SmartWait((rand() % 3) + 2));
		return;
	};

	bool queueUpdate = false;
	std::vector<glm::ivec2*> temp;
	for each (GameObject* var in this->parent_manager->parent->SceneGraph)
	{
		// Check its not the same object
		if (var->pos == this->parent_manager->parent->pos) continue;

		if (var->Tag == "Treasure") continue;

		// Check if player is in range of us
		if (var->Tag == "Player" && glm::distance(glm::vec2(var->pos), glm::vec2(this->parent_manager->parent->pos)) < 5)
		{
			if (!var->isInvisible())
			{
				this->parent_manager->QueueState(new SeekPlayer(var));
				return;
			}
		}

		if (var->Tag == "Enemy")
		{
			temp.push_back(&var->pos);
		}

		if (this->parent_manager->GetPath()->at(path_pos).pos == var->pos)
		{
			queueUpdate = true;
			continue;
		}
	}

	this->parent_manager->GetPathAlgo()->SetAIPositions(temp);
	if (queueUpdate)
	{
		this->parent_manager->DoPath(
			this->parent_manager->parent->pos,
			this->parent_manager->GetPath()->at(this->parent_manager->GetPath()->size() - 1).pos,
			this->parent_manager->parent->parent_level->nmap,
			false);
		path_pos = 1;
	}

	if (this->parent_manager->GetPath()->size() > 1)
		this->GetManager()->parent->Move(this->parent_manager->GetPath()->at(path_pos).pos - this->GetManager()->parent->pos);

	path_pos++;
}

void EnemyAI::SeekPlayer::Update()
{
	// check if we don't have a parent so we don't access nullptr
	if (this->GetManager()->parent == nullptr)
	{
		return;
	}

	if(this->parent_manager->activePathFinder != StateManager::ALGORITHMS::ASTAR)
		parent_manager->SwitchPathAlgorithm(StateManager::ALGORITHMS::ASTAR);

	if (this->GetManager()->parent->pos == m_player->pos)
	{
		this->GetManager()->SwitchState(new Explode(m_player, this->parent_manager->parent->brain), true);
		delete this;
		return;
	}

	// Check if player is in range of us
	if (glm::distance(glm::vec2(m_player->pos), glm::vec2(this->parent_manager->parent->pos)) >= 5)
	{
		this->GetManager()->SwitchState(this->GetManager()->GetQueue()->front());
		this->GetManager()->GetQueue()->pop();
		parent_manager->SwitchPathAlgorithm(this->parent_manager->defaultPathfinder);
		delete this;
		return;
	}

	std::vector<glm::ivec2*> temp;
	for each (GameObject* var in this->parent_manager->parent->SceneGraph)
	{
		// Check its not the same object
		if (var->pos == this->parent_manager->parent->pos) continue;

		if (var->Tag == "Treasure" || var->Tag == "Player") continue;

		if (var->Tag == "Enemy")
		{
			temp.push_back(&var->pos);
			continue;
		}
		else
		{
			continue;
		}
	}

	this->parent_manager->GetPathAlgo()->SetAIPositions(temp);
	this->parent_manager->DoPath(
		this->parent_manager->parent->pos,
		m_player->pos,
		this->parent_manager->parent->parent_level->nmap,
		false);

	if (this->parent_manager->GetPath()->size() >= 2)
	{
		this->GetManager()->parent->Move(this->parent_manager->GetPath()->at(1).pos - this->GetManager()->parent->pos);
	}

	if (this->GetManager()->parent->pos == m_player->pos)
	{
		this->GetManager()->SwitchState(new Explode(m_player, this->parent_manager->parent->brain), true);
		delete this;
		return;
	}
};

void EnemyAI::Explode::Update()
{

};

EnemyAI::Explode::Explode(GameObject* _player, StateManager* _parentstateman)
{
	dynamic_cast<Player*>(_player)->lives--;
	this->parent_manager = _parentstateman;
	this->parent_manager->parent->parent_level->engine->PlaySFX(this->parent_manager->parent->parent_level->engine->RMANAGER->GetSFX("./sfx/explosion.ogg"));
	FiniteAnimation* temp = new FiniteAnimation(this->parent_manager->parent->pos, this->parent_manager->parent->parent_level->engine->RMANAGER->GetTexture("./graphics/explosion.png", this->parent_manager->parent->parent_level->engine->RENDERER), glm::ivec2(96, 96), 19, this->parent_manager->parent->parent_level);
	this->parent_manager->parent->Destroy();
	delete this;
}

void EnemyAI::SmartWait::Update()
{
	for each (GameObject* var in this->parent_manager->parent->SceneGraph)
	{
		if (var->pos == this->parent_manager->parent->pos) continue;
		if (var->Tag == "Treasure") continue;

		// Check if player is in range of us
		if (var->Tag == "Player" && glm::distance(glm::vec2(var->pos), glm::vec2(this->parent_manager->parent->pos)) < 5)
		{
			if (!var->isInvisible())
			{
				this->parent_manager->QueueState(new SeekPlayer(var));
				timer = UINT32_MAX;
				return;
			};
		}
	}
	timer += 1;
	if (timer >= length)
	{
		this->GetManager()->SwitchState(this->GetManager()->GetQueue()->front());
		this->GetManager()->GetQueue()->pop();
		delete this;
	}
}