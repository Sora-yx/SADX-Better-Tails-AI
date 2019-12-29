#include "stdafx.h"
#include <SADXModLoader.h>
#include "helper.h"
#include <fstream>

extern bool IsChaoGardenBanned;
extern bool IsHubBanned;
extern bool IsBossBanned;
extern bool IsTCBanned;
bool isAIActive = false;
int FlagAI = 0;
extern bool IsStoryIA;
bool banCharacter[];
bool ForceAI = false;
int AICutsceneOk = 0;

//If you are reading this, I apologize for this mess, LOL.

ObjectMaster* LoadCharObj(int i)
{
	//setup AI correctly
	ObjectMaster* obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Tails_Main);
	obj->Data1->CharID = Characters_Tails;
	obj->Data1->CharIndex = (char)1;
	EntityData1Ptrs[1] = (EntityData1*)obj->Data1;
	EntityData2Ptrs[1] = (EntityData2*)obj->Data2;
	return obj;
}

//Tails AI Flag Check

int CheckTailsAI_R(void) {

	isAIActive = false;

	if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2 || CurrentLevel == LevelIDs_ChaoRace)
		{
			return CheckTailsAI(); //don't load AI
		}
		else
		{
			//Player Settings

			if (IsChaoGardenBanned && CurrentLevel > LevelIDs_SandHill && CurrentLevel < LevelIDs_ChaoRace)
				return 0; //don't load AI if the player banned Chao Garden.
			
			if (IsHubBanned && CurrentLevel > LevelIDs_E101R && CurrentLevel < LevelIDs_TwinkleCircuit)
				return 0;  //don't load AI if the player banned Hub World.

			if (IsBossBanned && CurrentLevel > 14 && CurrentLevel < 26)
				return 0;  //don't load AI if the player banned boss fight.

			if (IsTCBanned && CurrentLevel == LevelIDs_TwinkleCircuit)
				return 0;  //don't load AI if the player banned TC.

			if (CurrentCharacter == Characters_Tails)
				return CheckTailsAI(); //Restore original function, don't load Tails AI 

			if (CurrentChaoStage == 1) 
				return 0; //don't load AI during Chao Race

			if (CurrentLevel == LevelIDs_EggCarrierInside && CurrentAct == 5)
				return 0;

			if (CurrentLevel == LevelIDs_Casinopolis && CurrentAct > 1)
				return CheckTailsAI(); //don't load AI if we are in the casino pinball.

			//Story plot check for coherent Tails AI spawn (if setting enabled)

			if (IsStoryIA)
			{
				switch (CurrentCharacter)
				{
				case Characters_Sonic:
					if (EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == false)
						return CheckTailsAI(); //don't load Tails until we rescue him.

					if (SonicSkyChaseAct1Clear == 1 && EventFlagArray[EventFlags_Sonic_RedMountainClear] == false)
					{
						AICutsceneOk = 0;  //don't load Tails after Sky Chase Crash until Sonic and Tails land on the Egg Carrier.
						return 0x0;
					}

					if (EventFlagArray[EventFlags_Sonic_GammaDefeated] == true && EventFlagArray[EventFlags_SonicAdventureComplete] == false)
						return CheckTailsAI(); //Don't load Tails after Gamma Defeated until we beat the game.

					break;
				default:
					return CheckTailsAI();
				}
			}

			
			//bug fixes (Disable AI if necessary to avoid crash until I find a better way to do this)

			if (CurrentCharacter == Characters_Sonic)
			{
				switch (CurrentLevel)
				{
				case LevelIDs_StationSquare:
					if (EventFlagArray[EventFlags_TailsUnlockedAdventure] == false && CurrentSong != 76)
						return CheckTailsAI(); //Avoid loading AI during Sonic emerald coast post cutscene. (crash) There is 100% a better way to do this, but I don't have any better flag after tails cutscene for now. :(

					if (!AICutsceneOk && EventFlagArray[EventFlags_Sonic_CasinopolisClear] == true && EventFlagArray[EventFlags_Sonic_IceCapOpen] == false && EventFlagArray[EventFlags_Sonic_WindyValleyClear] == true)
						return CheckTailsAI(); //Avoid loading AI during Sonic Casino post cutscene. (funny crash)

					break;
				case LevelIDs_MysticRuins:
					if (IceCapFlag == 1 && EventFlagArray[EventFlags_Sonic_Chaos4Clear] == false)
						return CheckTailsAI(); //Fix Knuckles AI behavior.

					if (EventFlagArray[EventFlags_Sonic_EggViperClear] == true && EventFlagArray[EventFlags_SonicAdventureComplete] == false)
						return CheckTailsAI(); //fix funny ending crash
					
					break;
				case LevelIDs_RedMountain:
					if (EventFlagArray[EventFlags_Sonic_RedMountainClear] == false && CurrentAct >= 1)
						return CheckTailsAI(); //Tornado cutscene
		
					break;
				case LevelIDs_EggCarrierOutside:
					if (!AICutsceneOk && EventFlagArray[EventFlags_Sonic_Chaos6Clear] == false)
						return CheckTailsAI(); //pre Gamma & post Gamma cutscene fix

					if (EventFlagArray[EventFlags_Sonic_GammaDefeated] == false && EventFlagArray[EventFlags_Sonic_SkyDeckClear] == true)
						return CheckTailsAI(); //pre Gamma & post Gamma cutscene fix
					break;
				case LevelIDs_EggCarrierInside:
					if (EventFlagArray[EventFlags_Sonic_GammaDefeated] == false)
					{
						AICutsceneOk = 0;
						return CheckTailsAI(); //Post Sky Deck cutscene crash fix
					}
					break;
				}
			}

			if (CurrentCharacter == Characters_Knuckles)
			{
				if (EventFlagArray[EventFlags_Knuckles_Chaos4Clear] == false && CurrentLevel == LevelIDs_MysticRuins)
					return CheckTailsAI(); //fix Sonic AI fight
			}

			if (CurrentCharacter == Characters_Amy)
			{
				if (CurrentLevel == LevelIDs_StationSquare || CurrentLevel == LevelIDs_EggCarrierOutside)
					if (EventFlagArray[EventFlags_Amy_FinalEggClear] == false && EventFlagArray[EventFlags_Amy_HotShelterClear] == true)
						return CheckTailsAI(); //fix Sonic AI fight
			}

			if (CurrentCharacter == Characters_Big)
			{
				switch (CurrentLevel)
				{
				case LevelIDs_MysticRuins:
					if (CurrentAct == 0)
						if (EventFlagArray[EventFlags_Big_EmeraldCoastClear] == false)
							return CheckTailsAI(); //fix Sonic AI fight
					break;
				case LevelIDs_StationSquare:
					if (CurrentAct == 2)
						if (EventFlagArray[EventFlags_Big_TwinkleParkClear] == false)
							return CheckTailsAI(); //fix Sonic AI fight
					break;
				case LevelIDs_EmeraldCoast:
						if (EventFlagArray[EventFlags_Big_HotShelterClear] == false)
							return CheckTailsAI(); //fix Sonic AI fight
					break;
				}
			}

			if (CurrentCharacter == Characters_Gamma)
			{
				switch (CurrentLevel)
				{
				case LevelIDs_EggCarrierOutside:
						if (EventFlagArray[EventFlags_Gamma_E101mkIIClear] == false)
							return CheckTailsAI(); //fix Sonic AI fight
					break;
				}
			}

			if (SelectedCharacter == 6) //if Super Sonic Story
			{
				if (!AICutsceneOk && CurrentLevel == LevelIDs_MysticRuins)
					return CheckTailsAI(); //fix Super Sonic cutscene crash.

				if (IsStoryIA && CurrentLevel == LevelIDs_Past)
					return CheckTailsAI(); //Don't load Tails in the past if story option is enabled.
				
				if (CurrentLevel == LevelIDs_PerfectChaos)
					return CheckTailsAI(); //Fight harder, for no reason.
			}

			isAIActive = true;
			return 1; //Return Load AI
		}
}


