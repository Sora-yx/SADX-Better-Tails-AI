#include "pch.h"

static FunctionHook<void, task*> Chao_Main_t((intptr_t)Chao_Main);
static FunctionHook<void, int> EV_Load2_t(EV_Load2);
static FunctionHook<void, task*> ScoreDisplay_main_t((intptr_t)ScoreDisplay_Main);
static FunctionHook<void, task*> execTPCoaster_t((intptr_t)0x61D6E0);

bool disableCol = false;

void SpinDash_Check(unsigned char ID, taskwk* data) {
	if (!isNewTricksActive())
		return;

	auto pData = playertwp[0];

	if (pData->charID == Characters_Sonic && pData->mode == 4 || pData->charID == Characters_Knuckles && pData->mode == 59) {
		if (playertwp[ID]->flag & 0x10)
		{
			return;
		}

		data->ang = pData->ang;
		PressedButtons[ID] |= Buttons_B;
		HeldButtons[ID] |= Buttons_B;
	}
}

void InvincibilityCheck(unsigned char playerID)
{
	if (disableCol)
	{
		playerpwp[playerID]->item |= Powerups_Invincibility;
	}
}

//Make Tails using snowboard again when trying to catch Sonic
void SnowboardRespawn(unsigned char playerID) {
	if (GameState != 15 || isRandoActive())
		return;

	auto milesAI = playertwp[playerID];

	if (milesAI->mode< boardSlide || milesAI->mode > boardHurt) {
		if (isPlayerUsingSnowboard()) {
			DisableTailsAICol(playerID);
			playertwp[playerID]->flag &= ~(Status_Attack | Status_Ball | Status_LightDash);
			playerpwp[playerID]->p.height = PhysicsArray[Characters_Tails].CollisionSize; //Reset Miles physic properly
			playerpwp[playerID]->p.center_height = PhysicsArray[Characters_Tails].YOff;
			playerpwp[playerID]->p.jmp_y_spd = PhysicsArray[Characters_Tails].JumpSpeed;
			ForcePlayerAction(playerID, 44);
		}
	}
}

void snowboard_Follow(unsigned char playerID)
{
	auto P1 = playertwp[0];
	auto P2 = playertwp[playerID];

	if (isPlayerUsingSnowboard()) {
		P2->ang = P1->ang;
		playerpwp[playerID]->spd.x = playerpwp[0]->spd.x + 0.3f;
		SnowboardRespawn(playerID);
	}
}

void speedHighwayBuilding_Follow(unsigned char playerID) {
	if (CurrentCharacter != Characters_Sonic || CurrentLevel != LevelIDs_SpeedHighway && CurrentAct != 1)
		return;

	auto P1 = playertwp[0];
	auto P2 = playertwp[playerID];
	auto p2CO2 = playerpwp[playerID];

	if (P2->pos.y <= -18660.0f) {
		if (P2->mode == AIObjControl) {
			disableCol = false;
			CharColliOn(P2);
			ForcePlayerAction(playerID, 24);
			return;
		}
	}
	else {
		if (P1->mode >= 46 && P1->mode <= 50)
		{
			if (!disableCol) {
				ForcePlayerAction(playerID, 48);
				P2->flag &= ~Status_Ball;
				p2CO2->mj.reqaction = 37;
				DisableTailsAICol(playerID);
				disableCol = true;
			}

			P2->ang = P1->ang;

			if (P2->pos.y > -10790.0f)
			{
				P2->pos.x = P1->pos.x + 10.0f;
				P2->pos.y = P1->pos.y + 2.0f;
				P2->pos.z = P1->pos.z + 10.0f;
			}
			else if (P2->pos.y <= -10791.0f && P2->pos.y > -18659.0f) {
				p2CO2->mj.reqaction = 12;
				P2->pos.x = P1->pos.x;
				P2->pos.y = P1->pos.y - 7.0f;
				P2->pos.z = P1->pos.z;
			}
		}
	}
}

