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

FunctionPointer(int, sub_42FB00, (), 0x42FB00); //Knuckles thing
FunctionPointer(ObjectMaster*, CheckLoadBird, (), 0x4C6820);
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
void CatchUP(unsigned char playerID);

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
void CheckAndLoadTailsTravelObjects(ObjectMaster* obj);
void TailsAI_ResetValue();
void CheckAndDeleteAI();
void AI_Fixes();
void SnowboardRespawn(unsigned char ID);
void AI_Improvement();
int isCharacterPetting();
void FadeoutScreen(ObjectMaster* obj);
bool isInputModActive();

NJS_VECTOR UnitMatrix_GetPoint(NJS_VECTOR* orig, Rotation3* rot, float x, float y, float z);
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

void ReduceRespawnDelay();

void MilesAI_OnFrames(unsigned char playerID);
bool isNewTricksActive();

FunctionPointer(int, isHostelOpen, (), 0x630900);
FunctionPointer(int, isAngelIslandOpen, (), 0x534570);

FunctionPointer(long, Chao_Pleasure, (ObjectMaster* Chao), 0x75db80);
FunctionPointer(void, Chao_SetBehavior, (ObjectMaster* Chao, long* a2), 0x71EF10);

void GetPlayerSidePos(NJS_VECTOR* v1, EntityData1* a2, float m);

VoidFunc(FUN_0042ce20, 0x42ce20); //Called with Delete Tails AI
DataPointer(int, int_NPCMilesStandByFlag, 0x3B2A304);

FunctionPointer(signed int, SetPlayerPosition, (unsigned __int8 playerNum, char a2, NJS_POINT3* a3, long* a4), 0x441a40);

FunctionPointer(int, GetCutsceneFlagArray, (int cutscene), 0x42fb10);

DataPointer(char, IceCapFlag, 0x3B188B4);
DataPointer(unsigned char, SelectedCharacter, 0x3B2A2FD);

DataPointer(char, SonicTPClear, 0x3B188B1);
DataPointer(char, SonicSkyChaseAct1Clear, 0x3B188BD);
DataPointer(char, SonicSkyChaseAct2Clear, 0x3B188BE);
DataPointer(float*, HorizontalStretchPointer, 0x51285E);

//SADX Map Stuff
FunctionPointer(void, sub_62ECE0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x62ECE0);
FunctionPointer(void, sub_51C130, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x51C130);
FunctionPointer(void, sub_525980, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x525980);
FunctionPointer(void, sub_52F9C0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x52F9C0);
FunctionPointer(void, sub_541BF0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x541BF0);

FunctionPointer(void, KillPlayer, (int player), 0x440cd0);
DataArray(int, IslandDoor_Col, 0x111e010, 7);

void CallTailsAI_R();
VoidFunc(FUN_00412ad0, 0x412ad0); //called when you soft reset / quit

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

struct _camcontwk
{
	char cammode;
	char camsmode;
	char bBlank;
	char btimer;
	unsigned __int16 wtimer;
	__int16 ssFlag;
	float tgtdist;
	NJS_VECTOR pos;
	NJS_VECTOR targetpos;
	Rotation3 angle;
	NJS_VECTOR oldcampos;
	int angx_spd;
	int angy_spd;
	int angz_spd;
	NJS_VECTOR speed;
	NJS_VECTOR acceleration;
};

struct _OBJ_ADJUSTPARAM
{
	__int16 ssAdjustFlag;
	int angSpeed[3];
	float fSpeed;
	int counter;
};



enum class CameraAdjustsIDs {
	None,
	Normal,
	Normal_S,
	Slow,
	Slow_S,
	Time,
	Three1,
	Three1C,
	Three2,
	Three2C,
	Three3,
	Three3C,
	Three4,
	Three4C,
	Three5,
	Three5C,
	Relative1,
	Relative1C,
	Relative2,
	Relative2C,
	Relative3,
	Relative3C,
	Relative4,
	Relative4C,
	Relative5,
	Relative5C,
	Relative6,
	FreeCam
};

enum class CameraDirectIDs {
	Sonic,
	Unknown,
	Target
};

typedef void(__cdecl* CameraFuncPtr)(_OBJ_CAMERAPARAM*);
typedef void(__cdecl* CamAdjustPtr)(EntityData1*, EntityData1*, _OBJ_ADJUSTPARAM*);

FunctionPointer(void, SetCameraEvent, (CameraFuncPtr func, CameraAdjustsIDs adjust, CameraDirectIDs direct), 0x437D20);
VoidFunc(RemoveCameraEvent, 0x436140);
DataPointer(_camcontwk, CameraTask, 0x3B2C660);
void CheckAndLoadMapPVM();
void Force_MilesToFollow(unsigned char playerID);
unsigned char getAI_ID();