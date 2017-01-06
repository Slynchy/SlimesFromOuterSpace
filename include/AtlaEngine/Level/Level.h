#pragma once

#include "../../glm/glm.hpp"
#include <vector>

class Atla;

struct NODE;

class GameObject;

struct ROOM
{
	glm::ivec2 pos;
	glm::ivec2 size;
};

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Level
{
protected:
	void ClearMap();
	void ClearMap(glm::ivec2 _size);
private:
public:
	Atla* engine;
	std::vector<std::vector<int>> map;
	std::vector<std::vector<NODE*>> nmap;
	void DumpNMAP();
	std::vector<GameObject*>* scene_graph_ptr;
	std::vector<ROOM> rooms;
	SDL_Texture* texture;
	SDL_Rect* texture_rect;
	SDL_Texture* backdrop;
	SDL_Rect* backdrop_rect;
	Level(glm::ivec2 _size, Atla* _engine);
	~Level();
	void UpdateNodeMap();
	void CreateNodeMap();
	void Draw(SDL_Renderer* _renderer, glm::ivec2 _offset, float _zoomfactor);
};