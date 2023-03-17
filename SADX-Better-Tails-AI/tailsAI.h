#pragma once

const float spdYGain = 0.6f;
const float spdYFall = -0.8f;

struct MilesAI_Spawn {
	int curCharacter;
	int curLevel;
	int eventFlag;
	int cutsceneFlag;
};

struct MilesAI_Fly {
	uint8_t level;
	uint8_t act;
	NJS_VECTOR destination;
	NJS_POINT2 cursor;
};

enum ETailsGrab {
	initFly,
	getAltitude,
	checkGrab,
	grabbed,
	transitionMap,
	displayMap,
	movetoDestination,
	leaving,
	errorMove
};

enum ETailsRescue {
	initMilesFalling,
	catchPlayer,
	playerGrabbed,
	landingTransition,
};

enum EMilesMap {
	Sstation,
	ShostelPool,
	SCasinoArea,
	SChaos0,
	ECarrier,
	MRStation,
	MRAngelIsland,
	MRJungleLW,
	MRJungleBig
};

enum milesAI_CustomAction
{
	AIObjControl = 18,
	boardSlide = 48,
	boardHurt = 54,
	debugAction = 110,
	passengerCart = 115,
	followBuilding,
	AIFlyTravel = 135,
	AIPet
};

extern int rngDeathZoneRescue;
extern int rngRegularDeathRescue;

void __cdecl LoadCharacter_r();
void CheckMilesBossRescue(unsigned char ID);

int CheckTailsAI_R(void); //Tails flag spawn allowed
void LoadAISnowBoard_R(); //Load Snowboard for Tails AI

bool isPlayerUsingSnowboard();
void speedHighwayBuilding_Follow(unsigned char playerID);
void DisableTailsAICol(char pnum);

void PlayCharacterLeaveAnimation(taskwk* p1, playerwk* co2, int playerID);
void Rescue_Init();
void PlayCharacterGrabAnimation(taskwk* p1, playerwk* co2);
void FlySoundOnFrames(int playerID);

bool isNewTricksActive();
float getMilesDistance(taskwk* p1, taskwk* p2);

void GetPlayerSidePos(NJS_VECTOR* v1, taskwk* a2, float m);

void FlyTravel_Init();

void CheckAndLoadMapPVM();
unsigned char getAI_ID();

void moveAItoPlayer(unsigned char playerID, float posX, float posZ);
void Miles_AbilitiesOnFrames(unsigned char pnum);
void ResetMilesAI(char pnum, char action);
void Fix_AIPos_ActTransition();

void DeleteMilesAI();
bool isTailsAI();
bool isP1AndTailsAIEnabled(char playerID);

void Load2PTails_r();

void LookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);
void moveAItoPlayer(unsigned char playerID);
bool isMilesSaving();
void CheckAndLoadTailsTravelObjects(task* obj);
void TailsAI_Delete_r(task* obj);

void SnowboardRespawn(unsigned char ID);
void AI_Improvement();
int isCharacterPetting();
bool isFlyTravelEnabled();
bool isRescue();