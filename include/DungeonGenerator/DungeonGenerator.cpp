#include "DungeonGenerator.h"
#include "../ModularPathFinding/Node.h"
#include "../SDL2/SDL.h"
#include "../SDL2/SDL_image.h"
#include "../ModularPathFinding/AStar/AStar.h"
#include "../ModularPathFinding/BreadthFirst/BreadthFirst.h"
#include "../ModularPathFinding/BestFirst/BestFirst.h"

#include "../AtlaEngine/GameObject/GameObject.h"

#include "../AtlaEngine/Atla.h"

void DungeonGenerator::PlaceRoom(Level* _MAP, glm::ivec2 _pos, glm::ivec2 _size, int _type)
{
	SDL_Texture* randomobjects = _MAP->engine->RMANAGER->GetTexture("./graphics/objects.png", _MAP->engine->RENDERER);
	std::vector<std::vector<int>>* MAP = &_MAP->map;
	for (size_t y = 0; y < size_t(_size.y); y++)
	{
		for (size_t x = 0; x < size_t(_size.x); x++)
		{
			MAP->at(y + _pos.y).at(x + _pos.x) = _type;
		};
	};
	_MAP->UpdateNodeMap();
	ROOM tempRoom;
	tempRoom.pos = _pos;
	tempRoom.size = _size;
	GameObject* treasure = new GameObject(_pos + glm::ivec2(rand() % _size.x, rand() % _size.y), _MAP, "Treasure");
	treasure->SetActiveSprite(randomobjects);
	_MAP->nmap.at(treasure->pos.y).at(treasure->pos.x)->cost = 0.5f;
	_MAP->rooms.push_back(tempRoom);
}

void DungeonGenerator::PopulateWithRooms(Level* _MAP, unsigned int _roomcount)
{
	_MAP->rooms.clear();
	std::vector<std::vector<int>>* MAP = &_MAP->map;
	unsigned int MAPY = MAP->size();
	unsigned int MAPX = MAP->at(0).size();
	unsigned int roomCount = 0;
	while (roomCount < _roomcount)
	{
		glm::ivec2 breakout = glm::ivec2();

		unsigned int initialRoomWidth = rand() % 6 + 4;
		size_t testX = rand() % (MAPX - initialRoomWidth); // where the room will go X
		size_t testY = rand() % (MAPY - initialRoomWidth); // where the room will go Y
		if (testX == 0) testX++;
		if (testY == 0) testY++;

		bool conflict = false;
		for (size_t y = 0; y < initialRoomWidth; y++)
		{
			for (size_t x = 0; x < initialRoomWidth; x++)
			{
				if (MAP->at(y + testY).at(x + testX) == 1)
				{
					conflict = true;
				}
				else
				{
					//breakout = nullptr;
				};
			};
		};
		// if it exceeds vector size
		if (conflict == false && MAP->size() <= (testY + initialRoomWidth))
		{
			// do nothing since breakout is still null anyway
		}
		else
		{
			if (MAP->at(0).size() <= (testX + initialRoomWidth))
			{
				// do nothing since breakout is still null anyway
			}
			else
			{
				// shouldn't be OOB, so lets do it
				breakout = glm::ivec2(testX, testY);
			}
		}

		if (breakout != glm::ivec2())
		{
			glm::vec2 size = { (float)initialRoomWidth, (float)initialRoomWidth };
			PlaceRoom(_MAP, breakout, size, 0);

			roomCount++;
			if (roomCount == (_roomcount - 1))
			{
				GameObject* exit = new GameObject(glm::ivec2(_MAP->rooms.back().pos.x + (rand() % _MAP->rooms.back().size.x), _MAP->rooms.back().pos.y + (rand() % _MAP->rooms.back().size.y)), _MAP, "Exit");
				exit->SetActiveSprite(_MAP->engine->RMANAGER->GetTexture("./graphics/stairs_exit.png", _MAP->engine->RENDERER));
			}
		};
	};
}

