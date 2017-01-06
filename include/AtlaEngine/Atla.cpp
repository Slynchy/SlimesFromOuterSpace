#include "Atla.h"

#include "./Level/Level.h"
#include "../DungeonGenerator/DungeonGenerator.h"
#include "./GameObject/NPC/Enemy/Enemy.h"
#include "./GameObject/Player/Player.h"

#include "./UI/HealthDisplay/HealthDisplay.h"
#include "./UI/ScoreDisplay/ScoreDisplay.h"

#include "../ModularPathFinding/node.h"

#include "../SDL2/SDL_ttf.h"
#include "../SDL2/SDL_mixer.h"
#include "./ResourceManager.h"

#include <utility>

float Atla::zoomFactor = 3.0f;
Uint32 Atla::DeltaTime = 0;

Uint32 Atla::GetDT()
{
	return DeltaTime;
}

void Atla::UpdateLastTime()
{
	lastTime = SDL_GetTicks();
}

void Atla::CalculateDT()
{
	DeltaTime = (SDL_GetTicks() - lastTime);
}

Level* Atla::GetLevel()
{
	return m_activeLevel;
}

void Atla::SetLevel(Level* _level)
{
	m_activeLevel = _level;
}

void Atla::DrawEverything()
{
	SDL_Rect dstrect =
	{
		int((0 - int(this->m_activeLevel->backdrop_rect->w * 0.2f)) - (m_player->draw_pos.x * 8)), // (origin - halftheimagewidth) - (player_lerp_pos * 8)
		int((0 - int(this->m_activeLevel->backdrop_rect->h * 0.2f)) - (m_player->draw_pos.y * 8)), // the 8 is just an arbitrary number to get a parralax-like effect
		this->m_activeLevel->backdrop_rect->w,
		this->m_activeLevel->backdrop_rect->h
	};
	SDL_RenderCopy(RENDERER, this->m_activeLevel->backdrop, this->m_activeLevel->backdrop_rect, &dstrect);

	this->m_activeLevel->Draw(RENDERER, GetViewportOffset(), this->zoomFactor);

	if (this->debug->Enabled())
	{
		this->Debug_Draw();
	}

	for each (GameObject* var in *this->m_activeLevel->scene_graph_ptr)
	{
		if (var->Tag == "Player") continue;
		var->Draw();
	}
	m_player->Draw();

	this->m_UI->Draw();
}

