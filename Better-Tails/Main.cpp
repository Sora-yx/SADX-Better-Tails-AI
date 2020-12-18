#include "stdafx.h"

#define ReplaceSound(C, D) helperFunctions.ReplaceFile("system\\sounddata\\se\\" C ".dat", "system\\" D ".dat")

bool isAIActive = false;
int FlagAI = 0;
Trampoline* TailsAI_Main_t;


MilesAI_Spawn TailsArray[] { //Used to prevent Miles to be called in some very specfic places/cutscenes where the game will crash.
	{ Characters_Sonic, LevelIDs_StationSquare, EventFlags_Sonic_EmeraldCoastClear, 0x006 },
	{ Characters_Sonic, LevelIDs_StationSquare, EventFlags_Sonic_CasinopolisClear, 0x009 },
	{ Characters_Sonic, LevelIDs_MysticRuins, EventFlags_Sonic_Chaos4Clear, 0x00B },
	{ Characters_Sonic, LevelIDs_StationSquare, EventFlags_Sonic_SpeedHighwayClear, 0x014 },
	{ Characters_Sonic, LevelIDs_MysticRuins, EventFlags_Sonic_SpeedHighwayClear, 0x016 },
	{ Characters_Sonic, LevelIDs_EggCarrierOutside, EventFlags_Sonic_RedMountainClear, 0x100},
	{ Characters_Sonic, LevelIDs_EggCarrierInside, EventFlags_Sonic_SkyDeckClear, 0x01B},
	{ Characters_Sonic, LevelIDs_EggCarrierOutside, EventFlags_Sonic_SkyDeckClear, 0x01B},
	{ Characters_Knuckles, LevelIDs_MysticRuins, EventFlags_Knuckles_RedMountainClear, 0x08B },
	{ Characters_Amy, LevelIDs_StationSquare,  EventFlags_Amy_HotShelterClear, 0x06D},
	{ Characters_Amy, LevelIDs_EggCarrierOutside,  EventFlags_Amy_HotShelterClear, 0x06D},
	{ Characters_Big, LevelIDs_MysticRuins, EventFlags_Big_IceCapClear, 0x0D3 },
	{ Characters_Big, LevelIDs_EmeraldCoast, EventFlags_Big_IceCapClear, 0x0D4},
	{ Characters_Gamma, LevelIDs_EggCarrierOutside, EventFlags_Gamma_EmeraldCoastClear, 0x0C2},
};

int LastStoryCutscene[4]{
	0x0F4, 0x0F5, 0x0F9, 0x0FA
};


bool isTailsAIAllowed() {

	for (uint8_t i = 0; i < LengthOfArray(TailsArray); i++) {

		if (CurrentCharacter == TailsArray[i].curCharacter && CurrentLevel == TailsArray[i].curLevel && EventFlagArray[TailsArray[i].eventFlag] == 1)
		{
			if (!GetCutsceneFlagArray(TailsArray[i].cutsceneFlag)) //if the cutscene didn't play, don't call Tails
				return false;
		}
	}

	return true;
}


//Tails AI Flag Check
int CheckTailsAI_R(void) {

	isAIActive = false;
	HMODULE isSA2Mod = GetModuleHandle(L"sadx-sa2-mod");

	if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2 || !isSA2Mod && CurrentLevel == LevelIDs_ChaoRace || EV_MainThread_ptr) {
		
		return 0x0; //don't load AI
	}

	//bug fixes (Disable AI if necessary to avoid crash
	if (!isTailsAIAllowed())
		return 0x0;

	//Player Settings
	if (CurrentLevel >= LevelIDs_SSGarden && CurrentLevel <= LevelIDs_MRGarden && (IsChaoGardenBanned || CurrentChaoStage < 4 && CurrentChaoStage > 6))
		return 0; 
	
	if (IsHubBanned && CurrentLevel >= LevelIDs_StationSquare && CurrentLevel <= LevelIDs_Past || IsBossBanned && CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel <= LevelIDs_E101R)
		return 0;  

	if (IsTCBanned && CurrentLevel == LevelIDs_TwinkleCircuit || CurrentCharacter == Characters_Tails || CurrentChaoStage == 1)
		return 0;  

	//General Place where we don't want AI to spawn
	if (CurrentLevel == LevelIDs_EggCarrierInside && CurrentAct == 5 || CurrentLevel == LevelIDs_Casinopolis && CurrentAct > 1)
		return 0x0;

	//Story plot check for coherent Tails AI spawn (if setting enabled)
	if (IsStoryIA)
	{
		if (CurrentCharacter == Characters_Sonic) {
			if (EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == 0 || (EventFlagArray[EventFlags_Sonic_GammaDefeated] == 1 && EventFlagArray[EventFlags_SonicAdventureComplete] == 0))
				return 0x0; //don't load Tails until we rescue him and don't load him after gamma's defeat

			if (SonicSkyChaseAct1Clear == 1 && EventFlagArray[EventFlags_Sonic_RedMountainClear] == 0)
				return 0x0;
		}
		else {
			return 0x0;
		}
	}
	

	if (CurrentCharacter == Characters_Sonic)
	{
		switch (CurrentLevel)
		{
		case LevelIDs_MysticRuins:
			if (!GetCutsceneFlagArray(0x00B) && IceCapFlag)
				return 0x0; //Fix Knuckles AI behavior.

			if (EventFlagArray[EventFlags_Sonic_EggViperClear] == 1 && !EventFlagArray[EventFlags_SonicAdventureComplete])
				return 0x0; //fix funny ending crash
			
			break;
		case LevelIDs_RedMountain:
			if (!EventFlagArray[EventFlags_Sonic_RedMountainClear] && CurrentAct >= 1)
				return 0x0; //Tornado cutscene
	
			break;
		}
	}

	if (CurrentCharacter == Characters_Big && CurrentAct == 2 && CurrentLevel == LevelIDs_StationSquare) {

		if (EventFlagArray[EventFlags_Big_TwinkleParkClear] == false)
			return 0x0; //fix Sonic AI fight
	}

	if (SelectedCharacter == 6) //Super Sonic Story
	{
		if (CurrentLevel == LevelIDs_MysticRuins)
		{
			for (int i = 0; i < LengthOfArray(LastStoryCutscene); i++) {

				if (!GetCutsceneFlagArray(LastStoryCutscene[i]))
					return 0x0; //fix Super Sonic cutscene crash.
			}
		}
			
		if (IsStoryIA && CurrentLevel == LevelIDs_Past)
			return 0x0; //Don't load Tails in the past if story option is enabled.
		
		if (CurrentLevel == LevelIDs_PerfectChaos)
			return 0x0;  //Fight harder, for no reason.
	}

	isAIActive = true;
	return 1; //Return Load AI
}



