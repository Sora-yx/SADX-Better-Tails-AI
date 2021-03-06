#include "stdafx.h"

//every improvement and new abilities for Tails go here.

Trampoline* Chao_Main_t;
Trampoline* ScoreDisplay_main_t;

void CatchUP() {

	if (CurrentCharacter != Characters_Sonic)
		return;

	EntityData1* P1 = EntityData1Ptrs[0];
	EntityData1* P2 = EntityData1Ptrs[1];

	if (CurrentLevel == LevelIDs_SpeedHighway && CurrentAct == 1 && (P1->Action >= 46 && P1->Action <= 50))
	{
		P2->Rotation = P1->Rotation;

		CharObj2Ptrs[1]->Powerups |= Powerups_Invincibility;
		if (P2->Position.y > -10790)
		{
			CharObj2Ptrs[1]->AnimationThing.Index = 37;
			P2->Action = 125;
			P2->Position.x = P1->Position.x + 10;
			P2->Position.y = P1->Position.y + 2;
			P2->Position.z = P1->Position.z + 10;
		}
		else if (P2->Position.y <= -10791 && P2->Position.y > -18659) {
			CharObj2Ptrs[1]->AnimationThing.Index = 12;
			P2->Action = 125;
			P2->Position.x = P1->Position.x;
			P2->Position.y = P1->Position.y - 7;
			P2->Position.z = P1->Position.z;
		}
		else if (P2->Position.y <= -18660 && P2->Action > 90) {
			CharObj2Ptrs[1]->Powerups &= 0x100u; 
			P2->Action = 1;
		}
	}
		
	if (isPlayerUsingSnowboard()) { 
		if (CurrentLevel == LevelIDs_IceCap && CurrentAct == 2 && P1->Position.y > -3800) {
			P1->Rotation = P1->Rotation;
			if (P2->Status & Status_Ground)
				CharObj2Ptrs[1]->Speed.x = CharObj2Ptrs[0]->Speed.x + 0.8;
		}
		else {
			P2->Rotation = P1->Rotation;
			if (P1->Status & Status_Ground)
				CharObj2Ptrs[1]->Speed.x = CharObj2Ptrs[0]->Speed.x + 0.10;
		}
	}
}

