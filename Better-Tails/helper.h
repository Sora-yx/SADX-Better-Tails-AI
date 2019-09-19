#pragma once
#include <SADXModLoader.h>

int CheckTailsAI_R(void); //Tails flag spawn allowed

void DisableTime_R(); //result screen Stuff

void LoadAISnowBoard_R(); //Load Snowboard for Tails AI

void LoadTails_AI_R(); //Load Tails AI when not Sonic or Tails

DataPointer(char, IceCapFlag, 0x3B188B4);

DataPointer(unsigned char, SelectedCharacter, 0x3B2A2FD);