ObjectMaster* LoadTails()
{
	ObjectMaster* obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Tails_Main);
	obj->Data1->CharID = Characters_Tails;
	obj->Data1->CharIndex = (char)1;
	EntityData1Ptrs[1] = (EntityData1*)obj->Data1;
	EntityData2Ptrs[1] = (EntityData2*)obj->Data2;
	return obj;
}

//Load Tails AI
ObjectMaster* Load2PTails_r() {

	FlagAI = CheckTailsAI_R();

	if (FlagAI != 1)
	{
		isAIActive = false;
		return (ObjectMaster*)0x0;
	}
	else
	{
		ObjectMaster* AI = LoadObject(LoadObj_Data1, 0, TailsAI_Main);  //load AI 
		TailsAI_ptr = AI;

		if (AI)
		{
			AI->Data1->CharID = Characters_Tails;
			AI->Data1->CharIndex = (char)1;
			AI->DeleteSub = TailsAI_Delete;
			ObjectMaster* Chara = LoadTails(); //set the character
			if (Chara) {
				isAIActive = true;
				GetPlayerSidePos(&Chara->Data1->Position, AI->Data1, 10);

			}
			AI->Data1->Action = 0;
			dword_3B2A304 = 0;
			return AI;
		}
	}

	isAIActive = false;
	return nullptr;
}


void LoadCharacterAndAI() {

	if (banCharacter[CurrentCharacter] != true && !EV_MainThread_ptr && !EntityData1Ptrs[1])
		Load2PTails_r();

	if (isCharSelActive())
		return LoadCharacter_r();


	return LoadCharacter(); //call original function
}


void MilesAI_OnFrames() { //Only run when TailsAI_Main is active

	if (GameState != 15 || !EntityData1Ptrs[0] || !EntityData1Ptrs[1] || EntityData1Ptrs[1]->CharID != Characters_Tails || !TailsAI_ptr)
		return;

	PreventTailsAIDamage();
	SnowboardRespawn();
}

//Reset value when Tails AI is deleted
void TailsAI_ResetValue() {
	rngKill = 0;
	isChaoPetByAI = false; //just to be safe
	isAIActive = false;
	isRescued = false;
	return FUN_0042ce20();
}

//Reset value when the player quit or soft reset
void SoftReset_R() {
	FUN_00412ad0();
}

void TailsAI_Main_R(ObjectMaster* obj) {

	CheckAndLoadTailsTravelObjects(obj);
	MilesAI_OnFrames();

	ObjectFunc(origin, TailsAI_Main_t->Target());
	origin(obj);
}

void AI_Init(const HelperFunctions& helperFunctions) {

	//Allow Tails AI to spawn in acton stages, hub world, bosses and chao garden + fixes
	WriteJump(CheckTailsAI, CheckTailsAI_R);

	WriteData<5>((void*)0x415948, 0x90); //remove the original load2PTails in LoadCharacter as we use a custom one
	
	AI_Fixes();
	AI_Improvement();

	TailsAI_Main_t = new Trampoline((int)TailsAI_Main, (int)TailsAI_Main + 0x5, TailsAI_Main_R);
	
	ReplaceSound("P_SONICTAILS_BANK03", "P_SONICTAILS_BANK03");
}

