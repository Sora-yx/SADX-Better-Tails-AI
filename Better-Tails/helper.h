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
DataPointer(int, dword_3B2A304, 0x3B2A304);

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

VoidFunc(FunnyCutsceneThing, 0x4303e0);
void FixAIPosition();

void DeleteAI();
