#include "State.h"
#include "../GameObject/NPC/NPC.h"
#include "../../ModularPathFinding/AStar/AStar.h"
#include "../../ModularPathFinding/BreadthFirst/BreadthFirst.h"
#include "../../ModularPathFinding/DepthFirst/DepthFirst.h"
#include "../../ModularPathFinding/BestFirst/BestFirst.h"
#include "../../ModularPathFinding/AStarEpsilon/AStarEpsilon.h"
#include "../../ModularPathFinding/Dijkstra/Dijkstra.h"
#include "../../ModularPathFinding/Node.h"

void AIState::Start()
{

};
void AIState::Stop()
{

};
void AIState::Update()
{

};

StateManager* AIState::GetManager()
{
	return parent_manager;
}

void AIState::SetManager(StateManager* _param)
{
	parent_manager = _param;
}

AIState::AIState()
{
	parent_manager = nullptr;
	Start();
};
AIState::AIState(StateManager* _parent)
{
	parent_manager = _parent;
	Start();
};
AIState::~AIState()
{
	if (parent_state != nullptr)
		delete parent_state;
};

void StateManager::QueueState(AIState* _state)
{
	_state->SetManager(this);
	m_state_queue.push(this->m_active_state);
	this->m_active_state = _state;
}

std::queue<AIState*>* StateManager::GetQueue()
{
	return &m_state_queue;
}

void StateManager::SwitchState(AIState* _state, bool _force)
{
	if (_state != nullptr)
	{
		if (_force)
		{
			for (size_t i = 0; i < m_state_queue.size(); i++)
			{
				if (m_state_queue.front() == m_active_state)
				{
					m_state_queue.pop();
				}
				else
				{
					delete m_state_queue.front();
					m_state_queue.pop();
				}
			}
		};

		m_active_state = _state;
		_state->SetManager(this);

	};
}

void StateManager::Update()
{
	if (m_active_state != nullptr) m_active_state->Update();
}

void StateManager::SwitchPathAlgorithm(ALGORITHMS _algo)
{
	if (path_algo != nullptr) delete path_algo;
	path_algo = nullptr;
	switch (_algo)
	{
		case ALGORITHMS::ASTAR:
			path_algo = new PathAlgorithms::AStar();
			break;
		case ALGORITHMS::BREADTH_FIRST:
			path_algo = new PathAlgorithms::BreadthFirst();
			break;
		case ALGORITHMS::DEPTH_FIRST:
			path_algo = new PathAlgorithms::DepthFirst();
			break;
		default:
		case ALGORITHMS::BEST_FIRST:
			path_algo = new PathAlgorithms::BestFirst();
			break;
		case ALGORITHMS::ASTAR_EPSILON:
			path_algo = new PathAlgorithms::AStarEpsilon();
			break;
		case ALGORITHMS::DJ_KASTRAS:
			path_algo = new PathAlgorithms::Dijkstra();
			break;
	};
	activePathFinder = _algo;
};

void StateManager::DoPath(glm::ivec2 _start, glm::ivec2 _goal, std::vector<std::vector<NODE*>> _map, bool _allowDiagonal)
{
	if (path_algo != nullptr)
		path_algo->path = path_algo->GeneratePath(_start, _goal, _map, _allowDiagonal);
}

std::vector<NODE>* StateManager::GetPath()
{
	if (path_algo != nullptr)
	{
		return &path_algo->path;
	}
	else
	{
		return nullptr;
	}
}

StateManager::StateManager(NPC* _parent)
{
	parent = _parent;
};

StateManager::~StateManager()
{
	if (path_algo != nullptr)
		delete path_algo;
	if (m_active_state != nullptr)
		delete m_active_state;
	while (m_state_queue.size() > 0)
	{
		if (m_state_queue.front() != nullptr)
			delete m_state_queue.front();
		m_state_queue.pop();
	}
};

size_t StateManager::GetPathPos()
{
	if (path_algo != nullptr)
	{
		size_t result = 0;
		while (result < path_algo->path.size())
		{
			if (parent->pos == path_algo->path.at(result).pos)
				return result;
			result++;
		}
	};
	return SIZE_MAX;
}