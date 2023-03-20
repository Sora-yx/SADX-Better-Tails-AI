#include "pch.h"

static FunctionHook<void, int> LoadCharacterBoss_t(LoadCharacterBoss);
static FunctionHook<void, int> Ev_Load2_t(EV_Load2);
static FunctionHook<void, Uint8, float, float, float> PositionPlayer_t(PositionPlayer);

void __cdecl DisableTailsAI_Controller(Uint8 index)
{
	ControllerEnabled[index] = 0;
}

void __cdecl EnableTailsAI_Controller(Uint8 index)
{
	ControllerEnabled[index] = 1;
}

unsigned char getAI_ID() 
{
	if (!playertwp[AIIndex])
		return 0;

	if (playertwp[AIIndex]->charID == Characters_Tails && TailsAI_ptr) {
		return AIIndex;
	}

	return 0;
}

void RemoveAttackSolidColFlags(uint8_t pID) 
{
	if (!playertwp[pID] || EV_MainThread_ptr || CharacterBossActive || AIIndex > 1)
		return;

	auto pData = playertwp[pID];

	if (pData->cwp && pData->cwp->nbInfo)
	{
		for (int8_t i = 0; i < pData->cwp->nbInfo; i++)
		{
			playertwp[pID]->cwp->info[i].damage &= ~0x20u; //Remove damage on other players
			playertwp[pID]->cwp->info[i].push &= ~0x1u; //remove push flag on other players
		}
	}
}

void RestorePlayerCollision(unsigned char ID) 
{
	if (!playertwp[ID])
		return;

	auto data = playertwp[ID];

	if (data->cwp)
	{
		if (data->cwp->nbInfo) 
		{
			for (int8_t i = 0; i < data->cwp->nbInfo; i++) 
			{
				playertwp[ID]->cwp->info[i].damage |= 0x20u; //Restore damage on other players
				playertwp[ID]->cwp->info[i].push |= 0x1u; //Restore push flag on other players
			}
		}
	}
}

//remove AI when cutscene start
void __cdecl FreeNpcMilesPlayerTask_r(int a2)
{
	DeleteMilesAI();

	return Ev_Load2_t.Original(a2);
}

void __cdecl LoadCharacterBoss_r(int boss_id)
{
	DeleteMilesAI();
	LoadCharacterBoss_t.Original(boss_id);

	for (uint8_t i = 0; i < MaxPlayers; i++) 
	{
		RestorePlayerCollision(i);
	}
}

//teleport AI to Player
void moveAItoPlayer(unsigned char playerID) {
	if (isAIActive)
	{
		if (isP1AndTailsAIEnabled(playerID))
		{
			auto p1 = playertwp[0];
			auto p2 = playertwp[playerID];

			if (CurrentCharacter != Characters_Big && CurrentCharacter != Characters_Gamma)
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, -7.0f, 0.0f, 5.0f);
			else
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, -10.0f, 0.0f, 8.0f);
		}
	}
}

void moveAItoPlayer(unsigned char playerID, float posX, float posZ) {
	if (isAIActive)
	{
		if (isP1AndTailsAIEnabled(playerID))
		{
			auto p1 = playertwp[0];
			auto p2 = playertwp[playerID];

			if (CurrentCharacter != Characters_Big && CurrentCharacter != Characters_Gamma)
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, posX, 0.0f, posZ);
			else
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, posX - 3.0f, 0.0f, posZ + 3.0f);
		}
	}
}

bool isPlayerUsingSnowboard() 
{
	if (CurrentCharacter == Characters_Sonic && playertwp[0]->mode >= 62 && playertwp[0]->mode <= 68)
		return true;

	return false;
}

//Fix AI Start Position in hub world

void FixAIHubTransition() 
{
	ForcePlayerAction(0, 0x18);

	if (!IsHubBanned && isAIActive)
	{
		if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1))
			return;

		unsigned char ID = getAI_ID();

		if (!ID)
			return;

		if (CurrentLevel > LevelIDs_E101R && CurrentLevel < LevelIDs_TwinkleCircuit)
			moveAItoPlayer(ID);
	}

	return;
}

void FixAIHubTransition2() 
{
	HudDisplayRingTimeLife_Check();

	unsigned char ID = getAI_ID();

	if (!ID)
		return;

	if (!IsHubBanned && isAIActive)
	{
		if (CurrentCharacter != Characters_Big)
		{
			if (CurrentLevel == LevelIDs_StationSquare && (CurrentAct == 3 || CurrentAct == 1) || CurrentLevel >= LevelIDs_ECGarden && CurrentLevel < LevelIDs_ChaoRace)
				moveAItoPlayer(ID);
		}
		else
		{
			if (CurrentLevel == LevelIDs_StationSquare && CurrentAct == 1 || CurrentLevel >= LevelIDs_ECGarden && CurrentLevel < LevelIDs_ChaoRace)
				moveAItoPlayer(ID);
		}
	}

	return;
}

