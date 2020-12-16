#include "stdafx.h"

//every improvement and new abilities for Tails go here.

Trampoline* Chao_Main_t;


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

	EntityData1* data = EntityData1Ptrs[1];

	if (data->Action == 15) {
		if (EntityData1Ptrs[0]->Action >= 62 && EntityData1Ptrs[0]->Action <= 68 && CurrentCharacter == Characters_Sonic) { 
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


void AI_Improvement() {
	//Miles General Improvement
	WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill 
	WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
	WriteCall((void*)0x4e9664, LoadAISnowBoard_R);

	ReduceRespawnDelay();

	WriteData<1>((int*)0x47DC5F, 0x3);
	WriteData<1>((int*)0x47DC5E, 0x8F);
	WriteData<1>((int*)0x47DC5D, 0x52); //Reduce the Tails "Range out" check so he can catch faster. (changing the float value from 1000 to 500)
	WriteData<1>((int*)0x47DC5C, 0xD8);

	if (isFlyTravel)
		FlyTravel_Init();

	Rescue_Init();

	Chao_Main_t = new Trampoline((int)Chao_Main, (int)Chao_Main + 0x6, Chao_Main_R);
}