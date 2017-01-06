#pragma once

#include "../SDL2/SDL.h"

// Forward declare
class Atla;

/// Debug class, holding info for the debug window/renderer
class DEBUG
{
	protected:
		/// Draws the AI grid + paths
		void DrawAIGrid(Atla* ENGINE, SDL_Renderer* RENDERER);
	private:

		/// Pointer to engine
		Atla* ENGINE;
		/// is the debug mode enabled?
		bool isEnabled = false;
	public:
		SDL_Window* WINDOW;
		SDL_Renderer* RENDERER;

		/// Toggle the debug mode/window
		void Toggle()
		{
			isEnabled = !isEnabled;

			if(isEnabled == false) SDL_HideWindow(WINDOW);
			else SDL_ShowWindow(WINDOW);
		}

		/// Is it enabled?
		bool Enabled()
		{
			return isEnabled;
		}

		/// Update the grid with newest info if enabled
		void Update()
		{
			if (isEnabled == true)
			{
				SDL_ShowWindow(WINDOW);

				SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 255);
				SDL_RenderClear(RENDERER);

				DrawAIGrid(ENGINE, RENDERER);

				SDL_RenderPresent(RENDERER);
			}
		}

		DEBUG(Atla* _eng)
		{
			ENGINE = _eng;
			WINDOW = SDL_CreateWindow("AI Renderer", 48, 48, 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
			RENDERER = SDL_CreateRenderer(WINDOW, -1, NULL);
			SDL_RenderSetLogicalSize(RENDERER, 640, 480);
			SDL_HideWindow(WINDOW);
		}

		~DEBUG()
		{
			SDL_DestroyRenderer(RENDERER);
			SDL_DestroyWindow(WINDOW);
		}
};