//Load Tails AI Snowboard when playing Sonic
void LoadAISnowBoard_R() {
	ForcePlayerAction(0, 0x2c);

	unsigned char ID = getAI_ID();

	if (!ID)
		return;

	if (CurrentCharacter == Characters_Sonic && isAIActive)
	{
		ForcePlayerAction(ID, 44); //Force AI to use Snowboard
		ObjectMaster* board = LoadObject((LoadObj)(LoadObj_Data1 | LoadObj_Data2), 2, Snowboard_Tails_Load);
		if (board != nullptr)
		{
			board->Data1->CharID = ID;
			board->Data1->CharIndex = ID;
			DisableTailsAICol(ID);
			disableCol = true;

			return;
		}
	}
}

int CharacterPetmodeNumber[8] = { 72, 64, 64, 54, 54, 50, 56, 52 };

int isCharacterPetting() {
	return CharacterPetmodeNumber[playertwp[0]->charID];
}

bool isChaoPetByAI = false;
int chaoHappyTimer = 0;

void MakeAIPetChao(task* Chao) {
	auto data = Chao->twp;
	auto p1 = playertwp[0];
	auto ID = getAI_ID();

	if (!ID)
		return;

	auto p2 = playertwp[ID];

	if (p1->mode != isCharacterPetting()) {
		if (p2->mode > 40) {
			p2->mode = 1;
		}
		data->btimer = 0;
		p2->id = 0;
	}

	float dist = GetDistance(&p2->pos, &data->pos);

	switch (data->btimer)
	{
	case 0:
		if (++p2->id == 40)
		{
			data->wtimer = 0;
			data->btimer++;
		}
		break;
	case 1:
		if (dist < 9) {
			chaoHappyTimer = 0;
			p2->mode = 64;
			playerpwp[ID]->mj.reqaction = 133;
			isChaoPetByAI = true;
		}
		break;
	}
}

void CheckAndMakeAIPetChao(task* Chao) {
	unsigned char ID = getAI_ID();

	if (!ID)
		return;

	if (IsChaoGardenBanned || !playertwp[ID])
		return;

	if (CurrentLevel >= LevelIDs_SSGarden && CurrentLevel <= LevelIDs_MRGarden) {
		MakeAIPetChao(Chao);
	}
}

void Chao_Main_R(task* obj) {
	auto p1 = playertwp[0];

	if (IsPlayerInsideSphere(&obj->twp->pos, 10)) {
		CheckAndMakeAIPetChao(obj);
	}

	if (p1 && p1->mode != isCharacterPetting() && isChaoPetByAI) {
		if (++chaoHappyTimer == 120) {
			Chao_SetBehavior(obj, (long*)Chao_Pleasure);
			isChaoPetByAI = false;
			chaoHappyTimer = 0;
		}
	}

	Chao_Main_t.Original(obj);
}

void MilesAI_VictoryPose(task* obj) {
	auto data = obj->twp;
	auto pnum = data->pNum;

	auto p1 = playertwp[0];
	auto p2 = playertwp[pnum];

	if (!isP1AndTailsAIEnabled(pnum) || !isAIActive)
		return;

	switch (data->mode)
	{
	case 0:
		if (p1->smode == 0x13) {
			disableCol = true;
			DisableTailsAICol(pnum);
			ForcePlayerAction(pnum, 24);
			p2->mode = AIObjControl;
			float res = GetDistance(&p2->pos, &p1->pos);
			if (res > 10.0f)
			{
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p2->ang, 0.0f, 0.0f, 7.0f);
			}
			p2->ang = p1->ang;

			if (++data->wtimer == 10) {
				disableCol = false;
				CharColliOn(p2);
				data->mode++;
			}
		}
		break;
	case 1:

		if (((p2->flag & 3) == 0) || p2->pos.y > p1->pos.y + 2.5f || p2->pos.y < p1->pos.y - 2.5f)
		{
			p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p2->ang, 0.0f, 0.0f, 6.0f);  //fix floating victory pose
		}

		if (++data->btimer == 5) {
			if ((p2->flag & 3) == 0) { //last failsafe
				p2->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p2->ang, 0.0f, 0.0f, -6.0f); //try other side
			}
			data->mode++;
		}

		break;
	case 2:
		p2->ang = p1->ang;
		ForcePlayerAction(pnum, 19); //Force AI to Victory pose
		break;
	}
}

