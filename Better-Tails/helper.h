#pragma once

#define MaxPlayers 8

int CheckTailsAI_R(void); //Tails flag spawn allowed

void DisableTime_R(); //result screen Stuff

void LoadAISnowBoard_R(); //Load Snowboard for Tails AI

void LoadTails_AI_R(); //Load Tails AI 
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
void FixAIHubTransition();
void FixAIHubTransition2();
void LoadCharacter_r();
void AI_Init(const HelperFunctions& helperFunctions);

void moveAItoPlayer();
void TailsAI_ResetValue();
void AllowTailsAI_R();
void CheckAndDeleteAI();
void AI_Fixes();
void DeleteTailsAI();

void OTaraiChild_Main_r(ObjectMaster* obj);
VoidFunc(OTaraiChild_Main, 0x52c8f0);
void MilesAI_OnFrames();
NJS_VECTOR UnitMatrix_GetPoint(NJS_VECTOR* orig, Rotation3* rot, float x, float y, float z);

VoidFunc(FUN_0042ce20, 0x42ce20); //Called with Delete Tails AI
DataPointer(int, dword_3B2A304, 0x3B2A304);

FunctionPointer(int, sub_47DE00, (NJS_VECTOR *a1, float* a2, Angle *a3), 0x47DE00);

FunctionPointer(void, GetPlayerSidePos, (NJS_VECTOR* a1, EntityData1* a2, float m), 0x47dd50);
FunctionPointer(signed int, SetPlayerPosition, (unsigned __int8 playerNum, char a2, NJS_POINT3* a3, long* a4), 0x441a40);
FunctionPointer(bool, sub_42FB00, (), 0x42FB00);
FunctionPointer(signed int, sub_47DC20, (EntityData1* a1, EntityData1* a2, ObjectMaster* a3), 0x47DC20);
DataPointer(int, NpcMilesStandByFlag, 0x3b2a304); //I have no idea, ask sonic team
FunctionPointer(void, sub_47DBE0, (unsigned int a1), 0x47DBE0);

FunctionPointer(int, GetCutsceneFlagArray, (int cutscene), 0x42fb10);

FunctionPointer(void, Start_Cutscene, (int cutscene), 0x4136e0);

DataPointer(char, IceCapFlag, 0x3B188B4);
DataPointer(char, AmyTPFieldOpen, 0x3B1894C);
DataPointer(unsigned char, SelectedCharacter, 0x3B2A2FD);
DataPointer(char, IsInCutscene, 0x3B2C55C);

DataPointer(char, AmyCutscene, 0x3B1861a);

DataPointer(char, SonicTPClear, 0x3B188B1);
DataPointer(char, SonicSkyChaseAct1Clear, 0x3B188BD);
DataPointer(char, SonicSkyChaseAct2Clear, 0x3B188BE);

DataPointer(char, Init_Cutscene, 0x3b2c4e4);

DataPointer(char, TailsHotelOpen, 0x3B188C9);
DataPointer(char, TailsKnuxDefeated, 0x3B188E8);

void CallTailsAI_R();
void CallTailsAI_R2();
void CallTailsAI_R3();
VoidFunc(FUN_00412ad0, 0x412ad0); //called when you soft reset / quit

void SoftReset_R();


extern bool IsChaoGardenBanned;
extern bool IsHubBanned;
extern bool IsBossBanned;
extern bool IsTCBanned;
extern bool isAIActive;
extern bool ForceAI;
extern int AICutsceneOk;
extern bool IsStoryIA;
extern bool flySound;