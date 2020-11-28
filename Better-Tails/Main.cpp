#include "stdafx.h"

#define ReplaceSound(C, D) helperFunctions.ReplaceFile("system\\sounddata\\se\\" C ".dat", "system\\" D ".dat")

bool isAIActive = false;
int FlagAI = 0;
bool ForceAI = false;
int AICutsceneOk = 0;


MilesAI_Spawn TailsArray[] {
	{ Characters_Sonic, LevelIDs_StationSquare, EventFlags_Sonic_EmeraldCoastClear, 0x006 },
	{ Characters_Sonic, LevelIDs_StationSquare, EventFlags_Sonic_CasinopolisClear, 0x009 },
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


bool isTailsAIAllowed() {

	for (uint8_t i = 0; i < LengthOfArray(TailsArray); i++) {

		if (CurrentCharacter == TailsArray[i].curCharacter && CurrentLevel == TailsArray[i].curLevel && !GetCutsceneFlagArray(TailsArray[i].cutsceneFlag))
		{
			if (EventFlagArray[TailsArray[i].eventFlag] == 1)
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
	if (IsChaoGardenBanned && CurrentLevel >= LevelIDs_SSGarden && CurrentLevel <= LevelIDs_MRGarden)
		return 0; 
	
	if (IsHubBanned && CurrentLevel >= LevelIDs_StationSquare && CurrentLevel <= LevelIDs_Past || IsBossBanned && CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel <= LevelIDs_E101R)
		return 0;  

	if (IsTCBanned && CurrentLevel == LevelIDs_TwinkleCircuit || CurrentCharacter == Characters_Tails || CurrentChaoStage == 1)
		return 0;  

	//General Place where we don't want AI to spawn
	if (CurrentLevel == LevelIDs_EggCarrierInside && CurrentAct == 5 || CurrentLevel == LevelIDs_Casinopolis && CurrentAct > 1)
		return 0;

	//Story plot check for coherent Tails AI spawn (if setting enabled)
	if (IsStoryIA)
	{
		if (CurrentCharacter == Characters_Sonic) {
			if (EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == 0 || (EventFlagArray[EventFlags_Sonic_GammaDefeated] == 1 && EventFlagArray[EventFlags_SonicAdventureComplete] == 0))
				return 0x0; //don't load Tails until we rescue him and don't load him after gamma's defeat

			if (SonicSkyChaseAct1Clear == 1 && EventFlagArray[EventFlags_Sonic_RedMountainClear] == 0)
			{
				AICutsceneOk = 0;  //don't load Tails after Sky Chase Crash until Sonic and Tails land on the Egg Carrier.
				return 0x0;
			}
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
		if (!AICutsceneOk && CurrentLevel == LevelIDs_MysticRuins)
			return 0x0; //fix Super Sonic cutscene crash.

		if (IsStoryIA && CurrentLevel == LevelIDs_Past)
			return 0x0; //Don't load Tails in the past if story option is enabled.
		
		if (CurrentLevel == LevelIDs_PerfectChaos)
			return 0x0;  //Fight harder, for no reason.
	}

	isAIActive = true;
	return 1; //Return Load AI
}


//teleport AI to Player 

void moveAItoPlayer() {

	if (isAIActive) //Move AI to player 1
	{
		if (EntityData1Ptrs[0] && EntityData1Ptrs[1])
		{
			EntityData1* p1 = EntityData1Ptrs[0];
			EntityData1* p2 = EntityData1Ptrs[1];

			if (CurrentCharacter != Characters_Big && CurrentCharacter != Characters_Gamma)
				p2->Position = UnitMatrix_GetPoint(&p1->Position, &p1->Rotation, -7.0f, 0.0f, 5.0f);
			else
				p2->Position = UnitMatrix_GetPoint(&p1->Position, &p1->Rotation, -10.0f, 0.0f, 8.0f);

		}
	}
}

NJS_VECTOR UnitMatrix_GetPoint(NJS_VECTOR* orig, Rotation3* rot, float x, float y, float z) {
	NJS_VECTOR point;

	njPushMatrix(_nj_unit_matrix_);
	njTranslateV(0, orig);
	if (rot) njRotateXYZ(0, rot->x, rot->y, rot->z);
	njTranslate(0, x, y, z);
	njGetTranslation(_nj_current_matrix_ptr_, &point);
	njPopMatrix(1u);

	return point;
}



//While load result: Teleport AI close to the player and Force Victory Pose.

void DisableTime_R() {

	TimeThing = 0;

	if (isAIActive && CurrentCharacter != Characters_Tails) 
	{
		if (EntityData1Ptrs[0] && EntityData1Ptrs[1]) //Move AI to player 1 
		{
			EntityData1* p1 = EntityData1Ptrs[0];
			EntityData1* p2 = EntityData1Ptrs[1];
			p2->Status &= ~(Status_Attack | Status_Ball | Status_LightDash | Status_Unknown3);
			p2->Action = 1;
			CharObj2Ptrs[1]->AnimationThing.Index = 1;

			p2->Position = UnitMatrix_GetPoint(&p1->Position, &p1->Rotation, -8.0f, 0.0f, 5.0f);
			p2->Rotation = p1->Rotation;

			if (p2->CharID == Characters_Tails) {
				SetTailsRaceVictory(); //Fix Tails AI victory animation
				ForcePlayerAction(1, 19); //Force AI to Victory pose
			}

			dword_3B2A304 = 0;
		}

	}
}


void FlySoundOnFrames() {

	if (GameState != 15 || !EntityData1Ptrs[1] || !TailsAI_ptr)
		return;

	if (EntityData1Ptrs[1]->Action == 15 && EntityData1Ptrs[1]->CharID == Characters_Tails) {

		if (EntityData1Ptrs[1]->Unknown == 0) 
			PlaySound(0x302, NULL, 0, NULL);

		if (++EntityData1Ptrs[1]->Unknown == 31) {
			PlaySound(0x302, NULL, 0, NULL);
			EntityData1Ptrs[1]->Unknown = 1;
		}
	}
}

void PreventTailsAIDamage() {

	if (EntityData1Ptrs[1]->CharID != Characters_Tails || !EntityData1Ptrs[1])
		return;
	
	EntityData1* data = EntityData1Ptrs[0];

	if (GetCollidingEntityA(data)) {
		CharObj2Ptrs[1]->Powerups |= Powerups_Invincibility;
	}
	else {
		if ((CharObj2Ptrs[1]->Powerups & Powerups_Invincibility) == Powerups_Invincibility) {
			CharObj2Ptrs[1]->Powerups &= 0x100u; //Remove invincibility
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

void SnowboardRespawn() { 

	EntityData1* data = EntityData1Ptrs[1];

	if (data->Action == 15) { 
		if (EntityData1Ptrs[0]->Action >= 62 && EntityData1Ptrs[0]->Action <= 68 && CurrentCharacter == Characters_Sonic) { //Make Tails using snowboard again when trying to catch Sonic
			EntityData1Ptrs[1]->Status &= ~(Status_Attack | Status_Ball | Status_LightDash);
			CharObj2Ptrs[1]->PhysicsData.CollisionSize = PhysicsArray[Characters_Tails].CollisionSize; //Reset Miles physic properly
			CharObj2Ptrs[1]->PhysicsData.YOff = PhysicsArray[Characters_Tails].YOff;
			CharObj2Ptrs[1]->PhysicsData.JumpSpeed = PhysicsArray[Characters_Tails].JumpSpeed;
			ForcePlayerAction(1, 44);
		}
	}
}


ObjectMaster* LoadMilesAI()
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
	if (!ForceAI)
		FlagAI = CheckTailsAI_R();
	else
		FlagAI = 1;


	if (FlagAI != 1 || EV_MainThread_ptr)
	{
		isAIActive = false;
		return (ObjectMaster*)0x0;
	}
	else
	{
		ObjectMaster* v1 = LoadObject(LoadObj_Data1, 0, TailsAI_Main);
		TailsAI_ptr = v1;

		if (v1)
		{
			v1->Data1->CharID = (char)Characters_Tails;
			v1->Data1->CharIndex = 1;
			v1->DeleteSub = TailsAI_Delete;

			ObjectMaster* v3 = LoadMilesAI(); //load AI 
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


void LoadCharacter_r() {

	if (banCharacter[CurrentCharacter] != true && !EV_MainThread_ptr && !EntityData1Ptrs[1])
		Load2PTails_r();

	LoadCharacter(); //call original function

	return;
}


void MilesAI_OnFrames() {

	if (GameState != 15 || !EntityData1Ptrs[0] || !EntityData1Ptrs[1] || EntityData1Ptrs[1]->CharID != Characters_Tails || !TailsAI_ptr)
		return;

	PreventTailsAIDamage();
	SnowboardRespawn();
	//FlySoundOnFrames();
}

void AI_Init(const HelperFunctions& helperFunctions) {

	//Allow Tails AI to spawn in acton stages, hub world, bosses and chao garden + fixes
	WriteCall((void*)0x47ed8e, CheckTailsAI_R);
	WriteCall((void*)0x47e943, CheckTailsAI_R);
	WriteCall((void*)0x47ea46, CheckTailsAI_R);
	WriteCall((void*)0x47ec62, CheckTailsAI_R);

	WriteData<5>((void*)0x415948, 0x90); //remove the original load2PTails in LoadCharacter as we use a custom one
	
	//Miles General Improvement
	WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill 
	WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
	WriteCall((void*)0x4e9664, LoadAISnowBoard_R);

	WriteData<6>((int*)0x47ea72, 0x90); //remove the unnecessary high altitude when Tails AI respawn, so he can catch you faster
	WriteData<1>((int*)0x47DC5D, 0xF2); //Reduce the Tails "Range out" check so he can catch faster. (changing the float value from 1000 to 352)
	WriteData<1>((int*)0x47DC5C, 0xF0);
	
	ReplaceSound("P_SONICTAILS_BANK03", "P_SONICTAILS_BANK03");
}