void DungeonGenerator::ConnectRooms(Level* _MAP)
{
	// init path variable + pathfinder
	std::vector<NODE> path;
	PathAlgorithms::BreadthFirst* pathfinder = new PathAlgorithms::BreadthFirst();

	// Create the temporary empty node array for the pathfinder to use
	std::vector<std::vector<NODE*>> tempnodearray;
	for (size_t y = 0; y < _MAP->map.size(); y++)
	{
		std::vector<NODE*> temporary;
		for (size_t x = 0; x < _MAP->map.at(y).size(); x++)
		{
			temporary.push_back(new NODE(glm::ivec2(x, y), false));
		}
		tempnodearray.push_back(temporary);
	}

	// for every room (barring the last one
	for (size_t i = 0; i < (_MAP->rooms.size() - 1); i++)
	{
		std::vector<NODE> path = pathfinder->GeneratePath(_MAP->rooms.at(i).pos, _MAP->rooms.at(i + 1).pos + (_MAP->rooms.at(i + 1).size / 2), tempnodearray, false);
		for each (NODE var in path)
		{
			_MAP->map.at(var.pos.y).at(var.pos.x) = 0;
		}
	}

	delete pathfinder;
	for (size_t y = 0; y < tempnodearray.size(); y++)
	{
		for (size_t x = 0; x < tempnodearray.at(y).size(); x++)
		{
			delete tempnodearray.at(y).at(x);
		}
	}

	return;
}

void DungeonGenerator::GenerateTexture(SDL_Renderer* _renderer, Level* _MAP)
{
	SDL_Texture* floor = _MAP->engine->RMANAGER->GetTexture("./graphics/floor.png", _renderer);
	SDL_Texture* wall = _MAP->engine->RMANAGER->GetTexture("./graphics/wall.png", _renderer);

	_MAP->texture = SDL_CreateTexture(_renderer, NULL, SDL_TEXTUREACCESS_TARGET, _MAP->map.at(0).size() * 16, _MAP->map.size() * 16);
	SDL_SetTextureBlendMode(_MAP->texture, SDL_BLENDMODE_BLEND);

	_MAP->texture_rect = new SDL_Rect();
	SDL_QueryTexture(_MAP->texture, NULL, NULL, &_MAP->texture_rect->w, &_MAP->texture_rect->h);
	_MAP->texture_rect->h *= 3;
	_MAP->texture_rect->w *= 3;

	SDL_SetRenderTarget(_renderer, _MAP->texture);
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderClear(_renderer);

	for (size_t y = 0; y < _MAP->map.size(); y++)
	{
		for (size_t x = 0; x < _MAP->map.at(y).size(); x++)
		{


			int sprite_size = 16;
			SDL_Rect temp = { 0 + (sprite_size * (int)x) ,  0 + (sprite_size * (int)y), sprite_size,sprite_size };
			if (_MAP->map.at(y).at(x) != 1)
			{
				int16_t type = DetermineFloorType(x, y, _MAP);
				SDL_Rect srctemp = { 16 * type, 0, 16, 16 };

				SDL_RenderCopy(_renderer, floor, &srctemp, &temp);
			}
			else
			{
				int8_t type = -1;

				if ((y - 1) != SIZE_MAX && (y + 1) < _MAP->map.size())
				{
					if (
						_MAP->map.at(y - 1).at(x) == 0 &&		//    0			
						_MAP->map.at(y).at(x) == 1			    //  x 1 x
						)										//    x	
					{
						type = 0;
					}
				}

				SDL_Rect srctemp = { 16 * type, 0, 16,16 };
				SDL_RenderCopy(_renderer, wall, &srctemp, &temp);
			}

		}
	}
	SDL_SetRenderTarget(_renderer, NULL);
	return;
}

