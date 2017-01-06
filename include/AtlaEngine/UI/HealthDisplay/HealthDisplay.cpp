#include "HealthDisplay.h"
#include "../../GameObject/Player/Player.h"
#include "../../Atla.h"

HealthDisplay::HealthDisplay(UIManager* _parent, SDL_Texture* _spritesheet)
{
	m_parent = _parent;
	m_spritesheet = _spritesheet;
	activeTexture = SDL_CreateTexture(m_parent->GetEngine()->RENDERER, NULL, SDL_TEXTUREACCESS_TARGET, 48, 16);
	SDL_SetTextureBlendMode(activeTexture, SDL_BLENDMODE_BLEND);
	dimensions = {
		int(_parent->GetEngine()->GetViewport()->size.x / 2) - ((48 * 4) / 2),
		int(_parent->GetEngine()->GetViewport()->size.y * 0.044444444444f),		 // imgsize / screen res = scaling ratio
		int(_parent->GetEngine()->GetViewport()->size.x * 0.15f),				// 192 / 1280 == 0.15f
		int(_parent->GetEngine()->GetViewport()->size.y * 0.0888888888888889f) // 64 / 720 == 0.08888f
	};
	m_player = &m_parent->GetEngine()->m_player;
	m_lives = (*m_player)->lives;
	this->Update();
}

void HealthDisplay::Draw()
{
	if (m_lives != (*m_player)->lives)
	{
		Update();
	}

	dimensions = {
		int(int(m_parent->GetEngine()->GetViewport()->size.x * 0.5f) - (dimensions.w * 0.5f)),
		int(dimensions.h - (dimensions.h * 0.5f)),
		48 * 4,
		16 * 4,
	};

	SDL_RenderCopy(m_parent->GetEngine()->RENDERER, activeTexture, NULL, &dimensions);
}

void HealthDisplay::Update()
{
	m_lives = (*m_player)->lives;
	SDL_Renderer* rend = m_parent->GetEngine()->RENDERER;
	SDL_SetRenderTarget(rend, activeTexture);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
	SDL_RenderClear(rend);

	for (auto i = (m_lives - m_lives); i < m_lives; i++)
	{
		SDL_Rect tempSrc =
		{
			0,
			0,
			16,
			16
		};
		SDL_Rect tempDst =
		{
			16 * i,
			0,
			16,
			16
		};
		SDL_RenderCopy(rend, m_spritesheet, &tempSrc, &tempDst);
	}

	if (m_lives < 3)
	{
		int16_t lifeDif = (3 - m_lives);
		for (auto i = (lifeDif - lifeDif); i < lifeDif; i++)
		{
			SDL_Rect tempSrc =
			{
				16,
				0,
				16,
				16
			};
			SDL_Rect tempDst =
			{
				32 - ((16) * i),
				0,
				16,
				16
			};
			SDL_RenderCopy(rend, m_spritesheet, &tempSrc, &tempDst);
		}
	}

	SDL_SetRenderTarget(rend, NULL);
}