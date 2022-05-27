#include "stdafx.h"

Trampoline* MovePlayerToStartPoint_t;
ObjectMaster* TailsGrab = nullptr;
uint8_t isMoving = 0;
ObjectMaster* TailsLanding = nullptr;
const int8_t cursorMaxPos = 8;

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

NJS_TEXNAME TravelMap_TEXNAMES[30];
NJS_TEXLIST TravelMap_TEXLIST = { arrayptrandlength(TravelMap_TEXNAMES) };

int Cursor = -1;
int MilesCurTex = 0;

const char* DestinationText[9] = {
	"Station Square (Main)", "Station Square (Hostel Pool)", "Station Square Casino Area",
	"Station Square City Hall", "Egg Carrier (Outside)", "Mystic Ruins (Station)", "Mystic Ruins (Angel Island)",
	"Mystic Ruins (Jungle Temple)", "Mystic Ruins (Big's House)"
};

const char* getDestinationText() {
	return DestinationText[Cursor];
}

//Tails Grab Fly abilities
MilesAI_Fly DestinationArray[]{
	{ LevelIDs_StationSquare, 3,  42.5287, 363.153, 1799.71, {300, 280}}, //Station (main)
	{ LevelIDs_StationSquare, 4, -473.697, 202.824, 2051.46, {170, 320 }}, //Hostel pool
	{ LevelIDs_StationSquare, 1, -445.595, 380.35, 1462.26, {170, 150}},	//Casino area
	{ LevelIDs_StationSquare, 0, 275.532, 250.9145, 328.018, {300, 60}}, //chaos 0
	{ LevelIDs_EggCarrierOutside, 0,  97.0032, 950.573, 817.829, {420, 160}},
	{ LevelIDs_MysticRuins, 0, -48.1315, 300.108, 1033.91, {180, 300}},	//station
	{ LevelIDs_MysticRuins, 1, -9.2187, -15.838, 2220.16, {352, 310}}, //angel island
	{ LevelIDs_MysticRuins, 2, -515.191, 287.349, -865.042, {245, 180}},	//jungle lost world
	{ LevelIDs_MysticRuins, 2,  1307.67, 284.549, -814.303, {455, 195}}, //jungle big's house
};

void LoadDestination(int playerID) {
	ControllerPointers[playerID]->PressedButtons = 0;
	ControllerPointers[playerID]->HeldButtons = 0;
	DisableTailsAI_Controller(playerID);
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
	LoadPVM("TravelMapEng", &TravelMap_TEXLIST);
	LoadPVM("MilesCursor", &MilesCursor_TEXLIST);
	return;
}

void ReleaseAllTravelTexture() {
	njReleaseTexture(&TravelMap_TEXLIST);
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
	if (!isUIScale())
		return;

	SetMaterialAndSpriteColor_Float(1, 1, 1, 1);

	float scale = 2.0F;
	float x = (float)HorizontalResolution / 2.0f;
	float y = (float)VerticalResolution / 2.0f;
	float incr = 0x10 * scale;
	float sclx = 10;
	float scly = 1;

	MilesCursor_SPRITE.sx = 2 * scale;
	MilesCursor_SPRITE.sy = 2 * scale;

	if (MilesCurTex >= 14)
		MilesCurTex = 0;

	if (FrameCounterUnpaused % 10 == 0 && MilesCurTex < 15)
		MilesCurTex++;

	MilesCursor_SPRITE.p.x = DestinationArray[Cursor].cursor.x;
	MilesCursor_SPRITE.p.y = DestinationArray[Cursor].cursor.y;
	njDrawSprite2D_Queue(&MilesCursor_SPRITE, MilesCurTex, -1.501, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, QueuedModelFlagsB_SomeTextureThing);
}

void UpdatePlayerCursorPos() {
	if (PressedButtons[0] & Buttons_Up) {
		if (Cursor >= cursorMaxPos)
			Cursor = 0;
		else
			Cursor++;

		PlaySound(1, NULL, 0, NULL);
	}

	if (PressedButtons[0] & Buttons_Down) {
		if (Cursor <= 0)
			Cursor = cursorMaxPos;
		else
			Cursor--;

		PlaySound(1, NULL, 0, NULL);
	}
}

