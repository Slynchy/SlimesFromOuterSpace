/// \file 		Atla.h
/// \author 	Sam Lynch
/// \brief 		Header file for Atla engine
/// \version 	0.7.6
/// \date 		November 2016
/// \details	The header file for declaring the engine and its functions/variables.

#pragma once

#include <string>
#include <chrono>

#include "../SDL2/SDL.h"
#include "../SDL2/SDL_image.h"
#include "AI/State.h"
#include "./ResourceManager.h"
#include "./InputManager/InputManager.h"
#include "Debug.h"

// Forward declarations
class Level;
class Enemy;
class Player;
class UIManager;
class InputManager;
struct Viewport;

/// Defines the default/constant volume
#define VOLUME 0.1f

/// \class 		Atla Atla.h "include/AtlaEngine/Atla.h"
/// \brief 		The main class of Atla
/// \details 	This class is what connects most functions/variables/components for basic operation (SDL window/renderer, input manager, etc.)
class Atla // <-- Dark Cloud fans should get this reference
{
protected:
private:

	/// The active level being currently played
	Level* m_activeLevel;

	Viewport* m_viewport;

	/// The timestamp of the last frame, to calculate delta
	Uint32 lastTime;

	/// The UI manager which handles HUD and interface
	UIManager* m_UI;

	/// Current music track
	Music* m_active_music;

public:

	/// Object that holds debug information/objects
	DEBUG* debug;

	/// Sets the current music track and plays it
	/// \param _mus The Music object to play
	void SetActiveMusic(Music* _mus)
	{
		if (_mus == nullptr || _mus->m_music == nullptr) return;
		if (m_active_music != nullptr || m_active_music != NULL)
		{
			Mix_HaltMusic();
		}
		m_active_music = _mus;
		Mix_PlayMusic(m_active_music->m_music, -1);
	}

	/// Plays the SFX specified as soon as possible
	/// \param _sound The Sound object to play
	void PlaySFX(Sound* _sound)
	{
		Mix_PlayChannel(-1, _sound->m_sound, 0);
	}

	/// Plays the SFX specified as soon as possible
	/// \param _sound The Mix_Chunk pointer to play
	void PlaySFX(Mix_Chunk* _sound)
	{
		Mix_PlayChannel(-1, _sound, 0);
	}

	/// Pointer to the player, to connect inputmanager to
	Player* m_player;

	/// SDL Window variable
	SDL_Window* WINDOW;

	/// SDL Renderer variable
	/// \details These are public because they are used widely throughout the engine; using a getter is just a waste of a CPU cycle
	SDL_Renderer* RENDERER;

	/// SDL Event variable
	SDL_Event EVENTS;

	/// Resource manager for instance of the engine; handles loading/deleting of assets
	ResourceManager* RMANAGER;

	/// Handles input information from SDL_Event (mouse/keyboard)
	InputManager* INPUTMANAGER;

	/// The scaling amount for sprites (for zooming)
	static float zoomFactor;

	/// The delta time...
	static Uint32 DeltaTime;

	/// \details Interpolates two points based on an alpha, not my own function
	/// \author http://www.gamedev.net/topic/624285-smooth-rotation-and-movement-of-camera/
	static glm::vec2 Atla::interpolate(const glm::vec2 &start, const glm::vec2 &end, float alpha);
	static float Atla::interpolate(const float &start, const float &end, float alpha);

	/// Gets the offset of the viewport from the origin
	///
	/// \return The viewport offset as an integerised vector
	glm::ivec2 Atla::GetViewportOffset();

	/// Translates the viewport
	///
	/// \param _x X translation
	/// \param _y Y translation
	void Atla::TranslateViewport(int _x, int _y);

	/// Gets pointer to the engine viewport
	///
	/// \return Pointer to engine viewport
	Viewport* Atla::GetViewport();

	/// Sets the viewport size (i.e. the resolution); mainly for UI
	///
	/// \param _size Integerised vector of desired size
	void Atla::SetViewportSize(glm::ivec2 _size);

	/// Gets the delta time
	/// \deprecated No longer required as DeltaTime is public
	/// \return Delta time as integer
	Uint32 Atla::GetDT(); 

	/// Calculates delta time based on last frame time
	void Atla::CalculateDT();

	/// Updates the timestamp used for DT calculation
	void Atla::UpdateLastTime();

	/// Draws AI paths and other debug information
	void Atla::Debug_Draw();

	/// Toggles the debug mode variable
	/// \deprecated Just access debug and toggle manually
	void TOGGLE_DEBUG_MODE()
	{
		debug->Toggle();
	}

	/// Are we in debug mode?
	/// \return Boolean of debug mode
	/// \deprecated debug object is public, use that
	bool IS_DEBUG_MODE()
	{
		return debug->Enabled();
	}

	/// Gets the UI system from engine
	/// \return the active UI system
	UIManager* Atla::GetUI();

	/// Gets pointer to the active level
	/// \return Pointer to active level
	Level* Atla::GetLevel();

	/// Deletes the active level
	void Atla::DeleteLevel();

	/// Sets the player to the provided one
	void Atla::SetPlayer(Player* _player);

	/// Sets the active level to parameter
	/// \param _level New level object pointer
	void Atla::SetLevel(Level* _level);

	/// Automates the creation process of a level, using active level (more rooms -> longer load times)
	/// \param _num_of_rooms The desired number of rooms the level will have
	void Atla::CreateNewLevel(uint8_t _num_of_rooms);

	/// Gets the player object
	/// \return Pointer to player
	Player* Atla::GetPlayer();

	/// Populates the level with enemies of specified AI algorithm with specified spritesheet
	/// \param _num_of_enemies Number of enemies to create; must be less than room number (for now)!
	/// \param _algo The specified algorithm; refer to Pathalgo.h for possible options
	/// \param _spritesheet Gives enemies specified spritesheet
	void Atla::PopulateWithEnemies(uint8_t _num_of_enemies, StateManager::ALGORITHMS _algo = StateManager::ALGORITHMS::ASTAR, SDL_Texture* _spritesheet = nullptr);

	/// Updates all AI in scene graph
	void Atla::UpdateAI();

	/// Forces an update of UI states (fixes weird bugs when resizing window)
	void Atla::ForceUIUpdate();

	/// Automates the drawing process; draws level, UI and GameObjects
	void Atla::DrawEverything();

	/// Constructor for Atla
	/// \param _title Window title
	/// \param _xsize Resolution (X)
	/// \param _ysize Resolution (Y)
	/// \param _windowflags SDL window flags to pass to the SDL_ Window constructor
	/// \param _rendererAPI Optional; can specify render API (see SDL documentation for options)
	Atla(const char* _title, int _xsize, int _ysize, SDL_WindowFlags _windowflags, const char* _rendererAPI = "direct3d");

	~Atla();
};

/// Structure for Atla viewport
struct Viewport
{
	glm::vec2 pos;
	glm::vec2 size;
	void Viewport::Update(glm::ivec2 _target, Atla* _engine);
	Viewport(glm::ivec2 _size);
};