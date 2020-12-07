#include "stdafx.h"


Trampoline* TailsAI_Main_t;
Trampoline* MovePlayerToStartPoint_t;
ObjectMaster* TailsGrab = nullptr;
uint8_t isMoving = 0;
ObjectMaster* TailsLanding = nullptr;

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
float CursorPosX = -1;
float CursorPosY = -1;
int MilesCurTex = 0;

//Tails Grab Fly abilities
MilesAI_Fly DestinationArray[]{
	{ LevelIDs_StationSquare, 3,  112.861, 70.962, 1555.12}, //Station (main)
	{ LevelIDs_StationSquare, 4, -483.971, 173.856, 2070.45 }, //Hostel pool
	{ LevelIDs_StationSquare, 1, -449.364, 21.0912, 1458.25},	//Casino area
	{ LevelIDs_StationSquare, 0, 271.549, 250.1508, 329.312 }, //chaos 0
	{ LevelIDs_EggCarrierOutside, 0,  97.0032, 950.573, 817.829 },
	{ LevelIDs_MysticRuins, 0, -48.1315, 300.108, 1033.91},	//station
	{ LevelIDs_MysticRuins, 1, -9.2187, -15.838, 2220.16}, //angel island
	{ LevelIDs_MysticRuins, 2, -515.191, 287.349, -865.042},	//jungle lost world
	{ LevelIDs_MysticRuins, 2,  1307.67, 284.549, -814.303}, //jungle big's house
};

Map_Cursor CursorArray[]{
	{300, 280 },
	{170, 320 },
	{170, 150 },
	{300, 60 },
	{420, 160 },
	{180, 300 },
	{352, 310 },
	{245, 180 },
	{455, 195 },
};

void LoadDestination() {
	ControllerPointers[1]->PressedButtons = 0;
	ControllerPointers[1]->HeldButtons = 0;
	DisableController(1);
	LastLevel = CurrentLevel;
	LastAct = CurrentAct;
	SetNextLevelAndAct_CutsceneMode(DestinationArray[Cursor].level, DestinationArray[Cursor].act);
	return;
}

void __cdecl MovePlayerToStartPoint_r(EntityData1* data) {
	if (isMoving > 0) {
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

	float scale = 2.0F;
	float x = (float)HorizontalResolution / 2.0f;
	float y = (float)VerticalResolution / 2.0f;
	float incr = 0x10 * scale;
	float sclx = 10;
	float scly = 1;

	MilesCursor_SPRITE.sx = 2 * scale;
	MilesCursor_SPRITE.sy = 2 * scale;
	//float newx = x - (incr * sclx / 2 + incr / 2);


	if (MilesCurTex >= 14)
		MilesCurTex = 0;

	if (FrameCounterUnpaused % 10 == 0 && MilesCurTex < 15)
		MilesCurTex++;


	MilesCursor_SPRITE.p.x = CursorArray[Cursor].x;
	MilesCursor_SPRITE.p.y = CursorArray[Cursor].y;
	njDrawSprite2D_Queue(&MilesCursor_SPRITE, MilesCurTex, -1.501, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, QueuedModelFlagsB_SomeTextureThing);
}

void CheckPlayerCursorPos() {

	if (ControllerPointers[0]->PressedButtons & Buttons_Up) {
		if (Cursor >= 8)
			Cursor = 0;
		else
			Cursor++;
		CheckAndLoadMapPVM();
	}

	if (ControllerPointers[0]->PressedButtons & Buttons_Down) {
		if (Cursor <= 0)
			Cursor = 8;
		else
			Cursor--;
		CheckAndLoadMapPVM();
	}
}

void __cdecl DisplayMilesMap_r()
{

	signed int v3; // ebx
	int v4; // edi
	signed int v5; // esi
	float a3; // ST10_4
	float a2; // ST0C_4
	double v8 = 0; // st7
	double v9 = 0; // st6
	signed int v12; // [esp+1Ch] [ebp-4h]
	float v13; // [esp+1Ch] [ebp-4h]
	signed int v14; // [esp+24h] [ebp+4h]


	if (Cursor >= Sstation && Cursor <= SChaos0)
		njSetTexture(&map_ss_TEXLIST);

	if (Cursor == ECarrier)
		njSetTexture(&map_ec_X_TEXLIST);

	if (Cursor >= MRStation && Cursor <= MRJungleBig)
		njSetTexture(&map_mr_X_TEXLIST);

	SetVtxColorB(0xFFFFFFFF);
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	v3 = 0;
	v12 = 0;
	v4 = TextLanguage != 0 ? 6 : 0;
	do
	{
		v5 = 0;
		v14 = 0;
		v13 = (double)v12 * 256.0 - 16.0 - 240.0;
		do
		{
			a3 = VerticalStretch * 240.0 + v13;
			a2 = (double)v14 * 256.0 - 64.0 - 320.0 + HorizontalStretch * 320.0;

			DisplayScreenTexture(v4 + v5++, a2, a3, 1.1);
			v14 = v5;
		} while (v5 < 3);
		++v3;
		v4 += 3;
		v12 = v3;
	} while (v3 < 2);
}

void PauseMenuMap_OriginalFunction() {

	switch ((unsigned __int16)((((unsigned __int16)CurrentAct | (unsigned __int16)(CurrentLevel << 8)) & 0xFF00) >> 8))
	{
	case 26u:
	case 27u:
	case 28u:
		sub_62ECE0((unsigned __int8)CurrentAct, &Current_CharObj1->Position);
		break;
	case 29u:
	case 30u:
	case 31u:
		sub_51C130((unsigned __int8)CurrentAct, &Current_CharObj1->Position);
		break;
	case 32u:
		sub_525980((unsigned __int8)CurrentAct, &Current_CharObj1->Position);
		break;
	case LevelIDs_MysticRuins:
		sub_52F9C0((unsigned __int8)CurrentAct, &Current_CharObj1->Position);
		break;
	case LevelIDs_Past:
		sub_541BF0((unsigned __int8)CurrentAct, &Current_CharObj1->Position);
		break;
	}
}

void __cdecl PauseMenu_Map_Display_r() {

	if (GameState == 16) //pause menu
	{
		PauseMenuMap_OriginalFunction();
		return;
	}

	if (Cursor < 0 || Cursor > 8)
		return;

	DisplayMilesMap_r();
	DisplayCursorAnimation();
	
	return;
}

bool isTailsAI_GrabAllowed() {

	EntityData1* p1 = EntityData1Ptrs[0];

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 0)
	{
		if (p1->Position.x <= -410 && p1->Position.x > -672 && p1->Position.z > 772 && p1->Position.z < 880)
			return false;
	}

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 1) {
		if (p1->Position.z < 1020)
			return false;
	}

	if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 2 || CurrentAct == 5))
		return false;

	if (CurrentLevel == LevelIDs_EggCarrierOutside && CurrentAct != 0)
		return false;

	if (CurrentLevel < LevelIDs_StationSquare || CurrentLevel == LevelIDs_EggCarrierInside || CurrentLevel > LevelIDs_MysticRuins)
		return false;

	return true;
}

