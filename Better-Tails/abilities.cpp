#include "stdafx.h"

//every improvement and new abilities for Tails go here.

Trampoline* Chao_Main_t;

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

void ReduceRespawnDelay() {
	WriteData<1>((int*)0x47EA74, 0x2C);
	WriteData<1>((int*)0x47EA75, 0x01);
	WriteData<1>((int*)0x47EA76, 0x7E);
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
			p2->Position = UnitMatrix_GetPoint(&p1->Position, &p1->Rotation, 0.0f, 0.0f, 0.0f);
			GetPlayerSidePos(&p2->Position, EntityData1Ptrs[0], 5.0);
			SetPlayerPosition(1u, 0, &EntityData1Ptrs[1]->Position, 0);
			p2->Position.x += 6;
			if (++data->InvulnerableTime == 10)
				data->Action = 1;
		}
		break;
	case 1:
		if ((p2->Status & Status_Ground) == Status_Ground || p2->Action <= 2) { //fix floating victory pose
			if (p2->Position.y >= p1->Position.y + 6 || p2->Position.y <= p1->Position.y - 6 || p1->Position.z == p2->Position.z) { //failsafe
				p2->Rotation = p1->Rotation;
				p2->Position = UnitMatrix_GetPoint(&p1->Position, &p1->Rotation, 0.0f, 0.0f, 6.0f);
			}
			if (CurrentLevel == LevelIDs_IceCap && CurrentAct == 2 && IceCapFlag)
				ForcePlayerAction(1, 24);
			SetTailsRaceVictory(); //Fix Tails AI victory animation
			ForcePlayerAction(1, 19); //Force AI to Victory pose
		}
		break;
	}
}

//While load result: Teleport AI close to the player and Force Victory Pose.

void DisableTime_R() {
	TimeThing = 0;

	LoadObject((LoadObj)(LoadObj_Data1 | LoadObj_Data2), 2, MilesAI_VictoryPose);
	return;
}

void PreventTailsAIDamage() {
	if (EntityData1Ptrs[1]->CharID != Characters_Tails || !EntityData1Ptrs[1] || !CharObj2Ptrs[1])
		return;

	EntityData1* data = EntityData1Ptrs[0];
	CharObj2* co2 = CharObj2Ptrs[1];

	if (co2->Upgrades & Upgrades_SuperSonic)
		return;

	if (GetCollidingEntityA(data)) {
		co2->Powerups |= Powerups_Invincibility;
	}
	else {
		if ((co2->Powerups & Powerups_Invincibility)) {
			co2->Powerups &= 0x100u; //Remove invincibility
		}
	}
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

	if (isFlyTravel)
		FlyTravel_Init();

	if (isRescueAllowed)
		Rescue_Init();

	Chao_Main_t = new Trampoline((int)Chao_Main, (int)Chao_Main + 0x6, Chao_Main_R);
}