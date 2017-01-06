#include "UI.h"
#include "../GameObject/Player/Player.h"
#include "../Atla.h"

UIManager::UIManager(Atla* _engineptr)
{
	engine = _engineptr;
}

UIManager::~UIManager()
{
	while (m_queue.size() > 0)
	{
		delete m_queue.back();
		m_queue.pop_back();
	}
}

void UIManager::Draw()
{
	for each (Interface* var in m_queue)
	{
		var->Draw();
	}
}