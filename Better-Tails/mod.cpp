#include "stdafx.h"

bool IsChaoGardenBanned = false;
bool IsHubBanned = false;
bool IsBossBanned = false;
bool IsTCBanned = false;
bool IsStoryIA = false;
bool banCharacter[8] = { -1 };
bool isFlyTravel = true;
bool isRescue = true;


extern "C" {

	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		IsChaoGardenBanned = config->getBool("Stage", "IsChaoGardenBanned", false);
		IsHubBanned = config->getBool("Stage", "IsHubBanned", false);
		IsBossBanned = config->getBool("Stage", "IsBossBanned", false);
		IsTCBanned = config->getBool("Stage", "IsTCBanned", false);

		banCharacter[0] = config->getBool("Roster", "Sonic", false);
		banCharacter[3] = config->getBool("Roster", "Knuckles", false);
		banCharacter[5] = config->getBool("Roster", "Amy", false);
		banCharacter[7] = config->getBool("Roster", "Big", false);
		banCharacter[6] = config->getBool("Roster", "Gamma", false);

		IsStoryIA = config->getBool("AI", "IsStoryIA", false);
		isFlyTravel = config->getBool("AI", "isFlyTravel", true);
		isRescue = config->getBool("AI", "isRescue", true);

		delete config;

		HMODULE Rando = GetModuleHandle(L"SADX-Randomizer");

		if (!Rando) //do not call better tails AI if rando mod is activated
		{
			WriteCall((void*)0x415a25, LoadCharacter_r); //Call Tails AI when Load Character.

			// Tails AI Reset Values  
			WriteCall((void*)0x42ca4f, SoftReset_R); //Reset value and stuff properly when you Soft Reset and quit.
			WriteJump((void*)0x47db1a, TailsAI_ResetValue); //Reset value and stuff properly when Tails AI is deleted by the game.

			//Tails AI Stuff (Load, Fixes...)
			AI_Init(helperFunctions);

			WriteCall((void*)0x415556, DisableTime_R); //While result screen, force Tails AI to victory pose.
		}
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{

	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

}