#pragma once

#define MaxPlayers 8

int CheckTailsAI_R(void); //Tails flag spawn allowed
void DisableTime_R(); //result screen Stuff
void LoadAISnowBoard_R(); //Load Snowboard for Tails AI

ObjectMaster* Load2PTails_r(); //Load Tails AI 
void RestoreAIControl();
void FixAIHubTransition();
void FixAIHubTransition2();
void LoadCharacter_r();
void LookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);
void AI_Init(const HelperFunctions& helperFunctions);
void PreventTailsAIDamage();
void moveAItoPlayer();
bool isMilesSaving();
void CheckAndLoadTailsTravelObjects(ObjectMaster* obj);
void TailsAI_ResetValue();
void AllowTailsAI_R();
void CheckAndDeleteAI();
void AI_Fixes();
void DeleteTailsAI();
void SnowboardRespawn();
void AI_Improvement();
int isCharacterPetting();
void FadeoutScreen(ObjectMaster* obj);

NJS_VECTOR UnitMatrix_GetPoint(NJS_VECTOR* orig, Rotation3* rot, float x, float y, float z);
float GetSquare(NJS_VECTOR* orig, NJS_VECTOR* dest);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
float CheckDistance(NJS_VECTOR* vec1, NJS_VECTOR* vec2);
void PlayerLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);
float GetCharacterPositionY(EntityData1* p1);
void UpdateP1Position(CharObj2* co2p1, CharObj2* co2p2, EntityData1* p1, EntityData1* p2);
void PlayCharacterLeaveAnimation(EntityData1* p1, CharObj2* co2);
void Rescue_Init();
void PlayCharacterGrabAnimation(EntityData1* p1, CharObj2* co2);
void FlySoundOnFrames();

bool IsSpecificPlayerInSphere(NJS_VECTOR* center, float radius, uint8_t player);
VoidFunc(OTaraiChild_Main, 0x52c8f0);
void MilesAI_OnFrames();

FunctionPointer(long, Chao_Pleasure, (ObjectMaster* Chao), 0x75db80);
FunctionPointer(void, Chao_SetBehavior, (ObjectMaster* Chao, long* a2), 0x71EF10);

//Sphere check functions

//FunctionPointer(void, GetPlayerSidePos, (NJS_VECTOR* a1, EntityData1* a2, float m), 0x47dd50);
void GetPlayerSidePos(NJS_VECTOR* v1, EntityData1* a2, float m);
void __declspec() GetPlayerSidePos_asm(NJS_VECTOR* v1, EntityData1* a2, float m);


VoidFunc(FUN_0042ce20, 0x42ce20); //Called with Delete Tails AI
DataPointer(int, dword_3B2A304, 0x3B2A304);

FunctionPointer(signed int, SetPlayerPosition, (unsigned __int8 playerNum, char a2, NJS_POINT3* a3, long* a4), 0x441a40);

FunctionPointer(int, GetCutsceneFlagArray, (int cutscene), 0x42fb10);

DataPointer(char, IceCapFlag, 0x3B188B4);
DataPointer(unsigned char, SelectedCharacter, 0x3B2A2FD);

DataPointer(char, SonicTPClear, 0x3B188B1);
DataPointer(char, SonicSkyChaseAct1Clear, 0x3B188BD);
DataPointer(char, SonicSkyChaseAct2Clear, 0x3B188BE);

//SADX Map Stuff
FunctionPointer(void, sub_62ECE0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x62ECE0);
FunctionPointer(void, sub_51C130, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x51C130);
FunctionPointer(void, sub_525980, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x525980);
FunctionPointer(void, sub_52F9C0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x52F9C0);
FunctionPointer(void, sub_541BF0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x541BF0);

void CallTailsAI_R();
void CallTailsAI_R2();
void CallTailsAI_R3();
VoidFunc(FUN_00412ad0, 0x412ad0); //called when you soft reset / quit

void SoftReset_R();
void FlyTravel_Init();

extern bool IsChaoGardenBanned;
extern bool IsHubBanned;
extern bool IsBossBanned;
extern bool IsTCBanned;
extern bool isAIActive;
extern bool ForceAI;
extern int AICutsceneOk;
extern bool IsStoryIA;
extern bool banCharacter[8];
extern bool isChaoPetByAI;
extern uint8_t isMoving;
extern bool isFlyTravel;
extern bool isRescue;

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
};

struct Map_Cursor {
	float x;
	float y;
};

enum ETailsGrab {
	initFly,
	getAltitude,
	checkGrab,
	grabbed,
	transitionMap,
	displayMap,
	movetoDestination,
	leaving
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