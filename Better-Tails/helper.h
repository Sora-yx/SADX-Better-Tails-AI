#pragma once

#define MaxPlayers 8

extern bool IsChaoGardenBanned;
extern bool IsHubBanned;
extern bool IsBossBanned;
extern bool IsTCBanned;
extern bool isAIActive;
extern bool IsStoryIA;
extern bool banCharacter[8];
extern bool isChaoPetByAI;
extern uint8_t isMoving;
extern bool isFlyTravel;
extern bool isRescueAllowed;
extern bool isRescued;
extern bool isMSBanned;
extern bool fasterRespawn;


bool isTailsAIAllowed();
bool isCharSelActive();
bool isRandoActive();
void __cdecl LoadCharacter_r();
void CheckMilesBossRescue(unsigned char ID);
void PreventTailsAIAction(unsigned char playerID);

int CheckTailsAI_R(void); //Tails flag spawn allowed
void LoadAISnowBoard_R(); //Load Snowboard for Tails AI
bool isUIScale();
bool isPlayerUsingSnowboard();
void speedHighwayBuilding_Follow(unsigned char playerID);

extern int rngDeathZoneRescue;
extern int rngRegularDeathRescue;

ObjectMaster* Load2PTails_r();
void RestoreAIControl(unsigned char ID);
void FixAIHubTransition();
void FixAIHubTransition2();
void LoadCharacterAndAI();
void LookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);
void AI_Init(const HelperFunctions& helperFunctions);
void moveAItoPlayer(unsigned char playerID);
bool isMilesSaving();
void CheckAndLoadTailsTravelObjects(task* obj);
void TailsAI_Delete_r();
void CheckAndDeleteAI();
void AI_Patches();
void SnowboardRespawn(unsigned char ID);
void AI_Improvement();
int isCharacterPetting();
void FadeoutScreen(ObjectMaster* obj);
bool isInputModActive();

NJS_VECTOR UnitMatrix_GetPoint_Player(NJS_VECTOR* orig, Rotation3* rot, float x, float y, float z);
float GetSquare(NJS_VECTOR* orig, NJS_VECTOR* dest);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
float CheckDistance(NJS_VECTOR* vec1, NJS_VECTOR* vec2);
void PlayerLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);
float GetCharacterPositionY(EntityData1* p1);
void UpdateP1Position(CharObj2* co2p1, CharObj2* co2p2, EntityData1* p1, EntityData1* p2);
void PlayCharacterLeaveAnimation(EntityData1* p1, CharObj2* co2, int playerID);
void Rescue_Init();
void PlayCharacterGrabAnimation(EntityData1* p1, CharObj2* co2);
void FlySoundOnFrames(int playerID);

void MilesAI_OnFrames(unsigned char playerID);
bool isNewTricksActive();
float getMilesDistance(EntityData1* p1, EntityData1* p2);


void GetPlayerSidePos(NJS_VECTOR* v1, EntityData1* a2, float m);

void CallTailsAI_R();

void FlyTravel_Init();

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


void CheckAndLoadMapPVM();
void Force_MilesToFollow(unsigned char playerID);
unsigned char getAI_ID();
void RemovePlayerCollision(unsigned char ID);
void RestorePlayerCollision(unsigned char ID);
void moveAItoPlayer(unsigned char playerID, float posX, float posZ);
void Miles_AbilitiesOnFrames(unsigned char pnum);
void ResetMilesAI(char pnum, char action);
void Fix_AIPos_ActTransition();

enum milesAI_CustomAction
{
	boardSlide = 48,
	boardHurt = 54,
	debugAction = 110,
	passengerCart = 115,
	followBuilding

};

#define TARGET_STATIC(name) ((decltype(name##_r)*)name##_t.Target())

void config(const char* path);