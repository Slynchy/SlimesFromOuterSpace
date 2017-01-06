#include "GameObject.h"
#include "../Level/Level.h"
#include "../../SDL2/SDL.h"

#include "../Atla.h"

std::vector<GameObject*> GameObject::SceneGraph;

SDL_Texture* GameObject::GetActiveSprite()
{
	return active_sprite;
}

void GameObject::Update()
{
	
};

void GameObject::SetActiveSprite(SDL_Texture* _spr)
{
	active_sprite = _spr;
}

void GameObject::Draw()
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
}

void GameObject::RemoveFromSceneGraph(GameObject* _var)
{
	for (size_t i = 0; i < SceneGraph.size(); i++)
	{
		if (SceneGraph.at(i) == _var)
		{
			SceneGraph.erase(SceneGraph.begin() + i);
			return;
		}
	}
	printf("Cannot remove object %s from SceneGraph; is not in SceneGraph!\n", _var->Tag.c_str());
};

GameObject::GameObject()
{
	parent_level = nullptr;
	Tag = "Object";
	SceneGraph.push_back(this);
}

GameObject::GameObject(Level* _level)
{
	SceneGraph.push_back(this);
	parent_level = _level;
	Tag = "Object";
}

GameObject::GameObject(glm::ivec2 _pos, Level* _level, std::string _tag)
{
	SceneGraph.push_back(this);
	parent_level = _level;
	Tag = _tag;
	pos = _pos;
	draw_pos = _pos;
}

void GameObject::Destroy()
{
	delete this;
}

GameObject::DIRECTION GameObject::GetDirectionFromDelta(glm::ivec2 _deltapos)
{
	switch (_deltapos.x)
	{
		case -1:
			return LEFT;
		case 1:
			return RIGHT;
		default:
		case 0:
			break;
	}
	switch (_deltapos.y)
	{
	case -1:
		return UP;
	case 1:
		return DOWN;
	default:
	case 0:
		break;
	}
	return DOWN;
}