#include "stdafx.h"

//every improvement and new abilities for Tails go here.

Trampoline* Chao_Main_t;
Trampoline* TailsAI_Main_t;
ObjectMaster* TailsGrab = nullptr;

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
	WriteData<1>((int*)0x47EA74, 0x48);
	WriteData<1>((int*)0x47EA75, 0xDC);
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

void TailsAI_Grab(ObjectMaster* obj) {

	if (!EntityData1Ptrs[0] || !EntityData1Ptrs[1] || GameState != 15)
		CheckThingButThenDeleteObject(obj);

	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[1];
	EntityData1* data = obj->Data1;
	CharObj2* co2p1 = CharObj2Ptrs[0];
	CharObj2* co2p2 = CharObj2Ptrs[1];


		if (data->Action > 0 && data->Action < 3) {

			if (++data->InvulnerableTime == 300) {
				data->Action = leaving;
			}
		}

		if (data->Action == 3)
		{
			if (ControllerPointers[0]->PressedButtons & Buttons_A || ControllerPointers[0]->PressedButtons & Buttons_B) {
				co2p1->AnimationThing.Index = 14;
				p1->Status |= Status_Ball;
				data->Action = leaving;
			}
		}
	

		switch (data->Action)
		{
		case initFly:
				data->Position = p2->Position;
				EnableController(1);
				ControllerPointers[1]->PressedButtons |= Buttons_A;
				data->Action = getAltitude;
			break;
		case getAltitude:
			if (p2->Position.y < p1->Position.y + 15) {
				ControllerPointers[1]->PressedButtons = 0;
				ControllerPointers[1]->HeldButtons |= Buttons_A;
			}
			else {
				p2->Action = 125;
				data->Action = checkGrab;
			}
			break;
		case checkGrab:
			CharObj2Ptrs[1]->AnimationThing.Index = 37;
			if (GetCollidingEntityA(p2)) {
				ControllerPointers[1]->HeldButtons = 0;
				DisableController(1);
				p1->Status &= ~(Status_Attack | Status_Ball | Status_LightDash);
				p1->Action = 125;
				data->Action = 3;
			}
			break;
		case grabbed:
			CharObj2Ptrs[0]->AnimationThing.Index = 47;
			co2p2->Speed = co2p1->Speed;
			p1->Position = p2->Position;
			p1->Position.y -= 6.5f;
			p1->Rotation = p2->Rotation;
			break;
		case leaving:
			if (p1->Action == 125) {
				p1->Action = 8;
				p1->Status &= 0x100u;
			}
			p2->Status &= 0x100u;
			ControllerPointers[1]->PressedButtons = 0;
			ControllerPointers[1]->HeldButtons = 0;
			DisableController(1);
			p2->Action = 10;
			TailsGrab = nullptr;
			CheckThingButThenDeleteObject(obj);
			break;
		}
	

}

void TailsAI_Main_R(ObjectMaster* obj) {

	if (obj->Data1->Action > 0) {
		if (ControllerPointers[0]->PressedButtons & Buttons_Z)
		{
			if (!TailsGrab)
				TailsGrab = LoadObject((LoadObj)2, 1, TailsAI_Grab);
		}
	}

	ObjectFunc(origin, TailsAI_Main_t->Target());
	origin(obj);
}


void AI_Improvement() {
	//Miles General Improvement
	WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill 
	WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
	WriteCall((void*)0x4e9664, LoadAISnowBoard_R);

	ReduceRespawnDelay();

	WriteData<1>((int*)0x47DC5D, 0xF2); //Reduce the Tails "Range out" check so he can catch faster. (changing the float value from 1000 to 352)
	WriteData<1>((int*)0x47DC5C, 0xF0);

	Chao_Main_t = new Trampoline((int)Chao_Main, (int)Chao_Main + 0x6, Chao_Main_R);
	TailsAI_Main_t = new Trampoline((int)TailsAI_Main, (int)TailsAI_Main + 0x5, TailsAI_Main_R);
}