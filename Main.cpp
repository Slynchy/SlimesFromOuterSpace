/// \file 		Main.cpp
/// \author 	Sam Lynch
/// \brief 		Entry point to game + game logic
/// \date 		October 2016

/// The major version number
#define VERSION_MAJOR 0
/// The minor version number
#define VERSION_MINOR 8
/// The patch version number
#define VERSION_PATCH 3

/// The target FPS, used in the framerate throttling
#define TARGET_FPS 60

#include "include/ModularPathFinding/Node.h"
#include "include/DungeonGenerator/DungeonGenerator.h"
#include "./include/SDL2/SDL.h"
#include "./include/SDL2/SDL_image.h"
#include "include/AtlaEngine/GameObject/GameObject.h"
#include <random>
#include <chrono>
#include "include/AtlaEngine/GameObject/Player/Player.h"
#include "include/AtlaEngine/Atla.h"
#include "include/AtlaEngine/UI/UI.h"

/// Enumerator of possible gamestates
enum GAMESTATES
{
	MAIN_MENU,
	INGAME,
	GAMEOVER,
	GAMEOVER_TIMING,
	GAMEWIN,
	GAMEWIN_TIMING
};

/// Resets the active level and creates a new one
void ResetLevel(Atla* ENGINE, GameObject* &exitObject);

/// Umbrella function for the following "drawXmenu" functions
void DrawMenu(GAMESTATES _gamestate, Atla* ENGINE, SDL_Rect &mainMenuDstRect, double &angleOfImage);

/// Draws the main menu screen
void DrawMainMenu(Atla* ENGINE, SDL_Rect &mainMenuDstRect, double &angleOfImage);

/// Draws the game over screen
void DrawGameOver(Atla* ENGINE, SDL_Rect &mainMenuDstRect, double &angleOfImage);

/// Draws the game win screen
void DrawGameWin(Atla* ENGINE, SDL_Rect &mainMenuDstRect, double &angleOfImage);

// Draws the AI grid/positions to the specified renderer
//void DrawAIGrid(Atla* ENGINE, SDL_Renderer* RENDERER = NULL);

// We need to undef main because SDL does weird voodoo by hijacking the main function
#undef main
int main()
{
	// Seeding random!
	srand((unsigned int)time(NULL));

	// Instantiate the Atla engine with some SDL_Window params
	Atla* ENGINE = new Atla(
		std::string("Slimes from Outer Space! - v" + std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR) + "." + std::to_string(VERSION_PATCH)).c_str(), 
		1280, 
		800, 
		SDL_WINDOW_RESIZABLE
	);
	ENGINE->SetActiveMusic(ENGINE->RMANAGER->GetMUS("./sfx/bgm.mp3"));

	// -------------- Game Logic variables! --------------
		GAMESTATES GAMESTATE = MAIN_MENU;
		GameObject* exitObject = nullptr;
		uint16_t gameEndTimer = 0;

	// -------------- Main menu variables ! --------------
		double angleOfImage = 0.0;
		SDL_Rect mainMenuDstRect;


	// Main game loop
	bool exit = false;
	while (exit == false)
	{
		// Update the last frame time for delta time calc later
		ENGINE->UpdateLastTime();

		// Iterate through the buffer of SDL events and handle them accordingly
		while (SDL_PollEvent(&ENGINE->EVENTS))
		{
			switch (ENGINE->EVENTS.type)
			{
				case SDL_QUIT:
					exit = true;
					break;
				case SDL_KEYUP:
				case SDL_KEYDOWN:
					if (ENGINE->EVENTS.key.keysym.scancode == SDL_SCANCODE_ESCAPE) exit = true;
					ENGINE->INPUTMANAGER->Update(ENGINE->EVENTS);
					break;
				case SDL_WINDOWEVENT:
					switch (ENGINE->EVENTS.window.event)
					{
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						case SDL_WINDOWEVENT_RESIZED:
							glm::ivec2 tempVec2;
							SDL_GetWindowSize(ENGINE->WINDOW, &tempVec2.x, &tempVec2.y);
							ENGINE->SetViewportSize(tempVec2);

							// because Render Targets are weird in SDL2, we need to redraw them when the screen is resized
							// But only if ingame because mainmenu hasn't created the level yet!
							if (GAMESTATE == INGAME)
							{
								ENGINE->ForceUIUpdate();
								DungeonGenerator::GenerateTexture(ENGINE->RENDERER, ENGINE->GetLevel());
							};
							break;
					}
					break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEMOTION:
					// This currently is unused
					ENGINE->INPUTMANAGER->UpdateMouseInput(ENGINE->EVENTS);
					break;
			}
		}

		// Clear the screen
		SDL_SetRenderDrawColor(ENGINE->RENDERER, 0, 0, 0, 255);
		SDL_RenderClear(ENGINE->RENDERER);

		// Switch the gamestate and do apropos action
		switch (GAMESTATE)
		{
			case MAIN_MENU:
			case GAMEWIN:
			case GAMEOVER:
				DrawMenu(GAMESTATE, ENGINE, mainMenuDstRect, angleOfImage);
				if (ENGINE->INPUTMANAGER->SDL_KEYBOARDSTATE[SDL_SCANCODE_SPACE])
				{
					if (GAMESTATE != GAMEWIN)
					{
						ResetLevel(ENGINE, exitObject);
						GAMESTATE = INGAME;
					}
					else exit = true;
				}
				break;
			case GAMEWIN_TIMING:
			case GAMEOVER_TIMING:
			case INGAME:

				ENGINE->debug->Update();

				if (GAMESTATE != GAMEWIN_TIMING)
				{
					ENGINE->GetPlayer()->Update();

					for (size_t i = 0; i < GameObject::SceneGraph.size(); i++)
					{
						GameObject* var = GameObject::SceneGraph.at(i);
						if (var->Tag != "Enemy" && var->Tag != "Player")
						{
							var->Update();
						}
						if (var == nullptr)
						{
							// Because the destructor removes itself from the scene graph, we need to realign the loop
							i--;
						}
					}
				};

				// -------------- GameLogic! --------------
				if (ENGINE->GetPlayer()->pos == exitObject->pos)
					GAMESTATE = GAMEWIN_TIMING; // We want the game win before game loss so the player loses if he dies in the same move as winning!
				if (ENGINE->GetPlayer()->lives <= 0 && GAMESTATE != GAMEOVER_TIMING)
				{
					GAMESTATE = GAMEOVER_TIMING;
					ENGINE->GetPlayer()->Hide();
				}
				// ------------ end gamelogic -------------

				if (GAMESTATE == GAMEWIN_TIMING || GAMESTATE == GAMEOVER_TIMING)
				{
					gameEndTimer += ENGINE->DeltaTime;
					if (gameEndTimer > 1200) // 1.2 seconds
					{
						if(GAMESTATE == GAMEWIN_TIMING) GAMESTATE = GAMEWIN;
						else if (GAMESTATE == GAMEOVER_TIMING) GAMESTATE = GAMEOVER;
					}
				}


				ENGINE->GetViewport()->Update(ENGINE->GetPlayer()->pos, ENGINE);
				ENGINE->DrawEverything();
				break;
			default:
				break;

		};

		// Flip buffer to screen
		SDL_RenderPresent(ENGINE->RENDERER);

		// Calculate the new delta time and delay the FPS by the required amount to throttle to 60
		ENGINE->CalculateDT();
		long int delay = Uint32(std::round(1000 / TARGET_FPS)) - ENGINE->DeltaTime;
		if (delay > 0)
		{
			SDL_Delay(delay);
			Atla::DeltaTime += delay;
		}
	}

	// Oh dear, lots of GC to do...
	delete ENGINE;
	// oh wait, no there isn't!

	return 0;
}

