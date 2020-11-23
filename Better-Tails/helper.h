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
void DeleteAI();
void AI_Fixes();

void OTaraiChild_Main_r(ObjectMaster* obj);
VoidFunc(OTaraiChild_Main, 0x52c8f0);
void MilesAI_OnFrames();


VoidFunc(FUN_0042ce20, 0x42ce20); //Called with Delete Tails AI
DataPointer(int, dword_3B2A304, 0x3B2A304);
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