ObjectMaster* Load2PTails_r(ObjectMaster* player1) //Custom AI
{
	if (!ForceAI)
		FlagAI = CheckTailsAI_R();
	else
		FlagAI = 1;


	if (FlagAI != 1)
	{
		isAIActive = false;
		return (ObjectMaster*)0x0;
	}
	else
	{
		ObjectMaster* v1 = LoadObject(LoadObj_Data1, 0, TailsAI_Main); //load AI moveset (basically?) 
		TailsAI_ptr = v1;

		if (v1)
		{
			v1->Data1->CharID = (char)Characters_Tails;
			v1->Data1->CharIndex = 1;
			v1->DeleteSub = TailsAI_Delete;

			ObjectMaster* v3 = LoadCharObj(1); //load AI 
			isAIActive = true;
			ForceAI = false;
			v3->Data1->Position.x = v1->Data1->Position.x - njCos(v1->Data1->Rotation.y) * 30;
			v3->Data1->Position.y = v1->Data1->Position.y;
			v3->Data1->Position.z = v1->Data1->Position.z - njSin(v1->Data1->Rotation.y) * 30;
			
			v3->Data1->Action = 0;
			dword_3B2A304 = 0;
			return v3;
		}
	}

	isAIActive = false;
	return nullptr;
}