//Load Tails AI Snowboard when playing Sonic
void LoadAISnowBoard_R() {
	ForcePlayerAction(0, 0x2c);

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

//Make Tails using snowboard again when trying to catch Sonic
void SnowboardRespawn() {
	if (GameState != 15)
		return;

	EntityData1* data = EntityData1Ptrs[1];

	if (data->Action == 15) {
		if (isPlayerUsingSnowboard()) {
			EntityData1Ptrs[1]->Status &= ~(Status_Attack | Status_Ball | Status_LightDash);
			CharObj2Ptrs[1]->PhysicsData.CollisionSize = PhysicsArray[Characters_Tails].CollisionSize; //Reset Miles physic properly
			CharObj2Ptrs[1]->PhysicsData.YOff = PhysicsArray[Characters_Tails].YOff;
			CharObj2Ptrs[1]->PhysicsData.JumpSpeed = PhysicsArray[Characters_Tails].JumpSpeed;
			ForcePlayerAction(1, 44);
		}
	}
}


float spawnDelay = 15.0;
float* spawnDelayptr = &spawnDelay; 

void ReduceRespawnDelay() {
	spawnDelay = 15.0;
	WriteData((float**)0x47EA74, spawnDelayptr);
}


int CharacterPetActionNumber[8] = { 72, 64, 64, 54, 54, 50, 56, 52 };

int isCharacterPetting() {
	return CharacterPetActionNumber[EntityData1Ptrs[0]->CharID];
}

bool isChaoPetByAI = false;

void MakeAIPetChao(ObjectMaster* Chao) {
	EntityData1* data = Chao->Data1;
	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[1];

	if (p1->Action != isCharacterPetting()) {
		if (p2->Action > 40) {
			p2->Action = 1;
		}
		data->Index = 0;
		p2->Unknown = 0;
	}

	float dist = GetDistance(&p2->Position, &data->Position);

	switch (data->Index)
	{
	case 0:
		if (++p2->Unknown == 40)
		{
			data->InvulnerableTime = 0;
			data->Index = 1;
		}
		break;
	case 1:
		if (dist < 9) {
			p2->Action = 64;
			CharObj2Ptrs[1]->AnimationThing.Index = 133;
			isChaoPetByAI = true;
		}
		break;
	}
}

void CheckAndMakeAIPetChao(ObjectMaster* Chao) {
	if (IsChaoGardenBanned || !EntityData1Ptrs[1])
		return;

	if (CurrentLevel >= LevelIDs_SSGarden && CurrentLevel <= LevelIDs_MRGarden) {
		MakeAIPetChao(Chao);
	}
}

void Chao_Main_R(ObjectMaster* obj) {
	EntityData1* p1 = EntityData1Ptrs[0];
	obj->Data1 = obj->Data1;

	if (IsPlayerInsideSphere(&obj->Data1->Position, 10)) {
		CheckAndMakeAIPetChao(obj);
	}

	if (p1->Action != isCharacterPetting() && isChaoPetByAI) {
		if (++obj->Data1->InvulnerableTime == 120) {
			Chao_SetBehavior(obj, (long*)Chao_Pleasure);
			isChaoPetByAI = false;
		}
	}

	ObjectFunc(origin, Chao_Main_t->Target());
	origin(obj);
}

void MilesAI_VictoryPose(ObjectMaster* obj) {

	if (!EntityData1Ptrs[0] || !EntityData1Ptrs[1] || !isAIActive || EntityData1Ptrs[1]->CharID != Characters_Tails)
		return;

	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[1];
	EntityData1* data = obj->Data1;

	switch (obj->Data1->Action)
	{
	case 0:
		if (p1->NextAction == 0x13) {
			p2->Position = UnitMatrix_GetPoint(&p1->Position, &p2->Rotation, 0.0f, 0.0f, 7.0f);
			p2->Rotation = p1->Rotation;
			if (++data->InvulnerableTime == 10)
				data->Action = 1;
		}
		break;
	case 1:

		if (CurrentLevel == LevelIDs_IceCap && CurrentAct == 2)
			ForcePlayerAction(1, 24);

		if ((p2->Status & Status_Ground) == 0 && (p2->Status & Status_Unknown1) == 0 || p2->Position.y > p1->Position.y + 2 || p2->Position.y < p1->Position.y - 2) 
		{
			p2->Position = UnitMatrix_GetPoint(&p1->Position, &p2->Rotation, 0.0f, 0.0f, 6.0f);  //fix floating victory pose
		}

		if (++data->Index == 5) {

		if ((p2->Status & Status_Ground) == 0 && (p2->Status & Status_Unknown1) == 0) { //last failsafe
			p2->Position = UnitMatrix_GetPoint(&p1->Position, &p2->Rotation, 0.0f, 0.0f, -6.0f); //try other side
		}
			data->Action = 2;
		}
		
		break;
	case 2:
		p2->Rotation = p1->Rotation;
		ForcePlayerAction(1, 19); //Force AI to Victory pose
		break;
	}
}

int GetRaceWinnerPlayer_r() {
	if (CurrentCharacter != Characters_Tails && EntityData1Ptrs[1] != nullptr) {
			if (EntityData1Ptrs[1]->CharID == Characters_Tails) {
				return 1;
			}
	}

	return RaceWinnerPlayer;
}

//While load result: Teleport AI close to the player and Force Victory Pose.
void ScoreDisplayMain_R(ObjectMaster* obj) {

	if (obj->Data1->Action == 0) {
		LoadObject((LoadObj)(LoadObj_Data1 | LoadObj_Data2), 2, MilesAI_VictoryPose);
	}

	ObjectFunc(origin, ScoreDisplay_main_t->Target());
	origin(obj);
}




void PreventTailsAIDamage() {
	if (EntityData1Ptrs[1]->CharID != Characters_Tails || !EntityData1Ptrs[1] || !CharObj2Ptrs[1])
		return;

	EntityData1* data = EntityData1Ptrs[0];
	CharObj2* co2 = CharObj2Ptrs[1];

	if (co2->Upgrades & Upgrades_SuperSonic)
		return;

	if (GetCollidingEntityA(data)) { //There is actually a flag init in Tails AI that could be removed instead of doing this hacky thing, but this might create issue with AI fight, need to be tested.
		co2->Powerups |= Powerups_Invincibility;
	}
	else {
		if ((co2->Powerups & Powerups_Invincibility)) {
			co2->Powerups &= 0x100u; //Remove invincibility
		}
	}
}


void RestoreRespawnDelay() {
	spawnDelay = 50.0;
}

void PreventTailsAIAction() {
	if (EntityData1Ptrs[1]->CharID != Characters_Tails || !EntityData1Ptrs[1] || !CharObj2Ptrs[1])
		return;

	CharObj2* co2 = CharObj2Ptrs[1];

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 0)
	{
		if (EntityData1Ptrs[1]->Action == 38)
		{
			EntityData1Ptrs[1]->Action = 1;
			EntityData1Ptrs[1]->Position.x += 5;
		}
	}

	if (CurrentLevel == LevelIDs_TwinklePark)
		RestoreRespawnDelay();
}


void AI_Improvement() {
	//Miles General Improvement
	WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill
	WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
	WriteCall((void*)0x4e9664, LoadAISnowBoard_R);

	ReduceRespawnDelay();

	//Reduce Tails AI's "Range out" check, so he can catch faster. (Changing the float value from 1000 to 700)
	WriteData<1>((int*)0x47DC5E, 0x81);
	WriteData<1>((int*)0x47DC5D, 0x36); 
	WriteData<1>((int*)0x47DC5C, 0xb0);

	WriteJump(GetRaceWinnerPlayer, GetRaceWinnerPlayer_r); //fix wrong victory pose for Tails AI.

	if (isFlyTravel)
		FlyTravel_Init();

	if (isRescueAllowed)
		Rescue_Init();

	Chao_Main_t = new Trampoline((int)Chao_Main, (int)Chao_Main + 0x6, Chao_Main_R);
	ScoreDisplay_main_t = new Trampoline((int)ScoreDisplay_Main, (int)ScoreDisplay_Main + 0x5, ScoreDisplayMain_R);
}