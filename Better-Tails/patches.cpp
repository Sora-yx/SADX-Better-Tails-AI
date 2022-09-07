#include "stdafx.h"

static FunctionHook<void, int> LoadCharacterBoss_t(LoadCharacterBoss);
static FunctionHook<void, int> Ev_Load2_t(EV_Load2);
static FunctionHook<void, taskwk*, motionwk2*, playerwk*> ProcessVertexWelds_t((intptr_t)ProcessVertexWelds);

//Despite Tails AI being deleted right before a cutscene start, the game can still crash when trying to process the welds at the same time (race issue)
//So we force the welds function to return until Tails AI is fully deleted.
void ProcessVertexWelds_r(taskwk* a1, motionwk2* a2, playerwk* a3)
{
	if (EV_MainThread_ptr) //if a cutscene is playing and the game isn't trying to delete the welds, abort the process of welding.
	{
		if (a3 && a3->mj.jvmode != 2)
		{
			if (a1->charID == Characters_Tails && TailsAI_ptr)
				return;
		}
	}

	ProcessVertexWelds_t.Original(a1, a2, a3);
}

void __cdecl DisableTailsAI_Controller(Uint8 index)
{
	ControllerEnabled[index] = 0;
}

void __cdecl EnableTailsAI_Controller(Uint8 index)
{
	ControllerEnabled[index] = 1;
}

unsigned char getAI_ID() {

	if (!playertwp[AIIndex])
		return 0;

	if (playertwp[AIIndex]->charID == Characters_Tails && TailsAI_ptr) {
		return AIIndex;
	}

	return 0;
}

void RemovePlayerCollision(unsigned char ID) {

	if (!playertwp[0] || ID > 0 && !playertwp[ID] || EV_MainThread_ptr || CharacterBossActive || AIIndex > 1)
		return;

	auto data = playertwp[0];

	if (data->cwp)
	{
		if (data->cwp->nbInfo) {

			for (int8_t i = 0; i < data->cwp->nbInfo; i++) {

				playertwp[0]->cwp->info->damage &= ~0x20u; //Remove damage on other players
			}
		}
	}
}

void RestorePlayerCollision(unsigned char ID) {

	if (!playertwp[ID])
		return;

	auto data = playertwp[ID];


	if (data->cwp)
	{
		if (data->cwp->nbInfo) {

			for (int8_t i = 0; i < data->cwp->nbInfo; i++) {

				playertwp[ID]->cwp->info->damage |= 0x20u; //Remstore damage on other players
			}
		}
	}
}

//remove AI when cutscene start
void __cdecl FreeNpcMilesPlayerTask_r(int a2)
{
	DeleteMilesAI();

	return Ev_Load2_t.Original(a2);
}

void __cdecl LoadCharacterBoss_r(int boss_id)
{

	DeleteMilesAI();
	LoadCharacterBoss_t.Original(boss_id);

	for (uint8_t i = 0; i < MaxPlayers; i++) {

		RestorePlayerCollision(i);
	}
}

//teleport AI to Player
void moveAItoPlayer(unsigned char playerID) {

	if (isAIActive)
	{
		if (isP1AndTailsAIEnabled(playerID))
		{
			auto p1 = playertwp[0];
			auto p2 = playertwp[playerID];

			if (CurrentCharacter != Characters_Big && CurrentCharacter != Characters_Gamma)
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, -7.0f, 0.0f, 5.0f);
			else
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, -10.0f, 0.0f, 8.0f);
		}
	}

	return;
}

void moveAItoPlayer(unsigned char playerID, float posX, float posZ) {
	if (isAIActive)
	{
		if (isP1AndTailsAIEnabled(playerID))
		{
			auto p1 = playertwp[0];
			auto p2 = playertwp[playerID];

			if (CurrentCharacter != Characters_Big && CurrentCharacter != Characters_Gamma)
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, posX, 0.0f, posZ);
			else
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, posX - 3.0f, 0.0f, posZ + 3.0f);
		}
	}

	return;
}

bool isPlayerUsingSnowboard() {

	if (CurrentCharacter == Characters_Sonic && playertwp[0]->mode >= 62 && playertwp[0]->mode <= 68)
		return true;

	return false;
}

//Fix AI Start Position in hub world

void FixAIHubTransition() {
	ForcePlayerAction(0, 0x18);

	if (!IsHubBanned && isAIActive)
	{
		if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1))
			return;

		unsigned char ID = getAI_ID();

		if (!ID)
			return;

		if (CurrentLevel > LevelIDs_E101R && CurrentLevel < LevelIDs_TwinkleCircuit)
			moveAItoPlayer(ID);
	}

	return;
}

void FixAIHubTransition2() {
	HudDisplayRingTimeLife_Check();

	unsigned char ID = getAI_ID();

	if (!ID)
		return;

	if (!IsHubBanned && isAIActive)
	{
		if (CurrentCharacter != Characters_Big)
		{
			if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1) || CurrentLevel >= LevelIDs_ECGarden && CurrentLevel < LevelIDs_ChaoRace)
				moveAItoPlayer(ID);
		}
		else
		{
			if (CurrentLevel == LevelIDs_StationSquare && CurrentAct == 1 || CurrentLevel >= LevelIDs_ECGarden && CurrentLevel < LevelIDs_ChaoRace)
				moveAItoPlayer(ID);
		}
	}

	return;
}

