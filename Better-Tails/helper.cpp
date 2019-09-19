#include "stdafx.h"
#include <SADXModLoader.h>
#include "helper.h"

extern bool IsChaoGardenBanned;
extern bool IsHubBanned;
extern bool IsBossBanned;


//Tails AI Check

int CheckTailsAI_R(void) {

	if (CurrentCharacter == Characters_Tails)
	{
		return CheckTailsAI(); //Restore original function, don't load Tails AI 
	}
	else
	{
		if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2 || CurrentLevel == LevelIDs_ChaoRace)
		{
			return 0; //don't load Tails AI 
		}
		else
		{
			//Player Settings

			if (IsChaoGardenBanned && CurrentLevel > LevelIDs_SandHill && CurrentLevel < LevelIDs_ChaoRace)
			{
				return 0; //don't load Tails AI if the player banned Chao Garden.
			}

			if (IsHubBanned && CurrentLevel > 25 && CurrentLevel < 35)
			{
				return 0;  //don't load Tails AI if the player banned Hub World.
			}

			if (IsBossBanned && CurrentLevel > 14 && CurrentLevel < 26)
			{
				return 0;  //don't load Tails AI if the player banned boss fight.
			}

			//Bug fixes

			if (CurrentLevel == LevelIDs_EggCarrierInside && CurrentAct == 5)
			{
				return 0; //avoid loading Tails in the Egg Carrier Chao Garden entrance room to avoid crazy shit.
			}

			

			if (CurrentCharacter == Characters_Sonic)
			{
				switch (CurrentLevel)
				{
				case LevelIDs_StationSquare:
					if (EventFlagArray[EventFlags_TailsUnlockedAdventure] == false && CurrentSong != 76)
					{
						return CheckTailsAI(); //Avoid loading Tails during Sonic emerald coast post cutscene. (crash) There is 100% a better way to do this, but I don't have any better flag after tails cutscene for now. :(
					}

					if (EventFlagArray[EventFlags_Sonic_IceCapOpen] == false && EventFlagArray[EventFlags_Sonic_CasinopolisClear] == true)
					{
						return CheckTailsAI(); //Avoid loading Tails during Sonic Casino post cutscene. (funny crash)
					}
					break;
				case LevelIDs_MysticRuins:
					if (IceCapFlag == 1 && EventFlagArray[EventFlags_Sonic_Chaos4Clear] == false)
					{
						return CheckTailsAI(); //Fix Knuckles AI Crash.
					}
					if (EventFlagArray[EventFlags_Sonic_EggViperClear] == true && EventFlagArray[EventFlags_SonicAdventureComplete] == false)
					{
						return CheckTailsAI(); //fix funny ending crash
					}
					break;
				case LevelIDs_EggCarrierOutside:
					if (EventFlagArray[EventFlags_Sonic_Chaos6Clear] == false && CurrentAct < 3)
					{
						return CheckTailsAI(); //pre Gamma & post gamma cutscene fix
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
				{
					return CheckTailsAI(); //fix Sonic AI fight
				}
			}

				return 1; //Load Tails AI
		}

	}
}


//Tails AI Snowboard

void LoadAISnowBoard_R() {

	ForcePlayerAction(0, 0x2c);

	//Load Tails AI Snowboard when playing Sonic
	if (CurrentCharacter == Characters_Sonic) 
	{
			ForcePlayerAction(1, 44); //Force AI to use Snowboard
			ObjectMaster* v1 = LoadObject((LoadObj)(LoadObj_Data1 | LoadObj_Data2), 2, Snowboard_Tails_Load);
			if (v1)
			{
				v1->Data1->CharID = 1;
				v1->Data1->CharIndex = 1;
				return;
			}
	}
}


//While load result: Force Tails AI to Victory Pose.

void DisableTime_R() {

	TimeThing = 0;

	if (CurrentCharacter != Characters_Tails) 
	{
		SetTailsRaceVictory(); //Fix Tails AI victory animation
		ForcePlayerAction(1, 19); //Force AI to Victory pose
		
	}
}


//Load Tails AI when not Sonic or Tails.

void LoadTails_AI_R() {

	SetFrameRateMode(1, 1);

	if (CurrentCharacter == Characters_Sonic || CurrentCharacter == Characters_Tails)
	{
		return;
	}
	else
	{
	if ((GameMode != GameModes_Movie) && (GameMode != GameModes_Credits))
	{
	
			if (CurrentCharacter == Characters_Knuckles)
			{
				if (EventFlagArray[EventFlags_Knuckles_Chaos4Clear] == false && CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 0)
				{
					return;
				}
			}

			LoadObject(LoadObj_Data1, 0, Load2PTails);
			return;
		}

	}

}


