#include "pch.h"

bool IsChaoGardenBanned = false;
bool IsHubBanned = false;
bool IsBossBanned = false;
bool IsTCBanned = false;
bool IsStoryIA = false;
bool banCharacter[8] = { false };
bool isFlyTravel = true;
bool isRescueAllowed = true;
bool isMSBanned = false;
bool fasterRespawn = true;
int rescueChance = 65;

void config(const char* path)
{
	const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
	IsChaoGardenBanned = config->getBool("Stage", "IsChaoGardenBanned", false);
	IsHubBanned = config->getBool("Stage", "IsHubBanned", false);
	IsBossBanned = config->getBool("Stage", "IsBossBanned", false);
	IsTCBanned = config->getBool("Stage", "IsTCBanned", false);

	banCharacter[0] = config->getBool("Roster", "Sonic", false);
	isMSBanned = config->getBool("Roster", "isMSBanned", true);
	banCharacter[3] = config->getBool("Roster", "Knuckles", false);
	banCharacter[5] = config->getBool("Roster", "Amy", false);
	banCharacter[7] = config->getBool("Roster", "Big", true);
	banCharacter[6] = config->getBool("Roster", "Gamma", true);

	IsStoryIA = config->getBool("AI", "IsStoryIA", true);
	isFlyTravel = config->getBool("AI", "isFlyTravel", true);
	isRescueAllowed = config->getBool("AI", "isRescueAllowed", true);

	int chance = config->getInt("AI", "rescueChance", rescueChance);
	if (chance > -1 && chance < 101)
		rescueChance = chance;

	fasterRespawn = config->getBool("AI", "fasterRespawn", true);

	delete config;
}