#include "stdafx.h"

ObjectMaster* MilesRescue = nullptr;
ObjectMaster* TailsRescueLanding = nullptr;



void TailsAI_LandingDelete2(ObjectMaster* obj) {
	TailsRescueLanding = nullptr;
}

void TailsAI_RescueDelete(ObjectMaster* obj) {
	MilesRescue = nullptr;
}

//Actually need a second separate object for the landing part, otherwise Tails AI behavior become dumb, idk, kill me, this game sucks.
void TailsAI_Landing2(ObjectMaster* obj) {

	if (!EntityData1Ptrs[0] || !EntityData1Ptrs[1] || GameState != 15 && GameState != 4) {
		return;
	}

	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[1];
	EntityData1* data = obj->Data1;
	CharObj2* co2p1 = CharObj2Ptrs[0];
	CharObj2* co2p2 = CharObj2Ptrs[1];

	LookAt(&p2->Position, &data->Position, nullptr, &p2->Rotation.y);
	FlySoundOnFrames();

	switch (data->Action) {
	case 0:
		obj->DeleteSub = TailsAI_LandingDelete2;
		//CharObj2Ptrs[1]->Speed = { 0, 0, 0 };
		//CharObj2Ptrs[0]->Speed = { 0, 0, 0 };
		DisableController(0);
		EnableController(1);
		p1->Action = 125;
		p2->Action = 15;
		CharObj2Ptrs[1]->AnimationThing.Index = 37;
		PlayCharacterGrabAnimation(p1, co2p1);
		data->Action = 1;
		break;
	case 1:
		UpdateP1Position(co2p1, co2p2, p1, p2);
		CharObj2Ptrs[1]->Speed.y -= 0.4;

		if (++data->InvulnerableTime == 90 || ((p1->Status & Status_Ground) == Status_Ground) || (p1->Status & Status_Unknown1) == Status_Unknown1) {
			data->Action = 2;
		}
		break;
	case 2:
		EnableController(0);
		PlayCharacterLeaveAnimation(p1, co2p1);
		RestoreAIControl();
		CheckThingButThenDeleteObject(obj);
		break;
	}
}


void MilesRescuesCharacterFall(ObjectMaster* obj) {

	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[1];
	CharObj2* p1co2 = CharObj2Ptrs[0];
	CharObj2* p2co2 = CharObj2Ptrs[1];
	EntityData1* data = obj->Data1;
	short Fadeout = 0;

	FlySoundOnFrames();

	switch (data->Action) {

	case 0:
		obj->DeleteSub = TailsAI_RescueDelete;
		p1co2->Speed.y = 2;
		p2->Position.x = p1->Position.x;
		p2->Position.y = p1->Position.y + 60;
		p2->Position.z = p1->Position.z;
		p2->Status &= ~(Status_Attack | Status_Ball | Status_LightDash);
		p2->Action = 15;
		CharObj2Ptrs[1]->AnimationThing.Index = 37;
		data->Action = 1;
		break;
	case 1:
		if (p2->Position.y - p1->Position.y <= GetCharacterPositionY(p1) + 5) {
			p1->Status &= ~(Status_Attack | Status_Ball | Status_LightDash);
			p1->Action = 125;
			data->Position = RestartLevel.Position;
			PlayCharacterGrabAnimation(p1, p1co2);
			data->Action = 2;
		}
		else {
			p1co2->Speed.y = 2;
			CharObj2Ptrs[1]->Speed.y -= 0.3;
		}
		break;
	case 2:
		if (++data->InvulnerableTime == 200) {
			Controllers[1].HeldButtons = 0;
			Controllers[1].PressedButtons = 0;
			p1->Position = RestartLevel.Position;
			p1->Position.y += 50;
			p2->Position = p1->Position;
			p1co2->Speed = { 0, 0, 0 };
			data->Action = 3;
		}
		else {
			UpdateP1Position(p1co2, p2co2, p1, p2);
			CharObj2Ptrs[1]->Speed.y += 0.5;
		}
		break;
	case 3:
		if (!TailsRescueLanding)
			TailsRescueLanding = LoadObject((LoadObj)2, 1, TailsAI_Landing2);

		if (TailsRescueLanding)
			CheckThingButThenDeleteObject(obj);
		break;
	}
}

void PlayCharacterDeathSound_r(ObjectMaster* a1, int pid) {

	if (!EntityData1Ptrs[1] || EntityData1Ptrs[1]->CharID != Characters_Tails)
		return PlayCharacterDeathSound(a1, pid);


	int getRng = (rand() % 100) < 21;
	//TO DO: add rng check
	DisableController(0);

	if (!MilesRescue)
		MilesRescue = LoadObject((LoadObj)2,1, MilesRescuesCharacterFall);
}

void Rescue_Init() {
	WriteCall((void*)0x44af87, PlayCharacterDeathSound_r);
	WriteCall((void*)0x44affe, PlayCharacterDeathSound_r);
}
