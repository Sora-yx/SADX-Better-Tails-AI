#include "stdafx.h"
#include <SADXModLoader.h>
#include "helper.h"
#include <fstream>

extern bool IsChaoGardenBanned;
extern bool IsHubBanned;
extern bool IsBossBanned;
extern bool IsTCBanned;
bool isAIActive = false;
extern int isTailsFlying;
int FlagAI = 0;
bool isIADeleted = false;
extern bool IsStoryIA;
bool banCharacter[];



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
			{
				return 0; //don't load AI if the player banned Chao Garden.
			}

			if (IsHubBanned && CurrentLevel > LevelIDs_E101R && CurrentLevel < LevelIDs_TwinkleCircuit)
			{
				return 0;  //don't load AI if the player banned Hub World.
			}

			if (IsBossBanned && CurrentLevel > 14 && CurrentLevel < 26)
			{
				return 0;  //don't load AI if the player banned boss fight.
			}

			if (IsTCBanned && CurrentLevel == LevelIDs_TwinkleCircuit)
			{
				return 0;  //don't load AI if the player banned TC.
			}

			if (CurrentCharacter == Characters_Tails)
			{
				return CheckTailsAI(); //Restore original function, don't load Tails AI 
			}


			//Story plot check for coherent AI character spawn (if setting enabled)

			if (IsStoryIA)
			{
				switch (CurrentCharacter)
				{
				case Characters_Sonic:
					if (EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == false)
					{
						return CheckTailsAI(); //don't load Tails until we rescue him.
					}
					if (SonicSkyChaseAct1Clear == 1 && CurrentLevel != LevelIDs_TwinklePark && SonicTPClear == 0)
					{
						return CheckTailsAI(); //don't load Tails after SKy Chase Crash
					}

					if (SonicTPClear == 1 && EventFlagArray[EventFlags_Sonic_Chaos6Clear] == false)
					{
						return CheckTailsAI(); //Don't load any AI after Sonic lost Amy until Post SC2.
					}
					if (EventFlagArray[EventFlags_Sonic_Chaos6Clear] == true && EventFlagArray[EventFlags_SonicAdventureComplete] == false)
					{
						return CheckTailsAI();
					}
					break;
				}
			}

			
			//bug fixes (Disable AI if necessary to avoid crash until I find a better way to do this)

			if (CurrentCharacter == Characters_Sonic)
			{
				switch (CurrentLevel)
				{
				case LevelIDs_StationSquare:
					if (EventFlagArray[EventFlags_TailsUnlockedAdventure] == false && CurrentSong != 76)
					{
						return CheckTailsAI(); //Avoid loading AI during Sonic emerald coast post cutscene. (crash) There is 100% a better way to do this, but I don't have any better flag after tails cutscene for now. :(

					}
					if (EventFlagArray[EventFlags_Sonic_IceCapOpen] == false && EventFlagArray[EventFlags_Sonic_CasinopolisClear] == true)
					{
						return CheckTailsAI(); //Avoid loading AI during Sonic Casino post cutscene. (funny crash)
					}
					break;
				case LevelIDs_MysticRuins:
					if (IceCapFlag == 1 && EventFlagArray[EventFlags_Sonic_Chaos4Clear] == false)
					{
						return CheckTailsAI(); //Fix Knuckles AI behavior.
					}
					if (EventFlagArray[EventFlags_Sonic_EggViperClear] == true && EventFlagArray[EventFlags_SonicAdventureComplete] == false)
					{
						return CheckTailsAI(); //fix funny ending crash
					}
					break;
				case LevelIDs_Casinopolis:
					if (CurrentAct > 1)
					{
						return CheckTailsAI(); //don't load AI if we are in the casino pinball.
					}
					break;
				case LevelIDs_EggCarrierOutside:
					if (EventFlagArray[EventFlags_Sonic_Chaos6Clear] == false && CurrentAct < 3)
					{
						return CheckTailsAI(); //pre Gamma & post Gamma cutscene fix
					}
					break;
				case LevelIDs_EggCarrierInside:
					if (EventFlagArray[EventFlags_Sonic_GammaDefeated] == false)
					{
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


			if (SelectedCharacter == 6) //if Super Sonic Story
			{
				if (CurrentLevel == LevelIDs_MysticRuins)
					return CheckTailsAI(); //fix Super Sonic cutscene crash.
				
				if (CurrentLevel == LevelIDs_PerfectChaos)
					return CheckTailsAI(); //Fight harder for no reason.
			}


			isAIActive = true;
			return 1; //Return Load AI
		
		}

}

ObjectMaster* Load2PTails_r(ObjectMaster* player1) //Custom AI
{

	FlagAI = CheckTailsAI_R();

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

			if (CurrentLevel == LevelIDs_StationSquare && CurrentAct == 3)
			{
				moveAItoPlayer();
			}
			else
			{
				v3->Data1->Position.x = v1->Data1->Position.x - njCos(v1->Data1->Rotation.y) * 30;
				v3->Data1->Position.y = v1->Data1->Position.y;
				v3->Data1->Position.z = v1->Data1->Position.z - njSin(v1->Data1->Rotation.y) * 30;
			}

			v3->Data1->Action = 0;
			dword_3B2A304 = 0;
			return v3;
		}
	}

	isAIActive = false;
	return (ObjectMaster*)0x0;
}

