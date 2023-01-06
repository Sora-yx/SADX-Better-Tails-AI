#include "stdafx.h"

time_t t;
HMODULE multi = NULL;
HelperFunctions HelperFunctionsGlobal;

FunctionPointer(int, SerifStatus, (void), 0x425740);



void PrintVoiceDone()
{
	if (GameState != 15)
		return;

	//PrintDebug("current voice state: %d \n", SerifStatus());
	SetDebugFontSize(20);
	DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "current voice state: %d\n", SerifStatus());
}


extern "C" {

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HelperFunctionsGlobal = helperFunctions; // Save the helper pointer for external use

		config(path);
		srand((unsigned)time(&t));

		// Tails AI Reset Values
		WriteJump(TailsAI_Delete, TailsAI_Delete_r); //Reset value and stuff properly when Tails AI is deleted by the game.

		//Tails AI Stuff (Load, Fixes...)
		AI_Init(helperFunctions);

		multi = GetModuleHandle("sadx-multiplayer");
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		//LoadAI_OnFrames();
		PrintVoiceDone();

		//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Tails AI Pointer: %d", TailsAI_ptr != nullptr);



	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}