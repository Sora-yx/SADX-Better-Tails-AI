#include "stdafx.h"


static Trampoline* LoadCharacterBoss_t = nullptr;
static FunctionHook<void, int> EV_Load2_t(EV_Load2);

void __cdecl DisableTailsAI_Controller(Uint8 index)
{
	ControllerEnabled[index] = 0;
}

void __cdecl EnableTailsAI_Controller(Uint8 index)
{
	ControllerEnabled[index] = 1;
}

unsigned char getAI_ID() {

	if (!EntityData1Ptrs[AIIndex])
		return 0;

	if (EntityData1Ptrs[AIIndex]->CharID == Characters_Tails && TailsAI_ptr) {
		return AIIndex;
	}

	return 0;
}

void RemovePlayerCollision(unsigned char ID) {

	if (!EntityData1Ptrs[0] || ID > 0 && !EntityData1Ptrs[ID] || EV_MainThread_ptr || CharacterBossActive || AIIndex > 1)
		return;

	EntityData1* data = EntityData1Ptrs[0];

	if (data->CollisionInfo)
	{
		if (data->CollisionInfo->nbInfo) {

			for (int8_t i = 0; i < data->CollisionInfo->nbInfo; i++) {

				EntityData1Ptrs[0]->CollisionInfo->CollisionArray[i].damage &= ~0x20u; //Remove damage on other players
			}
		}
	}
}

void RestorePlayerCollision(unsigned char ID) {

	if (!EntityData1Ptrs[ID])
		return;

	EntityData1* data = EntityData1Ptrs[ID];

	if (data->CollisionInfo)
	{
		if (data->CollisionInfo->nbInfo) {
			for (int8_t i = 0; i < data->CollisionInfo->nbInfo; i++) {

				EntityData1Ptrs[ID]->CollisionInfo->CollisionArray[i].damage |= 0x20u; //Remstore damage on other players
			}
		}
	}
}

//remove AI when cutscene start
void __cdecl EV_Load2_r(int a2)
{
	DeleteMilesAI();

	return EV_Load2_t.Original(a2);
}

void __cdecl LoadCharacterBoss_r(int boss_id)
{
	FunctionPointer(void, origin, (int boss_id), LoadCharacterBoss_t->Target());
	origin(boss_id);

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

int __cdecl IsMilesInsideSphere(NJS_VECTOR* x_1, float radius)
{

	unsigned char ID = getAI_ID();

	float v2;
	float v3;
	int v4;
	EntityData1* v5;
	CollisionInfo* v6;
	float* v7;
	float v8;
	float v10;
	float v11;
	NJS_VECTOR v;
	float v13;

	v2 = x_1->y;
	v3 = x_1->z;
	v13 = x_1->x;
	v10 = v2;
	v11 = v3;
	v4 = 1;
	while (1)
	{
		v5 = EntityData1Ptrs[v4];
		if (v5)
		{
			v6 = v5->CollisionInfo;
			if (v6)
			{
				v7 = (float*)&v6->CollisionArray->kind;
				v8 = v7[2];
				// pointer to Y of the first vector because sega hates everything
				v7 += 3;
				v.x = v8 - v13;
				v.y = *v7 - v10;
				v.z = v7[1] - v11;
				if (njScalor(&v) - radius < 0.0f)
				{
					break;
				}
			}
		}
		if (++v4 >= 2)
		{
			return 0;
		}
	}
	return v4 + 1;
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

//Manually Call Tails AI when necessary.
void CallTailsAI_R() {
	MusicIDs CurZic = MusicIDs_s_square;

	if (CurrentLevel == LevelIDs_MysticRuins)
		CurZic = MusicIDs_mstcln;

	if (CurrentLevel == LevelIDs_EggCarrierOutside || CurrentLevel == LevelIDs_EggCarrierInside)
		CurZic = MusicIDs_egcarer1;


	unsigned char ID = getAI_ID();

	if (ID > 0 && EntityData1Ptrs[ID] || ID > 0 && CharObj2Ptrs[ID] || IsAdventureComplete(SelectedCharacter) && SelectedCharacter != 6)
		return PlayMusic(CurZic);

	Load2PTails_r();

	return PlayMusic(CurZic);
}


void GetPlayerSidePos(NJS_VECTOR* v1, EntityData1* a2, float m)
{
	Float _sin;

	if (a2)
	{
		if (v1)
		{
			_sin = njSin(a2->Rotation.y);
			v1->x = a2->Position.x - njCos(a2->Rotation.y) * m;
			v1->y = a2->Position.y;
			v1->z = a2->Position.z - _sin * m;
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


	if (ID > 0 && CurrentCharacter != Characters_Tails && EntityData1Ptrs[ID] != nullptr) {
		if (EntityData1Ptrs[ID]->CharID == Characters_Tails) {
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
	EV_Load2_t.Hook(EV_Load2_r);
	LoadCharacterBoss_t = new Trampoline((int)LoadCharacterBoss, (int)LoadCharacterBoss + 0xc, LoadCharacterBoss_r);


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

	WriteCall((void*)0x42f72d, CallTailsAI_R); //Manually Call Tails AI After few early Cutscene
	WriteCall((void*)0x42f78c, CallTailsAI_R);
	WriteCall((void*)0x42f747, CallTailsAI_R);

}