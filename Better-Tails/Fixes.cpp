#include "stdafx.h"




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
	if (CurrentCharacter == Characters_Sonic && EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == false)
	{
		if (!IsStoryIA && !isAIActive)
		{
			ForceAI = true;
			LoadTails_AI_R();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Tails Rescued Cutscene
	if (CurrentCharacter == Characters_Sonic && EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == true && EventFlagArray[EventFlags_Sonic_EggHornetClear] == false)
	{
		if (!isAIActive)
		{
			ForceAI = true;
			LoadTails_AI_R();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Sonic Tails Post Casino Cutscene
	if (CurrentCharacter == Characters_Sonic && AICutsceneOk && EventFlagArray[EventFlags_Sonic_CasinopolisClear] == true && EventFlagArray[EventFlags_Sonic_IceCapOpen] == false)
	{
		if (!isAIActive)
		{
			ForceAI = true;
			LoadTails_AI_R();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Sonic Amy cutscenes 
	if (CurrentCharacter == Characters_Sonic && (SonicSkyChaseAct1Clear == 1 && SonicTPClear == 0) || (SonicTPClear == 1 && EventFlagArray[EventFlags_Sonic_RedMountainClear] == false))
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
	if (CurrentCharacter == Characters_Sonic && EventFlagArray[EventFlags_Sonic_Chaos4Clear] == true && SonicSkyChaseAct1Clear == 0)
	{
		if (isAIActive && TailsAI_ptr != 0)
		{
			AICutsceneOk = 0;
			moveAItoPlayer();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	//Sonic loses Amy Cutscene
	if (CurrentCharacter == Characters_Sonic && EventFlagArray[EventFlags_Sonic_SpeedHighwayClear] == true && EventFlagArray[EventFlags_Sonic_RedMountainClear] == false)
	{
		if (isAIActive && !IsStoryIA && TailsAI_ptr != 0)
		{
			AICutsceneOk = 0;
			moveAItoPlayer();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	//post Chaos 6 scene && post lw cutscene
	if (CurrentCharacter == Characters_Sonic && (EventFlagArray[EventFlags_Sonic_Chaos6Clear] == true && EventFlagArray[EventFlags_Sonic_LostWorldClear] == false) || (EventFlagArray[EventFlags_Sonic_LostWorldClear] == true && EventFlagArray[EventFlags_Sonic_FinalEggClear] == false))
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

	if (CurrentCharacter == Characters_Sonic && EventFlagArray[EventFlags_Sonic_SkyDeckClear] == false && SonicSkyChaseAct2Clear == 1)
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

//Sphere check functions
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return sqrtf(powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2));
}

bool IsPointInsideSphere(NJS_VECTOR* center, NJS_VECTOR* pos, float radius) {
	return GetDistance(center, pos) <= radius;
}

int IsPlayerInsideSphere_(NJS_VECTOR* center, float radius) {
	for (uint8_t player = 0; player < MaxPlayers; ++player) {
		if (!EntityData1Ptrs[player]) continue;

		NJS_VECTOR* pos = &EntityData1Ptrs[player]->Position;
		if (IsPointInsideSphere(center, pos, radius)) {
			return player + 1;
		}
	}

	return 0;
}

bool IsSpecificPlayerInSphere(NJS_VECTOR* center, float radius, uint8_t player) {
	return IsPlayerInsideSphere_(center, radius) == player + 1;
}


//Prevent Tails AI to accidentally activate trigger originally made for Player 1.
void BlockTailsAI(ObjectMaster* obj) { 

	ObjectMaster* P1 = GetCharacterObject(0);
	ObjectMaster* P2 = GetCharacterObject(1);

	if (P2 != nullptr)
	{
		if (IsSpecificPlayerInSphere, (&obj->Data1->Position, 10, 1) && IsSpecificPlayerInSphere, (&obj->Data1->Position, 10, 0))
		{
			P2->Data1->Action = 120; //Block Tails AI
		}
		else
		{
			P2->Data1->Action = 1;
			return;
		}
	}

	return;
}

static Trampoline OTaraiChild_Main_t((int)OTarai, (int)OTarai + 0x5, OTaraiChild_Main_r);

//static Trampoline OTaraiChild_Main_t((int)OTaraiChild_Main, (int)OTaraiChild_Main + 0x6, OTaraiChild_Main_r);

void OTaraiChild_Main_r(ObjectMaster* obj) {

	BlockTailsAI(obj);

	ObjectFunc(origin, OTaraiChild_Main_t.Target());
	origin(obj);
}


void CheckAndDeleteAI() {

	if (isAIActive && TailsAI_ptr != 0)
	{
		AICutsceneOk = 0;
		DeleteTailsAI();

	}

	isAIActive = false;
	return DisableControl();
}

void DeleteTailsAI() {
	if (EntityData1Ptrs[1] != nullptr)
	{
		ObjectMaster* AI = GetCharacterObject(1);
		EntityData1* P2 = EntityData1Ptrs[1];
		if (P2->CharID == Characters_Tails) {
			TailsAI_Delete(AI);
			CheckThingButThenDeleteObject(AI); //delete AI completely
		}
	}
}

void AI_Fixes() {

	//Tails AI Fixes and small optimization/improvement.
	WriteCall((void*)0x4151ba, FixAIHubTransition); //Fix AI position when you change act in hub world.
	WriteCall((void*)0x417588, FixAIHubTransition2);

	if (!IsHubBanned) {
		WriteCall((void*)0x42f72d, CallTailsAI_R); //Manually Call Tails AI After few early Cutscene to avoid crash.
		WriteCall((void*)0x6cd3de, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene SonicAndTails_WakeUP
		WriteCall((void*)0x42f78c, CallTailsAI_R2);  //Move Tails to Sonic Position after Chaos 4 fight. Also call Tails AI in Super Sonic Story
		WriteJump((void*)0x657c4a, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene SonicAndTails_LandEggCarrier
		WriteCall((void*)0x42f747, CallTailsAI_R3); //Manually Call Tails AI After the cutscene SonicAndTails_LandEggCarrier
		WriteCall((void*)0x664f68, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene Tails Find Sonic (Super Sonic cutscene)

		WriteCall((void*)0x65f82f, CheckAndDeleteAI); //Remove Tails before "Sonic finds Knuckles cutscene" (super sonic)
		WriteCall((void*)0x663d4a, CheckAndDeleteAI); //Remove Tails before "Sonic and Tails find tornado 2 cutscene" (super sonic)
		WriteCall((void*)0x6601ab, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene Sonic_WakeUP (super sonic cutscene)
	}
}