void __cdecl DisplayMilesMap_r()
{
	signed int v3; // ebx
	int texId = 0; // edi
	signed int v5; // esi
	float a3; // ST10_4
	float a2; // ST0C_4
	signed int v12; // [esp+1Ch] [ebp-4h]
	float v13; // [esp+1Ch] [ebp-4h]
	signed int v14; // [esp+24h] [ebp+4h]

	njSetTexture(&TravelMap_TEXLIST);

	SetVtxColorB(0xFFFFFFFF);
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);

	v3 = 0;
	v12 = 0;

	if (Cursor >= Sstation && Cursor <= SChaos0)
		texId = 0;

	if (Cursor == ECarrier)
		texId = 24;

	if (Cursor == MRStation)
		texId = 6;

	if (Cursor == MRAngelIsland)
		texId = 12;

	if (Cursor == MRJungleLW || Cursor == MRJungleBig)
		texId = 18;

	do
	{
		v5 = 0;
		v14 = 0;
		v13 = (double)v12 * 256.0f - 16.0f - 240.0f;
		do
		{
			a3 = VerticalStretch * 240.0f + v13;
			a2 = (float)v14 * 256.0f - 64.0f - 320.0f + HorizontalStretch * 320.0f;

			DisplayScreenTexture(texId + v5++, a2, a3, 1.1);

			v14 = v5;
		} while (v5 < 3);
		++v3;
		texId += 3;
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

	DisplayMilesMap_r(),
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

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 3 || CurrentAct == 4)
		return false;

	if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 2 || CurrentAct == 5))
		return false;

	if (CurrentLevel == LevelIDs_StationSquare && CurrentAct == 1 && p1->Position.x > -370)
		return false;

	if (CurrentLevel == LevelIDs_EggCarrierOutside && CurrentAct != 0 && CurrentAct != 2)
		return false;

	if (CurrentLevel < LevelIDs_StationSquare || CurrentLevel == LevelIDs_EggCarrierInside || CurrentLevel > LevelIDs_MysticRuins)
		return false;

	return true;
}

void CheckAndForceLeavingGrab(EntityData1* data) {
	EntityData1* p1 = EntityData1Ptrs[0];
	CharObj2* co2p1 = CharObj2Ptrs[0];

	if (data->Action >= grabbed && data->Action <= leaving)
	{
		if (ControllerPointers[0]->PressedButtons & Buttons_B) {
			PlaySound(3, NULL, 0, NULL);
			data->Action = leaving;
		}
	}
}

void FlySoundOnFrames(int playerID) {
	if (GameState != 15 || !EntityData1Ptrs[playerID] || !TailsAI_ptr)
		return;

	if (EntityData1Ptrs[playerID]->CharID == Characters_Tails && EntityData1Ptrs[0]->CharID <= Characters_Tails) {
		if (EntityData1Ptrs[playerID]->Unknown == 0)
			PlaySound(0x302, NULL, 0, NULL);

		if (++EntityData1Ptrs[playerID]->Unknown == 31) {
			PlaySound(0x302, NULL, 0, NULL);
			EntityData1Ptrs[playerID]->Unknown = 1;
		}
	}
}

void PlayCharacterGrabAnimation(EntityData1* p1, CharObj2* co2) {
	switch (p1->CharID) {
	case Characters_Sonic:
		if ((co2->Upgrades & Upgrades_SuperSonic) == 0)
			co2->AnimationThing.Index = 47;
		else
			co2->AnimationThing.Index = 141;
		break;
	case Characters_Knuckles:
		co2->AnimationThing.Index = 84;
		break;
	case Characters_Amy:
		co2->AnimationThing.Index = 55;
		break;
	case Characters_Big:
		co2->AnimationThing.Index = 32;
		break;
	}

	return;
}

void PlayCharacterLeaveAnimation(EntityData1* p1, CharObj2* co2, int playerID) {

	if (p1->Action < 100)
		return;

	switch (p1->CharID) {
	case Characters_Sonic:
		if (co2->Upgrades & Upgrades_SuperSonic) {
			p1->Action = 82;
			co2->AnimationThing.Index = 145;
		}
		else {
			p1->Action = 12;
			CharObj2Ptrs[playerID]->AnimationThing.Index = 18;
		}

		break;
	case Characters_Knuckles:
		p1->Action = 10;
		co2->AnimationThing.Index = 82;
		break;
	case Characters_Amy:
		p1->Action = 5;
		co2->AnimationThing.Index = 18;
		break;
	case Characters_Big:
		p1->Action = 4;
		co2->AnimationThing.Index = 15;
		break;
	case Characters_Gamma:
		p1->Action = 4;
		co2->AnimationThing.Index = 6;
		break;
	}

	return;
}

float GetCharacterPositionY(EntityData1* p1) {
	switch (p1->CharID) {
	case Characters_Sonic:
	case Characters_Knuckles:
	default:
		return 6.5f;
	case Characters_Amy:
		return 5.5f;
	case Characters_Gamma:
	case Characters_Big:
		return 18.5f;
	}
}

void UpdateP1Position(CharObj2* co2p1, CharObj2* co2p2, EntityData1* p1, EntityData1* p2) {
	co2p2->Speed = co2p1->Speed;
	p1->Position = p2->Position;
	p1->Position.y -= GetCharacterPositionY(p1);
	p1->Rotation = p2->Rotation;
	return;
}

