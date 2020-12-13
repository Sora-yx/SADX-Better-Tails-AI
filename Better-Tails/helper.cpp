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


//Manually Call Tails Ai when necessary.
void CallTailsAI_R() {

	//Tails Crash Cutscene
	if (CurrentCharacter == Characters_Sonic && !EventFlagArray[EventFlags_Sonic_EmeraldCoastClear])
	{
		if (!IsStoryIA && !isAIActive)
		{
			ForceAI = true;
			Load2PTails_r();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Tails Rescued Cutscene
	if (CurrentCharacter == Characters_Sonic && EventFlagArray[EventFlags_Sonic_EmeraldCoastClear] == 1 && !EventFlagArray[EventFlags_Sonic_EggHornetClear])
	{
		if (!isAIActive)
		{
			ForceAI = true;
			Load2PTails_r();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Sonic Tails Post Casino Cutscene
	if (CurrentCharacter == Characters_Sonic && AICutsceneOk && EventFlagArray[EventFlags_Sonic_CasinopolisClear] == 1 && !EventFlagArray[EventFlags_Sonic_IceCapOpen])
	{
		if (!isAIActive)
		{
			ForceAI = true;
			Load2PTails_r();
			return PlayMusic(MusicIDs_s_square);
		}
	}

	//Sonic Amy cutscenes 
	if (CurrentCharacter == Characters_Sonic && (SonicSkyChaseAct1Clear == 1 && SonicTPClear == 0) || (SonicTPClear == 1 && !EventFlagArray[EventFlags_Sonic_RedMountainClear]))
	{
		if (isAIActive && !IsStoryIA && TailsAI_ptr != 0)
		{
			AICutsceneOk = 0;
			moveAItoPlayer();
			return PlayMusic(MusicIDs_s_square);
		}
	}
	return PlayMusic(MusicIDs_s_square);
}

void CallTailsAI_R2() {

	//Sonic Tails Post Chaos 4 Cutscene
	if (CurrentCharacter == Characters_Sonic && EventFlagArray[EventFlags_Sonic_Chaos4Clear] == 1 && SonicSkyChaseAct1Clear == 0)
	{
		if (isAIActive && TailsAI_ptr != 0)
		{
			AICutsceneOk = 0;
			moveAItoPlayer();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	//Sonic loses Amy Cutscene
	if (CurrentCharacter == Characters_Sonic && EventFlagArray[EventFlags_Sonic_SpeedHighwayClear] == 1 && !EventFlagArray[EventFlags_Sonic_RedMountainClear])
	{
		if (isAIActive && !IsStoryIA && TailsAI_ptr != 0)
		{
			AICutsceneOk = 0;
			moveAItoPlayer();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	//post Chaos 6 scene && post lw cutscene
	if (CurrentCharacter == Characters_Sonic && (EventFlagArray[EventFlags_Sonic_Chaos6Clear] == 1 && !EventFlagArray[EventFlags_Sonic_LostWorldClear]) || (EventFlagArray[EventFlags_Sonic_LostWorldClear] == 1 && !EventFlagArray[EventFlags_Sonic_FinalEggClear]))
	{
		if (isAIActive && !IsStoryIA && TailsAI_ptr != 0)
		{
			moveAItoPlayer();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	//Super Sonic Story Cutscene
	if (SelectedCharacter == 6 && AICutsceneOk && CurrentLevel == LevelIDs_MysticRuins)
	{
		if (!isAIActive)
		{
			ForceAI = true;
			Load2PTails_r();
			return PlayMusic(MusicIDs_mstcln);
		}
	}

	return PlayMusic(MusicIDs_mstcln);
}

void CallTailsAI_R3() {

	if (CurrentCharacter == Characters_Sonic && !EventFlagArray[EventFlags_Sonic_SkyDeckClear] && SonicSkyChaseAct2Clear == 1)
	{
		if (!isAIActive)
		{
			ForceAI = true;
			Load2PTails_r();
			return PlayMusic(MusicIDs_egcarer1);
		}
		else
		{
			moveAItoPlayer();
			return PlayMusic(MusicIDs_egcarer1);
		}
	}

	return PlayMusic(MusicIDs_egcarer1);
}

void AllowTailsAI_R()
{
	AICutsceneOk = 1;
	return EnableControl();
}


void CheckAndDeleteAI() {

	if (isAIActive && TailsAI_ptr != 0)
	{
		AICutsceneOk = 0;
		DeleteTailsAI();
	}

	isAIActive = false;
	return DisableControl();
}

void DeleteTailsAI() {
	if (EntityData1Ptrs[1] != nullptr)
	{
		ObjectMaster* AI = GetCharacterObject(1);
		EntityData1* P2 = EntityData1Ptrs[1];
		if (P2->CharID == Characters_Tails) {
			TailsAI_Delete(AI);
			CheckThingButThenDeleteObject(AI); //delete AI completely
		}
	}
}

static const void* const GetPlayerSidePosPtr = (void*)0x47dd50;
void __declspec() GetPlayerSidePos_asm(NJS_VECTOR* v1, EntityData1* a2, float m)
{
	__asm
	{
		push[esp + 04h] // m
		push esi // e
		push edi // v

		// Call your __cdecl function here:
		call GetPlayerSidePosPtr

		pop edi // v
		pop esi // e
		add esp, 4 // m
		retn
	}
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

void AI_Fixes() {

	if (IsHubBanned)
		return;

	//Tails AI Fixes and small optimization/improvement.
	WriteCall((void*)0x4151ba, FixAIHubTransition); //Fix AI position when you change act in hub world.
	WriteCall((void*)0x417588, FixAIHubTransition2);

	WriteCall((void*)0x42f72d, CallTailsAI_R); //Manually Call Tails AI After few early Cutscene to avoid crash.
	WriteCall((void*)0x6cd3de, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene SonicAndTails_WakeUP
	WriteCall((void*)0x42f78c, CallTailsAI_R2);  //Move Tails to Sonic Position after Chaos 4 fight. Also call Tails AI in Super Sonic Story
	WriteJump((void*)0x657c4a, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene SonicAndTails_LandEggCarrier
	WriteCall((void*)0x42f747, CallTailsAI_R3); //Manually Call Tails AI After the cutscene SonicAndTails_LandEggCarrier
	WriteCall((void*)0x664f68, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene Tails Find Sonic (Super Sonic cutscene)

	WriteCall((void*)0x65f82f, CheckAndDeleteAI); //Remove Tails before "Sonic finds Knuckles cutscene" (super sonic)
	WriteCall((void*)0x663d4a, CheckAndDeleteAI); //Remove Tails before "Sonic and Tails find tornado 2 cutscene" (super sonic)
	WriteCall((void*)0x6601ab, AllowTailsAI_R); //Allow Tails AI to spawn after the cutscene Sonic_WakeUP (super sonic cutscene)
}
