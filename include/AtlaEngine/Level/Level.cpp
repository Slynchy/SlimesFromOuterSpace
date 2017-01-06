#include "Level.h"
#include "../../ModularPathFinding/Node.h"
#include "../../SDL2/SDL.h"
#include "../../SDL2/SDL_image.h"
#include "../GameObject/GameObject.h"
#include "../Atla.h"

#include <fstream>

Level::Level(glm::ivec2 _size, Atla* _engine)
{
	texture = nullptr;
	texture_rect = nullptr;
	scene_graph_ptr = &GameObject::SceneGraph;
	backdrop_rect = new SDL_Rect();
	engine = _engine;
	backdrop = _engine->RMANAGER->GetTexture("./graphics/backdrop.png", _engine->RENDERER);
	if(backdrop != NULL)
		SDL_QueryTexture(_engine->RMANAGER->GetTexture("./graphics/backdrop.png"), NULL, NULL, &backdrop_rect->w, &backdrop_rect->h);

	ClearMap(_size);
}

void Level::DumpNMAP()
{
	std::string output;
	for (size_t y = 0; y < nmap.size(); y++)
	{
		for (size_t x = 0; x < nmap.at(y).size(); x++)
		{
			if (nmap.at(y).at(x)->isObstacle)
			{
				output += "1";
			}
			else
			{
				output += "0";
			}
			//temp.write((const char*)nmap.at(y).at(x), 1);
		}
	}

	std::ofstream temp;
	temp.open("./dump.txt");
	temp << output;
	temp.close();

	return;
}

Level::~Level()
{
	if (texture_rect != nullptr)
		delete texture_rect;
	if (texture != nullptr)
		SDL_DestroyTexture(texture);
	if (backdrop_rect != nullptr)
		delete backdrop_rect;
	scene_graph_ptr = nullptr;
	for (size_t y = 0; y < nmap.size(); y++)
	{
		for (size_t x = 0; x < nmap.at(y).size(); x++)
		{
			delete nmap.at(y).at(x);
		}
		nmap.at(y).clear();
	}
	nmap.clear();
}

void Level::Draw(SDL_Renderer* _renderer, glm::ivec2 _offset, float _zoomfactor)
{
	this->texture_rect->x = (_offset.x * -1);
	this->texture_rect->y = (_offset.y * -1);
	this->texture_rect->w = ( this->map.front().size() * int(16 * _zoomfactor) );
	this->texture_rect->h = ( this->map.size() * int(16 * _zoomfactor) );
	SDL_RenderCopy(_renderer, this->texture, NULL, this->texture_rect);
}

void Level::ClearMap()
{
	for (size_t y = 0; y < map.size(); y++)
	{
		for (size_t x = 0; x < map.at(y).size(); x++)
		{
			map.at(y).at(x) = 1;
		}
	}
}

void Level::CreateNodeMap()
{
	for (size_t y = 0; y < nmap.size(); y++)
	{
		for (size_t x = 0; x < nmap.at(y).size(); x++)
		{
			delete nmap.at(y).at(x);
		}
		nmap.at(y).clear();
	}
	nmap.clear();

	for (size_t y = 0; y < map.size(); y++)
	{
		nmap.push_back(std::vector<NODE*>());
		for (size_t x = 0; x < map.at(y).size(); x++)
		{
			if (map.at(y).at(x) == 1)
			{
				nmap.back().push_back(new NODE(glm::ivec2(x, y), true));
			}
			else
			{
				nmap.back().push_back(new NODE(glm::ivec2(x, y), false));
			}
		}
	}
};

void Level::UpdateNodeMap()
{
	for (size_t y = 0; y < map.size(); y++)
	{
		for (size_t x = 0; x < map.at(y).size(); x++)
		{
			if (map.at(y).at(x) == 1)
			{
				nmap.at(y).at(x)->isObstacle = true;
			}
			else
			{
				nmap.at(y).at(x)->isObstacle = false;
			}
			nmap.at(y).at(x)->cost = DEFAULT_MOVE_COST;
		}
	}
	for each (GameObject* var in GameObject::SceneGraph)
	{
		if (var->Tag == "Treasure")
		{
			nmap.at(var->pos.y).at(var->pos.x)->cost = 0.5f;
		}
	}
};

void Level::ClearMap(glm::ivec2 _size)
{
	for (size_t y = 0; y < size_t(_size.y); y++)
	{
		std::vector<int> temp;
		map.push_back(temp);
		for (size_t x = 0; x < size_t(_size.x); x++)
		{
			map.at(y).push_back(1);
		}
	}
}