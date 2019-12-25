#include "stdafx.h"
#include "helper.h"
#include <SADXModLoader.h>
#include <IniFile.hpp>

bool IsChaoGardenBanned = false;
bool IsHubBanned = false;
bool IsBossBanned = false;
bool IsTCBanned = false;
bool IsStoryIA = false;
bool Sonic = false;
bool Knuckles = false;
bool Amy = false;
bool Big = false;
bool Gamma = false;
bool banCharacter[8] = { -1 };

extern bool isAIActive;


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

		delete config;

		HMODULE Rando = GetModuleHandle(L"SADX-Randomizer");

		if (!Rando) //do not call better tails AI if rando mod is activated
		{
			WriteCall((void*)0x415a25, LoadCharacter_r); //Hook Load Character
			//Allow Tails AI to spawn in acton stages, hub world, bosses and chao garden + fixes
			AI_Init();

			WriteCall((void*)0x415556, DisableTime_R); //While result screen, force Tails AI to victory pose.
		}

		//Prevent Character Select Mod 
		HMODULE CharSel = GetModuleHandle(L"SADXCharSel");
		
		if (CharSel)
			MessageBoxA(WindowHandle, "Warning, you are using the Character Select Mod, this mod is not compatible with Better Tails AI.", "Better Tails AI Mod", MB_ICONWARNING);

	}

	__declspec(dllexport) void __cdecl OnFrame()
	{

		//DisplayDebugStringFormatted(NJM_LOCATION(2, 1), "Is AI Active: %d", isAIActive);

	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

}