void Atla::Debug_Draw()
{
	SDL_SetRenderDrawColor(RENDERER, 255, 0, 0, 255);

	uint32_t enemy_counter = 0;
	uint32_t treas_counter = 0;
	for each (Enemy* var in *this->m_activeLevel->scene_graph_ptr)
	{
		if (var->Tag == "Enemy") 
			enemy_counter++;
		if (var->Tag == "Treasure")
			treas_counter++;
	};
	SDL_Color color = { 255,0,0 };
	SDL_Rect tempDst = { 0,0,0,0 };
	std::string tempStr = "Number of enemies: ";
	tempStr += std::to_string(enemy_counter);
	SDL_Surface *text_surface = TTF_RenderText_Solid(RMANAGER->GetFont("./graphics/PixelFJVerdana12pt.ttf", 8), tempStr.c_str() , color);
	SDL_Texture* temp = SDL_CreateTextureFromSurface(RENDERER, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_QueryTexture(temp, NULL, NULL, &tempDst.w, &tempDst.h);
	SDL_RenderCopy(RENDERER, temp, NULL, &tempDst);
	SDL_DestroyTexture(temp);

	tempStr = "Number of treasure: ";
	tempStr += std::to_string(treas_counter);
	tempDst.y += 92;
	text_surface = TTF_RenderText_Solid(RMANAGER->GetFont("./graphics/PixelFJVerdana12pt.ttf", 8), tempStr.c_str(), color);
	temp = SDL_CreateTextureFromSurface(RENDERER, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_QueryTexture(temp, NULL, NULL, &tempDst.w, &tempDst.h);
	SDL_RenderCopy(RENDERER, temp, NULL, &tempDst);
	SDL_DestroyTexture(temp);

	tempStr = "Active pathfinder: ";
	for each (GameObject* var in GameObject::SceneGraph)
	{
		Enemy* temp;
		if (var->Tag == "Enemy")
			temp = dynamic_cast<Enemy*>(var);
		else continue;

		if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::ASTAR)
		{
			tempStr += "AStar";
		}
		else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::DEPTH_FIRST)
		{
			tempStr += "Depthfirst";
		}
		else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::BREADTH_FIRST)
		{
			tempStr += "Breadthfirst";
		}
		else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::BEST_FIRST)
		{
			tempStr += "Bestfirst";
		}
		else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::ASTAR_EPSILON)
		{
			tempStr += "AstarEpsilon";
		}
		else if (temp->GetBrain()->defaultPathfinder == StateManager::ALGORITHMS::DJ_KASTRAS)
		{
			tempStr += "Dijkstras";
		}
		break;
	}
	tempDst.y += 32;
	text_surface = TTF_RenderText_Solid(RMANAGER->GetFont("./graphics/PixelFJVerdana12pt.ttf", 8), tempStr.c_str(), color);
	temp = SDL_CreateTextureFromSurface(RENDERER, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_QueryTexture(temp, NULL, NULL, &tempDst.w, &tempDst.h);
	SDL_RenderCopy(RENDERER, temp, NULL, &tempDst);
	SDL_DestroyTexture(temp);

	for each (Enemy* var in *this->m_activeLevel->scene_graph_ptr)
	{
		if (var->Tag != "Enemy") continue;
		
		std::vector<NODE>* temp = var->brain->GetPath();
		for (size_t i = var->brain->GetPathPos(); i < (temp->size()-1); i++)
		{
			SDL_RenderDrawLine(
				Atla::RENDERER,
				(int(8 * Atla::zoomFactor) + int((int(16 * Atla::zoomFactor) * temp->at(i).pos.x)) - (int)GetViewport()->pos.x),
				(int(8 * Atla::zoomFactor) + int((int(16 * Atla::zoomFactor) * temp->at(i).pos.y)) - (int)GetViewport()->pos.y),
				(int(8 * Atla::zoomFactor) + int((int(16 * Atla::zoomFactor) * temp->at(i+1).pos.x)) - (int)GetViewport()->pos.x),
				(int(8 * Atla::zoomFactor) + int((int(16 * Atla::zoomFactor) * temp->at(i+1).pos.y)) - (int)GetViewport()->pos.y)
			);
		}
	}
}

void Atla::TranslateViewport(int _x, int _y)
{
	m_viewport->pos.x += _x;
	m_viewport->pos.y += _y;
}

Player* Atla::GetPlayer()
{
	return m_player;
}

glm::ivec2 Atla::GetViewportOffset()
{
	return m_viewport->pos;
}
Viewport* Atla::GetViewport()
{
	return m_viewport;
}

void Atla::ForceUIUpdate()
{
	m_UI->ForceUpdateOfQueue();
}

UIManager* Atla::GetUI()
{
	return m_UI;
}

void Atla::DeleteLevel()
{
	delete m_activeLevel;
}

void Atla::CreateNewLevel(uint8_t _num_of_rooms)
{
	//Add rooms and print
	m_activeLevel->CreateNodeMap();
	DungeonGenerator::PopulateWithRooms(m_activeLevel, _num_of_rooms);
	// Connect the rooms with pathfinder and print
	DungeonGenerator::ConnectRooms(m_activeLevel);
	m_activeLevel->UpdateNodeMap();
	DungeonGenerator::GenerateTexture(RENDERER, m_activeLevel);
	this->m_player->parent_level = m_activeLevel;
	this->m_player->pos = m_activeLevel->rooms.back().pos;
	GameObject* entrance = new GameObject(m_activeLevel->rooms.back().pos, m_activeLevel, "Entrance");
	entrance->SetActiveSprite(RMANAGER->GetTexture("./graphics/stairs_entrance.png", RENDERER));
}

void Atla::UpdateAI()
{
	for (size_t i = 0; i < GameObject::SceneGraph.size(); i++)
	{
		Enemy* current = (Enemy*)GameObject::SceneGraph.at(i);
		if (current->Tag == "Enemy")
		{
			current->GetBrain()->Update();
			if (current == nullptr || (unsigned)current->brain == 0xdddddddd)
			{
				i--;
			}
		}
	}
}