//While load result: Teleport AI close to the player and Force Victory Pose.
void ScoreDisplayMain_R(task* obj) {
	unsigned char ID = getAI_ID();

	if (ID > 0) {
		if (obj->twp->mode == 0) {
			task* victory = CreateElementalTask((LoadObj)(LoadObj_Data1 | LoadObj_Data2), 2, MilesAI_VictoryPose);
			victory->twp->pNum = ID;
		}
	}

	ScoreDisplay_main_t.Original(obj);
}

int copyDebugmode = 110;

void PreventTailsAImode(unsigned char aiID) {
	if (playertwp[aiID]->charID != Characters_Tails || !playerpwp[aiID])
		return;

	auto co2 = playerpwp[aiID];
	auto aiData = playertwp[aiID];

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 0)
	{
		if (aiData->mode == 38)
		{
			aiData->mode = 1;
			aiData->pos.x += 5.0f;
		}
	}

	if (DebugMode)
	{
		if (playertwp[0])
		{
			GetPlayerSidePos(&aiData->pos, playertwp[0], 20.0f);
			SetPlayerPosition(aiID, 0, &aiData->pos, 0);
		}

		copyDebugmode = aiData->mode;
	}
	else {
		if (aiData->mode == copyDebugmode)
		{
			aiData->flag &= ~Status_Ball;
			aiData->mode = 1;
		}
	}
}

void ResetMilesAI(char pnum, char mode)
{
	auto milesData = playertwp[AIIndex];
	auto miles = (task*)PlayerPtrs[AIIndex];
	auto co2Miles = playerpwp[AIIndex];

	if (!co2Miles)
		return;

	if (mode > 0)
		ForcePlayerAction(pnum, mode);

	disableCol = false;
	CharColliOn(milesData);
	EV_ClrAction(miles);
	milesData->mode = 1;
	co2Miles->mj.reqaction = 1;
}

void AI_SitInCart(taskwk* p1, taskwk* milesData, task* miles)
{
	char pnum = milesData->pNum;

	if (CurrentLevel == LevelIDs_TwinklePark && CurrentAct == 1 && p1->mode == 45)
		return;

	if (p1->mode == 45 && milesData->mode != passengerCart)
	{
		disableCol = true;
		DisableTailsAICol(pnum);
		milesData->flag &= ~Status_Ball;
		EV_SetAction(miles, &action_m_m9002_miles, &MILES_TEXLIST, 1.0f, 3, 0);
		milesData->mode = passengerCart;
	}
	else if (milesData->mode == passengerCart)
	{
		if (p1->mode == 45)
		{
			milesData->ang = p1->ang;
			milesData->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, -4.5f, 4.5f, 2.0f);
		}
		else
		{
			ResetMilesAI(AIIndex, 28);
		}
	}
}

void AI_HubWorld_Vehicle(taskwk* p1, taskwk* milesData, task* miles)
{
	if (!isInHubWorld())
	{
		return;
	}

	if (p1->mode == 20)
	{
		moveAItoPlayer(AIIndex, -5.0, 0.0f);
		ForcePlayerAction(AIIndex, 12);
		milesData->ang = p1->ang;
		if (CurrentLevel != LevelIDs_StationSquare && CurrentAct != 3)
			EV_SetAction(miles, &action_m_m9002_miles, &MILES_TEXLIST, 1.0f, 3, 0);
		disableCol = true;
		DisableTailsAICol(AIIndex);
		milesData->mode = AIObjControl;
	}

	if (milesData->mode == AIObjControl)
	{
		milesData->ang = p1->ang;
		milesData->pos = UnitMatrix_GetPoint_Player(&p1->pos, &p1->ang, 2.0f, 1.0f, 5.0f);
	}

	char nextmode = p1->smode;

	if ((nextmode == 24 || nextmode == 5) && milesData->mode == AIObjControl)
	{
		ResetMilesAI(AIIndex, nextmode);
	}
}

