#ifndef __HUMANTOWNHALL_H__
#define __HUMANTOWNHALL_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "StaticEnt.h"
#include "EntityRequest.h"

enum TownHall_states
{
	ST_TOWNHALL_PREVIEW,
	ST_TOWNHALL_IN_CONSTRUCTION,
	ST_TOWNHALL_FINISHED,
	ST_TOWNHALL_AUTOMATIC,
	ST_TOWNHALL_DESTROYED
};

class HumanTownHall : public StaticEnt
{
public:
	// Constructor
	HumanTownHall(int posx, int posy);

	// Destructor
	~HumanTownHall();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	bool CleanUp();

	void CheckWalkable(iPoint map);

	// Animation
	void checkAnimation(float dt);

	// UI
	void CreateTownHallUI();
	void DeleteTownHallUI();
	void GuiInput(GuiItem*);
	void QueueSwap();
	
	//Queue
	void CheckQueue();
	void ImageSelected();

	//FreeTile
	iPoint Searchtile(iPoint respawn);

public:
	// Animations
	Animation inconstruction;
	Animation finishedconst2;
	SDL_Rect Construction;
	SDL_Rect Created;
	TownHall_states	actualState;

	// Stats
	int attackrange;

	// Queue
	float timer_queue;
	j1Timer start_queue;
	j1Timer timer_bar;
	bool time_bar_start;

	// Queue_v2
	struct QueueTroop
	{
		int			time;
		GuiItem*	image;
		GuiItem*	bar;
		j1Timer		timer;
	};

	vector<QueueTroop*>	Troop;

	// UI
	GuiItem* Button_Create_Gatherer;
	bool create_gatherer;
	GuiItem* Gatherer_image;
	GuiItem* Gatherer_stone_cost;
	GuiItem* Gatherer_Text_stone;
	GuiItem* Gatherer_gold_cost;
	GuiItem* Gatherer_Text_Gold;

	//UI BARRACK
	GuiItem* Button_Create_Barrack;
	GuiItem* Button_Create_Barrack_definition;
	GuiItem* BarrackImage;
	GuiItem* Barrack_stone_cost;
	GuiItem* Barrack_Text_stone;
	GuiItem* Barrack_wood_cost;
	GuiItem* Barrack_Text_Wood;

	//UI lab
	GuiItem* Button_Create_Lab;
	GuiItem* Lab_Image;
	GuiItem* Lab_stone_cost;
	GuiItem* Lab_Text_stone;
	GuiItem* Lab_wood_cost;
	GuiItem* Lab_Text_Wood;

	//UI Wall
	GuiItem* Button_Create_Wall;
	GuiItem* Wall_Image;
	GuiItem* Wall_stone_cost;
	GuiItem* Wall_Text_stone;

	//UI Barn
	GuiItem* Button_Create_Barn;
	GuiItem* Barn_Image;
	GuiItem* Barn_stone_cost;
	GuiItem* Barn_Text_stone;
	GuiItem* Barn_wood_cost;
	GuiItem* Barn_Text_Wood;

	//UI Progress Bar
	GuiItem* creation_TownHall_bar;

	// Queue positions
	iPoint pos0;
	iPoint pos1;
	iPoint pos2;
	iPoint pos3;
	iPoint pos4;
	iPoint pos5;

	//Description_UI
	GuiItem* Description_text_1;
	GuiItem* Description_text_2;
	GuiItem* Description_text_3;
	GuiItem* Description_text_4;
private:
	iPoint respawn;

};
#endif // __TEST_1_H__
