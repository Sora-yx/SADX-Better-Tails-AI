#include "stdafx.h"

float getMilesDistance(EntityData1* p1, EntityData1* p2)
{
	if (!p1 || !p2)
		return 0.0f;

	float v4 = p2->Position.x - p1->Position.x;
	float v5 = p2->Position.y - p1->Position.y;
	float v6 = p2->Position.z - p1->Position.z;
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
	bool charSel = GetModuleHandle("SADXCharSel");

	if (charSel)
		return true;

	return false;
}

bool isRandoActive() {
	bool Rando = GetModuleHandle("SADX-Randomizer");

	if (Rando)
		return true;

	return false;
}

bool isInputModActive() {

	bool Input = GetModuleHandle("input-mod");
	bool Input2 = GetModuleHandle("sadx-input-mod");

	if (Input || Input2)
		return true;

	return false;
}

bool isNewTricksActive() {
	bool tricks = GetModuleHandle("sadx-new-tricks");

	if (tricks)
		return true;

	return false;

}


NJS_VECTOR UnitMatrix_GetPoint_Player(NJS_VECTOR* orig, Rotation3* rot, float x, float y, float z) {
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


bool isInHubWorld()
{
	return CurrentLevel >= LevelIDs_StationSquare && CurrentLevel <= LevelIDs_Past;
}

bool isTailsAI()
{
	if (!TailsAI_ptr || !EntityData1Ptrs[0] || EntityData1Ptrs[0]->CharID != Characters_Sonic || !EntityData1Ptrs[AIIndex] || EntityData1Ptrs[AIIndex]->CharID != Characters_Tails)
		return false;

	return true;
}