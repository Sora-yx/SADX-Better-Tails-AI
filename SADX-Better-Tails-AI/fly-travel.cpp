#include "pch.h"
#include "ScaleInfo.h"

static FunctionHook<void, taskwk*> MovePlayerToStartPoint_t((intptr_t)MovePlayerToStartPoint);

task* TailsGrab = nullptr;
uint8_t isMoving = 0;
task* TailsLanding = nullptr;
const int8_t cursorMaxPos = 8;
static std::string destTitle = "";

static std::vector<std::string> DestinationText;
static const uint8_t defaultFrames = 15;
static uint16_t nbFrames = defaultFrames;
static int frameTransition = 2;

enum
{
	LvlChange = 1,
	ActChange
};

std::vector<NJS_TEXANIM> MilesCursor_TEXANIM
{
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
NJS_SPRITE MilesCursor_SPRITE = { { 0, 0, 0 }, 1.0, 1.0, 0, &MilesCursor_TEXLIST, MilesCursor_TEXANIM.data() };

NJS_TEXNAME TravelMap_TEXNAMES[30];
NJS_TEXLIST TravelMap_TEXLIST = { arrayptrandlength(TravelMap_TEXNAMES) };

static int Cursor = -1;
static int MilesCurTex = 0;

bool isFlyTravelEnabled()
{
	return TailsGrab || TailsLanding;
}

const char* getDestinationText() {

	if (DestinationText.size() > 1)
		return DestinationText[Cursor].c_str();
	else
		return "";
}

//Tails Grab Fly abilities
MilesAI_Fly DestinationArray[]{
	{ LevelIDs_StationSquare, 3,  42.5287, 373.153, 1799.71, {300, 280}}, //Station (main)
	{ LevelIDs_StationSquare, 4, -473.697, 212.824, 2051.46, {170, 320 }}, //Hostel pool
	{ LevelIDs_StationSquare, 1, -445.595, 390.35, 1462.26, {170, 150}},	//Casino area
	{ LevelIDs_StationSquare, 0, 275.532, 260.9145, 328.018, {300, 60}}, //chaos 0
	{ LevelIDs_EggCarrierOutside, 0,  97.0032, 960.573, 817.829, {420, 160}},
	{ LevelIDs_MysticRuins, 0, -48.1315, 310.108, 1033.91, {180, 300}},	//station
	{ LevelIDs_MysticRuins, 1, -9.2187, -15.838, 2220.16, {352, 310}}, //angel island
	{ LevelIDs_MysticRuins, 2, -515.191, 297.349, -865.042, {245, 180}},	//jungle lost world
	{ LevelIDs_MysticRuins, 2,  1307.67, 294.549, -814.303, {455, 195}}, //jungle big's house
};

void LoadDestination(int playerID)
{
	DisableTailsAI_Controller(playerID);
	LastLevel = CurrentLevel;
	LastAct = CurrentAct;
	SetNextLevelAndAct_CutsceneMode(DestinationArray[Cursor].level, DestinationArray[Cursor].act);
	return;
}

void __cdecl MovePlayerToStartPoint_r(taskwk* data) 
{
	if (isMoving > 0 && isFlyTravel) 
	{
		playertwp[0]->pos = DestinationArray[Cursor].destination;
		playertwp[0]->pos.y -= 6.5f;

	}
	else 
	{
		MovePlayerToStartPoint_t.Original(data);

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

int setCursorPos(int curLevel, int curAct) 
{
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

void DisplayCursorAnimation()
{
	if (!isUIScale())
		return;

	const float scale = 2.0f;

	MilesCursor_SPRITE.sx = 2 * scale;
	MilesCursor_SPRITE.sy = 2 * scale;

	MilesCursor_SPRITE.p.x = DestinationArray[Cursor].cursor.x;
	MilesCursor_SPRITE.p.y = DestinationArray[Cursor].cursor.y;

	njDrawSprite2D_Queue(&MilesCursor_SPRITE, MilesCurTex, -1.501f, NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR, QueuedModelFlagsB_SomeTextureThing);

	MilesCurTex = FrameCounterUnpaused / frameTransition % nbFrames;
}

void UpdatePlayerCursorPos() 
{
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
	signed int count = 0;
	int texId = 0;
	signed int diff = 0;
	float posY = 0.0f;
	float posX = 0.0f;
	signed int v12 = 0;
	float diffPosY = 0.0f;
	signed int v14 = 0;

	njSetTexture(&TravelMap_TEXLIST);
	SetVtxColorB(0xFFFFFFFF);

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
		++count;
		texId += 3;
		v12 = count;
	} while (count < 2);
}

void __cdecl PauseMenu_Map_Display_r()
{
	if (Cursor < 0 || Cursor > 8)
		return;

	HelperFunctionsGlobal.PushScaleUI(uiscale::Align::Align_Center, false, 1.0f, 1.0f);
	SetMaterialAndSpriteColor_Float(1.0f, 1.0f, 1.0f, 1.0f);
	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	DisplayMilesMap_r();
	DisplayCursorAnimation();

	njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
	HelperFunctionsGlobal.PopScaleUI();
}

bool isTailsAI_GrabAllowed() 
{
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

void CheckAndForceLeavingGrab(taskwk* data) 
{
	if (data->mode >= grabbed && data->mode <= leaving)
	{
		if (ControllerPointers[0]->PressedButtons & Buttons_B) {
			PlaySound(3, NULL, 0, NULL);
			data->mode = leaving;
		}
	}
}

void FlySoundOnFrames(int playerID) 
{
	auto ai = playertwp[playerID];

	if (GameState != 15 || !ai || !TailsAI_ptr)
		return;

	if (ai->charID == Characters_Tails && playertwp[0]->charID <= Characters_Tails) 
	{
		if (playertwp[playerID]->id == 0)
			PlaySound(0x302, NULL, 0, NULL);

		if (++playertwp[playerID]->id == 31) {
			PlaySound(0x302, NULL, 0, NULL);
			playertwp[playerID]->id = 1;
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

void PlayCharacterLeaveAnimation(taskwk* p1, playerwk* co2, int playerID) 
{

	if ((co2->equipment & Upgrades_SuperSonic) == 0)
		ForcePlayerAction(playerID, 24);

	switch (p1->charID) 
	{
	case Characters_Sonic:

		if (co2->equipment & Upgrades_SuperSonic) {
			p1->mode = 82;
			co2->mj.reqaction = 145;
		}
		else {
			p1->mode = 12;
			co2->mj.reqaction = 18;
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
	switch (p1->charID) 
	{
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
	p1->pos = p2->pos;
	p1->pos.y -= GetCharacterPositionY(p1);
	p1->ang = p2->ang;
	return;
}

void RestoreAIControl(unsigned char ID) 
{
	ControllerPointers[ID]->PressedButtons = 0;
	ControllerPointers[ID]->HeldButtons = 0;
	return;
}

int CheckFastTravelStoryProgression() 
{
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
		if (p2) 
		{
			if (p2->mode == AIFlyTravel) //failsafe if the player start fly travel but leave the level/act
				p2->mode = 1;
		}
		TailsGrab = nullptr;
		FreeTask(obj);
		return;
	}

	if ((++data->counter.w[1] == 230 && data->mode > initFly && data->mode < grabbed) || p2->charID != Characters_Tails) {
		data->mode = leaving;
	}

	switch (data->mode)
	{
	case initFly:
		obj->dest = TailsAI_GrabDelete;
		if (!isTailsAI_GrabAllowed()) 
		{
			SetDebugFontSize(24);
			SetDebugFontColor(0xFF0000);
			DisplayDebugStringFormatted(NJM_LOCATION(12, 12), "You cannot fast travel at the moment.");

			if (++data->wtimer == 60) 
			{
				SetDebugFontColor(0xFFFFFFFF);
				FreeTask(obj);
				return;
			}
		}
		else 
		{
			data->pos = p2->pos;
			data->mode = getAltitude;
		}
		break;
	case getAltitude:
		if ((p1->charID <= Characters_Amy && p2->pos.y < p1->pos.y + 15.0f) || (p1->charID >= Characters_Gamma && p2->pos.y < p1->pos.y + 25.0f)) {
			Controllers[pnum].HeldButtons |= JumpButtons;
			Controllers[pnum].PressedButtons |= JumpButtons;
		}
		else 
		{
			p2->flag &= 0x100u;
			p2->mode = AIFlyTravel;
			co2p2->mj.reqaction = 37;
			data->mode = checkGrab;
		}
		break;
	case checkGrab:

		if (GetCollidingEntityA((EntityData1*)p2)) 
		{
			p1->flag &= ~(Status_Attack | Status_Ball | Status_LightDash);
			p1->mode = AIFlyTravel;
			DisablePause();
			data->mode = grabbed;
		}
		break;
	case grabbed:
		data->id = 0;
		data->wtimer = 0;
		data->counter.w[1] = 0;

		UpdateP1Position(co2p1, co2p2, p1, p2);
		CheckAndForceLeavingGrab(data);
		PlayCharacterGrabAnimation(p1, co2p1);

		Cursor = setCursorPos(CurrentLevel, CurrentAct);
		if (Cursor > -1)
		{
			data->mode = transitionMap;
		}
		else {
			data->mode = leaving;
		}
		break;
	case transitionMap:
		CheckAndForceLeavingGrab(data);
		if (++data->id == 50) 
		{
			PlaySound(21, NULL, 0, NULL);
			data->btimer = 0;
			data->mode = displayMap;
		}
		break;
	case displayMap:

		CheckAndForceLeavingGrab(data);
		SetDebugFontSize(20);

		DisplayDebugStringFormatted(NJM_LOCATION(2, 1), destTitle.c_str(), getDestinationText());
		UpdatePlayerCursorPos();
		DrawModelCallback_Queue((void(__cdecl*)(void*))PauseMenu_Map_Display_r, 0, 22047.998f, QueuedModelFlagsB_EnableZWrite); //fix transparency issue

		if (ControllerPointers[0]->PressedButtons & Buttons_A || ControllerPointers[0]->PressedButtons & Buttons_Start) 
		{
			if (!CheckFastTravelStoryProgression()) 
			{
				data->btimer = 0;
				data->mode = errorMove;
			}
			else 
			{
				p2->mode = 15;
				PlaySound(0x2, NULL, 0, NULL);

				if (CurrentLevel == DestinationArray[Cursor].level)
					isMoving = ActChange;
				else
					isMoving = LvlChange;

				data->id = 0;
				data->wtimer = 0;
				data->counter.w[1] = 0;
				CharColliOff(p1);
				CharColliOff(p2);
		
				data->mode = movetoDestination;
			}
		}
		break;
	case movetoDestination:
		FlySoundOnFrames(pnum);
		co2p2->spd.y = spdYGain;
		co2p2->spd.x = 0.6f;
		UpdateP1Position(co2p1, co2p2, p1, p2);

		if (++data->wtimer == 180) 
		{
			LoadDestination(pnum);

			if (isMoving == ActChange)
			{
				FreeTask(TailsGrab);
				return;
			}
		}

		break;
	case leaving:
		p2->mode = 10;
		RestoreAIControl(pnum);
		Cursor = -1;
		EnablePause();
		PlayCharacterLeaveAnimation(p1, co2p1, pnum);
		isMoving = 0;
		p1->flag &= ~Status_Ball;
		FreeTask(obj);
		break;
	case errorMove:
		SetDebugFontSize(24);
		SetDebugFontColor(0xFF0000);
		DisplayDebugStringFormatted(NJM_LOCATION(12, 12), "You cannot fly there at the moment.");
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

void TailsAI_Landing(task* tp) 
{
	auto twp = tp->twp;
	auto p1 = playertwp[0];
	auto pnum = twp->pNum;
	auto p2 = playertwp[pnum];

	auto co2p1 = playerpwp[0];
	auto co2p2 = playerpwp[pnum];

	if (!p1 || !p2 || !IsIngame()) {
		return;
	}

	if (EV_MainThread_ptr)
	{
		CharColliOn(p1);
		CharColliOn(p2);
		TailsAI_LandingDelete(tp);
		FreeTask(tp);
		return;
	}

	SetTailsAILookAt(p2, twp);

	FlySoundOnFrames(pnum);

	switch (twp->mode)
	{
	case 0:
		tp->dest = TailsAI_LandingDelete;
		twp->pos = DestinationArray[Cursor].destination;
		p1->ang = p2->ang;
		p1->mode = AIFlyTravel;
		ForcePlayerAction(pnum, 48);
		co2p2->mj.reqaction = 39;
		PlayCharacterGrabAnimation(p1, co2p1);
		twp->mode++;
		break;
	case 1:
		PlayCharacterGrabAnimation(p1, co2p1);
		co2p2->spd.y = -0.3f;
		co2p2->spd.x += 0.3f;
		UpdateP1Position(co2p1, co2p2, p1, p2);

		if (++twp->wtimer == 140 || (p1->flag & 3)) 
		{
			for (uint8_t i = 0; i < MaxPlayers; i++)
			{
				ForcePlayerAction(i, 17);
			}	

			twp->mode++;
		}
		break;
	case 2:
		CharColliOn(p1);
		CharColliOn(p2);
		PlayCharacterLeaveAnimation(p1, co2p1, pnum);
		RestoreAIControl(pnum);
		EnableTailsAI_Controller(pnum);
		EnableControl();
		EnablePause();
		FreeTask(tp);
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

void SetDestinationStringToArray()
{
	std::string iniPath = "SYSTEM\\flyTravelStrings.ini";

	auto originFilePath = HelperFunctionsGlobal.GetReplaceablePath(iniPath.c_str()); //used to make other mods able to replace the strings

	if (!IsPathExist(originFilePath))
	{
		PrintDebug("Failed to get Fly travel strings... destination texts won't show up.\n");
		return;
	}

	const IniFile* ini = new IniFile(std::string(originFilePath));

	std::string title = ini->getString("0", "title", "");
	destTitle = title + ": %s";

	uint8_t i = 1;
	while (ini->hasGroup(std::to_string(i)))
	{
		std::string s = std::to_string(i);
		std::string text = ini->getString(s, "dest", "");
		DestinationText.push_back(text);
		i++;
	}

	delete ini;
}

void SetFlyTravelCursorAnim()
{
	std::string iniPath = "SYSTEM\\cursorInfoT.ini";

	auto originFilePath = HelperFunctionsGlobal.GetReplaceablePath(iniPath.c_str()); //used to make other mods able to edit the cursor anim data

	if (!IsPathExist(originFilePath))
	{
		PrintDebug("Failed to get mod cursor info...\n");
		return;
	}

	//get cursor anim data...
	const IniFile* ini = new IniFile(std::string(originFilePath));
	nbFrames = ini->getInt("cursorAnim", "nbFrames", nbFrames);
	frameTransition = ini->getInt("cursorAnim", "frameTransition", frameTransition);
	delete(ini);

	//if the user increased the number of tex ids the anim should use, fill the array with the changes.
	if (nbFrames > defaultFrames) 
	{
		uint16_t difference = nbFrames - defaultFrames;

		for (uint16_t i = defaultFrames; i < nbFrames; i++)
		{
			NJS_TEXANIM newData = { 0x10, 0x10, 0, 0, 0, 0, 0x100, 0x100, i, 0x20 };
			MilesCursor_TEXANIM.push_back(newData);
		}	
	}	

	//if the user edited the number of tex ids the anim should use; resize the array and update the sprite with the new tex anim data.
	if (nbFrames != defaultFrames)
	{
		MilesCursor_TEXANIM.resize(nbFrames);
		MilesCursor_SPRITE.tanim = MilesCursor_TEXANIM.data();
	}
}

void FlyTravel_Init() {
	MovePlayerToStartPoint_t.Hook(MovePlayerToStartPoint_r);
	SetDestinationStringToArray();
	SetFlyTravelCursorAnim();
}