void RestoreAIControl(unsigned char ID) {
	ControllerPointers[ID]->PressedButtons = 0;
	ControllerPointers[ID]->HeldButtons = 0;
	DisableTailsAI_Controller(ID);
	return;
}

int CheckFastTravelStoryProgression() {

	if (DestinationArray[Cursor].level == CurrentLevel && CurrentAct == DestinationArray[Cursor].act)
		return 0;

	if (CurrentCharacter >= Characters_Gamma && Cursor == 2)
		return 0;

	if (IsAdventureComplete(SelectedCharacter))
		return 1;

	if (!IsEggCarrierSunk() && CurrentLevel != LevelIDs_EggCarrierOutside && Cursor == ECarrier) {
		return 0;
	}

	if (!TrainsInService())
	{
		if (CurrentLevel == LevelIDs_StationSquare && (Cursor >= MRStation && Cursor <= MRJungleBig || Cursor == SCasinoArea)) {
			return 0;
		}

		if (CurrentLevel == LevelIDs_MysticRuins && Cursor >= Sstation && Cursor <= SChaos0) {
			return 0;
		}
	}

	if (Cursor == MRAngelIsland) {
		int v14 = IslandDoor_Col[GetCharacterID(0)];
		if (v14 < 0 || !EventFlagArray[v14]) {
			return 0;
		}
	}

	if (!isHostelOpen() && Cursor == ShostelPool) {
		return 0;
	}


	return 1;
}

void TailsAI_GrabDelete(ObjectMaster* obj) {
	if (TailsGrab) {
		TailsGrab = nullptr;
	}
}

