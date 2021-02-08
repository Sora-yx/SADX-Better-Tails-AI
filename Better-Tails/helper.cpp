#include "stdafx.h"

//teleport AI to Player
void moveAItoPlayer() {
	if (isAIActive)
	{
		if (EntityData1Ptrs[0] && EntityData1Ptrs[1])
		{
			EntityData1* p1 = EntityData1Ptrs[0];
			EntityData1* p2 = EntityData1Ptrs[1];

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

NJS_VECTOR UnitMatrix_GetPoint(NJS_VECTOR* orig, Rotation3* rot, float x, float y, float z) {
	NJS_VECTOR point;

	njPushMatrix(_nj_unit_matrix_);
	njTranslateV(0, orig);
	if (rot) njRotateXYZ(0, rot->x, rot->y, rot->z);
	njTranslate(0, x, y, z);
	njGetTranslation(_nj_current_matrix_ptr_, &point);
	njPopMatrix(1u);

	return point;
}

float GetSquare(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2);
}

float CheckDistance(NJS_VECTOR* vec1, NJS_VECTOR* vec2) {
	float x_dist = vec2->x - vec1->x;
	float y_dist = vec2->y - vec1->y;
	float z_dist = vec2->z - vec1->z;

	float len = y_dist * y_dist + x_dist * x_dist + z_dist * z_dist;

	if (len < 0.02500000037252903)
	{
		return 0.0f;
	}

	return sqrt(len);
}

void LookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy) {
	NJS_VECTOR unit = *to;

	njSubVector(&unit, from);

	if (outy) {
		*outy = static_cast<Angle>(atan2f(unit.x, unit.z) * 65536.0f * 0.1591549762031479f);
	}

	if (outx) {
		if (from->y == to->y) {
			*outx = 0;
		}
		else {
			Float len = 1.0f / squareroot(unit.z * unit.z + unit.x * unit.x + unit.y * unit.y);

			*outx = static_cast<Angle>((acos(len * 3.3499999f) * 65536.0f * 0.1591549762031479f)
				- (acos(-(len * unit.y)) * 65536.0f * 0.1591549762031479f));
		}
	}
}

void PlayerLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy) {
	LookAt(from, to, outx, outy);

	if (outy) {
		*outy = -(*outy) + 0x4000;
	}
}

//Sphere check functions
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return sqrtf(powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2));
}

bool IsPointInsideSphere(NJS_VECTOR* center, NJS_VECTOR* pos, float radius) {
	return GetDistance(center, pos) <= radius;
}

int IsPlayerInsideSphere_(NJS_VECTOR* center, float radius) {
	for (uint8_t player = 0; player < MaxPlayers; ++player) {
		if (!EntityData1Ptrs[player])
			continue;

		NJS_VECTOR* pos = &EntityData1Ptrs[player]->Position;
		if (IsPointInsideSphere(center, pos, radius)) {
			return player + 1;
		}
	}

	return 0;
}

bool IsSpecificPlayerInSphere(NJS_VECTOR* center, float radius, uint8_t player) {
	return IsPlayerInsideSphere_(center, radius) == player + 1;
}

//Fix AI Start Position in hub world

void FixAIHubTransition() {
	ForcePlayerAction(0, 0x18);

	if (!IsHubBanned && isAIActive)
	{
		if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1))
			return;

		if (CurrentLevel > LevelIDs_E101R && CurrentLevel < LevelIDs_TwinkleCircuit)
			moveAItoPlayer();
	}

	return;
}

void FixAIHubTransition2() {
	HudDisplayRingTimeLife_Check();

	if (!IsHubBanned && isAIActive)
	{
		if (CurrentCharacter != Characters_Big)
		{
			if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1) || CurrentLevel >= LevelIDs_ECGarden && CurrentLevel < LevelIDs_ChaoRace)
				moveAItoPlayer();
		}
		else
		{
			if (CurrentLevel == LevelIDs_StationSquare && CurrentAct == 1 || CurrentLevel >= LevelIDs_ECGarden && CurrentLevel < LevelIDs_ChaoRace)
				moveAItoPlayer();
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

	if (EntityData1Ptrs[1] || CharObj2Ptrs[1] || IsAdventureComplete(SelectedCharacter) && SelectedCharacter != 6)
		return PlayMusic(CurZic);

	Load2PTails_r();

	return PlayMusic(CurZic);
}

void CheckAndDeleteAI() {
	if (EntityData1Ptrs[1] != nullptr)
	{
		if (EntityData1Ptrs[1]->CharID == Characters_Tails) {
			TailsAI_ptr = nullptr;
			FUN_0042ce20();
			DeleteObjectMaster(GetCharacterObject(1));
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

bool isUIScale() {
	if (HorizontalStretchPointer != &HorizontalStretch)
		return true;

	return false;
}

bool isCharSelActive() {
	HMODULE charSel = GetModuleHandle(L"SADXCharSel");

	if (charSel)
		return true;

	return false;
}

bool isRandoActive() {
	HMODULE Rando = GetModuleHandle(L"sadx-randomizer");

	if (Rando)
		return true;

	return false;
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
			}
			if (CurrentCharacter == Characters_Amy)
			{
				object = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Amy_Main);
			}
			if (CurrentCharacter == Characters_Gamma)
			{
				object = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Gamma_Main);
			}

			SetCharaInfo(object, i);
		}
	}
}

void AI_Fixes() {

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
}