void ResetLevel(Atla* ENGINE, GameObject* &exitObject)
{
	// Delete the current level
	ENGINE->DeleteLevel();

	// Set the new one
	ENGINE->SetLevel(new Level(glm::ivec2(75, 50), ENGINE));

	// Delete everything in the scene graph
	while (GameObject::SceneGraph.size() > 0)
	{
		delete GameObject::SceneGraph.front();
	}
	exitObject = nullptr;

	// Instantiate the new player
	ENGINE->SetPlayer(new Player(ENGINE->GetLevel()));
	ENGINE->GetPlayer()->SetActiveSprite(ENGINE->RMANAGER->GetTexture("./graphics/player.png", ENGINE->RENDERER));

	// Actually create the new level and populate it 
	ENGINE->CreateNewLevel(11);
	ENGINE->PopulateWithEnemies(5, StateManager::ASTAR_EPSILON, ENGINE->RMANAGER->GetTexture("./graphics/redslime.png", ENGINE->RENDERER));

	// Create pointer to the exit
	for each (GameObject* var in GameObject::SceneGraph)
	{
		if (var->Tag == "Exit")
		{
			exitObject = var;
		}
	}
	if (exitObject == nullptr) printf("Failed to find Exit in SceneGraph; expect crash!\n");

	// Force a UI update just in case
	ENGINE->ForceUIUpdate();
};

void DrawMenu(GAMESTATES _gamestate, Atla* ENGINE, SDL_Rect &mainMenuDstRect, double &angleOfImage)
{
	switch (_gamestate)
	{
		case MAIN_MENU:
			DrawMainMenu(ENGINE, mainMenuDstRect, angleOfImage);
			break;
		case GAMEWIN:
			DrawGameWin(ENGINE, mainMenuDstRect, angleOfImage);
			break;
		case GAMEOVER:
			DrawGameOver(ENGINE, mainMenuDstRect, angleOfImage);
			break;
	}
}

