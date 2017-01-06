#pragma once

#include "../AtlaEngine/GameObject/GameObject.h"
#include "../SDL2/SDL.h"

class FiniteAnimation : public GameObject
{
	protected:
	private:
		SDL_Rect frame;
		size_t max_frames;
		float counter;
		float speed;
	public:

		FiniteAnimation(glm::ivec2 _pos, SDL_Texture* _spritesheet, glm::ivec2 _frame_size, size_t _max_frames, Level* _level, float _speed = 3.0f)
		{
			frame.x = 0;
			frame.y = 0;
			frame.w = _frame_size.x;
			frame.h = _frame_size.y;
			max_frames = _max_frames;
			counter = 0.0f;
			this->pos = _pos;
			this->draw_pos = _pos;
			this->Layer = 1;
			this->Tag = "Explosion";
			this->parent_level = _level;
			this->active_sprite = _spritesheet;
			speed = _speed;
		}

		void Draw()
		{
			SDL_Rect temprectsrc = {
				96 * (int)std::floor(this->counter),
				0,
				96,
				96
			};
			SDL_Rect temprect = {
				int((int(16 * Atla::zoomFactor) * this->draw_pos.x) - (int)this->parent_level->engine->GetViewport()->pos.x) - 96,
				int((int(16 * Atla::zoomFactor) * this->draw_pos.y) - (int)this->parent_level->engine->GetViewport()->pos.y) - 96,
				int(96 * Atla::zoomFactor),
				int(96 * Atla::zoomFactor)
			};
			SDL_RenderCopy(this->parent_level->engine->RENDERER, this->GetActiveSprite(), &temprectsrc, &temprect);
		}

		void Update()
		{
			if (counter < max_frames)
			{
				counter += ((float)this->parent_level->engine->GetDT() * 0.01f) * speed;
			}
			else
			{
				delete this;
				return;
			}
		};
};