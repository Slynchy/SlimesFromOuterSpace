#pragma once

#include "../UI.h"

class HealthDisplay : public Interface
{
protected:
private:
	Player** m_player;
	int16_t m_lives;
public:

	HealthDisplay(UIManager* _parent, SDL_Texture* _spritesheet);

	void Draw();

	void Update();
};