void GetPlayerSidePos(NJS_VECTOR* v1, taskwk* a2, float m)
{
	Float _sin = 0.0f;

	if (a2)
	{
		if (v1)
		{
			_sin = njSin(a2->ang.y);
			v1->x = a2->pos.x - njCos(a2->ang.y) * m;
			v1->y = a2->pos.y;
			v1->z = a2->pos.z - _sin * m;
		}
	}
}

void SetCharaInfo(task* obj, int i) 
{
	obj->twp->charID = (char)CurrentCharacter;
	obj->twp->pNum = i;

	playertwp[i] = obj->twp;
	playermwp[i] = (motionwk2*)obj->mwp;
	MovePlayerToStartPoint((EntityData1*)obj->twp);
	return;
}

void __cdecl LoadCharacter_r()
{
	task* p1 = nullptr;

	ClearPlayerArrays();

	for (__int16 i = 0; i < 2; ++i)
	{
		if (i)
		{
			LoadTailsOpponent(CurrentCharacter, i, CurrentLevel);
		}
		else
		{
			if (!CurrentCharacter)
			{
				if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2)
				{
					p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Tornado_Main);
				}
				else
				{
					p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Sonic_Main);
				}
			}
			if (CurrentCharacter == Characters_Knuckles)
			{
				p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Knuckles_Main);
				if (sub_42FB00() != 1
					&& (GameMode == GameModes_Adventure_ActionStg
						|| GameMode == GameModes_Mission
						|| GameMode == GameModes_Trial))
				{
					CreateElementalTask(LoadObj_Data1, 6, (TaskFuncPtr)EmeraldRadarHud_Load_Load);
				}
			}
			if (CurrentCharacter == Characters_Tails)
			{
				if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2)
				{
					p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Tornado_Main);
				}
				else
				{
					p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Tails_Main);
				}
			}
			if (CurrentCharacter == Characters_Big)
			{
				p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Big_Main);
				CreateElementalTask(LoadObj_Data1, 6, (TaskFuncPtr)BigHud_Main);
			}
			if (CurrentCharacter == Characters_Amy)
			{
				p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Amy_Main);
				CheckLoadBird();
			}
			if (CurrentCharacter == Characters_Gamma)
			{
				p1 = CreateElementalTask((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, (TaskFuncPtr)Gamma_Main);
			}

			SetCharaInfo(p1, i);
		}
	}
}

int GetRaceWinnerPlayer_r() 
{
	unsigned char ID = getAI_ID();

	if (ID > 0 && CurrentCharacter != Characters_Tails && playertwp[ID] != nullptr) {
		if (playertwp[ID]->charID == Characters_Tails) {
			return 1;
		}
	}

	return RaceWinnerPlayer;
}

void FixTailsAI_Train(int ID, void* a2, int a3, void* a4)
{
	DisableTailsAICol(AIIndex);
	moveAItoPlayer(AIIndex);

	PlaySound(ID, a2, a3, a4);
}

void __cdecl FixTailsAI_ECAreaTransition(unsigned __int8 playerNum, char action)
{
	ForcePlayerAction(playerNum, action);
	moveAItoPlayer(AIIndex);
}

void Fix_AIPos_ActTransition()
{
	moveAItoPlayer(AIIndex);
	ResetMilesAI(AIIndex, 24);
}

void __cdecl PositionPlayer_r(Uint8 charIndex, float x, float y, float z)
{
	PositionPlayer_t.Original(charIndex, x, y, z);

	if (!charIndex && CurrentLevel >= LevelIDs_StationSquare)
		moveAItoPlayer(AIIndex);
}

void __cdecl LeaveSnowBoard(uint8_t pnum, char action)
{
	if (isAIActive)
	{
		ForcePlayerAction(AIIndex, action);
	}

	return ForcePlayerAction(pnum, action);
}

void AI_Patches() 
{
	WriteJump(GetRaceWinnerPlayer, GetRaceWinnerPlayer_r); //fix wrong victory pose for Tails AI.
	//delete Tails AI when a cutscene start
	Ev_Load2_t.Hook(FreeNpcMilesPlayerTask_r);
	//re create Tails AI after a cutscene ends.
	WriteJump(CreateNPCMilesPlayerTask, Load2PTails_r);
	WriteData<1>((int*)0x42FD98, 0x3); //Load Event Char: change task lvl index from 1 to 3 (fix cutscene char crash, race process weld issue)

	PositionPlayer_t.Hook(PositionPlayer_r);
	LoadCharacterBoss_t.Hook(LoadCharacterBoss_r);

	WriteData<6>((int*)0x460fcf, 0x90); //restore Miles's tail effect when AI
	WriteCall((void*)0x4E95DC, LeaveSnowBoard); //make tails leave snowboard at the end of ice cap

	if (IsHubBanned)
		return;

	//Tails AI Fixes and small optimization/improvement.

	WriteCall((void*)0x64015A, FixTailsAI_Train);
	WriteCall((void*)0x53A29B, FixTailsAI_Train);
	WriteCall((void*)0x51BDD0, FixTailsAI_ECAreaTransition);
	WriteCall((void*)0x62EECA, FixTailsAI_ECAreaTransition); //fix hostel respawn pos
}