void LoadTails_AI_R() {

	ObjectMaster* obj;
	obj = GetCharacterObject(0);
	ObjectMaster* lastobj = obj;
	ObjectMaster* o2 = nullptr;
	o2 = Load2PTails_r(obj);

	return;
}

//Reset value when Tails AI is deleted
void TailsAI_ResetValue() {

	isAIActive = false;
	ForceAI = false;
	return FUN_0042ce20();
}

//Reset value when the player quit or soft reset
void SoftReset_R() {

	ForceAI = false;
	isAIActive = false;
	AICutsceneOk = 0;
	FUN_00412ad0();
}



//Manually Call Tails Ai when necessary.
void CallTailsAI_R() {

	//Tails Crash Cutscene
	if (CurrentCharacter == 0 && EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == false)
	{
		if (!IsStoryIA && !isAIActive)
		{
			ForceAI = true;
			LoadTails_AI_R();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Tails Rescued Cutscene
	if (CurrentCharacter == 0 && EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == true && EventFlagArray[EventFlags_Sonic_EggHornetClear] == false)
	{
		if (!isAIActive)
		{
			ForceAI = true;
			LoadTails_AI_R();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Sonic Tails Post Casino Cutscene
	if (CurrentCharacter == 0 && AICutsceneOk && EventFlagArray[EventFlags_Sonic_CasinopolisClear] == true && EventFlagArray[EventFlags_Sonic_IceCapOpen] == false)
	{
		if (!isAIActive)
		{
			ForceAI = true;
			LoadTails_AI_R();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Sonic Amy cutscenes 
	if (CurrentCharacter == 0 && (SonicSkyChaseAct1Clear == 1 && SonicTPClear == 0) || (SonicTPClear == 1 && EventFlagArray[EventFlags_Sonic_RedMountainClear] == false))
	{
		if (isAIActive && !IsStoryIA && TailsAI_ptr != 0)
		{
			AICutsceneOk = 0;
			moveAItoPlayer();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	return PlayMusic(MusicIDs_s_square);
}

void CallTailsAI_R2() {

	//Sonic Tails Post Chaos 4 Cutscene
	if (CurrentCharacter == 0 && EventFlagArray[EventFlags_Sonic_Chaos4Clear] == true && SonicSkyChaseAct1Clear == 0)
	{
		if (isAIActive && TailsAI_ptr != 0)
		{
			AICutsceneOk = 0;
			moveAItoPlayer();
			return PlayMusic(MusicIDs_mstcln);
		}
	}	

	//Sonic loses Amy Cutscene
	if (CurrentCharacter == 0 && EventFlagArray[EventFlags_Sonic_SpeedHighwayClear] == true && EventFlagArray[EventFlags_Sonic_RedMountainClear] == false)
	{
		if (isAIActive && !IsStoryIA && TailsAI_ptr != 0)
		{
			AICutsceneOk = 0;
			moveAItoPlayer();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	//post Chaos 6 scene && post lw cutscene
	if (CurrentCharacter == 0 && (EventFlagArray[EventFlags_Sonic_Chaos6Clear] == true && EventFlagArray[EventFlags_Sonic_LostWorldClear] == false) || (EventFlagArray[EventFlags_Sonic_LostWorldClear] == true && EventFlagArray[EventFlags_Sonic_FinalEggClear] == false))
	{
		if (isAIActive && !IsStoryIA && TailsAI_ptr != 0)
		{
			moveAItoPlayer();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	//Super Sonic Story Cutscene
	if (SelectedCharacter == 6 && AICutsceneOk && CurrentLevel == LevelIDs_MysticRuins)
	{
		if (!isAIActive)
		{
			ForceAI = true;
			LoadTails_AI_R();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	return PlayMusic(MusicIDs_mstcln);
}

void CallTailsAI_R3() {


	if (CurrentCharacter == 0 && EventFlagArray[EventFlags_Sonic_SkyDeckClear] == false && SonicSkyChaseAct2Clear == 1)
	{
		if (!isAIActive)
		{
			ForceAI = true;
			LoadTails_AI_R();
			return PlayMusic(MusicIDs_egcarer1);
		}
		else
		{
			moveAItoPlayer();
			return PlayMusic(MusicIDs_egcarer1);
		}
	}

	return PlayMusic(MusicIDs_egcarer1);
}

void AllowTailsAI_R()
{
	AICutsceneOk = 1;
	return EnableControl();
}



//teleport AI to Player 

void moveAItoPlayer() {

	if (isAIActive) //Move AI to player 1
	{
		ObjectMaster* play1 = GetCharacterObject(0);
		ObjectMaster* play2 = GetCharacterObject(1);

		if (play2 != nullptr && play1 != nullptr)
		{
			if (CurrentCharacter != Characters_Big && CurrentCharacter != Characters_Gamma)
			{
				play2->Data1->Position.x = play1->Data1->Position.x - 7;
				play2->Data1->Position.y = play1->Data1->Position.y;
				play2->Data1->Position.z = play1->Data1->Position.z + 5;
				play2->Data1->Rotation.y = play1->Data1->Rotation.y;

			}
			else
			{
				play2->Data1->Position.x = play1->Data1->Position.x - 10;
				play2->Data1->Position.y = play1->Data1->Position.y;
				play2->Data1->Position.z = play1->Data1->Position.z + 8;
				play2->Data1->Rotation.y = play1->Data1->Rotation.y;

			}
		}
	}
}


//Tails AI Snowboard

void LoadAISnowBoard_R() {

	ForcePlayerAction(0, 0x2c);

	//Load Tails AI Snowboard when playing Sonic
	if (CurrentCharacter == Characters_Sonic && isAIActive) 
	{
			ForcePlayerAction(1, 44); //Force AI to use Snowboard
			ObjectMaster* v1 = LoadObject((LoadObj)(LoadObj_Data1 | LoadObj_Data2), 2, Snowboard_Tails_Load);
			if (v1 != nullptr)
			{
				v1->Data1->CharID = 1;
				v1->Data1->CharIndex = 1;
				return;
			}
	}
}

void LoadCharacter_r()
{

	if (banCharacter[CurrentCharacter] != true)
		LoadTails_AI_R();

	LoadCharacter(); //call original function

	return;
}

//While load result: Teleport AI close to the player and Force Victory Pose.

void DisableTime_R() {

	TimeThing = 0;

	if (isAIActive && CurrentCharacter != Characters_Tails) 
	{
		ObjectMaster* play1 = GetCharacterObject(0);
		ObjectMaster* AI = GetCharacterObject(1);

		if (AI != nullptr && play1 != nullptr) //Move AI to player 1 
		{
			AI->Data1->Position.x = play1->Data1->Position.x - 7;
			AI->Data1->Position.y = play1->Data1->Position.y;
			AI->Data1->Position.z = play1->Data1->Position.z + 5;
			AI->Data1->Rotation.y = play1->Data1->Rotation.y;

			if (CurrentCharacter != Characters_Tails)
				if (AI->Data1->CharID == Characters_Tails && isAIActive == true)
				{
					SetTailsRaceVictory(); //Fix Tails AI victory animation
				}

			ForcePlayerAction(1, 19); //Force AI to Victory pose

			dword_3B2A304 = 0;
		}

	}
}


//Fix AI Start Position in hub world

void FixAIHubTransition() {

	ForcePlayerAction(0, 0x18);

	if (!IsHubBanned && isAIActive)
	{
		if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1))
			return;

		if (CurrentLevel > LevelIDs_E101R && CurrentLevel < LevelIDs_TwinkleCircuit)
			moveAItoPlayer(); //teleport AI
	}

	return;
}

void FixAIHubTransition2() {

	HudDisplayRingTimeLife_Check();

	if (!IsHubBanned && isAIActive) 
	{
		if (CurrentCharacter != Characters_Big)
		{
			if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1) || CurrentLevel >= LevelIDs_ECGarden && CurrentLevel < LevelIDs_ChaoRace)
				moveAItoPlayer(); //teleport AI
		}
		else
		{
			if (CurrentLevel == LevelIDs_StationSquare && CurrentAct == 1 || CurrentLevel >= LevelIDs_ECGarden && CurrentLevel < LevelIDs_ChaoRace)
				moveAItoPlayer(); //teleport AI
		}

	}

	return;
}

void DeleteAI() {

	if (isAIActive && TailsAI_ptr != 0)
	{
		AICutsceneOk = 0;
		ObjectMaster* AI = GetCharacterObject(1);
		if (AI != nullptr)
		{
			TailsAI_Delete(AI); //delete ptr
			DeleteObjectMaster(AI); //delete AI completely
		}
	}

	isAIActive = false;
	return DisableControl();
}


void AI_Init() {

	//Allow Tails AI to spawn in acton stages, hub world, bosses and chao garden + fixes
	WriteCall((void*)0x47ed8e, CheckTailsAI_R);
	WriteCall((void*)0x47e943, CheckTailsAI_R);
	WriteCall((void*)0x47ea46, CheckTailsAI_R);
	WriteCall((void*)0x47ec62, CheckTailsAI_R);

	WriteData<5>((void*)0x415948, 0x90); //remove the original load2PTails in LoadCharacter as we use a custom one.


	//Tails AI Fixes and small optimization/improvement.
	WriteCall((void*)0x4151ba, FixAIHubTransition); //Fix AI position when you change act in hub world.
	WriteCall((void*)0x417588, FixAIHubTransition2);

	WriteCall((void*)0x42f72d, CallTailsAI_R); //Manually Call Tails AI After few early Cutscene to avoid crash.

	WriteCall((void*)0x6cd3de, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene SonicAndTails_WakeUP
	
	WriteCall((void*)0x42f78c, CallTailsAI_R2);  //Move Tails to Sonic Position after Chaos 4 fight. Also call Tails AI in Super Sonic Story


	WriteJump((void*)0x657c4a, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene SonicAndTails_LandEggCarrier
	WriteCall((void*)0x42f747, CallTailsAI_R3); //Manually Call Tails AI After the cutscene SonicAndTails_LandEggCarrier

	WriteCall((void*)0x664f68, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene Tails Find Sonic (Super Sonic cutscene)
	
	WriteCall((void*)0x65f82f, DeleteAI); //Remove Tails before "Sonic finds Knuckles cutscene" (super sonic)
	WriteCall((void*)0x663d4a, DeleteAI); //Remove Tails before "Sonic and Tails find tornado 2 cutscene" (super sonic)
	WriteCall((void*)0x6601ab, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene Sonic_WakeUP (super sonic cutscene)
	

	WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill 
	WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
	WriteCall((void*)0x4e9664, LoadAISnowBoard_R);
}

