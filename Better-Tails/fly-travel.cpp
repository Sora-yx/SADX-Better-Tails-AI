#include "stdafx.h"

Trampoline* MovePlayerToStartPoint_t;
task* TailsGrab = nullptr;
uint8_t isMoving = 0;
task* TailsLanding = nullptr;
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
	"Station Square (Main)", "Station Square (Hostel Pool", "Station Square Casino Area",
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

	if (isMoving > 0 && isFlyTravel) {
		EntityData1Ptrs[0]->Position = DestinationArray[Cursor].destination;
		EntityData1Ptrs[0]->Position.y -= 6.5f;
	}
	else {
		FunctionPointer(void, original, (EntityData1 * data), MovePlayerToStartPoint_t->Target());
		original(data);

		Fix_AIPos_ActTransition();
		return;
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
	njDrawSprite2D_Queue(&MilesCursor_SPRITE, MilesCurTex, -1.501f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, QueuedModelFlagsB_SomeTextureThing);
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
	signed int v3 = 0;
	int texId = 0;
	signed int diff = 0;
	float posY = 0.0f;
	float posX = 0.0f;
	signed int v12 = 0;
	float diffPosY = 0.0f;
	signed int v14 = 0;

	njSetTexture(&TravelMap_TEXLIST);

	SetVtxColorB(0xFFFFFFFF);
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);


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
		diff = 0;
		v14 = 0;
		diffPosY = v12 * 256.0f - 16.0f - 240.0f;
		do
		{
			posY = VerticalStretch * 240.0f + diffPosY;
			posX = (float)v14 * 256.0f - 64.0f - 320.0f + HorizontalStretch * 320.0f;

			DisplayScreenTexture(texId + diff++, posX, posY, 1.1f);

			v14 = diff;
		} while (diff < 3);
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

	auto p1 = playertwp[0];

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 0)
	{
		if (p1->pos.x <= -410.0f && p1->pos.x > -672.0f && p1->pos.z > 772.0f && p1->pos.z < 880.0f)
			return false;
	}

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 1) {
		if (p1->pos.z < 1020.0f)
			return false;
	}

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 3 || CurrentAct == 4)
		return false;

	if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 2 || CurrentAct == 5))
		return false;

	if (CurrentLevel == LevelIDs_StationSquare && CurrentAct == 1 && p1->pos.x > -370.0f)
		return false;

	if (CurrentLevel == LevelIDs_EggCarrierOutside && CurrentAct != 0 && CurrentAct != 2)
		return false;

	if (CurrentLevel < LevelIDs_StationSquare || CurrentLevel == LevelIDs_EggCarrierInside || CurrentLevel > LevelIDs_MysticRuins)
		return false;

	return true;
}

void CheckAndForceLeavingGrab(taskwk* data) {

	if (data->mode >= grabbed && data->mode <= leaving)
	{
		if (ControllerPointers[0]->PressedButtons & Buttons_B) {
			PlaySound(3, NULL, 0, NULL);
			data->mode = leaving;
		}
	}
}

void FlySoundOnFrames(int playerID) {

	auto ai = playertwp[playerID];

	if (GameState != 15 || !ai || !TailsAI_ptr)
		return;

	if (ai->charID == Characters_Tails && playertwp[0]->charID <= Characters_Tails) {
		if (EntityData1Ptrs[playerID]->Unknown == 0)
			PlaySound(0x302, NULL, 0, NULL);

		if (++EntityData1Ptrs[playerID]->Unknown == 31) {
			PlaySound(0x302, NULL, 0, NULL);
			EntityData1Ptrs[playerID]->Unknown = 1;
		}
	}
}

void PlayCharacterGrabAnimation(taskwk* p1, playerwk* co2) {

	switch (p1->charID) {

	case Characters_Sonic:
		if ((co2->equipment & Upgrades_SuperSonic) == 0)
			co2->mj.reqaction = 47;
		else
			co2->mj.reqaction = 141;
		break;
	case Characters_Knuckles:
		co2->mj.reqaction = 84;
		break;
	case Characters_Amy:
		co2->mj.reqaction = 55;
		break;
	case Characters_Big:
		co2->mj.reqaction = 32;
		break;
	}

	return;
}

