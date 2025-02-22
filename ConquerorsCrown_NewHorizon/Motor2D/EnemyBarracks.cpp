#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "EnemyBarracks.h"
#include "StaticEnt.h"
#include "Brofiler/Brofiler.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1Tutorial.h"

EnemyBarracks::EnemyBarracks(int posx, int posy) : StaticEnt(StaticEntType::HumanTownHall)
{
	name.create("enemy_barracks");
	position.x = posx;
	position.y = posy;
	vision = 30;
	body = 0;
	collrange = 70;
	selectable = false;
	isSelected = false;
	to_delete = false;
	canbuild = false;
	active = true;
	defense_cooldown.Start();
	spawn_cooldown.Start();
	defenses_spawned = false;
	spawn_counter = 0;
	createUI = false;

	//pos0 = { 827, 103 };
	//pos1 = { 890, 103 };
	//pos2 = { 827, 168 };
	//pos3 = { 890, 168 };
	//pos4 = { 827, 230 };
	//pos5 = { 890, 230 };
	// Load all animations
	inconstruction.PushBack({0,0,125,124}, 0.2, 0, 0, 0, 0);
	finishedconst2.PushBack({129,0,125,124}, 0.2, 0, 0, 0, 0);

	team = TeamType::IA;
	life_points = 300;
	max_hp = life_points;
}

EnemyBarracks::~EnemyBarracks()
{}

bool EnemyBarracks::Start()
{
	
	return true;
}

bool EnemyBarracks::Update(float dt)
{
	BROFILER_CATEGORY("Update_enemyBarrack", Profiler::Color::BlanchedAlmond);

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;

	if (life_points <= 0) 
	{
		to_delete = true;
	}
		
	checkAnimation(dt);

	if (life_points < max_hp && defense_cooldown.ReadSec() > 20)
	{
		SpawnDefenses();
		if (defenses_spawned == true) { defense_cooldown.Start(); }
	}
	if (defense_cooldown.ReadSec() < 20)
	{
		max_hp = life_points;
		defenses_spawned = false;
	}

	//Final blit
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	App->render->Blit(App->entity->enemy_building, (int)position.x - 65, (int)position.y - 65, r, 1.0f, 1.0f);

	hp_conversion = (float)60 / (float)max_hp;
	SDL_Rect section;
	section.x = 0;
	section.y = 0;
	section.w = ((int)life_points * hp_conversion);
	section.h = 2;
	if (life_points < max_hp)
	App->render->Blit(App->entity->life_bar, (int)(position.x - (*r).w / 4), (int)(position.y + (*r).h / 3), &section);
	return true;
}

bool EnemyBarracks::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Pos_enemyBarrack", Profiler::Color::BurlyWood)

		return true;
}

bool EnemyBarracks::CleanUp()
{
	// Now it only clear the path when the building is finished (before it could delete non walkable walls with preview mode)
	
	return true;
}


void EnemyBarracks::checkAnimation(float dt)
{
	current_animation = &finishedconst2;

	
}



iPoint EnemyBarracks::Searchtile(iPoint map)
{
	map.x += 3;
	map.y += 3;
	iPoint home =map;
	iPoint normal_start = map;
	int loops=0;

	normal_start.y -= 1;
	normal_start.x -= 3;

	if (App->pathfinding->IsWalkable(normal_start) == true) 
	{
		respawn = normal_start;
		respawn = App->map->MapToWorld(respawn.x, respawn.y);
		return respawn;
	}

	do
	{
		if (loops >= 1) 
		{
			home.x += 1; 
			home.y += 1;
			map = home;

		}
		for (int i = 0; i < 5 + loops; i++)
		{
			map.y -= 1;

			for (int d = 0; d < 5 + loops; d++)
			{
				map.x -= 1;

				if (App->pathfinding->IsWalkable(map) == true)
				{
					respawn = { map.x,map.y };
					respawn = App->map->MapToWorld(respawn.x, respawn.y);
					return respawn;
				}
			}
			map.x += 5 + loops;
		}

		loops++;
	} while (loops < 10);

	respawn = { 1,1 };
	return respawn;
}

bool EnemyBarracks::SpawnDefenses()
{
	if (spawn_cooldown.ReadSec() > 0.5)
	{
		int r = rand() % 3 + 1;
		switch (r) {
		case 1:
			App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_TROLL, position.x + 20, position.y + 20);
			break;
		case 2:
			App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_GRUNT, position.x + 20, position.y + 20);
			break;
		case 3:
			App->entity->CreateEntity(DynamicEnt::DynamicEntityType::ENEMY_OGRE, position.x + 20, position.y + 20);
			break;
		}
		spawn_counter++;
		spawn_cooldown.Start();
	}

	if (spawn_counter == 7) { defenses_spawned = true; spawn_counter = 0; }

	return true;
}