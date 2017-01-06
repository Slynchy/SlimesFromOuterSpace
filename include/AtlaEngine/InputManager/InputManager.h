#pragma once

#include "../../SDL2/SDL.h"
#include "../../glm/glm.hpp"

class Atla;

class InputManager
{
	protected:
	private:
	public:

		glm::ivec2 mousePos;
		bool LMB_PRESSED;

		/// Public because a getter is just a waste of a cycle
		const Uint8* SDL_KEYBOARDSTATE = NULL;

		void Update(SDL_Event &_ev);
		
		void UpdateMouseInput(SDL_Event &_ev);

		InputManager()
		{
			SDL_KEYBOARDSTATE = SDL_GetKeyboardState(NULL);
		}
};