void TailsAI_Grab(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;
	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[data->CharIndex];

	CharObj2* co2p1 = CharObj2Ptrs[0];
	CharObj2* co2p2 = CharObj2Ptrs[data->CharIndex];

	if (data->Action != movetoDestination && (!p1
		|| !p2 || GameState != 15 || TailsLanding || isMilesSaving())) {

		if (p2) {
			if (p2->Action == 125) //failsafe if the player start fly travel but leave the level/act
				p2->Action = 1;
		}
		TailsGrab = nullptr;
		CheckThingButThenDeleteObject(obj);
	}



	if ((++data->field_A == 230 && data->Action > initFly && data->Action < grabbed) || p2->CharID != Characters_Tails) {
		data->Action = leaving;
	}

	switch (data->Action)
	{
	case initFly:
		obj->DeleteSub = TailsAI_GrabDelete;
		if (!isTailsAI_GrabAllowed()) {
			if (++data->InvulnerableTime == 20)
				CheckThingButThenDeleteObject(obj);
		}
		else {
			data->Position = p2->Position;
			data->Action = getAltitude;
		}
		break;
	case getAltitude:
		if ((p1->CharID <= Characters_Amy && p2->Position.y < p1->Position.y + 15) || (p1->CharID >= Characters_Gamma && p2->Position.y < p1->Position.y + 25)) {
			Controllers[data->CharIndex].HeldButtons |= JumpButtons;
			Controllers[data->CharIndex].PressedButtons |= JumpButtons;
		}
		else {
			p2->Status &= 0x100u;
			p2->Action = 125;
			co2p2->AnimationThing.Index = 37;
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

		UpdateP1Position(co2p1, co2p2, p1, p2);
		CheckAndForceLeavingGrab(data);
		DisablePause();
		PlayCharacterGrabAnimation(p1, co2p1);

		Cursor = setCursorPos(CurrentLevel, CurrentAct);
		if (Cursor > -1) {
			data->Action = transitionMap;
		}
		else {
			data->Action = leaving;
		}
		break;
	case transitionMap:
		CheckAndForceLeavingGrab(data);
		if (++data->Unknown == 50) {
			PlaySound(21, NULL, 0, NULL);
			data->Index = 0;
			data->Action = displayMap;
		}
		break;
	case displayMap:

		CheckAndForceLeavingGrab(data);
		SetDebugFontSize(20);
		DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Destination: %s", getDestinationText());
		UpdatePlayerCursorPos();
		DrawModelCallback_Queue((void(__cdecl*)(void*))PauseMenu_Map_DisplayCallback, 0, 22047.998, QueuedModelFlagsB_EnableZWrite); //fix transparency issue

		if (ControllerPointers[0]->PressedButtons & Buttons_A || ControllerPointers[0]->PressedButtons & Buttons_Start) {
			if (!CheckFastTravelStoryProgression()) {
				data->Index = 0;
				data->Action = errorMove;
			}
			else {
				PlaySound(0x2, NULL, 0, NULL);
				if (CurrentLevel == DestinationArray[Cursor].level)
					isMoving = 2;
				else
					isMoving = 1;
				data->Unknown = 0;
				data->InvulnerableTime = 0;
				data->field_A = 0;
				data->Action = movetoDestination;
			}
		}
		break;
	case movetoDestination:
		FlySoundOnFrames(data->CharIndex);
		p2->Action = 15; //fly mode
		UpdateP1Position(co2p1, co2p2, p1, p2);

		co2p2->Speed.y += 0.4f;
		co2p2->Speed.x += 0.8f;

		if (++data->InvulnerableTime == 180) {
			LoadDestination(data->CharIndex);
			if (isMoving == 2) //object isn't deleted between act transition unlike with level changes.
				CheckThingButThenDeleteObject(obj);
		}

		break;
	case leaving:
		p2->Action = 10;
		RestoreAIControl(data->CharIndex);
		Cursor = -1;
		EnablePause();
		PlayCharacterLeaveAnimation(p1, co2p1, data->CharIndex);
		isMoving = 0;
		CheckThingButThenDeleteObject(obj);
		break;
	case errorMove:
		SetDebugFontSize(23);
		SetDebugFontColor(0xFF0000);
		DisplayDebugStringFormatted(NJM_LOCATION(12, 12), "You cannot fly here at the moment.");
		if (++data->Index == 60) {
			data->Unknown = 0;
			data->InvulnerableTime = 0;
			data->Action = displayMap;
			SetDebugFontColor(0xFFFFFFFF);
		}
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

	EntityData1* data = obj->Data1;
	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[data->CharIndex];

	CharObj2* co2p1 = CharObj2Ptrs[0];
	CharObj2* co2p2 = CharObj2Ptrs[data->CharIndex];

	if (!p1 || !p2 || GameState != 15 && GameState != 4) {
		return;
	}

	if (EV_MainThread_ptr)
	{
		TailsAI_LandingDelete(obj);
		CheckThingButThenDeleteObject(obj);
		return;
	}


	LookAt(&p2->Position, &data->Position, nullptr, &p2->Rotation.y);
	FlySoundOnFrames(data->CharIndex);

	switch (data->Action) {
	case 0:
		obj->DeleteSub = TailsAI_LandingDelete;
		data->Position = DestinationArray[Cursor].destination;
		p1->Rotation = p2->Rotation;
		p1->Action = 125;
		p2->Action = 15;
		co2p2->AnimationThing.Index = 37;
		PlayCharacterGrabAnimation(p1, co2p1);
		data->Action++;
		break;
	case 1:
		UpdateP1Position(co2p1, co2p2, p1, p2);
		co2p2->Speed.y -= 0.8f;
		co2p2->Speed.z += 0.8f;

		if (++data->InvulnerableTime == 140 || ((p1->Status & Status_Ground)) || (p1->Status & Status_OnColli)) {
			data->Action++;
		}
		break;
	case 2:

		PlayCharacterLeaveAnimation(p1, co2p1, data->CharIndex);
		RestoreAIControl(data->CharIndex);
		ForcePlayerAction(0, 24);
		EnableTailsAI_Controller(data->CharIndex);
		EnableControl();
		EnablePause();
		CheckThingButThenDeleteObject(obj);
		break;
	}
}

extern unsigned char AIIndex;
void CheckAndLoadTailsTravelObjects(task* obj) {

	taskwk* data = obj->twp;

	char pid = AIIndex;

	if (!EntityData1Ptrs[pid])
		return;

	if (data->mode == 0 && isMoving == 1 || data->mode > 0 && isMoving == 2) {
		if (!TailsGrab && !TailsLanding) {
			TailsLanding = LoadObject((LoadObj)2, 1, TailsAI_Landing);
			TailsLanding->Data1->CharIndex = pid;
		}
	}

	if (data->mode > 0) {
		if (isInputModActive() && ControllerPointers[0]->PressedButtons & Buttons_C || !isInputModActive() && ControllerPointers[0]->PressedButtons & Buttons_Y) 
		{
			if (EntityData1Ptrs[pid]->CharID == Characters_Tails && EntityData1Ptrs[0]->Action < 3 && EntityData1Ptrs[pid]->Action < 3) {

				if (!TailsGrab) {
					TailsGrab = LoadObject((LoadObj)2, 1, TailsAI_Grab);
					TailsGrab->Data1->CharIndex = pid;
				}
			}
		}
	}
}

void FlyTravel_Init() {
	WriteCall((void*)0x458b86, PauseMenu_Map_Display_r);
	WriteCall((void*)0x458bd0, PauseMenu_Map_Display_r);
	WriteCall((void*)0x458bb8, PauseMenu_Map_Display_r);
	WriteCall((void*)0x458b6e, PauseMenu_Map_Display_r);

	MovePlayerToStartPoint_t = new Trampoline((int)MovePlayerToStartPoint, (int)MovePlayerToStartPoint + 0x6, MovePlayerToStartPoint_r);
}