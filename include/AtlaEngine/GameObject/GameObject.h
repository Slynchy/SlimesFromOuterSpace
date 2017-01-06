#pragma once

#include <vector>
#include "../../glm/glm.hpp"

struct SDL_Texture;
class Level;

/// Gameobject base class, but can be instantiated on its own
class GameObject
{
protected:
	/// The current sprite in use by the object
	SDL_Texture* active_sprite;

	/// Is the object hidden from AI?
	bool m_invisible = false;

	/// Is the object hidden from drawing on screen?
	bool m_hidden = false;
public:

	/// The direction the object is facing (corresponds to spritesheet position)
	enum DIRECTION
	{
		DOWN,
		UP,
		RIGHT,
		LEFT
	};

	/// The level this object is a part of
	Level* parent_level;

	/// The scene graph, containing all gameobjects in scene
	static std::vector<GameObject*> SceneGraph;

	/// Removes the specified object from the scene graph
	static void RemoveFromSceneGraph(GameObject* _var);

	/// Gets the direction the object is facing from the movement vector
	/// \param _deltapos The movement from which to extrapolate the direction
	static DIRECTION GetDirectionFromDelta(glm::ivec2 _deltapos);

	/// Current position in level
	glm::ivec2 pos;

	/// Current position to draw on screen
	glm::vec2 draw_pos;

	/// The current direction of the object
	DIRECTION direction;

	/// Tag of the game object
	std::string Tag;

	/// Getter for invisible member variable
	bool isInvisible() { return m_invisible; };

	/// Setter for invisible member variable
	void isInvisible(bool _param) { m_invisible = _param; };

	/// Layer to draw on
	/// \deprecated Not used if I recall correctly?
	int32_t Layer = 0;

	/// Getter for active sprite
	virtual SDL_Texture* GetActiveSprite();

	/// Setter for active sprite
	virtual void SetActiveSprite(SDL_Texture* _spr);

	/// Draws the object
	virtual void Draw();

	/// Hides the object from being drawn
	inline virtual void Hide()
	{
		m_hidden = !m_hidden;
	}

	/// Translates the object and updates the direction
	virtual bool Move(glm::ivec2 _movement)
	{
		this->pos += _movement;
		direction = GetDirectionFromDelta(_movement);
		return true;
	}

	/// Destroys this object
	virtual void Destroy();

	/// Updates this object
	virtual void Update();

	GameObject();
	GameObject(Level* _level);
	GameObject(glm::ivec2 _pos, Level* _level, std::string _tag);
	~GameObject()
	{
		RemoveFromSceneGraph(this);
	};
};