void DrawGameWin(Atla* ENGINE, SDL_Rect &mainMenuDstRect, double &angleOfImage)
{
	// Background
	if (angleOfImage > 360.0) angleOfImage -= 360.0;
	mainMenuDstRect = { -1024, -1024, 4096 , 3040 };
	SDL_RenderCopyEx(ENGINE->RENDERER, ENGINE->RMANAGER->GetTexture("./graphics/backdrop.png", ENGINE->RENDERER), NULL, &mainMenuDstRect, (angleOfImage += 0.0025 * (double)ENGINE->DeltaTime), NULL, SDL_RendererFlip::SDL_FLIP_NONE);

	// Treasure icon
	const int multiplier = 6;
	mainMenuDstRect = { int((ENGINE->GetViewport()->size.x * 0.5f) - (8 * multiplier)), int(ENGINE->GetViewport()->size.y * 0.33f), 16 * multiplier, 16 * multiplier };
	SDL_Rect gameOverSrcRect = { 0,0,16,16 };
	SDL_RenderCopy(ENGINE->RENDERER, ENGINE->RMANAGER->GetTexture("./graphics/objects.png"), &gameOverSrcRect, &mainMenuDstRect);

	// Text; drawing every frame is a really dirty way of doing it, but since we're not doing anything else it shouldn't impact performance noticably
	SDL_Color color = { 255,255,255 };
	SDL_Surface *text_surface = TTF_RenderText_Solid(
		ENGINE->RMANAGER->GetFont("./graphics/PixelFJVerdana12pt.ttf", 8),
		std::to_string(ENGINE->GetPlayer()->GetCollectedTreasureAmount()).c_str(),
		color
	);
	SDL_Texture* temp = SDL_CreateTextureFromSurface(ENGINE->RENDERER, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_QueryTexture(temp, NULL, NULL, &mainMenuDstRect.w, &mainMenuDstRect.h);
	mainMenuDstRect.x = int((ENGINE->GetViewport()->size.x * 0.5f) - (mainMenuDstRect.w * 0.5f)) - 16;
	mainMenuDstRect.y += ((16 * multiplier) + 0);
	mainMenuDstRect.w *= 6;
	mainMenuDstRect.h *= 6;
	SDL_RenderCopy(ENGINE->RENDERER, temp, NULL, &mainMenuDstRect);
	SDL_DestroyTexture(temp);

	mainMenuDstRect.x = int((ENGINE->GetViewport()->size.x / 2) - (960 / 2));
	mainMenuDstRect.y = int((ENGINE->GetViewport()->size.y / 3) - (544 / 3));
	mainMenuDstRect.w = 960;
	mainMenuDstRect.h = 544;
	SDL_RenderCopy(ENGINE->RENDERER, ENGINE->RMANAGER->GetTexture("./graphics/gamewin.png", ENGINE->RENDERER), NULL, &mainMenuDstRect);
}

void DrawGameOver(Atla* ENGINE, SDL_Rect &mainMenuDstRect, double &angleOfImage)
{
	// Draw background
	if (angleOfImage > 360.0) angleOfImage -= 360.0;
	mainMenuDstRect = { -1024, -1024, 4096 , 3040 };
	SDL_RenderCopyEx(ENGINE->RENDERER, ENGINE->RMANAGER->GetTexture("./graphics/backdrop.png", ENGINE->RENDERER), NULL, &mainMenuDstRect, (angleOfImage += 0.0025 * (double)ENGINE->DeltaTime), NULL, SDL_RendererFlip::SDL_FLIP_NONE);

	// Draw the gameover logo
	mainMenuDstRect.x = int((ENGINE->GetViewport()->size.x / 2) - (960 / 2));
	mainMenuDstRect.y = int((ENGINE->GetViewport()->size.y / 3) - (544 / 3));
	mainMenuDstRect.w = 960;
	mainMenuDstRect.h = 544;
	SDL_RenderCopy(ENGINE->RENDERER, ENGINE->RMANAGER->GetTexture("./graphics/gameover.png", ENGINE->RENDERER), NULL, &mainMenuDstRect);
}

void DrawMainMenu(Atla* ENGINE, SDL_Rect &mainMenuDstRect, double &angleOfImage)
{
	// If the angle is past 360, just minus 360 so it doesn't get too large
	if (angleOfImage > 360.0) angleOfImage -= 360.0;

	// Draw the main menu background image
	mainMenuDstRect = { -1024, -1024, 4096 , 3040 };
	SDL_RenderCopyEx(ENGINE->RENDERER, ENGINE->RMANAGER->GetTexture("./graphics/backdrop.png", ENGINE->RENDERER), NULL, &mainMenuDstRect, (angleOfImage += 0.0025 * (double)ENGINE->DeltaTime), NULL, SDL_RendererFlip::SDL_FLIP_NONE);

	// Recycle the SDL_Rect to draw the game logo
	mainMenuDstRect.x = int((ENGINE->GetViewport()->size.x / 2) - (960 / 2));
	mainMenuDstRect.y = int((ENGINE->GetViewport()->size.y / 3) - (544 / 3));
	mainMenuDstRect.w = 960;
	mainMenuDstRect.h = 800;
	SDL_RenderCopy(ENGINE->RENDERER, ENGINE->RMANAGER->GetTexture("./graphics/logo.png", ENGINE->RENDERER), NULL, &mainMenuDstRect);
}