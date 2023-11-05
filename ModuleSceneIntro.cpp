#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"
#include <string>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = NULL;
	ray_on = false;
	sensed = false;
	pause = false;
	forceSpring = 0;
	score = 0;
	extra = 0;
	forceFlipper = -150;
	lives = 3;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	Create();
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/Ball.png"); 
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	background = App->textures->Load("pinball/Background.png");
	muelle = App->textures->Load("pinball/Muelle.png");

	char lookupTable[] = { "! @,_./0123456789$;< ?abcdefghijklmnopqrstuvwxyz" };
	scoreFont = App->fonts->Load("pinball/rtype_font3.png", lookupTable, 2);

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(background, 0, 0);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if (lives == 0) {
		score = 0;
		lives = 3;
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 9));
		circles.getLast()->data->listener = this;
		circles.getLast()->fakeBall = true;
	}

	if (prevLives != lives) {
		circles.add(App->physics->CreateCircle(411, 420, 9));
		circles.getLast()->data->listener = this;
	}

	prevLives = lives;

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		lives++;
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		App->physics->gravityY += 0.5f;
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) {
		App->physics->gravityY -= 0.5f;
	}

	b2Fixture* skullFixture1 = skullBumper1->body->GetFixtureList();
	b2Fixture* skullFixture2 = skullBumper2->body->GetFixtureList();
	b2Fixture* skullFixture3 = skullBumper3->body->GetFixtureList();
	b2Fixture* catFixture = catBumper->body->GetFixtureList();
	b2Fixture* potionFixture1 = potion1->body->GetFixtureList();
	b2Fixture* potionFixture2 = potion2->body->GetFixtureList();
	if (skullFixture1 && skullFixture2 && skullFixture3) {
		if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) {
			skullFixture1->SetRestitution(skullFixture1->GetRestitution() + 0.1f);
			skullFixture2->SetRestitution(skullFixture2->GetRestitution() + 0.1f);
			skullFixture3->SetRestitution(skullFixture3->GetRestitution() + 0.1f);
			catFixture->SetRestitution(catFixture->GetRestitution() + 0.1f);
			potionFixture1->SetRestitution(potionFixture1->GetRestitution() + 0.1f);
			potionFixture2->SetRestitution(potionFixture2->GetRestitution() + 0.1f);
		}
		if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN) {
			skullFixture1->SetRestitution(skullFixture1->GetRestitution() - 0.1f);
			skullFixture2->SetRestitution(skullFixture2->GetRestitution() - 0.1f);
			skullFixture3->SetRestitution(skullFixture3->GetRestitution() - 0.1f);
			catFixture->SetRestitution(catFixture->GetRestitution() - 0.1f);
			potionFixture1->SetRestitution(potionFixture1->GetRestitution() - 0.1f);
			potionFixture2->SetRestitution(potionFixture2->GetRestitution() - 0.1f);
		}
	}


	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		pause = !pause;
	}
	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(circle, x-c->data->width, y-c->data->height);
		c = c->next;
	}

	int springx, springy;
	springUp->GetPosition(springx, springy);
	App->renderer->Blit(muelle, springx, springy);

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		if (forceSpring < 200) {
			forceSpring += 10;
		}
		springUp->body->ApplyForceToCenter(b2Vec2(0, forceSpring), 1);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP) {
		forceSpring = 0;
	}


	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		leftFlipper->body->ApplyForceToCenter(b2Vec2(0, forceFlipper), 1);
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		rightFlipper->body->ApplyForceToCenter(b2Vec2(0, forceFlipper), 1);
	}

	float32 leftFlipperAngle = leftFlipper->body->GetAngle();
	float32 rightFlipperAngle = rightFlipper->body->GetAngle();

	App->renderer->Blit(leftFlipperImage, 123, 453, NULL, 0, RADTODEG* (leftFlipperAngle), 0, 5);
	App->renderer->Blit(rightFlipperImage, 188, 458, NULL, 0, RADTODEG* (rightFlipperAngle), 65, 5);

	sprintf_s(scoreText, 10, "%7d", score);
	App->fonts->BlitText(5, 10, scoreFont, "score");
	App->fonts->BlitText(-15, 20, scoreFont, scoreText);


	sprintf_s(livesText, 10, "%7d", lives);
	App->fonts->BlitText(460, 10, scoreFont, "lives");
	App->fonts->BlitText(440, 20, scoreFont, livesText);

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
}