void LoadTails_AI_R() {

	ObjectMaster* obj;
	obj = GetCharacterObject(0);
	ObjectMaster* lastobj = obj;
	ObjectMaster* o2 = nullptr;
	o2 = Load2PTails_r(obj);

	return;
}


void DeleteAI() {

	ControlEnabled = 0;

	if (!isIADeleted)
	{
		ObjectMaster* obj = GetCharacterObject(1);

		if (obj)
			DeleteObject_(obj);
		isIADeleted = true;
		return;
	}

}

//teleport AI to Player 

void moveAItoPlayer() {

	if (isAIActive) //Move AI to player 1
	{
		ObjectMaster* play1 = GetCharacterObject(0);
		ObjectMaster* play2 = GetCharacterObject(1);

		if (play2 != nullptr && play1 != nullptr)
		{
			play2->Data1->Position.x = play1->Data1->Position.x - 7;
			play2->Data1->Position.y = play1->Data1->Position.y;
			play2->Data1->Position.z = play1->Data1->Position.z + 5;
			play2->Data1->Rotation.y = play1->Data1->Rotation.y;
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


//While load result: Teleport AI close to the player and Force Victory Pose.

void DisableTime_R() {

	TimeThing = 0;

	if (isAIActive && CurrentCharacter != Characters_Tails) //Move AI to player 1 if we are not racing.
	{
		ObjectMaster* play1 = GetCharacterObject(0);
		ObjectMaster* AI = GetCharacterObject(1);

		if (AI != nullptr && play1 != nullptr)
		{
			AI->Data1->Position.x = play1->Data1->Position.x - 7;
			AI->Data1->Position.y = play1->Data1->Position.y;
			AI->Data1->Position.z = play1->Data1->Position.z + 5;
			AI->Data1->Rotation.y = play1->Data1->Rotation.y;

			if (AI->Data1->CharID == Characters_Tails && isAIActive == true && CurrentCharacter != Characters_Tails)
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
		if (CurrentLevel == LevelIDs_StationSquare && CurrentAct == 3)
			return;

	//Check if AI is not deleted or banned.

		if (CurrentLevel > LevelIDs_E101R && CurrentLevel < LevelIDs_TwinkleCircuit)
			moveAItoPlayer(); //teleport AI
	}

	return;
}

void FixAIHubTransition2() {

	HudDisplayRingTimeLife_Check();
	//Check if AI is not deleted or banned.

	if (!IsHubBanned && isAIActive)
	{
		
		if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1))
		{
			moveAItoPlayer(); //teleport AI
		}
	}

}

void LoadCharacter_r()
{

	if (banCharacter[CurrentCharacter] != true)
		LoadTails_AI_R(); 

	LoadCharacter(); //original function

	return;
}

void AI_Init() {

	WriteCall((void*)0x47ed8e, CheckTailsAI_R);
	WriteCall((void*)0x47e943, CheckTailsAI_R);
	WriteCall((void*)0x47ea46, CheckTailsAI_R);
	WriteCall((void*)0x47ec62, CheckTailsAI_R);

	WriteData<5>((void*)0x415948, 0x90); //remove the original load2PTails in LoadCharacter as we use a custom one.

	//Tails AI Fixes and small optimization/improvement.
	WriteCall((void*)0x4151ba, FixAIHubTransition); //Fix AI position when you change act in hub world.
	WriteCall((void*)0x417588, FixAIHubTransition2); //Fix AI position when you change act in hub world.

	WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill 
	WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
	WriteCall((void*)0x4e9664, LoadAISnowBoard_R);
}

