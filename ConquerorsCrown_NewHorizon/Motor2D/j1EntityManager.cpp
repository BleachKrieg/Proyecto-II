#include "j1EntityManager.h"
#include "j1Entity.h"
#include "HumanBarracks.h"
#include "HumanTownHall.h"
#include "HumanUpgrade.h"
#include "HumanWall.h"
#include "HumanBarn.h"
#include "HumanFootman.h"
#include "HumanArcher.h"
#include "HumanGatherer.h"
#include "HumanKnight.h"
#include "Troll_Enemy.h"
#include "Ogre_Enemy.h"
#include "Grunt_Enemy.h"
#include "J1GroupMov.h"
#include "ResourceEntities.h"
#include "EnemyBarracks.h"
#include "j1App.h"
#include <stdio.h>
#include "p2Log.h"
#include "j1Render.h"
#include "MouseCursor.h"
#include "GoldMine.h"
#include "Emiter.h"
#include "ParticleSystem.h"
#include "j1Textures.h"
#include "Brofiler/Brofiler.h"
#include "j1Scene.h"
#include "AssetsManager.h"

j1EntityManager::j1EntityManager()
{
	name.create("entity_manager");
}

j1EntityManager::~j1EntityManager()
{}

bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading EntityManager");

	return true;
}

bool j1EntityManager::Start()
{
	pause = false;
	trees_time = 10000;
	quarries_time = 10000;
	mines_time = 10000;
	foot_man_tex = App->tex->Load("Assets/textures/units/Human Sprites/human_footman.png");
	arch_man_tex = App->tex->Load("Assets/textures/units/Human Sprites/human_archer.png");
	foot_man_tex2 = App->tex->Load("Assets/textures/units/Human Sprites/human_footman_2.png");
	foot_man_tex3 = App->tex->Load("Assets/textures/units/Human Sprites/human_footman_3.png");
	arch_man_tex2 = App->tex->Load("Assets/textures/units/Human Sprites/human_archer_2.png");
	arch_man_tex3 = App->tex->Load("Assets/textures/units/Human Sprites/human_archer_3.png");
	gather_man_tex = App->tex->Load("Assets/textures/units/Human Sprites/human_gatherer.png");
	knight_tex = App->tex->Load("Assets/textures/units/Human Sprites/human_knight.png");
	knight_tex2 = App->tex->Load("Assets/textures/units/Human Sprites/human_knight_2.png");
	troll_tex = App->tex->Load("Assets/textures/units/Orc Sprites/orc_troll.png");
	ally_sel_tex = App->tex->Load("Assets/textures/units/selection_ally.png");
	enemy_sel_tex = App->tex->Load("Assets/textures/units/selection_enemy.png");
	ogre_tex = App->tex->Load("Assets/textures/units/Orc Sprites/orc_ogre.png");
	grunt_tex = App->tex->Load("Assets/textures/units/Orc Sprites/orc_grunt.png");
	
	LOG("Loading Dynamic Entities Animations");
	LoadAnimations("Assets/textures/units/Human Units Animations/archer_animations.tmx", archer_animations);
	LoadAnimations("Assets/textures/units/Human Units Animations/footman_animations.tmx", footman_animations);
	LoadAnimations("Assets/textures/units/Human Units Animations/gatherer_animations.tmx", gatherer_animations);
	LoadAnimations("Assets/textures/units/Human Units Animations/knight_animations.tmx", knight_animations);
	LoadAnimations("Assets/textures/units/Orc Units Animations/troll_animations.tmx", troll_animations);
	LoadAnimations("Assets/textures/units/Orc Units Animations/ogre_animations.tmx", ogre_animations);
	LoadAnimations("Assets/textures/units/Orc Units Animations/grunt_animations.tmx", grunt_animations);

	building = App->tex->Load("Assets/textures/buildings/Human Buildings/human_buildings_summer.png");
	enemy_building = App->tex->Load("Assets/textures/buildings/Orc Buildings/orc_buildings_summer.png");
	miscs = App->tex->Load("Assets/textures/misc/misc.png");
	//arrow = App->tex->Load("Assets/textures/particles/projectiles.png");
	arrow = App->tex->Load("Assets/textures/particles/projectiles.png");
	life_bar = App->tex->Load("Assets/textures/gui/life.png");

	max_audio_attacks = 0;
	timer.Start();

	return true;
}

