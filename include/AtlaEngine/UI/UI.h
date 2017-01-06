#pragma once

#include <vector>
#include "../../SDL2/SDL.h"

class Player;
class Atla;
class UIManager;

/// Base class for each UI interface
/// i.e. one for HUD, one for gameover screen, etc.
class Interface
{
	protected:
		UIManager* m_parent;
		SDL_Texture* m_spritesheet;
		SDL_Texture* activeTexture;
	private:
	public:

		SDL_Rect dimensions;
		
		Interface()
		{
			dimensions = { 0,0,0,0 };
			m_parent = nullptr;
			m_spritesheet = nullptr;
			activeTexture = nullptr;
		}

		virtual void Draw()
		{

		}

		virtual void Update()
		{

		}
};

class UIManager
{
	protected:
	private:
		std::vector<Interface*> m_queue;
		Atla* engine;
	public:

		Atla* GetEngine()
		{
			return engine;
		}

		void ForceUpdateOfQueue()
		{
			for each (Interface* var in m_queue)
			{
				var->Update();
			}
		}

		void AddInterface(Interface* _intrfc)
		{
			if (_intrfc == nullptr) return;
			m_queue.push_back(_intrfc);
		}

		UIManager(Atla* _engineptr);
		~UIManager();
		void Draw();
};