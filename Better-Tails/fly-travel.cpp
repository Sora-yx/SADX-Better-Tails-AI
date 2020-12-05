#include "stdafx.h"


Trampoline* TailsAI_Main_t;
Trampoline* MovePlayerToStartPoint_t;
ObjectMaster* TailsGrab = nullptr;
bool isMoving = false;

FunctionPointer(void, sub_62ECE0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x62ECE0);
FunctionPointer(void, sub_51C130, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x51C130);
FunctionPointer(void, sub_525980, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x525980);
FunctionPointer(void, sub_52F9C0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x52F9C0);
FunctionPointer(void, sub_541BF0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x541BF0);



NJS_TEXANIM	MilesCursor_TEXANIM[]{
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 0, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 1, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 2, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 3, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 4, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 5, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 6, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 7, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 8, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 9, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 10, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 11, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 12, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 13, 0x20 },
	{ 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, 14, 0x20 }
};

NJS_TEXNAME MilesCursor_TEXNAMES[15];
NJS_TEXLIST MilesCursor_TEXLIST = { arrayptrandlength(MilesCursor_TEXNAMES) };
NJS_SPRITE MilesCursor_SPRITE = { { 0, 0, 0 }, 1.0, 1.0, 0, &MilesCursor_TEXLIST, MilesCursor_TEXANIM };


int Cursor = -1;
int MilesCurTex = 0;

//Tails Grab Fly abilities
MilesAI_Fly DestinationArray[]{
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

void __cdecl MovePlayerToStartPoint_r(EntityData1* data) {
	if (isMoving) {
		EntityData1Ptrs[0]->Position = DestinationArray[Cursor].destination;
		EntityData1Ptrs[0]->Position.y -= 6.5f;
	}
	else {
		FunctionPointer(void, original, (EntityData1 * data), MovePlayerToStartPoint_t->Target());
		return original(data);
	}
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
		LoadPVM("map_ec_a", &map_ec_X_TEXLIST);
		break;
	case 5:
		LoadPVM("map_mr_s", &map_mr_X_TEXLIST);
		break;
	case 6:
		LoadPVM("map_mr_a", &map_mr_X_TEXLIST);
		break;
	case 7:
	case 8:
		LoadPVM("map_mr_j", &map_mr_X_TEXLIST);
		break;
	}

	LoadPVM("Map_icon", &map_icon_TEXLIST);
}

void ReleaseAllTravelTexture() {
	njReleaseTexture(&map_ss_TEXLIST);
	njReleaseTexture(&map_ec_X_TEXLIST);
	njReleaseTexture(&map_mr_X_TEXLIST);
	njReleaseTexture(&map_icon_TEXLIST);
	njReleaseTexture(&MilesCursor_TEXLIST);
	Cursor = -1;
	return;
}

int setCursorPos(int curLevel, int curAct) {
	if (curLevel == LevelIDs_StationSquare)
	{
		if (curAct == 3)
			return Sstation;

		if (curAct == 4)
			return ShostelPool;

		if (curAct == 1)
			return SCasinoArea;

		if (curAct == 0)
			return SChaos0;
	}

	if (curLevel == LevelIDs_EggCarrierOutside)
		return ECarrier;

	if (curLevel == LevelIDs_MysticRuins) {

		if (curAct == 0)
			return MRStation;

		if (curAct == 1)
			return MRAngelIsland;

		if (curAct == 2)
			return MRJungleBig;
	}

	return -1;
}

void DisplayCursorAnimation() {

	SetMaterialAndSpriteColor_Float(1, 1, 1, 1);

	float scale = 3.0F;
	float x = (float)HorizontalResolution / 2.0f;
	float y = (float)VerticalResolution / 2.0f;
	float incr = 0x10 * scale;
	float sclx = 10;
	float scly = 1;

	MilesCursor_SPRITE.sx = 2 * scale;
	MilesCursor_SPRITE.sy = 2 * scale;
	float newx = x - (incr * sclx / 2 + incr / 2);
	MilesCursor_SPRITE.p.x = newx;
	MilesCursor_SPRITE.p.y = y - y / 2;

	if (MilesCurTex >= 14)
		MilesCurTex = 0;

	if (FrameCounterUnpaused % 10 == 0 && MilesCurTex < 15)
		MilesCurTex++;

	njDrawSprite2D_Queue(&MilesCursor_SPRITE, MilesCurTex, -1.501, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, QueuedModelFlagsB_SomeTextureThing);
}

void CheckPlayerCursorPos() {
	if (Cursor <= -1)
		Cursor = 8;

	if (Cursor >= 9)
		Cursor = 0;

	if (ControllerPointers[0]->PressedButtons & Buttons_Up) {
		Cursor++;
		CheckAndLoadMapPVM();
	}

	if (ControllerPointers[0]->PressedButtons & Buttons_Down) {
		Cursor--;
		CheckAndLoadMapPVM();
	}
}

void __cdecl PauseMenu_Map_Display_r() {

	if (Cursor < 0 || Cursor > 8)
		return;

	if (Cursor >= Sstation && Cursor <= SChaos0)
		sub_62ECE0((unsigned __int8)CurrentAct, &Current_CharObj1->Position);

	if (Cursor == ECarrier)
		sub_51C130((unsigned __int8)CurrentAct, &Current_CharObj1->Position);

	if (Cursor >= MRStation && Cursor <= MRJungleBig)
		sub_52F9C0((unsigned __int8)CurrentAct, &Current_CharObj1->Position);

	return;
}

void TailsAI_Grab(ObjectMaster* obj) {

	if (!EntityData1Ptrs[0] || !EntityData1Ptrs[1] || GameState != 15) {
		TailsGrab = nullptr;
		CheckThingButThenDeleteObject(obj);
	}

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
			LoadPVM("MilesCursor", &MilesCursor_TEXLIST);
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
		DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Cursor Value %d", Cursor);
		CheckPlayerCursorPos();
		PauseMenu_Map_Display_r();
		DisplayCursorAnimation();
		if (ControllerPointers[0]->PressedButtons & Buttons_A) {
			data->Unknown = 0;
			isMoving = true;
			data->Action = movetoDestination;
		}
		break;
	case movetoDestination:
		p2->Action = 15;
		co2p2->Speed = co2p1->Speed;
		p1->Position = p2->Position;
		p1->Position.y -= 6.5f;
		p1->Rotation = p2->Rotation;
		if (++data->Unknown == 60) {
			CharObj2Ptrs[1]->Speed.y += 2;
			CharObj2Ptrs[1]->Speed.z += 3;
		}
		else {
			CharObj2Ptrs[1]->Speed.y += 1;
			CharObj2Ptrs[1]->Speed.x += 1;
		}
		if (++data->InvulnerableTime == 180) {
			LoadDestination();
		}
		break;
	case leaving:
		ReleaseAllTravelTexture();
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



void FlyTravel_Init() {

	TailsAI_Main_t = new Trampoline((int)TailsAI_Main, (int)TailsAI_Main + 0x5, TailsAI_Main_R);
	MovePlayerToStartPoint_t = new Trampoline((int)MovePlayerToStartPoint, (int)MovePlayerToStartPoint + 0x6, MovePlayerToStartPoint_r);
}