void TailsAI_GrabDelete(ObjectMaster* obj) {
	if (TailsGrab) {
		TailsGrab = nullptr;
	}
}

void ForceLeavingTailsAI(EntityData1* data) {

	EntityData1* p1 = EntityData1Ptrs[0];
	CharObj2* co2p1 = CharObj2Ptrs[0];

	if (data->Action >= grabbed && data->Action <= data->Action < leaving)
	{
		if (ControllerPointers[0]->PressedButtons & Buttons_B) {
			co2p1->AnimationThing.Index = 14;
			p1->Status |= Status_Ball;
			data->Action = leaving;
		}
	}
}

void TailsAI_Grab(ObjectMaster* obj) {

	if (obj->Data1->Action != movetoDestination && (!EntityData1Ptrs[0] || !EntityData1Ptrs[1] || GameState != 15)) {
		TailsGrab = nullptr;
		CheckThingButThenDeleteObject(obj);
	}

	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[1];
	EntityData1* data = obj->Data1;
	CharObj2* co2p1 = CharObj2Ptrs[0];
	CharObj2* co2p2 = CharObj2Ptrs[1];

	
	if (data->Action > 0 && data->Action < 3) {

		if (++data->field_A == 230) {
			data->Action = leaving;
		}
	}

	switch (data->Action)
	{
	case initFly:
		obj->DeleteSub = TailsAI_GrabDelete;
		if (!isTailsAI_GrabAllowed()) {
			DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Error, Tails doesn't have enough space to fly");
			if (++data->InvulnerableTime == 120) {
				TailsGrab = nullptr;
				CheckThingButThenDeleteObject(obj);
			}
		}
		else {
			data->Position = p2->Position;
			data->Action = getAltitude;
		}
		break;
	case getAltitude:
		if (p2->Position.y < p1->Position.y + 15) {
			Controllers[1].HeldButtons |= JumpButtons;
			Controllers[1].PressedButtons |= JumpButtons;
		}
		else {
			p2->Status &= 0x100u;
			p2->Action = 125;
			CharObj2Ptrs[1]->AnimationThing.Index = 37;
			data->Action = checkGrab;
		}
		break;
	case checkGrab:
		if (GetCollidingEntityA(p2)) {
			p1->Status &= ~(Status_Attack | Status_Ball | Status_LightDash);
			p1->Action = 125;
			data->Action = grabbed;
		}
		break;
	case grabbed:
		data->Unknown = 0;
		data->InvulnerableTime = 0;
		data->field_A = 0;
		ForceLeavingTailsAI(data);
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
		ForceLeavingTailsAI(data);
		if (++data->Unknown == 50) {
			PlaySound(3, NULL, 0, NULL);
			data->Action = displayMap;
		}
		break;
	case displayMap:
		ForceLeavingTailsAI(data);
		DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Cursor Value %d", Cursor);
		CheckPlayerCursorPos();
		PauseMenu_Map_Display();
		if (ControllerPointers[0]->PressedButtons & Buttons_A || ControllerPointers[0]->PressedButtons & Buttons_Start) {
			data->Unknown = 0;
			data->InvulnerableTime = 0;
			data->field_A = 0;
			if (CurrentLevel == DestinationArray[Cursor].level)
				isMoving = 2;
			else
				isMoving = 1;

			data->Action = movetoDestination;
		}
		break;
	case movetoDestination:
		p2->Action = 15;
		co2p2->Speed = co2p1->Speed;
		p1->Position = p2->Position;
		p1->Position.y -= 5.5f;
		p1->Rotation = p2->Rotation;

		CharObj2Ptrs[1]->Speed.y += 1;
		CharObj2Ptrs[1]->Speed.x += 1;
		
		if (++data->Unknown == 60) {
			CharObj2Ptrs[1]->Speed.y += 2;
			CharObj2Ptrs[1]->Speed.z += 3;
		}

		if (++data->InvulnerableTime == 180) {
			LoadDestination();
			CheckThingButThenDeleteObject(obj);
		}
	
		break;
	case leaving:
		p2->Action = 10;
		ControllerPointers[1]->PressedButtons = 0;
		ControllerPointers[1]->HeldButtons = 0;
		DisableController(1);
		ReleaseAllTravelTexture();
		EnablePause();
		if (p1->Action == 125) {
			p1->Action = 8;
			p1->Status &= 0x100u;
		}
		
		isMoving = 0;
		CheckThingButThenDeleteObject(obj);
		break;
	}
}

