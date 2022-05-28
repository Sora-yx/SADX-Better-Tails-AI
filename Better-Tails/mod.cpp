#include "stdafx.h"


time_t t;

extern "C" {

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{


		if (!isRandoActive())  //rando already has its own stuff for AI.
			WriteCall((void*)0x415a25, LoadCharacterAndAI); //Call Tails AI when Load Character.


		config(path);
		srand((unsigned)time(&t));

		// Tails AI Reset Values
		WriteJump(TailsAI_Delete, TailsAI_Delete_r); //Reset value and stuff properly when Tails AI is deleted by the game.

		//Tails AI Stuff (Load, Fixes...)
		AI_Init(helperFunctions);

	}

	__declspec(dllexport) void __cdecl OnFrame()
	{

		//LoadAI_OnFrames();


	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}