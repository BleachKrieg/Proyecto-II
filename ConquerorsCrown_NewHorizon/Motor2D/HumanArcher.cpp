#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanArcher.h"
#include "DynamicEnt.h"
#include "Brofiler/Brofiler.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Input.h"
#include "J1GroupMov.h"
#include <math.h>
#include "Emiter.h"
#include "ParticleSystem.h"
#include "FoWManager.h"

HumanArcher::HumanArcher(int posx, int posy) : DynamicEnt(DynamicEntityType::HUMAN_ARCHER)
{
	name.create("human_archer");

	// TODO: Should get all the DATA from a xml file
	speed = { NULL, NULL };
	life_points = 80; 
	max_hp = life_points;
	attack_vision = 200;
	attack_range = 140;
	time_attack = 1000;
	attack_damage = 16;
	stats_upgrade_damage = 8;
	stats_upgrade_life = 40;
	tier_archer = 0;
	vision = 26;
	body = 13;
	coll_range = 13;
	active = true;
	position.x = posx;
	position.y = posy;
	orientation = SDL_FLIP_NONE;
	to_delete = false;
	can_attack = true;
	isSelected = false;
	selectable = true;
	following_target = false;
	player_order = false;
	team = TeamType::PLAYER;
	target_entity = NULL;
	state = DynamicState::IDLE;
	entity_type = DynamicEntityType::HUMAN_ARCHER;

	visionEntity = App->fowManager->CreateFoWEntity({ posx, posy }, true);
	visionEntity->SetNewVisionRadius(5);
	speed_modifier = 1.2;

	// TODO ------------------------------------------
}

HumanArcher::~HumanArcher() {}

bool HumanArcher::Start()
{

	list<Animation*>::iterator animations_list;
	animations_list = App->entity->archer_animations.begin();
	moving_up = **animations_list;
	++animations_list;
	moving_diagonal_up = **animations_list;
	++animations_list;
	moving_right = **animations_list;
	++animations_list;
	moving_diagonal_down = **animations_list;
	++animations_list;
	moving_down = **animations_list;
	++animations_list;
	attacking_up = **animations_list;
	++animations_list;
	attacking_diagonal_up = **animations_list;
	++animations_list;
	attacking_right = **animations_list;
	++animations_list;
	attacking_diagonal_down = **animations_list;
	++animations_list;
	attacking_down = **animations_list;
	++animations_list;
	death_up = **animations_list;
	++animations_list;
	death_down = **animations_list;
	++animations_list;

	current_animation = &moving_down;

	
	particleSystem = App->entity->CreateParticleSys(position.x, position.y);
	Animation anim;
	anim.PushBack(SDL_Rect{ 0, 0, 32, 32 }, 1, 0, 0, 0, 0);

	anim.Reset();
	Emiter emiter(position.x, position.y, 0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 1, 2, nullptr, App->entity->arrow, anim, false);
	particleSystem->PushEmiter(emiter);
	particleSystem->Desactivate();


	return true;
}

