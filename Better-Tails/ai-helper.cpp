#include "stdafx.h"


unsigned char getAI_ID() {

	for (uint8_t i = 1; i < MaxPlayers; i++) {
		if (!EntityData1Ptrs[i])
			continue;

		if (EntityData1Ptrs[i]->CharID == Characters_Tails && TailsAI_ptr) {
			return i;
		}
	}

	return 0;
}

//teleport AI to Player
void moveAItoPlayer(unsigned char playerID) {
	if (isAIActive)
	{
		if (EntityData1Ptrs[0] && EntityData1Ptrs[playerID])
		{
			EntityData1* p1 = EntityData1Ptrs[0];
			EntityData1* p2 = EntityData1Ptrs[playerID];

			if (CurrentCharacter != Characters_Big && CurrentCharacter != Characters_Gamma)
				p2->Position = UnitMatrix_GetPoint(&p1->Position, &p1->Rotation, -7.0f, 0.0f, 5.0f);
			else
				p2->Position = UnitMatrix_GetPoint(&p1->Position, &p1->Rotation, -10.0f, 0.0f, 8.0f);
		}
	}

	return;
}

bool isPlayerUsingSnowboard() {
	if (CurrentCharacter == Characters_Sonic && EntityData1Ptrs[0]->Action >= 62 && EntityData1Ptrs[0]->Action <= 68)
		return true;

	return false;
}

int __cdecl IsMilesInsideSphere(NJS_VECTOR* x_1, float radius)
{

	unsigned char ID = getAI_ID();

	float v2; // edx
	float v3; // eax
	int v4; // esi
	EntityData1* v5; // eax
	CollisionInfo* v6; // eax
	float* v7; // eax
	double v8; // st7
	float v10; // [esp+4h] [ebp-14h]
	float v11; // [esp+8h] [ebp-10h]
	NJS_VECTOR v; // [esp+Ch] [ebp-Ch] BYREF
	float v13; // [esp+1Ch] [ebp+4h]

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
				if (njScalor(&v) - radius < 0.0)
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

void CheckAndDeleteAI() {
	unsigned char ID = getAI_ID();

	if (!ID)
		return DisableControl();

	if (EntityData1Ptrs[ID] != nullptr)
	{
		if (EntityData1Ptrs[ID]->CharID == Characters_Tails) {
			TailsAI_ptr = nullptr;
			FUN_0042ce20();
			DeleteObjectMaster(GetCharacterObject(ID));
		}
	}

	isAIActive = false;
	return DisableControl();
}

void GetPlayerSidePos(NJS_VECTOR* v1, EntityData1* a2, float m)
{
	Float _sin; // ST00_4

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



void SetCharaInfo(ObjectMaster* obj, int i) {
	obj->Data1->CharID = (char)CurrentCharacter;
	obj->Data1->CharIndex = i;
	EntityData1Ptrs[i] = obj->Data1;
	EntityData2Ptrs[i] = (EntityData2*)obj->Data2;
	MovePlayerToStartPoint(obj->Data1);
	return;
}


void __cdecl LoadCharacter_r()
{
	__int16 i; // di
	ObjectMaster* object; // esi
	ObjectMaster* v3; // eax
	ObjectMaster* v4; // eax
	ObjectMaster* player1 = nullptr;

	ClearPlayerArrays();
	object = player1;
	for (i = 0; i < 2; ++i)
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
					v3 = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Tornado_Main);
				}
				else
				{
					v3 = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Sonic_Main);
				}
				object = v3;
			}
			if (CurrentCharacter == Characters_Knuckles)
			{
				object = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Knuckles_Main);
				if (sub_42FB00() != 1
					&& (GameMode == GameModes_Adventure_ActionStg
						|| GameMode == GameModes_Mission
						|| GameMode == GameModes_Trial))
				{
					LoadObject(LoadObj_Data1, 6, EmeraldRadarHud_Load_Load);
				}
			}
			if (CurrentCharacter == Characters_Tails)
			{
				if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2)
				{
					v4 = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Tornado_Main);
				}
				else
				{
					v4 = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Tails_Main);
				}
				object = v4;
			}
			if (CurrentCharacter == Characters_Big)
			{
				object = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Big_Main);
				LoadObject(LoadObj_Data1, 6, BigHud_Main);
			}
			if (CurrentCharacter == Characters_Amy)
			{
				object = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Amy_Main);
				CheckLoadBird();
			}
			if (CurrentCharacter == Characters_Gamma)
			{
				object = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Gamma_Main);
			}

			SetCharaInfo(object, i);
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

void MoveForward(EntityData1* entity, float speed) {
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&entity->Position);
	njRotateY(0, entity->Rotation.y);
	njRotateX(0, entity->Rotation.x);
	njTranslate(0, 0, 0, speed);
	njGetTranslation(0, &entity->Position);
	njPopMatrix(1u);
}

void AI_Fixes() {

	WriteJump(GetRaceWinnerPlayer, GetRaceWinnerPlayer_r); //fix wrong victory pose for Tails AI.

	if (IsHubBanned)
		return;

	//Tails AI Fixes and small optimization/improvement.
	WriteCall((void*)0x4151ba, FixAIHubTransition); //Fix AI position when you change act in hub world.
	WriteCall((void*)0x417588, FixAIHubTransition2);

	WriteCall((void*)0x42f72d, CallTailsAI_R); //Manually Call Tails AI After few early Cutscene to avoid crash.
	WriteCall((void*)0x42f78c, CallTailsAI_R);
	WriteCall((void*)0x42f747, CallTailsAI_R);

	WriteCall((void*)0x65f82f, CheckAndDeleteAI); //Remove Tails before "Sonic finds Knuckles cutscene" (super sonic)
	WriteCall((void*)0x663d4a, CheckAndDeleteAI); //Remove Tails before "Sonic and Tails find tornado 2 cutscene" (super sonic)

	WriteData<6>((int*)0x460fcf, 0x90); //restore Miles's tail effect when AI
}