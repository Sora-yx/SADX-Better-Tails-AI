#include "pch.h"

bool isAIActive = false;
bool FlagDeleteMilesAI = false;
int FlagAI = 0;
static FunctionHook<void, task*> TailsAI_Main_t((intptr_t)TailsAI_Main);
static FunctionHook<void> LoadCharacter_t(LoadCharacter);

void DeleteMilesAI()
{
	char id = getAI_ID();

	if (id > 0) 
	{
		FlagDeleteMilesAI = true;
		auto AI = playertp[id];
		if (AI)
			FreeTask(AI);
	}
}

//Tails AI Flag Check
int CheckTailsAI_R(void) 
{
	bool isSA2Mod = GetModuleHandle(L"sadx-sa2-mod");

	if (isMultiEnabled())
	{
		return 0x0;
	}

	if (NPCMilesStandByFlag || CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2 || !isSA2Mod && CurrentLevel == LevelIDs_ChaoRace) 
	{
		return 0x0; //don't load AI
	}

	if (CurrentCharacter == Characters_Sonic && MetalSonicFlag && isMSBanned)
		return 0x0;

	//bug fixes (Disable AI if necessary to avoid crash
	if (CurrentCharacter == Characters_Big && !GetCutsceneFlagArray(0x0D3))
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
		else 
		{
			if (!IsAdventureComplete(SelectedCharacter))
				return 0x0;
		}
	}

	if (CurrentCharacter == Characters_Big) {
		if (CurrentLevel == LevelIDs_StationSquare) {
			if (EventFlagArray[EventFlags_Big_TwinkleParkClear] == false)
				return 0x0; //fix Sonic AI fight
		}

		if (CurrentLevel == LevelIDs_MysticRuins && EventFlagArray[EventFlags_Big_IceCapClear] == false)
			return 0x0;
	}

	if (SelectedCharacter == 6) //Super Sonic Story
	{
		if (IsStoryIA && CurrentLevel == LevelIDs_Past)
			return 0x0; //Don't load Tails in the past if story option is enabled.
	}

	if (CurrentLevel == LevelIDs_EggViper || CurrentLevel == LevelIDs_PerfectChaos)
		return 0x0;  //Fight harder, for no reason.

	isAIActive = true;
	return 1; //Return Load AI
}

unsigned char AIIndex = 1;

task* LoadTails()
{
	char pnum = (char)AIIndex;
	auto obj = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Tails_Main);
	obj->twp->charID = Characters_Tails;
	obj->twp->pNum = pnum;
	playertwp[pnum] = obj->twp;
	playermwp[pnum] = (motionwk2*)obj->mwp;
	return obj;
}

//Load Tails AI
void Load2PTails_r() {

	if (TailsAI_ptr != nullptr || (banCharacter[CurrentCharacter] == true || playertwp[AIIndex]))
	{
		return;
	}

	NPCMilesStandByFlag = 0;
	FlagAI = CheckTailsAI_R();

	if (FlagAI != 1)
	{
		isAIActive = false;
		return;
	}
	else
	{
		task* AI = (task*)LoadObject(LoadObj_Data1, 0, TailsAI_Main);  //load AI
		TailsAI_ptr = (ObjectMaster*)AI;

		if (AI)
		{
			AIIndex = (char)CharacterBossActive ? 2 : 1;
			AI->twp->counter.b[1] = Characters_Tails;
			AI->twp->counter.b[0] = AIIndex;
			AI->dest = TailsAI_Delete_r;
			auto Chara = LoadTails(); //set the character
			if (Chara) {
				isAIActive = true;
				GetPlayerSidePos(&Chara->twp->pos, AI->twp, 10);
			}
			AI->twp->mode = 0;
			NPCMilesStandByFlag = 0;
			return;
		}
	}

	isAIActive = false;
	return;
}

void CheckAndLoadAI()
{
	if (!isAIActive && (CurrentCharacter != Characters_Sonic || isCharSelActive()))
	{
		Load2PTails_r();
	}
}

void LoadCharacterAndAI() 
{
	if (isFlyTravel)
		CheckAndLoadMapPVM();

	LoadCharacter_t.Original();

	CheckAndLoadAI();
}

void LoadCharacter_r()
{
	if (isFlyTravel)
		CheckAndLoadMapPVM();

	LoadCharacter_Orgin();
	CheckAndLoadAI();
}

void MilesAI_OnFrames(taskwk* data, unsigned char aiID) { //Only run when TailsAI_Main is active

	if (!playertwp[0] || !playertwp[aiID] || !playerpwp[aiID] || playertwp[aiID]->charID != Characters_Tails || !TailsAI_ptr)
		return;

	Miles_AbilitiesOnFrames(aiID);

	if (isRescueAllowed)
		CheckMilesBossRescue(aiID);
}

//Reset value when Tails AI is deleted
void TailsAI_Delete_r(task* obj) {
	TailsAI_ptr = nullptr;
	AIIndex = 1;
	rngDeathZoneRescue = 0;
	isAIActive = false;
	isRescued = false;
	FlagDeleteMilesAI = false;
	NPCMilesStandByFlag = 0;
	rngRegularDeathRescue = 0;
	return FUN_0042ce20();
}

void TailsAI_Main_R(task* obj) {
	taskwk* aiDATA = obj->twp;
	char aiID = AIIndex;

	if (FlagDeleteMilesAI)
	{
		FreeTask(obj);
		return;
	}

	if (!playertwp[aiID])
		return;

	if (isFlyTravel)
		CheckAndLoadTailsTravelObjects(obj);

	MilesAI_OnFrames(aiDATA, aiID);

	if (aiDATA->mode == 0) 
	{
		for (uint8_t i = 0; i < MaxPlayers; i++)
		{
			RemoveAttackSolidColFlags(i);
		}
	}


	TailsAI_Main_t.Original(obj);
}

void DisableTailsAICol(char pnum)
{
	if (playertwp[pnum])
		CharColliOff(playertwp[pnum]);
}

void AI_Init(const HelperFunctions& helperFunctions) {

	if (!isRandoActive()) 
	{
		if (isCharSelActive())
			WriteCall((void*)0x415A25, LoadCharacter_r);
		else
			LoadCharacter_t.Hook(LoadCharacterAndAI);

		//Allow Tails AI to spawn in acton stages, hub world, bosses and chao garden + fixes
		WriteJump(CheckTailsAI, CheckTailsAI_R);
		WriteJump(Load2PTails, Load2PTails_r);

		AI_Patches();
		AI_Improvement();
	}

	TailsAI_Main_t.Hook(TailsAI_Main_R);
}