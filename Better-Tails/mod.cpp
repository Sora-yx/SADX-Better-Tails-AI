#include "stdafx.h"

time_t t;
HMODULE multi = NULL;

extern "C" {

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
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

		DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Tails AI Pointer: %d", TailsAI_ptr != nullptr);

	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}