int16_t DungeonGenerator::DetermineFloorType(int x, int y, Level* _MAP)
{
	int16_t type = -1;

	enum floortype
	{
		HORIZ,
		VERT,
		TOP_RIGHT_CORN,
		TOP_LEFT_CORN,
		BOTTOM_RIGHT_CORN,
		BOTTOM_LEFT_CORN,
		BOTTOMWALL,
		TOPWALL,
		FLOOR,
		CROSS_SECTION,
		LEFTWALL,
		RIGHTWALL
	};

	// if y-1 is not less than zero
	// if y+1 is less than the y-size of the map
	if (
		(y - 1) != SIZE_MAX &&
		size_t(y + 1) < _MAP->map.size() &&
		(x - 1) != SIZE_MAX &&
		size_t(x + 1) < _MAP->map.at(y).size()
		)
	{
		if (
			_MAP->map.at(y - 1).at(x) == 1 &&		//    1
			_MAP->map.at(y + 1).at(x) == 0 &&		//  0 x 0
			_MAP->map.at(y).at(x + 1) == 0 &&		//    0
			_MAP->map.at(y).at(x - 1) == 0
			)
		{
			type = BOTTOMWALL;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 0 &&		//    0
			_MAP->map.at(y + 1).at(x) == 1 &&		//  0 x 0
			_MAP->map.at(y).at(x + 1) == 0 &&		//    1
			_MAP->map.at(y).at(x - 1) == 0
			)
		{
			type = TOPWALL;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 0 &&		//    0
			_MAP->map.at(y + 1).at(x) == 0 &&		//  0 x 0
			_MAP->map.at(y).at(x + 1) == 0 &&		//    0
			_MAP->map.at(y).at(x - 1) == 0
			)
		{
			type = FLOOR;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 1 &&		//    1
			_MAP->map.at(y + 1).at(x) == 1 &&		//  0 x 0
			_MAP->map.at(y).at(x + 1) == 0 &&		//    1
			_MAP->map.at(y).at(x - 1) == 0
			)
		{
			type = HORIZ;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 0 &&		//    0
			_MAP->map.at(y + 1).at(x) == 0 &&		//  1 x 1
			_MAP->map.at(y).at(x + 1) == 1 &&		//    0
			_MAP->map.at(y).at(x - 1) == 1
			)
		{
			type = VERT;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 1 &&		//    1
			_MAP->map.at(y + 1).at(x) == 0 &&		//  0 x 1
			_MAP->map.at(y).at(x + 1) == 1 &&		//    0
			_MAP->map.at(y).at(x - 1) == 0
			)
		{
			type = TOP_RIGHT_CORN;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 1 &&		//    1
			_MAP->map.at(y + 1).at(x) == 0 &&		//  1 x 0
			_MAP->map.at(y).at(x + 1) == 0 &&		//    0
			_MAP->map.at(y).at(x - 1) == 1
			)
		{
			type = TOP_LEFT_CORN;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 0 &&		//    0
			_MAP->map.at(y + 1).at(x) == 1 &&		//  0 x 1
			_MAP->map.at(y).at(x + 1) == 1 &&		//    1
			_MAP->map.at(y).at(x - 1) == 0
			)
		{
			type = BOTTOM_RIGHT_CORN;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 0 &&		//    0
			_MAP->map.at(y + 1).at(x) == 1 &&		//  1 x 0
			_MAP->map.at(y).at(x + 1) == 0 &&		//    1
			_MAP->map.at(y).at(x - 1) == 1
			)
		{
			type = BOTTOM_LEFT_CORN;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 0 &&		//    0
			_MAP->map.at(y + 1).at(x) == 0 &&		//  1 x 0
			_MAP->map.at(y).at(x + 1) == 0 &&		//    0
			_MAP->map.at(y).at(x - 1) == 1
			)
		{
			type = LEFTWALL;
		}
		else if (
			_MAP->map.at(y - 1).at(x) == 0 &&		//    0
			_MAP->map.at(y + 1).at(x) == 0 &&		//  0 x 1
			_MAP->map.at(y).at(x + 1) == 1 &&		//    0
			_MAP->map.at(y).at(x - 1) == 0
			)
		{
			type = RIGHTWALL;
		};
	}
	return type;
}