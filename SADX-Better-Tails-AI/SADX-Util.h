#pragma once

VoidFunc(FUN_0042ce20, 0x42ce20); //Called with Delete Tails AI

FunctionPointer(int, GetCutsceneFlagArray, (int cutscene), 0x42fb10);

DataPointer(char, IceCapFlag, 0x3B188B4);
DataPointer(unsigned char, SelectedCharacter, 0x3B2A2FD);

DataPointer(char, SonicTPClear, 0x3B188B1);
DataPointer(char, SonicSkyChaseAct1Clear, 0x3B188BD);
DataPointer(char, SonicSkyChaseAct2Clear, 0x3B188BE);
DataPointer(float*, HorizontalStretchPointer, 0x51285E);

//SADX Map Stuff
FunctionPointer(void, sub_62ECE0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x62ECE0);
FunctionPointer(void, sub_51C130, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x51C130);
FunctionPointer(void, sub_525980, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x525980);
FunctionPointer(void, sub_52F9C0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x52F9C0);
FunctionPointer(void, sub_541BF0, (unsigned __int16 a1, NJS_VECTOR* p1Pos), 0x541BF0);

FunctionPointer(void, KillPlayer, (int player), 0x440cd0);
DataArray(int, IslandDoor_Col, 0x111e010, 7);
VoidFunc(FUN_00412ad0, 0x412ad0); //called when you soft reset / quit

FunctionPointer(int, isHostelOpen, (), 0x630900);
FunctionPointer(int, isAngelIslandOpen, (), 0x534570);

FunctionPointer(long, Chao_Pleasure, (task* Chao), 0x75db80);
FunctionPointer(void, Chao_SetBehavior, (task* Chao, long* a2), 0x71EF10);
FunctionPointer(int, EV_FreeNpcMilesPlayerTask, (), 0x42CDF0);
VoidFunc(FreeNpcMilesPlayerTask, 0x47DB70);
VoidFunc(CreateNPCMilesPlayerTask, 0x47DBB0);

enum class CameraAdjustsIDs {
	None,
	Normal,
	Normal_S,
	Slow,
	Slow_S,
	Time,
	Three1,
	Three1C,
	Three2,
	Three2C,
	Three3,
	Three3C,
	Three4,
	Three4C,
	Three5,
	Three5C,
	Relative1,
	Relative1C,
	Relative2,
	Relative2C,
	Relative3,
	Relative3C,
	Relative4,
	Relative4C,
	Relative5,
	Relative5C,
	Relative6,
	FreeCam
};

enum class CameraDirectIDs {
	Sonic,
	Unknown,
	Target
};

typedef void(__cdecl* CameraFuncPtr)(_OBJ_CAMERAPARAM*);

FunctionPointer(void, SetCameraEvent, (CameraFuncPtr func, CameraAdjustsIDs adjust, CameraDirectIDs direct), 0x437D20);
VoidFunc(RemoveCameraEvent, 0x436140);
DataPointer(_camcontwk, CameraTask, 0x3B2C660);
FunctionPointer(int, sub_42FB00, (), 0x42FB00); //Knuckles thing
FunctionPointer(ObjectMaster*, CheckLoadBird, (), 0x4C6820);
ThiscallFunctionPointer(bool, IsCutscenePlaying, (void* this_), 0x42FB00);

// void __usercall SetNPCMilesFlyMode(unsigned __int8 a1@<bl>)
static const void* const SetMilesFly_Ptr = (void*)0x47DBE0;
static inline void SetNPCMilesFlyMode(unsigned __int8 pnum)
{
	__asm
	{
		mov bl, [pnum]
		call SetMilesFly_Ptr
	}
}

// signed int __usercall TailsAI_RangeOut@<eax>(taskwk *p1@<eax>, taskwk *p2@<ecx>, task *a3@<edi>)
static const void* const TailsAI_RangeOut_Ptr = (void*)0x47DC20;
static inline signed int TailsAI_RangeOut(taskwk* p1, taskwk* p2, task* a2)
{
	int result;
	__asm
	{
		mov edi, [a2]
		mov ecx, [p2]
		mov eax, [p1]
		call TailsAI_RangeOut_Ptr
		mov eax, result
	}

	return result;
}

FunctionPointer(int, NpcGetAnalogForBrake, (NJS_VECTOR* a1, float* a2, Angle* a3), 0x47DE00);
DataPointer(int, NPCMilesStandByFlag, 0x03b2a304);
DataPointer(NJS_ACTION, action_m_m9002_miles, 0x3C84DC4);
DataPointer(NJS_ACTION, action_m_m9001_miles, 0x3C84DCC);