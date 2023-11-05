#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void Create();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;
	PhysBody* springUp;
	PhysBody* springDown;
	PhysBody* leftFlipper;
	PhysBody* rightFlipper;
	PhysBody* leftPivot;
	PhysBody* rightPivot;
	PhysBody* catBumper;
	PhysBody* skullBumper1;
	PhysBody* skullBumper2;
	PhysBody* skullBumper3;
	PhysBody* potion1;
	PhysBody* potion2;
	PhysBody* sensor;
	bool pause;
	bool sensed;
	int prevLives;
	int lives;
	int forceSpring;
	int forceFlipper;
	int scoreFont = -1;
	char scoreText[10] = { "\0" };
	char highscoreText[10] = { "\0" };
	char livesText[10] = { "\0" };
	int score;
	int highscore;
	int extra;
	SDL_Texture* leftFlipperImage;
	SDL_Texture* rightFlipperImage;
	SDL_Texture* circle;
	SDL_Texture* muelle;
	SDL_Texture* background;
	uint bonus_fx;
	uint cat;
	uint skull;
	uint potion;
	uint song;
	p2Point<int> ray;
	bool ray_on;
};