void PlayCharacterLeaveAnimation(taskwk* p1, playerwk* co2, int playerID) {

	if (p1->mode < 100)
		return;

	switch (p1->charID) {
	case Characters_Sonic:

		if (co2->equipment & Upgrades_SuperSonic) {
			p1->mode = 82;
			co2->mj.reqaction = 145;
		}
		else {
			p1->mode = 12;
			CharObj2Ptrs[playerID]->AnimationThing.Index = 18;
		}

		break;
	case Characters_Knuckles:
		p1->mode = 10;
		co2->mj.reqaction = 82;
		break;
	case Characters_Amy:
		p1->mode = 5;
		co2->mj.reqaction = 18;
		break;
	case Characters_Big:
		p1->mode = 4;
		co2->mj.reqaction = 15;
		break;
	case Characters_Gamma:
		p1->mode = 4;
		co2->mj.reqaction = 6;
		break;
	}

	return;
}

float GetCharacterPositionY(taskwk* p1) {
	switch (p1->charID) {
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

void UpdateP1Position(playerwk* co2p1, playerwk* co2p2, taskwk* p1, taskwk* p2) {
	co2p2->spd= co2p1->spd;
	p1->pos = p2->pos;
	p1->pos.y -= GetCharacterPositionY(p1);
	p1->ang = p2->ang;
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

void TailsAI_GrabDelete(task* obj) {
	if (TailsGrab) {
		TailsGrab = nullptr;
	}
}

void TailsAI_Grab(task* obj) {

	auto data = obj->twp;
	auto p1 = playertwp[0];
	auto pnum = data->pNum;
	auto p2 = playertwp[pnum];

	auto co2p1 = playerpwp[0];
	auto co2p2 = playerpwp[pnum];

	if (data->mode != movetoDestination && (!p1
		|| !p2 || GameState != 15 || TailsLanding || isMilesSaving())) {

		if (p2) {
			if (p2->mode == 125) //failsafe if the player start fly travel but leave the level/act
				p2->mode = 1;
		}
		TailsGrab = nullptr;
		FreeTask(obj);
	}

	if ((++data->counter.w[1] == 230 && data->mode > initFly && data->mode < grabbed) || p2->charID != Characters_Tails) {
		data->mode = leaving;
	}

	switch (data->mode)
	{
	case initFly:
		obj->dest = TailsAI_GrabDelete;
		if (!isTailsAI_GrabAllowed()) {
			if (++data->wtimer == 20)
				FreeTask(obj);
		}
		else {
			data->pos = p2->pos;
			data->mode = getAltitude;
		}
		break;
	case getAltitude:
		if ((p1->charID <= Characters_Amy && p2->pos.y < p1->pos.y + 15.0f) || (p1->charID >= Characters_Gamma && p2->pos.y < p1->pos.y + 25.0f)) {
			Controllers[pnum].HeldButtons |= JumpButtons;
			Controllers[pnum].PressedButtons |= JumpButtons;
		}
		else {
			p2->flag &= 0x100u;
			p2->mode = 125;
			co2p2->mj.reqaction = 37;
			data->mode = checkGrab;
		}
		break;
	case checkGrab:
		if (GetCollidingEntityA((EntityData1*)p2)) {
			p1->flag &= ~(Status_Attack | Status_Ball | Status_LightDash);
			p1->mode = 125;
			data->mode = grabbed;
		}
		break;
	case grabbed:
		data->id = 0;
		data->wtimer = 0;
		data->counter.w[1] = 0;

		UpdateP1Position(co2p1, co2p2, p1, p2);
		CheckAndForceLeavingGrab(data);
		DisablePause();
		PlayCharacterGrabAnimation(p1, co2p1);

		Cursor = setCursorPos(CurrentLevel, CurrentAct);
		if (Cursor > -1) {
			data->mode = transitionMap;
		}
		else {
			data->mode = leaving;
		}
		break;
	case transitionMap:
		CheckAndForceLeavingGrab(data);
		if (++data->id == 50) {
			PlaySound(21, NULL, 0, NULL);
			data->btimer = 0;
			data->mode = displayMap;
		}
		break;
	case displayMap:

		CheckAndForceLeavingGrab(data);
		SetDebugFontSize(20);
		DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Destination: %s", getDestinationText());
		UpdatePlayerCursorPos();
		DrawModelCallback_Queue((void(__cdecl*)(void*))PauseMenu_Map_DisplayCallback, 0, 22047.998f, QueuedModelFlagsB_EnableZWrite); //fix transparency issue

		if (ControllerPointers[0]->PressedButtons & Buttons_A || ControllerPointers[0]->PressedButtons & Buttons_Start) {
			if (!CheckFastTravelStoryProgression()) {
				data->btimer = 0;
				data->mode = errorMove;
			}
			else {
				PlaySound(0x2, NULL, 0, NULL);
				if (CurrentLevel == DestinationArray[Cursor].level)
					isMoving = 2;
				else
					isMoving = 1;
				data->id = 0;
				data->wtimer = 0;
				data->counter.w[1] = 0;
				data->mode = movetoDestination;
			}
		}
		break;
	case movetoDestination:
		FlySoundOnFrames(pnum);
		p2->mode = 15; //fly mode
		UpdateP1Position(co2p1, co2p2, p1, p2);

		co2p2->spd.y += 0.4f;
		co2p2->spd.x += 0.8f;

		if (++data->wtimer == 180) {
			LoadDestination(pnum);

			if (isMoving == 2) //object isn't deleted between act transition unlike with level changes.
				FreeTask(obj);
		}

		break;
	case leaving:
		p2->mode = 10;
		RestoreAIControl(pnum);
		Cursor = -1;
		EnablePause();
		PlayCharacterLeaveAnimation(p1, co2p1, pnum);
		isMoving = 0;
		FreeTask(obj);
		break;
	case errorMove:
		SetDebugFontSize(23);
		SetDebugFontColor(0xFF0000);
		DisplayDebugStringFormatted(NJM_LOCATION(12, 12), "You cannot fly here at the moment.");
		if (++data->btimer == 60) {
			data->id = 0;
			data->wtimer = 0;
			data->mode = displayMap;
			SetDebugFontColor(0xFFFFFFFF);
		}
	}
}

void TailsAI_LandingDelete(task* obj) {
	if (TailsLanding) {
		isMoving = 0;
		Cursor = -1;
		TailsLanding = nullptr;
	}
}

void TailsAI_Landing(task* obj) {

	auto data = obj->twp;
	auto p1 = playertwp[0];
	auto pnum = data->pNum;
	auto p2 = playertwp[pnum];

	auto co2p1 = playerpwp[0];
	auto co2p2 = playerpwp[pnum];

	if (!p1 || !p2 || GameState != 15 && GameState != 4) {
		return;
	}

	if (EV_MainThread_ptr)
	{
		TailsAI_LandingDelete(obj);
		FreeTask(obj);
		return;
	}

	LookAt(&p2->pos, &data->pos, nullptr, &p2->ang.y);
	FlySoundOnFrames(pnum);

	switch (data->mode) {
	case 0:
		obj->dest = TailsAI_LandingDelete;
		data->pos = DestinationArray[Cursor].destination;
		p1->ang = p2->ang;
		p1->mode = 125;
		p2->mode = 15;
		co2p2->mj.reqaction = 37;
		PlayCharacterGrabAnimation(p1, co2p1);
		data->mode++;
		break;
	case 1:
		UpdateP1Position(co2p1, co2p2, p1, p2);
		co2p2->spd.y -= 0.8f;
		co2p2->spd.z += 0.8f;

		if (++data->wtimer== 140 || (p1->flag & 3)) {
			data->mode++;
		}
		break;
	case 2:

		PlayCharacterLeaveAnimation(p1, co2p1, pnum);
		RestoreAIControl(pnum);
		ForcePlayerAction(0, 24);
		EnableTailsAI_Controller(pnum);
		EnableControl();
		EnablePause();
		FreeTask(obj);
		break;
	}
}


void CheckAndLoadTailsTravelObjects(task* obj) {

	taskwk* data = obj->twp;

	char pid = AIIndex;

	if (!playertwp[pid])
		return;

	if (data->mode == 0 && isMoving == 1 || data->mode > 0 && isMoving == 2) {
		if (!TailsGrab && !TailsLanding) {
			TailsLanding = CreateElementalTask((LoadObj)2, 1, TailsAI_Landing);
			TailsLanding->twp->pNum = pid;
		}
	}

	if (data->mode > 0) {

		if (isInputModActive() && ControllerPointers[0]->PressedButtons & Buttons_C || !isInputModActive() && ControllerPointers[0]->PressedButtons & Buttons_Y)
		{
			if (playertwp[pid]->charID == Characters_Tails && playertwp[pid]->mode < 3 && playertwp[0]->mode < 3) {

				if (!TailsGrab) {
					TailsGrab = CreateElementalTask((LoadObj)2, 1, TailsAI_Grab);
					TailsGrab->twp->pNum = pid;
				}
			}
		}
	}
}

void FlyTravel_Init() {

	if (isFlyTravel) {
		WriteCall((void*)0x458b86, PauseMenu_Map_Display_r);
		WriteCall((void*)0x458bd0, PauseMenu_Map_Display_r);
		WriteCall((void*)0x458bb8, PauseMenu_Map_Display_r);
		WriteCall((void*)0x458b6e, PauseMenu_Map_Display_r);
	}

	MovePlayerToStartPoint_t = new Trampoline((int)MovePlayerToStartPoint, (int)MovePlayerToStartPoint + 0x6, MovePlayerToStartPoint_r);
}