void Atla::PopulateWithEnemies(uint8_t _num_of_enemies, StateManager::ALGORITHMS _algo, SDL_Texture* _spritesheet)
{
	for (size_t i = 0; i < _num_of_enemies; i++)
	{
		Enemy* enemy = new Enemy(m_activeLevel->rooms.at(i).pos.x, m_activeLevel->rooms.at(i).pos.y, Enemy::STATES::PATROL);
		enemy->parent_level = m_activeLevel;
		if (_spritesheet != nullptr)
			enemy->SetActiveSprite(_spritesheet);
		enemy->brain->defaultPathfinder = _algo;
		enemy->brain->SwitchPathAlgorithm(enemy->brain->defaultPathfinder);
		enemy->brain->DoPath(enemy->pos, enemy->parent_level->rooms.at(rand() % enemy->parent_level->rooms.size()).pos, enemy->parent_level->nmap, false);
	}
};

Viewport::Viewport(glm::ivec2 _size)
{
	this->pos.x = 0;
	this->pos.y = 0;
	this->size = _size;
}

void Viewport::Update(glm::ivec2 _target, Atla* _engine)
{
	//ENGINE->GetPlayer()->pos.x * (16 * 3),
	//	ENGINE->GetPlayer()->pos.y * (16 * 3)
	//glm::mix(0,0,0

	this->pos = Atla::interpolate(
		this->pos, 
		glm::vec2(
			(_target.x * 16 * Atla::zoomFactor) - (int(this->size.x * 0.5f) - 8), 
			(_target.y * 16 * Atla::zoomFactor) - (int(this->size.y * 0.5f) - 8)
		),
		0.005f
	);

	//this->pos.x = (_target.x * int(16 * Atla::zoomFactor));
	//this->pos.y = (_target.y * int(16 * Atla::zoomFactor));
	
}

void Atla::SetViewportSize(glm::ivec2 _size)
{
	m_viewport->size = _size;
}

void Atla::SetPlayer(Player* _player)
{
	m_player = _player;
}

Atla::Atla(const char* _title, int _xsize, int _ysize, SDL_WindowFlags _windowflags, const char* _rendererAPI)
{
	// Init all of SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	// Set the API to what we want
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, _rendererAPI);

	// Init the window + renderer
	WINDOW = SDL_CreateWindow(_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _xsize, _ysize, _windowflags);//SDL_WINDOW_RESIZABLE);
	RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(RENDERER, SDL_BLENDMODE_BLEND);

	// Init the mixer + font libs
	TTF_Init();
	Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	{
		printf("SDL_mixer failed to init! Error: %s\n", Mix_GetError());
	};
	Mix_Volume(-1, int(MIX_MAX_VOLUME * 0.3f));
	Mix_VolumeMusic(int(MIX_MAX_VOLUME * 0.05f));

	// These used to set anisotropic filtering + upscaling, but we render at dynamic resolutions now so it's not needed
		//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
		//SDL_RenderSetLogicalSize(RENDERER, _xsize, _ysize);

	// Init components
	m_viewport = new Viewport(glm::ivec2(_xsize, _ysize));
	RMANAGER = new ResourceManager();
	m_player = new Player(m_activeLevel);
	m_player->SetActiveSprite(Atla::RMANAGER->GetTexture("./graphics/player.png", Atla::RENDERER));
	m_UI = new UIManager(this);
	m_UI->AddInterface(new HealthDisplay(m_UI, RMANAGER->GetTexture("./graphics/hearts.png", RENDERER)));
	m_UI->AddInterface(new ScoreDisplay(m_UI, RMANAGER->GetTexture("./graphics/objects.png", RENDERER)));
	INPUTMANAGER = new InputManager();
	debug = new DEBUG(this);
}

Atla::~Atla()
{
	delete m_player;
	delete m_activeLevel;
	delete m_viewport;
	delete RMANAGER;
	delete m_UI;
	delete INPUTMANAGER;
	delete debug;
	while (GameObject::SceneGraph.size() > 0)
	{
		delete GameObject::SceneGraph.at(0);
	}
	SDL_DestroyRenderer(RENDERER);
	SDL_DestroyWindow(WINDOW);
	SDL_Quit();
	TTF_Quit();
	Mix_Quit();
}

glm::vec2 Atla::interpolate(const glm::vec2 &start, const glm::vec2 &end, float alpha)
{
	alpha *= Atla::DeltaTime;
	return (start + alpha*(end - start));
}

float Atla::interpolate(const float &start, const float &end, float alpha)
{
	alpha *= Atla::DeltaTime;
	return (start + alpha*(end - start));
}