void ModuleSceneIntro::Create() {
	//Spring
	springUp = App->physics->CreateRectangle(388, 558, 40, 20);
	springUp->body->SetFixedRotation(true);
	springDown = App->physics->CreateRectangle(412, 580, 25, 10);
	springDown->body->SetType(b2_staticBody);

	b2DistanceJointDef SpringJointDef;

	SpringJointDef.bodyA = springUp->body;
	SpringJointDef.bodyB = springDown->body;

	SpringJointDef.localAnchorA.Set(0, 0);
	SpringJointDef.localAnchorB.Set(0, 0);

	SpringJointDef.length = 1.5f;

	SpringJointDef.collideConnected = true;

	SpringJointDef.frequencyHz = 4.0f;
	SpringJointDef.dampingRatio = 0.05f;

	
	b2PrismaticJoint* SpringJoint = (b2PrismaticJoint*)App->physics->world->CreateJoint(&SpringJointDef);
	//Flippers

	int x1 = 128, y1 = 463;
	int x2 = 256, y2 = 463;
	int w = 40, h = 10;

	leftFlipper = App->physics->CreateRectangle(x1, y1, w, h);
	leftPivot = App->physics->CreateCircle(x1, y1, 2);
	leftPivot->body->SetType(b2_staticBody);

	b2RevoluteJointDef flipperLeftJoint;

	flipperLeftJoint.bodyA = leftFlipper->body;
	flipperLeftJoint.bodyB = leftPivot->body;
	flipperLeftJoint.referenceAngle = 0 * DEGTORAD;
	flipperLeftJoint.enableLimit = true;
	flipperLeftJoint.lowerAngle = -30 * DEGTORAD;
	flipperLeftJoint.upperAngle = 30 * DEGTORAD;
	flipperLeftJoint.localAnchorA.Set(PIXEL_TO_METERS(-33), 0);
	flipperLeftJoint.localAnchorB.Set(0, 0);
	b2RevoluteJoint* joint_leftFlipper = (b2RevoluteJoint*)App->physics->world->CreateJoint(&flipperLeftJoint);

	rightFlipper = App->physics->CreateRectangle(x2, y2, w, h);
	rightPivot = App->physics->CreateCircle(x2, y2, 2);
	rightPivot->body->SetType(b2_staticBody);

	b2RevoluteJointDef flipperRightJoint;

	flipperRightJoint.bodyA = rightFlipper->body;
	flipperRightJoint.bodyB = rightPivot->body;
	flipperRightJoint.referenceAngle = 0 * DEGTORAD;
	flipperRightJoint.enableLimit = true;
	flipperRightJoint.lowerAngle = -30 * DEGTORAD;
	flipperRightJoint.upperAngle = 30 * DEGTORAD;
	flipperRightJoint.localAnchorA.Set(PIXEL_TO_METERS(33), 0);
	flipperRightJoint.localAnchorB.Set(0, 0);
	b2RevoluteJoint* joint_rightFlipper = (b2RevoluteJoint*)App->physics->world->CreateJoint(&flipperRightJoint);

	rightFlipperImage = App->textures->Load("pinball/R_Hand.png");
	leftFlipperImage = App->textures->Load("pinball/L_Hand.png");

	//Bumpers

	catBumper = App->physics->CreateCircularBumper(214, 87, 20);
	skullBumper1 = App->physics->CreateCircularBumper(165, 198, 15);
	skullBumper2 = App->physics->CreateCircularBumper(233, 198, 15);
	skullBumper3 = App->physics->CreateCircularBumper(199, 250, 15);
	potion1 = App->physics->CreateCircularBumper(129, 352, 20);
	potion2 = App->physics->CreateCircularBumper(266, 352, 16);


	//Stage
	int bottomLeftBlock[12] = {
	126, 456,
	83, 415,
	83, 298,
	67, 296,
	66, 421,
	118, 467
	};

	int bottomRightBlock[12] = {
	261, 461,
	303, 416,
	303, 301,
	320, 296,
	319, 422,
	271, 471
	};

	int leftWeb[12] = {
		32, 178,
		56, 185,
		71, 220,
		65, 256,
		47, 271,
		32, 270
	};

	int rightWeb[10] = {
		353, 189,
		338, 198,
		338, 218,
		343, 234,
		353, 240
	};

	int topLeftBlock[8] = {
	64, 152,
	103, 116,
	125, 133,
	88, 171
	};

	int topRightBlock[8] = {
	264, 135,
	285, 117,
	326, 150,
	303, 175
	};

	int leftBook[8] = {
	107, 364,
	157, 364,
	157, 373,
	108, 373
	};

	int rightBook[8] = {
	281, 363,
	231, 367,
	231, 376,
	281, 376
	};

	int innerBG[26] = {
	207, 586,
	207, 560,
	353, 437,
	354, 97,
	363, 96,
	389, 125,
	387, 581,
	376, 582,
	376, 127,
	364, 116,
	365, 441,
	218, 565,
	218, 586
	};

	int outerBG[92] = {
	179, 585,
	179, 560,
	31, 436,
	32, 94,
	34, 88,
	36, 81,
	38, 75,
	42, 68,
	45, 63,
	51, 57,
	65, 47,
	71, 44,
	77, 41,
	83, 39,
	91, 36,
	97, 34,
	106, 31,
	122, 28,
	136, 27,
	345, 27,
	355, 28,
	365, 30,
	371, 33,
	385, 39,
	392, 43,
	404, 51,
	415, 60,
	421, 65,
	429, 77,
	433, 94,
	435, 108,
	436, 114,
	435, 582,
	446, 582,
	447, 95,
	430, 56,
	395, 31,
	349, 16,
	135, 16,
	82, 26,
	49, 43,
	26, 69,
	20, 97,
	20, 443,
	168, 564,
	168, 585
	};

	App->physics->CreateSolidChain(0, 0, topLeftBlock, 8);
	App->physics->CreateSolidChain(0, 0, topRightBlock, 8);
	App->physics->CreateSolidChain(0, 0, leftBook, 8);
	App->physics->CreateSolidChain(0, 0, rightBook, 8);
	App->physics->CreateSolidChain(0, 0, bottomLeftBlock, 12);
	App->physics->CreateSolidChain(0, 0, bottomRightBlock, 12);
	App->physics->CreateSolidChain(0, 0, leftWeb, 12);
	App->physics->CreateSolidChain(0, 0, rightWeb, 10);
	App->physics->CreateSolidChain(0, 0, innerBG, 26);
	App->physics->CreateSolidChain(0, 0, outerBG, 92);
}
