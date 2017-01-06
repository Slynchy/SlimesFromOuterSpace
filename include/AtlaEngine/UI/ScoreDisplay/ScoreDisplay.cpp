
#include "ScoreDisplay.h"

#include "../../GameObject/Player/Player.h"
#include "../../Atla.h"

#include "../../../SDL2/SDL_ttf.h"

ScoreDisplay::ScoreDisplay(UIManager* _parent, SDL_Texture* _spritesheet)
{
	m_parent = _parent;
	m_spritesheet = _spritesheet;
	activeTexture = SDL_CreateTexture(m_parent->GetEngine()->RENDERER, NULL, SDL_TEXTUREACCESS_TARGET, 48, 16);
	SDL_SetTextureBlendMode(activeTexture, SDL_BLENDMODE_BLEND);
	dimensions = {
		int(_parent->GetEngine()->GetViewport()->size.x * 0.0375f),
		int(_parent->GetEngine()->GetViewport()->size.y * 0.07f),
		48 * 4,
		16 * 4,
	};
	m_player = &m_parent->GetEngine()->m_player;
	m_collectedTreasure = (*m_player)->GetCollectedTreasureAmount();
	this->Update();
};

void ScoreDisplay::Draw()
{
	if (m_collectedTreasure != (*m_player)->GetCollectedTreasureAmount())
	{
		Update();
	}

	dimensions = {
		int(int(m_parent->GetEngine()->GetViewport()->size.x * 0.1f) - (dimensions.w * 0.5f)),
		int(dimensions.h - (dimensions.h * 0.5f)),
		48 * 4,
		16 * 4,
	};

	SDL_RenderCopy(m_parent->GetEngine()->RENDERER, activeTexture, NULL, &dimensions);
}

void ScoreDisplay::Update()
{
	m_collectedTreasure = (*m_player)->GetCollectedTreasureAmount();
	SDL_Renderer* rend = m_parent->GetEngine()->RENDERER;
	SDL_SetRenderTarget(rend, activeTexture);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
	SDL_RenderClear(rend);

	SDL_Rect tempSrc = { 0,0,16,16 };
	SDL_Rect tempDst = { 0,0,16,16 };
	SDL_RenderCopy(rend, m_spritesheet, &tempSrc, &tempDst);

	SDL_Color color = { 255,255,255 };
	SDL_Surface *text_surface = TTF_RenderText_Solid(m_parent->GetEngine()->RMANAGER->GetFont("./graphics/PixelFJVerdana12pt.ttf", 8), std::to_string(m_collectedTreasure).c_str(), color);
	SDL_Texture* temp = SDL_CreateTextureFromSurface(rend, text_surface);
	SDL_FreeSurface(text_surface);
	SDL_QueryTexture(temp, NULL, NULL, &tempSrc.w, &tempSrc.h);
	tempSrc.x = 18;
	tempSrc.y -= 2;
	SDL_RenderCopy(rend, temp, NULL, &tempSrc);
	SDL_DestroyTexture(temp);

	SDL_SetRenderTarget(rend, NULL);
}