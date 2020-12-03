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

int Cursor = -1;

//Tails Grab Fly abilities
MilesAI_Fly DestinationArray[] {
	{ LevelIDs_StationSquare, 3,  3.55313, 167.466, 1504.42}, //Station (main)
	{ LevelIDs_StationSquare, 4, -458.052, 36.0911, 2024.9 }, //Hostel pool
	{ LevelIDs_StationSquare, 1, -440.442, -2.5, 1142.33 },	//Casino area
	{ LevelIDs_StationSquare, 0, 271.549, 43.1508, 329.312 }, //chaos 0
	{ LevelIDs_EggCarrierOutside, 0,  97.0032, 894.573, 817.829 },
	{ LevelIDs_MysticRuins, 0, -48.1315, 242.108, 1033.91},	//station
	{ LevelIDs_MysticRuins, 1, -9.2187, -118.838, 2220.16}, //angel island
	{ LevelIDs_MysticRuins, 2, -515.191, 237.349, -865.042},	//jungle lost world
	{ LevelIDs_MysticRuins, 2,  1307.67, 234.549, -814.303}, //jungle big's house
};

void LoadDestination() {

	LastLevel = CurrentLevel;
	LastAct = CurrentAct;
	SetNextLevelAndAct_CutsceneMode(DestinationArray[Cursor].level, DestinationArray[Cursor].act);
	return;
}

void CheckAndLoadMapPVM() {

	switch (Cursor) {
		case 0:
		case 1:
		case 2:
		case 3:
			LoadPVM("map_ss", &map_ss_TEXLIST);
			break;
		case 4:
			if (GetEventFlag(EggCarrierTransformed_CharacterFlags[EntityData1Ptrs[0]->CharID]))
				LoadPVM("map_ec_a", &map_ec_X_TEXLIST);
			else
				LoadPVM("map_ec_b", &map_ec_X_TEXLIST);
			break;
		case 5:
		case 6:
		case 7:
		case 8:
			if (CurrentAct == 0)
				LoadPVM("map_mr_s", &map_mr_X_TEXLIST);
			else if (CurrentAct == 1)
				LoadPVM("map_mr_a", &map_mr_X_TEXLIST);
			else if (CurrentAct == 2)
				LoadPVM("map_mr_j", &map_mr_X_TEXLIST);
			break;
	}

	LoadPVM("Map_icon", &map_icon_TEXLIST);
}

int setCursorPos(int curLevel, int curAct) {
	if (curLevel == LevelIDs_StationSquare)
	{
		if (curAct == 3)
			return 0;

		if (curAct == 4)
			return 1;

		if (curAct == 1)
			return 2;

		if (curAct == 0)
			return 3;
	}

	if (curLevel == LevelIDs_EggCarrierOutside)
		return 4;

	if (curLevel == LevelIDs_MysticRuins) {

		if (curAct == 0)
			return 5;

		if (curAct == 1)
			return 6;

		if (curAct == 2)
			return 7;
	}

	return -1;
}

void CheckPlayerCursorPos() {
	if (Cursor <= -1)
		Cursor = 8;

	if (Cursor >= 9)
		Cursor = 0;

	if (ControllerPointers[0]->PressedButtons & Buttons_Up)
		Cursor++;

	if (ControllerPointers[0]->PressedButtons & Buttons_Down)
		Cursor--;
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

		if (data->Action >= grabbed && data->Action <= data->Action < leaving)
		{
			if (ControllerPointers[0]->PressedButtons & Buttons_B) {
				co2p1->AnimationThing.Index = 14;
				p1->Status |= Status_Ball;
				data->Action = leaving;
			}
		}
	

		switch (data->Action)
		{
		case initFly:
				data->Unknown = 0;
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
				data->Action = grabbed;
			}
			break;
		case grabbed:
			DisablePause();
			CharObj2Ptrs[0]->AnimationThing.Index = 47;
			co2p2->Speed = co2p1->Speed;
			p1->Position = p2->Position;
			p1->Position.y -= 6.5f;
			p1->Rotation = p2->Rotation;
			Cursor = setCursorPos(CurrentLevel, CurrentAct);
			if (Cursor > -1) {
				CheckAndLoadMapPVM();
				data->Action = transitionMap;
			}
			else {
				data->Action = leaving;
			}
			break;
		case transitionMap:
			if (++data->Unknown == 60) {
				PlaySound(3, NULL, 0, NULL);
				data->Action = displayMap;
			}
			break;
		case displayMap:
			PauseMenu_Map_Display();
			CheckPlayerCursorPos();
			if (ControllerPointers[0]->PressedButtons & Buttons_A) {
				LoadDestination();
				data->Action = movetoDestination;
			}
			break;
		case movetoDestination:
			break;
		case leaving:
			EnablePause();
			if (p1->Action == 125) {
				p1->Action = 8;
				p1->Status &= 0x100u;
			}
			data->Unknown = 0;
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
			if (EntityData1Ptrs[1]->CharID == Characters_Tails && EntityData1Ptrs[0]->Action < 3 && EntityData1Ptrs[1]->Action < 3) {

				if (!TailsGrab)
					TailsGrab = LoadObject((LoadObj)2, 1, TailsAI_Grab);
			}
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

	WriteData<1>((int*)0x47DC5F, 0x3);
	WriteData<1>((int*)0x47DC5E, 0x8F);
	WriteData<1>((int*)0x47DC5D, 0x52); //Reduce the Tails "Range out" check so he can catch faster. (changing the float value from 1000 to 500)
	WriteData<1>((int*)0x47DC5C, 0xD8);


	Chao_Main_t = new Trampoline((int)Chao_Main, (int)Chao_Main + 0x6, Chao_Main_R);
	TailsAI_Main_t = new Trampoline((int)TailsAI_Main, (int)TailsAI_Main + 0x5, TailsAI_Main_R);
}