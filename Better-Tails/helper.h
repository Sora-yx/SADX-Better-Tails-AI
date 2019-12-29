#pragma once
#include <SADXModLoader.h>

int CheckTailsAI_R(void); //Tails flag spawn allowed

void DisableTime_R(); //result screen Stuff

void LoadAISnowBoard_R(); //Load Snowboard for Tails AI

void LoadTails_AI_R(); //Load Tails AI 

void FixAIHubTransition();
void FixAIHubTransition2();
void LoadCharacter_r();
void AI_Init();

void moveAItoPlayer();
void TailsAI_ResetValue();
void AllowTailsAI_R();
void DeleteAI();

VoidFunc(FUN_0042ce20, 0x42ce20); //Called with Delete Tails AI
DataPointer(int, dword_3B2A304, 0x3B2A304);
FunctionPointer(int, GetCutsceneFlagArray, (int cutscene), 0x42fb10);

FunctionPointer(void, Start_Cutscene, (int cutscene), 0x4136e0);

DataPointer(char, IceCapFlag, 0x3B188B4);
DataPointer(char, AmyTPFieldOpen, 0x3B1894C);
void MoveTailsAI_Upgrade();
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
void FixAIHurt();