void GetPlayerSidePos(NJS_VECTOR* v1, taskwk* a2, float m)
{
	Float _sin = 0.0f;

	if (a2)
	{
		if (v1)
		{
			_sin = njSin(a2->ang.y);
			v1->x = a2->pos.x - njCos(a2->ang.y) * m;
			v1->y = a2->pos.y;
			v1->z = a2->pos.z - _sin * m;
		}
	}
}

void FadeoutScreen(ObjectMaster* obj) {
	EntityData1* data = obj->Data1;

	if (++data->InvulnerableTime > 80) {
		int color = 0x00000000;
		ScreenFade_Color = *(NJS_COLOR*)&color;
		CheckThingButThenDeleteObject(obj);
	}
	else {
		int color = 0x0000000;
		ScreenFade_Color = *(NJS_COLOR*)&color;

		if (data->InvulnerableTime < 120) {
			if (data->InvulnerableTime < 60) {
				data->CharID += 4;
				ScreenFade_Color.argb.a = data->CharID;
			}
			else {
				ScreenFade_Color.argb.a = 0x0;
			}
		}
		else {
			data->CharID -= 20;
			ScreenFade_Color.argb.a = data->CharID;
		}

		ScreenFade_DrawColor();
	}
}

void SetCharaInfo(task* obj, int i) {
	obj->twp->charID = (char)CurrentCharacter;
	obj->twp->pNum = i;

	playertwp[i] = obj->twp;
	playermwp[i] = (motionwk2*)obj->mwp;
	MovePlayerToStartPoint((EntityData1*)obj->twp);
	return;
}

void __cdecl LoadCharacter_r()
{
	task* p1 = nullptr;

	ClearPlayerArrays();

	for (__int16 i = 0; i < 2; ++i)
	{
		if (i)
		{
			LoadTailsOpponent(CurrentCharacter, i, CurrentLevel);
		}
		else
		{
			if (!CurrentCharacter)
			{
				if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2)
				{
					p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Tornado_Main);
				}
				else
				{
					p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Sonic_Main);
				}
			}
			if (CurrentCharacter == Characters_Knuckles)
			{
				p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Knuckles_Main);
				if (sub_42FB00() != 1
					&& (GameMode == GameModes_Adventure_ActionStg
						|| GameMode == GameModes_Mission
						|| GameMode == GameModes_Trial))
				{
					CreateElementalTask(LoadObj_Data1, 6, (TaskFuncPtr)EmeraldRadarHud_Load_Load);
				}
			}
			if (CurrentCharacter == Characters_Tails)
			{
				if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2)
				{
					p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Tornado_Main);
				}
				else
				{
					p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Tails_Main);
				}
			}
			if (CurrentCharacter == Characters_Big)
			{
				p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Big_Main);
				CreateElementalTask(LoadObj_Data1, 6, (TaskFuncPtr)BigHud_Main);
			}
			if (CurrentCharacter == Characters_Amy)
			{
				p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Amy_Main);
				CheckLoadBird();
			}
			if (CurrentCharacter == Characters_Gamma)
			{
				p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Gamma_Main);
			}

			SetCharaInfo(p1, i);
		}
	}
}

int GetRaceWinnerPlayer_r() {

	unsigned char ID = getAI_ID();

	if (ID > 0 && CurrentCharacter != Characters_Tails && playertwp[ID] != nullptr) {
		if (playertwp[ID]->charID == Characters_Tails) {
			return 1;
		}
	}

	return RaceWinnerPlayer;
}

void FixTailsAI_Train(int ID, void* a2, int a3, void* a4)
{
	moveAItoPlayer(AIIndex);
	PlaySound(ID, a2, a3, a4);
}

void __cdecl FixTailsAI_BotAreaTransition(Uint8 charIndex, float x, float y, float z)
{
	PositionPlayer(charIndex, x, y, z);
	moveAItoPlayer(AIIndex);
}

void __cdecl FixTailsAI_ECAreaTransition(unsigned __int8 playerNum, char action)
{
	ForcePlayerAction(playerNum, action);
	moveAItoPlayer(AIIndex);
}

void Fix_AIPos_ActTransition()
{
	moveAItoPlayer(AIIndex);
	ResetMilesAI(AIIndex, 24);
}

void AI_Patches() {

	WriteJump(GetRaceWinnerPlayer, GetRaceWinnerPlayer_r); //fix wrong victory pose for Tails AI.
	//delete Tails AI when a cutscene start
	Ev_Load2_t.Hook(FreeNpcMilesPlayerTask_r);
	//re create Tails AI after a cutscene ends. 
	WriteJump(CreateNPCMilesPlayerTask, Load2PTails_r);
	ProcessVertexWelds_t.Hook(ProcessVertexWelds_r); //fix a crash with welds on cutscene

	LoadCharacterBoss_t.Hook(LoadCharacterBoss_r);

	WriteData<6>((int*)0x460fcf, 0x90); //restore Miles's tail effect when AI


	if (IsHubBanned)
		return;

	//Tails AI Fixes and small optimization/improvement.

	WriteCall((void*)0x64015A, FixTailsAI_Train);
	WriteCall((void*)0x53A29B, FixTailsAI_Train);
	WriteCall((void*)0x5339AB, FixTailsAI_BotAreaTransition);	//MR
	WriteCall((void*)0x5339EA, FixTailsAI_BotAreaTransition);
	WriteCall((void*)0x51D2AD, FixTailsAI_BotAreaTransition);	//EC
	WriteCall((void*)0x51D216, FixTailsAI_BotAreaTransition);
	WriteCall((void*)0x51BDD0, FixTailsAI_ECAreaTransition);

}