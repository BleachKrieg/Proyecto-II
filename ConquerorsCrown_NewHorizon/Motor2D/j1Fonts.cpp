#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "AssetsManager.h"
#include "SDL\include\SDL.h"
#include "SDL_TTF\include\SDL_ttf.h"
#include "j1Render.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

j1Fonts::j1Fonts() : j1Module()
{
	name.create("fonts");
}

// Destructor
j1Fonts::~j1Fonts()
{}

// Called before render is available
bool j1Fonts::Awake(pugi::xml_node& conf)
{
	LOG("Init True Type Font library");

	bool ret = true;

	if(TTF_Init() == -1)
	{
		LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		ret = false;
	}
	else
	{

		const char* path = conf.child("default_font").attribute("file").as_string(DEFAULT_FONT);
		LOG("%s", path);
		int bigfontSize = conf.child("font_size").attribute("big").as_int(DEFAULT_FONT_SIZE);
		int smallfontSize = conf.child("font_size").attribute("small").as_int(DEFAULT_FONT_SIZE);
		int xsfontsize = conf.child("font_size").attribute("xs").as_int(DEFAULT_FONT_SIZE);
		defaultfont = Load(path, bigfontSize);
		smallfont = Load(path, smallfontSize);
		xs_font = Load(path, xsfontsize);
	}

	return ret;
}

// Called before quitting
bool j1Fonts::CleanUp()
{
	LOG("Freeing True Type fonts and library");

	for (int i = 0; i < fonts.size(); i++) {
		TTF_CloseFont(fonts[i]);
	}

	fonts.clear();
	TTF_Quit();
	return true;
}

// Load new texture from file path
TTF_Font* const j1Fonts::Load(const char* path, int size)
{
	/*TTF_Font* font = TTF_OpenFont(path, size);*/
	TTF_Font* font = TTF_OpenFontRW(App->assetManager->Load(path), 1, size);

	if(font == NULL)
	{
		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());

	}
	else
	{
		LOG("Successfully loaded font %s size %d", path, size);
		fonts.push_back(font);
	}

	return font;
}

// Print text using font
SDL_Texture* j1Fonts::Print(const char* text, SDL_Color color, TTF_Font* font)
{
	SDL_Texture* ret = NULL;
	SDL_Surface* surface = TTF_RenderText_Blended((font) ? font : defaultfont, text, color);

	if(surface == NULL)
	{
		LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());


	}
	else
	{
		ret = SDL_CreateTextureFromSurface(App->render->renderer, surface);
		SDL_FreeSurface(surface);
	}

	return ret;
}

// calculate size of a text
bool j1Fonts::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret = false;

	if (TTF_SizeText((font) ? font : defaultfont, text, &width, &height) != 0)
	{
	LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}

	else
		ret = true;

	return ret;
}