bool HumanArcher::Update(float dt)
{
	BROFILER_CATEGORY("ArcherUpdate", Profiler::Color::BlanchedAlmond);

	// Speed resetted to 0 each iteration
	speed = { NULL, NULL };
	origin = App->map->WorldToMap(position.x, position.y);
	if (App->scene->debug)
		life_points = max_hp;

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_REPEAT && isSelected && App->scene->debug)
		life_points = 0;
	
	OrderPath(entity_type);
	AttackTarget(entity_type);
	fPoint auxPos = position;
	Movement(dt);

	if (auxPos != position)
		visionEntity->SetNewPosition({ (int)position.x, (int)position.y });

	if (particleSystem != nullptr)
		particleSystem->Move(position.x, position.y);
	
	if (life_points <= 0)
		state = DynamicState::DYING;

	if (state != DynamicState::INTERACTING)
		if (particleSystem->IsActive())
			particleSystem->Desactivate();

	switch (state)
	{
	case DynamicState::IDLE:
		current_animation = &moving_right;
		current_animation->Reset();
		//current_animation->loop = false;
		break;
	case DynamicState::UP:
		current_animation = &moving_up;
		break;
	case DynamicState::DOWN:
		current_animation = &moving_down;
		break;
	case DynamicState::HORIZONTAL:
		current_animation = &moving_right;
		break;
	case DynamicState::DIAGONAL_UP:
		current_animation = &moving_diagonal_up;
		break;
	case DynamicState::DIAGONAL_DOWN:
		current_animation = &moving_diagonal_down;
		break;
	case DynamicState::INTERACTING:
		current_animation = &attacking_right;
		if (particleSystem != nullptr && target_entity != nullptr)
		{
			if (target_entity->position.x > position.x)
				orientation = SDL_FLIP_NONE;
			else
				orientation = SDL_FLIP_HORIZONTAL;

			if (!particleSystem->IsActive())
			{
				particleSystem->Activate();
			}
				float xvec, yvec, ProjTime = 1;
				iPoint destiny = App->map->WorldToMap(target_entity->position.x, target_entity->position.y);
				iPoint start = App->map->WorldToMap(position.x + 15, position.y + 15);
				

				fPoint vec(destiny.x - start.x, destiny.y - start.y);

				float norm = sqrt(pow(vec.x, 2) + pow(vec.y, 2));
				yvec = (vec.y / norm);
				xvec = (vec.x / norm);

				
				xvec *= 3;
				yvec *= 3;
				if (norm > 0)
				{
					if (abs(xvec) > abs(yvec))
						ProjTime = norm / abs(xvec);
					else
						ProjTime = norm / abs(yvec);
					ProjTime *= 0.4;
				}
				else {
					ProjTime = 1;
				}
			
				if (particleSystem->emiterVector.size() > 0)
				{
					particleSystem->emiterVector[0].SetSpeed(xvec, yvec);
					particleSystem->emiterVector[0].SetMaxTime(ProjTime);
				}
			
			
		}
		break;
	case DynamicState::DYING:
		if (particleSystem->IsActive())
		{
			particleSystem->Desactivate();
		}
		Death(entity_type);
		break;
	}
	
	//App->render->DrawQuad({ (int)position.x, (int)position.y, 10, 10 }, 200, 200, 0);
	SDL_Rect* r = &current_animation->GetCurrentFrame(dt);
	if (isSelected)
	App->render->Blit(App->entity->ally_sel_tex, (int)(position.x - 20), (int)(position.y) - 10);
	//	App->render->DrawCircle((int)position.x, (int)position.y, 20, 0, 200, 0, 200);

	if (tier_archer != App->scene->upgrade_archer)
	{
		tier_archer = App->scene->upgrade_archer;
		life_points += stats_upgrade_life;
		max_hp += stats_upgrade_life;
		attack_damage += stats_upgrade_damage;
		if (tier_archer == 2)
		{
			life_points = 160;
			max_hp = 160;
			attack_damage = 32;
		}
	}

	if (attack_damage == 24)
	{
		App->render->Blit(App->entity->arch_man_tex2, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation);
	}
	else if (attack_damage == 32)
	{
		App->render->Blit(App->entity->arch_man_tex3, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation);
	}
	else 
	{ 
		App->render->Blit(App->entity->arch_man_tex, (int)(position.x - (*r).w / 2), (int)(position.y - (*r).h / 2), r, 1.0f, 1.0f, orientation); 
	}

	hp_conversion = (float)25 / (float)max_hp;
	SDL_Rect section;
	section.x = 0;
	section.y = 0;
	section.w = ((int)life_points * hp_conversion);
	section.h = 2;
	if (life_points < max_hp)
	App->render->Blit(App->entity->life_bar, (int)(position.x - (*r).w / 4), (int)(position.y + (*r).h / 3), &section);

	return true;
}

bool HumanArcher::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Archer_PostUpdate", Profiler::Color::BurlyWood)

	return true;
}

bool HumanArcher::CleanUp()
{
	close_entity_list.clear();
	colliding_entity_list.clear();
	visionEntity->deleteEntity = true;
	particleSystem->to_delete = true;

	App->fowManager->foWMapNeedsRefresh = true;
	path.clear();
	name.Clear();
	return true;
}