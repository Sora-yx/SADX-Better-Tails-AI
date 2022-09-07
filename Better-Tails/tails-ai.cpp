#include "stdafx.h"

#define ReplaceSound(C, D) helperFunctions.ReplaceFile("system\\sounddata\\se\\" C ".dat", "system\\" D ".dat")

bool isAIActive = false;
bool FlagDeleteMilesAI = false;
int FlagAI = 0;

static FunctionHook<void, task*> TailsAI_Main_t((intptr_t)TailsAI_Main);
static FunctionHook<void> LoadCharacter_t(LoadCharacter);

void DeleteMilesAI()
{
	char id = getAI_ID();

	if (id > 0) {
		FlagDeleteMilesAI = true;
		auto AI = playertp[id];
		FreeTask(AI);
	}
}


//Tails AI Flag Check
int CheckTailsAI_R(void) {
	bool isSA2Mod = GetModuleHandle("sadx-sa2-mod");

	if (isMultiEnabled())
	{
		return 0x0;
	}

	if (NPCMilesStandByFlag || CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2 || !isSA2Mod && CurrentLevel == LevelIDs_ChaoRace) {
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
		else {
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

	if (CurrentLevel == LevelIDs_PerfectChaos)
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

	if (TailsAI_ptr != nullptr)
	{
		return;
	}
	
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

void LoadCharacterAndAI() {

	PrintDebug("Load Character..\n");

	if (isFlyTravel)
		CheckAndLoadMapPVM();

	if (banCharacter[CurrentCharacter] != true && !playertwp[AIIndex])
		Load2PTails_r();

	if (isCharSelActive()) {
		return LoadCharacter_r();
	}

	return LoadCharacter_t.Original();
}


void MilesAI_OnFrames(unsigned char playerID) { //Only run when TailsAI_Main is active

	if (!IsIngame() || !playertwp[0] || !playertwp[playerID] || playertwp[playerID]->charID != Characters_Tails || !TailsAI_ptr)
		return;

	Miles_AbilitiesOnFrames(playerID);

	if (isRescueAllowed)
		CheckMilesBossRescue(playerID);
}

//Reset value when Tails AI is deleted
void TailsAI_Delete_r(task* obj) {

	TailsAI_ptr = nullptr;
	AIIndex = 1;
	rngDeathZoneRescue = 0;
	isChaoPetByAI = false; //just to be safe
	isAIActive = false;
	isRescued = false;
	disableCol = false;
	FlagDeleteMilesAI = false;
	rngRegularDeathRescue = 0;
	return FUN_0042ce20();
}

void TailsAI_Main_R(task* obj) {

	taskwk* data = obj->twp;
	char pid = AIIndex;

	if (FlagDeleteMilesAI)
	{
		FreeTask(obj);
		return;
	}

	if (!playertwp[pid])
		return;

	if (isFlyTravel)
		CheckAndLoadTailsTravelObjects(obj);

	MilesAI_OnFrames(pid);

	//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "AI Distance: % f", getMilesDistance(playertwp[0], playertwp[AIIndex]));

	if (data->mode == 0) {
		RemovePlayerCollision(pid);
	}

	TailsAI_Main_t.Original(obj);
}

		
void AI_Init(const HelperFunctions& helperFunctions) {

	if (!isRandoActive()) {

		LoadCharacter_t.Hook(LoadCharacterAndAI);

		//Allow Tails AI to spawn in acton stages, hub world, bosses and chao garden + fixes
		WriteJump(CheckTailsAI, CheckTailsAI_R);
		WriteJump(Load2PTails, Load2PTails_r);

		AI_Patches();

		AI_Improvement();
	}

	TailsAI_Main_t.Hook(TailsAI_Main_R);
}