bool j1EntityManager::CleanUp()
{
	
	j1Entity* it;

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->CleanUp();
		RELEASE(entities[i]);
	}

	for (int i = 0; i < resources_ent.size(); ++i)
	{
		resources_ent[i]->CleanUp();
		RELEASE(resources_ent[i]);
	}

	for (int i = 0; i < particles.size(); i++) {

		particles[i]->CleanUp();
		RELEASE(particles[i]);
	}
	entities.clear();
	particles.clear();

	App->tex->UnLoad(miscs);
	App->tex->UnLoad(life_bar);
	App->tex->UnLoad(arrow);
	App->tex->UnLoad(building);
	App->tex->UnLoad(grunt_tex);
	App->tex->UnLoad(ogre_tex);
	App->tex->UnLoad(enemy_sel_tex);
	App->tex->UnLoad(ally_sel_tex);
	App->tex->UnLoad(troll_tex);
	App->tex->UnLoad(gather_man_tex);
	App->tex->UnLoad(arch_man_tex);
	App->tex->UnLoad(foot_man_tex);

	return true;
}

bool j1EntityManager::Update(float dt)
{
	SDL_Rect r;
	BROFILER_CATEGORY("UpdateEntity", Profiler::Color::Bisque);

	iPoint pos;

	App->input->GetMousePosition(pos.x, pos.y);
	pos = App->render->ScreenToWorld(pos.x, pos.y);

	Mix_AllocateChannels(20);

	if (pause) {
		dt = 0;
	}

	if(timer.ReadMs() > 200)
	{
		max_audio_attacks = 0;
		timer.Start();
	}

	for (int i = 0; i < entities.size(); i++) 
	{		
		entities[i]->Update(dt);
	}

	if (App->movement->player_selected != nullptr && App->movement->player_selected->GetDynEntType() == uint(DynamicEnt::DynamicEntityType::HUMAN_GATHERER)) {
		for (int i = 0; i < resources_ent.size(); i++)
		{
			r.x = resources_ent[i]->position.x;
			r.y = resources_ent[i]->position.y;
			r.w = 32;
			r.h = 32;
			if (pos.x > r.x&& pos.x < (r.x + r.w) && pos.y > r.y&& pos.y < (r.y + r.h))
			{
				App->mouse_cursor->on_resources = true;
			}
		}
	}

	for (int i = 0; i < particles.size(); i++) {

		particles[i]->Update(dt);
	}
	/*LOG("entitites: %d", entities.size());
	LOG("particles: %d", particles.size());*/

	return true;
}

bool j1EntityManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateEntity", Profiler::Color::Azure)


	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->to_delete == true)
		{
			DeleteEntity(i, entities[i]);
			i--;
		}
		else
		{
			if (pause) {
				dt = 0;
			}
			entities[i]->PostUpdate();
		}
	}


	for (int i = 0; i < resources_ent.size(); i++) {
		if (resources_ent[i]->to_delete == true)
		{
			RELEASE(resources_ent[i]);
			resources_ent.erase(resources_ent.begin() + i);
			i--;
		}
	}
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i]->to_delete == true)
		{
			DeleteParticles(i, particles[i]);
			i--;
		}
		else
		{
			particles[i]->PostUpdate();
		}
	}
		return true;
}

j1Entity* j1EntityManager::CreateEntity(DynamicEnt::DynamicEntityType type, int posx, int posy)
{
	j1Entity* ret = nullptr;

	switch (type)
	{
	case DynamicEnt::DynamicEntityType::HUMAN_FOOTMAN: ret = new HumanFootman(posx, posy); player_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::HUMAN_ARCHER: ret = new HumanArcher(posx, posy); player_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::HUMAN_GATHERER: ret = new HumanGatherer(posx, posy); player_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::HUMAN_KNIGHT: ret = new HumanKnight(posx, posy); player_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::ENEMY_TROLL: ret = new TrollEnemy(posx, posy); ai_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::ENEMY_OGRE: ret = new OgreEnemy(posx, posy); ai_dyn_ent.push_back(ret); break;
	case DynamicEnt::DynamicEntityType::ENEMY_GRUNT: ret = new GruntEnemy(posx, posy); ai_dyn_ent.push_back(ret); break;
	}

	if (ret != nullptr)
	{
		entities.push_back(ret);
		entities.back()->Start();
	}
	return ret;
}