void MoveAI_Vehicle()
{
	if (!playertwp[0] || isFlyTravelEnabled() || isRescue())
		return;

	auto milesData = playertwp[AIIndex];
	auto miles = (task*)PlayerPtrs[AIIndex];
	auto p1 = playertwp[0];
	auto co2Miles = playerpwp[AIIndex];

	AI_SitInCart(p1, milesData, miles);
	AI_HubWorld_Vehicle(p1, milesData, miles);
}

//patch RC to make Miles able to use it
void __cdecl execTPCoaster_r(task* tp)
{
	execTPCoaster_t.Original(tp);

	auto twp = tp->twp;
	auto id = twp->value.b[0];
	float posY = 4.0f;

	if (id > 0 && playertwp[id])
	{
		switch (twp->mode)
		{
		case 1:
			NPCMilesStandByFlag = 0;
			twp->scl.x = (twp->pos.x - playertwp[0]->pos.x) * 0.05f;
			twp->scl.y = (twp->pos.y - playertwp[0]->pos.y) * 0.05f;
			twp->scl.z = (twp->pos.z - playertwp[0]->pos.z) * 0.01f;
			break;
		case 2:
			if (++twp->wtimer <= 0x14ui16)
			{
				EV_SetAction((task*)PlayerPtrs[AIIndex], &action_m_m9002_miles, &MILES_TEXLIST, 1.0f, 3, 0);
				twp->pos.x += twp->scl.x;
				twp->pos.y += twp->scl.y + posY;
				twp->pos.z += twp->scl.z;
			}
			else
			{
				SetInputP(id, PL_OP_PLACEWITHCART);
			}
			break;
		case 3:
			SetPositionP(id, twp->pos.x, twp->pos.y + posY, twp->pos.z);
			SetRotationP(id, twp->ang.x, twp->ang.y - 0x4000, twp->ang.z);
			break;
		case 4:
			ResetMilesAI(AIIndex, 28);
			GetOutOfCartP(id, -1.2f, 1.5f, 0.0f);
			RumbleA(id, 0);
			break;
		}
	}
}

uint32_t timingRespawn = 0;

void MilesFasterRespawn(taskwk* p1, taskwk* p2)
{
	float distance = getMilesDistance(p1, p2);

	if (distance > 150.0f)
	{
		timingRespawn++;
		if (timingRespawn > 300)
		{
			ForcePlayerAction(AIIndex, 48);
			p2->id = 0;
			p2->flag &= ~Status_Ball;
			p2->pos.y = p1->pos.y + 60.0f;
			p2->pos.x = p1->pos.x;
			p2->pos.z = p1->pos.z;
			timingRespawn = 0;
		}
	}
	else
	{
		timingRespawn = 0;
	}
}

void Miles_AbilitiesOnFrames(unsigned char pnum)
{
	MoveAI_Vehicle();

	if (!IsIngame())
		return;

	InvincibilityCheck(pnum);
	PreventTailsAImode(pnum);
	speedHighwayBuilding_Follow(pnum);
	SpinDash_Check(pnum, playertwp[pnum]);
	snowboard_Follow(pnum);

	if (fasterRespawn)
		MilesFasterRespawn(playertwp[0], playertwp[pnum]);
}

void AI_Improvement(const char* path) {
	//Miles General Improvement
	WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill
	WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
	WriteCall((void*)0x4e9664, LoadAISnowBoard_R);

	FlyTravel_Init(path);

	if (isRescueAllowed)
		Rescue_Init();

	Chao_Main_t.Hook(Chao_Main_R);
	ScoreDisplay_main_t.Hook(ScoreDisplayMain_R);
	execTPCoaster_t.Hook(execTPCoaster_r);
}