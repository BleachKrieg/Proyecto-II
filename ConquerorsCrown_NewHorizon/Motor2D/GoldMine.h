#ifndef __GOLDMINE_H__
#define __GOLDMINE_H__

#include "j1Module.h"
#include "p2Point.h"
#include "StaticEnt.h"

enum MINE_LIGHTS
{
	LIGHTS_ON,
	LIGHTS_OFF,
};

class GoldMine : public StaticEnt
{
public:
	// Constructor
	GoldMine(int posx, int posy, uint amount);

	// Destructor
	~GoldMine();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	bool PostUpdate(float dt);

	bool CleanUp();

	MINE_LIGHTS mine_lights;

	bool has_limit;

	uint GetExtractionLimit();

	uint GetPreCheck();

	void DecreasePreCheck();

	void DecreaseExtractionCount();

private:
	SDL_Rect no_light_mine;
	SDL_Rect light_mine;
	SDL_Rect out_of_material_mine;
	uint extraction_limit;
	uint max_resources;
	uint pre_check;
	int mine_time;
};

#endif // !__GOLDMINE_H__