j1Entity* j1EntityManager::CreateStaticEntity(StaticEnt::StaticEntType type, int posx, int posy, uint resource_type, uint amount)
{
	j1Entity* ret = nullptr;
	GoldMine* aux = nullptr;

	switch (type)
	{
	case StaticEnt::StaticEntType::HumanBarracks: ret = new HumanBarracks(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::HumanTownHall: ret = new HumanTownHall(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::HumanUpgrade: ret = new Human_Upgrade(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::GoldMine: aux = new GoldMine(posx, posy, amount); mines.push_back(aux); ret = (j1Entity*)aux; break;
	case StaticEnt::StaticEntType::Barn: ret = new HumanBarn(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::HumanWall: ret = new Human_Wall(posx, posy); player_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::enemy_barrack: ret = new	EnemyBarracks(posx, posy);  ai_stat_ent.push_back(ret); break;
	case StaticEnt::StaticEntType::Resource: ret = new ResourceEntity(posx, posy, resource_type); resources_ent.push_back(ret); break;
	}

	if (ret != nullptr && type != StaticEnt::StaticEntType::Resource)
	{
		entities.push_back(ret);
		entities.back()->Start();
	}
	else
	{
		resources_ent.back()->Start();
	}
	return ret;
}


bool j1EntityManager::Load(pugi::xml_node& data)
{
	LOG("load entities here");
	pugi::xml_node entity;
	//Then we iterate the xml, to find every entity that's been saved
	for (entity = data.child("Entity"); entity; entity = entity.next_sibling("Entity"))
	{
		int resource_id = 0;
		StaticEnt::StaticEntType static_ID;
		DynamicEnt::DynamicEntityType dynamic_ID;

		p2SString type(entity.child("type").attribute("movement").as_string());

		if (type == "static")
		{
			p2SString static_type(entity.child("type").attribute("name").as_string());
			if (static_type == "town_hall")
			{
				static_ID = StaticEnt::StaticEntType::HumanTownHall;
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
			}
			if (static_type == "human_barracks")
			{
				static_ID = StaticEnt::StaticEntType::HumanBarracks;
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
			}
			if (static_type == "enemy_barracks")
			{
				static_ID = StaticEnt::StaticEntType::enemy_barrack;
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
			}
			if (static_type == "gold_mine")
			{
				static_ID = StaticEnt::StaticEntType::GoldMine;
				uint amount = entity.child("type").attribute("amount_left").as_uint();
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int()+64, entity.child("position").attribute("pos_y").as_int()+64, 0u, amount);
			}
			if (static_type == "tree")
			{
				static_ID = StaticEnt::StaticEntType::Resource;
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int(), 1);
			}
			if (static_type == "quarry")
			{
				static_ID = StaticEnt::StaticEntType::Resource;
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int(), 2);
			}
			if (static_type == "human_barn")
			{
				static_ID = StaticEnt::StaticEntType::Barn;
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
			}
			if (static_type == "wall")
			{
				static_ID = StaticEnt::StaticEntType::HumanWall;
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
			}
			if (static_type == "human_upgrade")
			{
				static_ID = StaticEnt::StaticEntType::HumanUpgrade;
				CreateStaticEntity(static_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
			}
		}
		if (type == "dynamic")
		{
			p2SString dynamic_type(entity.child("type").attribute("name").as_string());
			if (dynamic_type == "enemy_grunt")
			{
				dynamic_ID = DynamicEnt::DynamicEntityType::ENEMY_GRUNT;
			}
			if (dynamic_type == "enemy_ogre")
			{
				dynamic_ID = DynamicEnt::DynamicEntityType::ENEMY_OGRE;
			}
			if (dynamic_type == "enemy_troll")
			{
				dynamic_ID = DynamicEnt::DynamicEntityType::ENEMY_TROLL;
			}
			if (dynamic_type == "human_archer")
			{
				dynamic_ID = DynamicEnt::DynamicEntityType::HUMAN_ARCHER;
			}
			if (dynamic_type == "human_footman")
			{
				dynamic_ID = DynamicEnt::DynamicEntityType::HUMAN_FOOTMAN;
			}
			if (dynamic_type == "human_knight")
			{
				dynamic_ID = DynamicEnt::DynamicEntityType::HUMAN_KNIGHT;
			}
			if (dynamic_type == "human_gatherer")
			{
				dynamic_ID = DynamicEnt::DynamicEntityType::HUMAN_GATHERER;
			}
			//Once localized an entity, we create it

			CreateEntity(dynamic_ID, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
		}

		//Once located an entity, we create it
		//CreateEntity(id, entity.child("position").attribute("pos_x").as_int(), entity.child("position").attribute("pos_y").as_int());
	}

	// For load buildings correctly
	App->scene->active = false;

	/*p2List_item<j1Entity*>* entities_list = entities.start;
	while (entities_list) {
		entities_list->data->Load(entity);
		entities_list = entities_list->next;
	}*/
	
	return true;
}

bool j1EntityManager::Save(pugi::xml_node& data) const
{
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->name.GetString())
		{
			pugi::xml_node entity = data.append_child("Entity");
			entities[i]->Save(entity);
		}
	}
	for (int i = 0; i < resources_ent.size(); i++) {
		if (resources_ent[i]->name.GetString())
		{
			pugi::xml_node entity = data.append_child("Entity");
			resources_ent[i]->Save(entity);
		}
	}
	return true;
}

ParticleSystem* j1EntityManager::CreateParticleSys(int posx, int posy)
{
	ParticleSystem* particleSystem = new ParticleSystem(posx, posy);

	j1Entity* ret = dynamic_cast<j1Entity*>(particleSystem);
	ret->to_delete = false;
	particles.push_back(ret);

	return particleSystem;
}
bool j1EntityManager::DeleteAllEntities()
{

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->to_delete = true;
	}
	for (int i = 0; i < resources_ent.size(); ++i)
	{
		resources_ent[i]->to_delete = true;
		resources_ent[i]->CleanUp();
	}

	for (int i = 0; i < particles.size(); i++) {
		particles[i]->to_delete = true;
	}

	return true;
}

