#include "Debug.h"
#include "Atla.h"
#include "Level\Level.h"
#include "GameObject\GameObject.h"
#include "GameObject\NPC\Enemy\Enemy.h"
#include "../ModularPathFinding/node.h"

void DEBUG::DrawAIGrid(Atla* ENGINE, SDL_Renderer* RENDERER)
{
	//if (RENDERER == NULL) RENDERER = ENGINE->RENDERER;
	const int squareSize = 8;

	SDL_SetRenderDrawColor(RENDERER, 111, 111, 111, 255);
	for (size_t y = 0; y < ENGINE->GetLevel()->map.size(); y++)
	{
		for (size_t x = 0; x < ENGINE->GetLevel()->map.at(y).size(); x++)
		{
			if (ENGINE->GetLevel()->map.at(y).at(x) == 1) continue;
			SDL_Rect temp = {
				0 + (squareSize * int(x)),
				0 + (squareSize * int(y)),
				squareSize,
				squareSize
			};

			SDL_RenderDrawRect(RENDERER, &temp);
		}
	}

	for each (GameObject* var in GameObject::SceneGraph)
	{
		SDL_Rect temp = {
			0 + (squareSize * var->pos.x),
			0 + (squareSize * var->pos.y),
			squareSize,
			squareSize
		};

		if (var->Tag == "Player")
		{
			SDL_SetRenderDrawColor(RENDERER, 0, 255, 0, 255);
		}
		else if (var->Tag == "Enemy")
		{
			SDL_SetRenderDrawColor(RENDERER, 255, 0, 0, 255);
		}
		else if (var->Tag == "Treasure")
		{
			SDL_SetRenderDrawColor(RENDERER, 0, 0, 255, 255);
		}
		else if (var->Tag == "Exit")
		{
			SDL_SetRenderDrawColor(RENDERER, 255, 0, 255, 255);
		}
		else
		{
			continue;
		}

		SDL_RenderFillRect(RENDERER, &temp);
	}

	SDL_SetRenderDrawColor(RENDERER, 255, 0, 0, 5);
	for each (GameObject* obj in GameObject::SceneGraph)
	{
		if (obj->Tag != "Enemy") continue;
		
		Enemy* var = dynamic_cast<Enemy*>(obj);

		std::vector<NODE>* temp = var->brain->GetPath();
		for (size_t i = var->brain->GetPathPos(); i < (temp->size() - 1); i++)
		{
			//	0 + (squareSize * var->pos.x),
			//	0 + (squareSize * var->pos.y)
			SDL_RenderDrawLine(
				RENDERER,
				(0 + (squareSize * temp->at(i).pos.x)) + (squareSize / 2),
				(0 + (squareSize * temp->at(i).pos.y)) + (squareSize / 2),
				(0 + (squareSize * temp->at(i+1).pos.x)) + (squareSize / 2),
				(0 + (squareSize * temp->at(i+1).pos.y)) + (squareSize / 2)
			);
		}
	}
}