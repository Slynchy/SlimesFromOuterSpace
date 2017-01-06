/// \file 		ResourceManager.h
/// \author 	Sam Lynch
/// \details 	Header file for the resource manager that (un)loads assets
/// \date 		November 2016

#pragma once

#include "../SDL2/SDL.h"
#include "../SDL2/SDL_image.h"
#include "../SDL2/SDL_ttf.h"
#include "../SDL2/SDL_mixer.h" 
#include <vector>
#include <string>

/// Structure for SDL mixer sounds wrapper
struct Sound
{
	/// Filename
	std::string m_filename;

	/// Actual SDL sound
	Mix_Chunk* m_sound;

	Sound(std::string _filename, Mix_Chunk* _sound)
	{
		m_filename = _filename;
		m_sound = _sound;
	}
	~Sound()
	{
		Mix_FreeChunk(m_sound);
		m_sound = nullptr;
	}
};

/// Structure for SDL mixer music wrapper
struct Music
{
	/// Filename
	std::string m_filename;

	/// Actual SDL music
	Mix_Music* m_music;

	Music(std::string _filename, Mix_Music* _music)
	{
		m_filename = _filename;
		m_music = _music;
	}
	~Music()
	{
		Mix_FreeMusic(m_music);
		m_music = nullptr;
	}
};

/// Structure for SDL_Texture wrapper
struct Texture
{
	/// Filename
	std::string m_filename;

	/// Actual SDL texture
	SDL_Texture* m_texture;

	Texture(std::string _filename, SDL_Texture* _texture)
	{
		m_filename = _filename;
		m_texture = _texture;
	}
	~Texture()
	{
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
};

/// Data structure for TTF_Font wrapper
struct Font
{
	/// Filename
	std::string m_filename;

	/// Actual font data
	TTF_Font* m_font;

	Font(std::string _filename, TTF_Font* _texture)
	{
		m_filename = _filename;
		m_font = _texture;
	}
	~Font()
	{
		TTF_CloseFont(m_font);
	}
};

/// Class for the resource manager
class ResourceManager
{
	protected:
	private:
		/// Array of loaded textures
		std::vector<Texture*> loaded_textures;

		/// Array of loaded fonts
		std::vector<Font*> loaded_fonts;

		/// Array of loaded fonts
		std::vector<Sound*> loaded_sfx;

		/// Array of loaded fonts
		std::vector<Music*> loaded_music;

	public:

		/// Gets the music, and loads it if it isn't already loaded
		/// \param _name Filename to load
		Music* GetMUS(std::string _name)
		{
			for (size_t i = 0; i < loaded_music.size(); i++)
			{
				if (loaded_music.at(i)->m_filename == _name)
				{
					return loaded_music.at(i);
				}
			}

			// texture not found, load it
			Mix_Music* temp;

			temp = Mix_LoadMUS(_name.c_str());

			// If temp == null, it's because it couldn't be loaded for whatever reason
			if (temp != NULL)
			{
				loaded_music.push_back(new Music(_name, temp));
				return loaded_music.back();
			}
			else
			{
				printf("Could not load file: %s\n", _name.c_str());
				return nullptr;
			}
		};

		/// Gets the music, and loads it if it isn't already loaded
		/// \param _name Filename to load
		Mix_Music* GetMUS_file(std::string _name)
		{
			for (size_t i = 0; i < loaded_music.size(); i++)
			{
				if (loaded_music.at(i)->m_filename == _name)
				{
					return loaded_music.at(i)->m_music;
				}
			}

			// texture not found, load it
			Mix_Music* temp;

			temp = Mix_LoadMUS(_name.c_str());

			// If temp == null, it's because it couldn't be loaded for whatever reason
			if (temp != NULL)
			{
				loaded_music.push_back(new Music(_name, temp));
				return temp;
			}
			else
			{
				printf("Could not load file: %s\n", _name.c_str());
				return NULL;
			}
		};

		/// Gets the SFX, and loads it if it isn't already loaded
		/// \param _name Filename to load
		Mix_Chunk* GetSFX(std::string _name)
		{
			for (size_t i = 0; i < loaded_sfx.size(); i++)
			{
				if (loaded_sfx.at(i)->m_filename == _name)
				{
					return loaded_sfx.at(i)->m_sound;
				}
			}

			// texture not found, load it
			Mix_Chunk* temp;

			temp = Mix_LoadWAV(_name.c_str());

			// If temp == null, it's because it couldn't be loaded for whatever reason
			if (temp != NULL)
			{
				loaded_sfx.push_back(new Sound(_name, temp));
				return temp;
			}
			else
			{
				printf("Could not load file: %s\n", _name.c_str());
				return NULL;
			}
		};

		/// Gets the texture, and loads it if it isn't already loaded
		/// \param _name Filename to load
		/// \param _rend The SDL renderer, used to load the texture (can be unused if you are sure the texture is loaded)
		SDL_Texture* GetTexture(std::string _name, SDL_Renderer* _rend = NULL)
		{
			for (size_t i = 0; i < loaded_textures.size(); i++)
			{
				if (loaded_textures.at(i)->m_filename == _name)
				{
					return loaded_textures.at(i)->m_texture;
				}
			}
			
			// texture not found, load it
			SDL_Texture* temp;

			// if no renderer passed
			// We don't pass renderer for assets that *should* already be loaded
			// If this error happens, it just needs access to the renderer somehow
			if (_rend == NULL)
			{
				printf("Could not load file (no renderer): %s\n", _name.c_str());
				return NULL;
			}
			else
			{
				temp = IMG_LoadTexture(_rend, _name.c_str());
			}

			// If temp == null, it's because it couldn't be loaded for whatever reason
			if (temp != NULL)
			{
				loaded_textures.push_back(new Texture(_name, temp));
				return temp;
			}
			else
			{
				printf("Could not load file: %s\n", _name.c_str());
				return NULL;
			}
		};

		/// Gets the font, and loads it if it isn't already loaded
		/// \param _name Filename to load
		/// \param _size The font size
		TTF_Font* GetFont(std::string _name, int _size = 16)
		{
			for (size_t i = 0; i < loaded_fonts.size(); i++)
			{
				if (loaded_fonts.at(i)->m_filename == _name)
				{
					return loaded_fonts.at(i)->m_font;
				}
			}

			// font not found, load it
			TTF_Font* temp;
			temp = TTF_OpenFont(_name.c_str(), _size);
			if (temp != NULL)
			{
				loaded_fonts.push_back(new Font(_name, temp));
				return loaded_fonts.back()->m_font;
			}
			else
			{
				printf("Could not load file: %s\n", _name.c_str());
				return NULL;
			}
		};

		/// Unloads all loaded resources
		void UnloadResources()
		{
			for each (Texture* var in loaded_textures)
			{
				delete var;
			}
			for each (Font* var in loaded_fonts)
			{
				delete var;
			}
			for each (Sound* var in loaded_sfx)
			{
				delete var;
			}
			for each (Music* var in loaded_music)
			{
				delete var;
			}
			loaded_textures.clear();
			loaded_music.clear();
			loaded_sfx.clear();
			loaded_fonts.clear();
		}

		~ResourceManager()
		{
			UnloadResources();
		}
};