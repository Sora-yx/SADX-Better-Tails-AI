#include "stdafx.h"
#include "helper.h"
#include <SADXModLoader.h>
#include <IniFile.hpp>

bool IsChaoGardenBanned = false;
bool IsHubBanned = false;
bool IsBossBanned = false;

bool TailsUnlocked = false;

extern "C" {


	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		IsChaoGardenBanned = config->getBool("Stage", "IsChaoGardenBanned", false);
		IsHubBanned = config->getBool("Stage", "IsHubBanned", false);
		IsBossBanned = config->getBool("Stage", "IsBossBanned", false);

		delete config;


		//Allow Tails AI to spawn in acton stages, hub world and chao garden.
		WriteCall((void*)0x47ed8e, CheckTailsAI_R); //make the game crash
		WriteCall((void*)0x47e943, CheckTailsAI_R);
		WriteCall((void*)0x47ea46, CheckTailsAI_R);
		WriteCall((void*)0x47ec62, CheckTailsAI_R);
		WriteData<1>((void*)0x47ed7f, 0x28); //Allow Fox Boy to spawn during boss fight. (changing the level check to < at 40)

		WriteCall((void*)0x4159b8, LoadTails_AI_R); //Load Tails AI when not Sonic
		WriteCall((void*)0x415556, DisableTime_R); //While result screen, force Tails AI to victory pose.

		WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill 
		WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
		WriteCall((void*)0x4e9664, LoadAISnowBoard_R);


		
		//Prevent Character Select Mod 
		HMODULE CharSel = GetModuleHandle(L"SADXCharSel");
		if (CharSel)
		{
			MessageBoxA(WindowHandle, "Warning, you are using the Character Select Mod, this mod is not compatible with Better Tails AI.", "Better Tails AI Mod", MB_ICONWARNING);
		}

	}



	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

}