void TailsAI_LandingDelete(ObjectMaster* obj) {
	if (TailsLanding) {
		isMoving = 0;
		Cursor = -1;
		TailsLanding = nullptr;
	}
}


void TailsAI_Landing(ObjectMaster* obj) {

	if (!EntityData1Ptrs[0] || !EntityData1Ptrs[1] || GameState != 15 && GameState != 4) {
		return;
	}

	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[1];
	EntityData1* data = obj->Data1;
	CharObj2* co2p1 = CharObj2Ptrs[0];
	CharObj2* co2p2 = CharObj2Ptrs[1];

	LookAt(&EntityData1Ptrs[1]->Position, &obj->Data1->Position, nullptr, &EntityData1Ptrs[1]->Rotation.y);

	switch (data->Action) {
	case 0:
		obj->DeleteSub = TailsAI_LandingDelete;
		data->Position = DestinationArray[Cursor].destination;
		DisableController(0);
		EnableController(1);
		p1->Action = 125;
		p2->Action = 15;
		CharObj2Ptrs[1]->AnimationThing.Index = 37;
		CharObj2Ptrs[0]->AnimationThing.Index = 47;
		data->Action = 1;
		break;
	case 1:
		EnableController(1);
		co2p2->Speed = co2p1->Speed;
		p1->Position = p2->Position;
		p1->Position.y -= 5.5f;
		p1->Rotation = p2->Rotation;
		CharObj2Ptrs[1]->Speed.y -= 0.8;
		CharObj2Ptrs[1]->Speed.z += 0.8;

		if (++data->InvulnerableTime == 140 || ((p1->Status & Status_Ground) == Status_Ground)) {
			data->Action = 2;
		}
		break;
	case 2:
		EnableController(0);
		EnablePause();
		if (p1->Action == 125) {
			p1->Status &= 0x100u;
			CharObj2Ptrs[1]->AnimationThing.Index = 18;
			p1->Action = 12;
		}
		data->Unknown = 0;
		ControllerPointers[1]->PressedButtons = 0;
		ControllerPointers[1]->HeldButtons = 0;
		DisableController(1);
		TailsAI_ptr->Data1->Action = 0;
		isMoving = 0;
		CheckThingButThenDeleteObject(obj);
		break;
	}
}

void TailsAI_Main_R(ObjectMaster* obj) {

	if (obj->Data1->Action == 0 && isMoving == 1 || obj->Data1->Action > 0 && isMoving == 2) {
		if (!TailsGrab && !TailsLanding)
			TailsLanding = LoadObject((LoadObj)2, 1, TailsAI_Landing);
	}

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

	WriteCall((void*)0x458b86, PauseMenu_Map_Display_r);
	WriteCall((void*)0x458bd0, PauseMenu_Map_Display_r);	
	WriteCall((void*)0x458bb8, PauseMenu_Map_Display_r);
	WriteCall((void*)0x458b6e, PauseMenu_Map_Display_r);	

	TailsAI_Main_t = new Trampoline((int)TailsAI_Main, (int)TailsAI_Main + 0x5, TailsAI_Main_R);
	MovePlayerToStartPoint_t = new Trampoline((int)MovePlayerToStartPoint, (int)MovePlayerToStartPoint + 0x6, MovePlayerToStartPoint_r);
}