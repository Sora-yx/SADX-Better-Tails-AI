#pragma once

#define MaxPlayers 8

void config(const char* path);
#define charID counter.b[1]
#define pNum counter.b[0]

extern HelperFunctions HelperFunctionsGlobal;
extern HMODULE multi;
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
extern uint8_t rescueChance;
extern bool chillInGarden;

void AI_Init(const HelperFunctions& helperFunctions, const char* path);