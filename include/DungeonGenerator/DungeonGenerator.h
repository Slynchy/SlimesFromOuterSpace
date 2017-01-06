#pragma once

#include <vector>
#include "../glm/glm.hpp"
#include "../AtlaEngine/Level/Level.h"

class DungeonGenerator
{
	protected:
	private:
		DungeonGenerator()
		{
			// private constructor
		}
	public:
		static void DungeonGenerator::PlaceRoom(Level* _MAP, glm::ivec2 _pos, glm::ivec2 _size, int _type);
		static void DungeonGenerator::PopulateWithRooms(Level* _MAP, unsigned int _roomcount);
		static void DungeonGenerator::ConnectRooms(Level* _MAP);
		static void DungeonGenerator::GenerateTexture(SDL_Renderer* _renderer, Level* _MAP);
		static int16_t DungeonGenerator::DetermineFloorType(int x, int y, Level* _MAP);
};