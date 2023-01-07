#include "pch.h"
#include <time.h>

time_t t;
HMODULE multi = NULL;
HelperFunctions HelperFunctionsGlobal;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HelperFunctionsGlobal = helperFunctions; // Save the helper pointer for external use

		config(path);
		srand((unsigned)time(&t));

		// Tails AI Reset Values
		WriteJump(TailsAI_Delete, TailsAI_Delete_r); //Reset value and stuff properly when Tails AI is deleted by the game.

		//Tails AI Stuff (Load, Fixes...)
		AI_Init(helperFunctions, path);


		multi = GetModuleHandleW(L"sadx-multiplayer");
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		//LoadAI_OnFrames();

		//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Tails AI Pointer: %d", TailsAI_ptr != nullptr);
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}