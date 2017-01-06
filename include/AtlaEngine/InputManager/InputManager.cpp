#include "InputManager.h"

void InputManager::Update(SDL_Event &_ev)
{
	if (_ev.key.repeat == 0)
	{
		SDL_KEYBOARDSTATE = SDL_GetKeyboardState(NULL);
	};
}

void InputManager::UpdateMouseInput(SDL_Event &_ev)
{
	Uint32 State = SDL_GetMouseState(&mousePos.x, &mousePos.y);

	if (State & SDL_BUTTON_LEFT)
	{
		LMB_PRESSED = true;
	}
	else
	{
		LMB_PRESSED = false;
	}
}