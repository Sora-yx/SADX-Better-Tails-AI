#include "pch.h"
#include "multiapi.h"

float getMilesDistance(taskwk* p1, taskwk* p2)
{
	if (!p1 || !p2)
		return 0.0f;

	float v4 = p2->pos.x - p1->pos.x;
	float v5 = p2->pos.y - p1->pos.y;
	float v6 = p2->pos.z - p1->pos.z;
	float v11 = v6 * v6 + v5 * v5 + v4 * v4;
	float v7 = squareroot(v11);
	return v7;
}

bool isUIScale() {
	if (HorizontalStretchPointer != &HorizontalStretch)
		return true;

	return false;
}

bool isCharSelActive() {
	bool charSel = GetModuleHandle(L"SADXCharSel");

	if (charSel)
		return true;

	return false;
}

bool isRandoActive() {
	bool Rando = GetModuleHandle(L"SADX-Randomizer");

	if (Rando)
		return true;

	return false;
}

bool isInputModActive() 
{
	bool input = GetModuleHandle(L"input-mod") != nullptr;
	bool input2 = GetModuleHandle(L"sadx-input-mod") != nullptr;
	bool input3 = false;

	//To do add SDL2 check with the new loader
	if (HelperFunctionsGlobal.Version >= 19)
	{
		input3 = HelperFunctionsGlobal.LoaderSettings->InputMod;
	}

	return input || input2 || input3;
}

bool isNewTricksActive() {
	bool tricks = GetModuleHandle(L"sadx-new-tricks") != nullptr;

	if (tricks)
		return true;

	return false;
}

NJS_VECTOR UnitMatrix_GetPoint_Player(NJS_VECTOR* orig, Angle3* rot, float x, float y, float z) {
	NJS_VECTOR point;

	njPushMatrix(_nj_unit_matrix_);
	njTranslateV(0, orig);

	njRotateY(0, (unsigned __int16)(-0x8000 - (rot->y)));

	njTranslate(0, x, y, z);
	njGetTranslation(_nj_current_matrix_ptr_, &point);
	njPopMatrix(1u);

	return point;
}

float GetSquare(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2);
}

float CheckDistance(NJS_VECTOR* vec1, NJS_VECTOR* vec2) 
{
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

	if (outx) 
	{
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

void PlayerLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy) 
{
	LookAt(from, to, outx, outy);

	if (outy) {
		*outy = -(*outy) + 0x4000;
	}
}


void ResetPlayerLook(uint8_t pnum)
{
	auto p = playertwp[pnum];
	if (p) {
		p->ewp->look.ang = { 0, 0, 0 };
		p->ewp->look.mode = 0;
		p->ewp->look.obj = 0;
	}
}

void MoveForward(taskwk* entity, float speed)
{
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&entity->pos);
	njRotateY(0, entity->ang.y);
	njRotateX(0, entity->ang.x);
	njTranslate(0, 0, 0, speed);
	njGetTranslation(0, &entity->pos);
	njPopMatrix(1u);
}


void PlayerMoveForward(taskwk* entity, float speed)
{
	njPushMatrix(_nj_unit_matrix_);
	njTranslateEx(&entity->pos);
	njRotateX(0, entity->ang.x);
	njRotateY(0, entity->ang.y);
	njTranslate(0, 0, 0, speed);
	njGetTranslation(0, &entity->pos);
	njPopMatrix(1u);
}

//Sphere check functions
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest) {
	return sqrtf(powf(dest->x - orig->x, 2) + powf(dest->y - orig->y, 2) + powf(dest->z - orig->z, 2));
}

bool isInHubWorld()
{
	return CurrentLevel >= LevelIDs_StationSquare && CurrentLevel <= LevelIDs_Past;
}

bool isTailsAI()
{
	if (!TailsAI_ptr || !playertwp[0] || playertwp[0]->charID != Characters_Sonic || !playertwp[AIIndex] || playertwp[AIIndex]->charID != Characters_Tails)
		return false;

	return true;
}

bool isMPMod()
{
	auto mp = GetModuleHandle(L"sadx-multiplayer") != NULL;

	if (!mp && HelperFunctionsGlobal.Version >= 16)
	{
		return HelperFunctionsGlobal.Mods->find("sadx.Multiplayer") != NULL;
	}

	return mp != NULL;
}

bool isMultiEnabled()
{
	if (!isMPMod())
		return false;

	return multi_is_enabled();
}

bool isP1AndTailsAIEnabled(char playerID)
{
	auto p1 = playertwp[0];
	auto p2 = playertwp[playerID];

	if (p1 && p2 && p2->charID == Characters_Tails)
		return true;

	return false;
}

bool IsPathExist(const std::string& s)
{
	struct stat buffer;
	return (stat(s.c_str(), &buffer) == 0);
}

void FadeoutScreen(task* obj)
{
	auto twp = obj->twp;

	if (++twp->wtimer > 80) 
	{
		int color = 0x00000000;
		ScreenFade_Color = *(NJS_COLOR*)&color;
		FreeTask(obj);
	}
	else
	{
		int color = 0x0000000;
		ScreenFade_Color = *(NJS_COLOR*)&color;

		if (twp->wtimer < 120)
		{
			if (twp->wtimer < 60) {
				twp->counter.b[1] += 4;

				ScreenFade_Color.argb.a = twp->counter.b[1];
			}
			else {
				ScreenFade_Color.argb.a = 0x0;
			}
		}
		else {
			twp->counter.b[1] -= 20;
			ScreenFade_Color.argb.a = twp->counter.b[1];
		}

		ScreenFade_DrawColor();
	}
}

void SetTailsAILookAt(taskwk* AI, taskwk* twp)
{
	Angle angy = njArcTan2((AI->pos.x - twp->pos.x), (AI->pos.z - twp->pos.z));
	AI->ang.y = -0x4000 - angy;
}

uint16_t cartAction[] = { 45, 45, 43, 52, 45, 48, 53, 55 };

uint16_t GetSitCartAction(uint8_t curChar)
{
	return cartAction[curChar];
}