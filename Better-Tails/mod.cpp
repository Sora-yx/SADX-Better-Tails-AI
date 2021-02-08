#include "stdafx.h"

bool IsChaoGardenBanned = false;
bool IsHubBanned = false;
bool IsBossBanned = false;
bool IsTCBanned = false;
bool IsStoryIA = false;
bool banCharacter[8] = { -1 };
bool isFlyTravel = true;
bool isRescueAllowed = true;
time_t t;

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
		isRescueAllowed = config->getBool("AI", "isRescueAllowed", true);

		delete config;

		srand((unsigned)time(&t));

		if (!isRandoActive()) {
			WriteCall((void*)0x415a25, LoadCharacterAndAI); //Call Tails AI when Load Character.

			// Tails AI Reset Values
			WriteJump((void*)0x47db1a, TailsAI_ResetValue); //Reset value and stuff properly when Tails AI is deleted by the game.

			//Tails AI Stuff (Load, Fixes...)
			AI_Init(helperFunctions);

			WriteCall((void*)0x415556, DisableTime_R); //While result screen, force Tails AI to victory pose.
		}
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		//LoadAI_OnFrames();
		if (CharObj2Ptrs[1]) {
			DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "is Miles here %f", isAIActive);
		}
		//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Cutscene Active %d", EV_MainThread_ptr);
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}