bool j1EntityManager::DeleteEntity(int id, j1Entity* entity)
{
	switch (entity->type)
	{
	case j1Entity::entityType::NO_TYPE:
		break;
	case j1Entity::entityType::DYNAMIC:
		switch (entity->team)
		{
		case j1Entity::TeamType::NO_TYPE:
			break;
		case j1Entity::TeamType::PLAYER:
			if (!player_dyn_ent.empty())
				player_dyn_ent.erase(std::find(player_dyn_ent.begin(), player_dyn_ent.end() + 1, entity));
			break;
		case j1Entity::TeamType::IA:
			if (!ai_dyn_ent.empty())
				ai_dyn_ent.erase(std::find(ai_dyn_ent.begin(), ai_dyn_ent.end() + 1, entity));
			break;
		}
		break;
	case j1Entity::entityType::STATIC:
		switch (entity->team)
		{
		case j1Entity::TeamType::NO_TYPE:
			if (!mines.empty())
				mines.erase(std::find(mines.begin(), mines.end() + 1, entity));
			break;
		case j1Entity::TeamType::PLAYER:
			if (!player_stat_ent.empty())
				player_stat_ent.erase(std::find(player_stat_ent.begin(), player_stat_ent.end() + 1, entity));
			break;
		case j1Entity::TeamType::IA:
				if (!ai_stat_ent.empty())
					ai_stat_ent.erase(std::find(ai_stat_ent.begin(), ai_stat_ent.end() + 1, entity));
			break;
		}
		break;
	}
	

	entities.erase(entities.begin() + id);
	entity->CleanUp();
	RELEASE(entity);



	return true;
}

bool j1EntityManager::DeleteParticles(int id, j1Entity* entity)
{

	particles.erase(particles.begin() + id);
	entity->CleanUp();
	RELEASE(entity);

	return true;
}


void j1EntityManager::LoadAnimations(const char* path, list<Animation*>& animations) {
	
	char* buffer;
	pugi::xml_document	entity_file;
	int bytesFile = App->assetManager->Load(path, &buffer);
	pugi::xml_parse_result result = entity_file.load_buffer(buffer, bytesFile);
	RELEASE_ARRAY(buffer);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());

	}
	TileSetData.firstgid = entity_file.child("map").child("tileset").attribute("firstgid").as_int();
	TileSetData.tile_width = entity_file.child("map").child("tileset").attribute("tilewidth").as_int();
	TileSetData.tile_height = entity_file.child("map").child("tileset").attribute("tileheight").as_int();
	TileSetData.tex_width = entity_file.child("map").child("tileset").child("image").attribute("width").as_int();
	TileSetData.Texname.create(entity_file.child("map").child("tileset").child("image").attribute("source").as_string());
	TileSetData.num_tiles_width = TileSetData.tex_width / TileSetData.tile_width;
	LOG("Tileset: %s", TileSetData.Texname.GetString());
	LOG("firstgid %d", TileSetData.firstgid);
	LOG("tile_width %d", TileSetData.tile_width);
	LOG("tile_height %d", TileSetData.tile_height);
	LOG("tex_width %d", TileSetData.tex_width);
	LOG("num_tiles_width %d", TileSetData.num_tiles_width);
	int i = 0;
	pugi::xml_node tile;
	pugi::xml_node frame;
	for (tile = entity_file.child("map").child("tileset").child("tile"); tile; tile = tile.next_sibling("tile")) {
		Animation* set = new Animation();
		for (frame = tile.child("animation").child("frame"); frame; frame = frame.next_sibling("frame")) {
			set->PushBack(TileSetData.GetAnimRect(frame.attribute("tileid").as_int()), (frame.attribute("duration").as_float()) / 2000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int(), 0, 0);
			LOG("Animation %d, %f, %d, %d", frame.attribute("tileid").as_int(), (frame.attribute("duration").as_float()) / 1000, frame.attribute("pivotx").as_int(), frame.attribute("pivoty").as_int());
		}
		set->loop = true;
		animations.push_back(set);
	}
}

bool j1EntityManager::IsSomethingSelected() 
{

	for (int i = 0; i < entities.size(); i++) {

		if (entities[i]->isSelected) return true;
	}
	return false;
}