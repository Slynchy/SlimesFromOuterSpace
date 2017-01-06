#pragma once

#include "../UI.h"

class ScoreDisplay : public Interface
{
protected:
private:
	Player** m_player;
	size_t m_collectedTreasure = 0;
public:

	ScoreDisplay(UIManager* _parent, SDL_Texture* _spritesheet);

	void Draw();

	void Update();
};