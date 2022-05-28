#include "stdafx.h"

//every improvement and new abilities for Tails go here.

Trampoline* Chao_Main_t = nullptr;
Trampoline* ScoreDisplay_main_t = nullptr;
Trampoline* forcePlayerAction_t = nullptr;

bool disableCol = false;

void SpinDash_Check(unsigned char ID) {

	if (!isNewTricksActive())
		return;

	EntityData1* data = EntityData1Ptrs[0];

	if (data->CharID == Characters_Sonic && data->Action == 4 || data->CharID == Characters_Knuckles && data->Action == 59) {
		PressedButtons[ID] |= Buttons_B;
		HeldButtons[ID] |= Buttons_B;
	}
}

void InvincibilityCheck(unsigned char playerID)
{
	if (disableCol)
	{
		CharObj2Ptrs[playerID]->Powerups |= Powerups_Invincibility;
	}
}

//Make Tails using snowboard again when trying to catch Sonic
void SnowboardRespawn(unsigned char playerID) {
	if (GameState != 15 || isRandoActive())
		return;

	EntityData1* milesAI = EntityData1Ptrs[playerID];

	if (milesAI->Action > boardHurt && milesAI->Action < boardSlide) {

		if (isPlayerUsingSnowboard()) {
			EntityData1Ptrs[playerID]->Status &= ~(Status_Attack | Status_Ball | Status_LightDash);
			CharObj2Ptrs[playerID]->PhysicsData.CollisionSize = PhysicsArray[Characters_Tails].CollisionSize; //Reset Miles physic properly
			CharObj2Ptrs[playerID]->PhysicsData.YOff = PhysicsArray[Characters_Tails].YOff;
			CharObj2Ptrs[playerID]->PhysicsData.JumpSpeed = PhysicsArray[Characters_Tails].JumpSpeed;
			ForcePlayerAction(playerID, 44);
		}
	}
}


void snowboard_Follow(unsigned char playerID, EntityData1* P1, EntityData1* P2)
{
	if (isPlayerUsingSnowboard()) {
		if (CurrentLevel == LevelIDs_IceCap && CurrentAct == 2 && P1->Position.y > -3800) {
			if (P2->Status & Status_Ground)
				CharObj2Ptrs[playerID]->Speed.x = CharObj2Ptrs[0]->Speed.x + 0.6f;
		}
		else {
			P2->Rotation = P1->Rotation;
			if (P1->Status & Status_Ground)
				CharObj2Ptrs[playerID]->Speed.x = CharObj2Ptrs[0]->Speed.x + 0.8f;
		}
	}

	SnowboardRespawn(playerID);
}

void speedHighwayBuilding_Follow(unsigned char playerID) {

	if (CurrentCharacter != Characters_Sonic || CurrentLevel != LevelIDs_SpeedHighway && CurrentAct != 1)
		return;

	EntityData1* P1 = EntityData1Ptrs[0];
	EntityData1* P2 = EntityData1Ptrs[playerID];

	if (P1->Action >= 46 && P1->Action <= 50)
	{
		disableCol = true;
		P2->Rotation = P1->Rotation;
		if (P2->Position.y > -10790)
		{
			CharObj2Ptrs[playerID]->AnimationThing.Index = 37;
			P2->Action = followBuilding;
			P2->Position.x = P1->Position.x + 10;
			P2->Position.y = P1->Position.y + 2;
			P2->Position.z = P1->Position.z + 10;
		}
		else if (P2->Position.y <= -10791 && P2->Position.y > -18659) {
			CharObj2Ptrs[playerID]->AnimationThing.Index = 12;
			P2->Action = followBuilding;
			P2->Position.x = P1->Position.x;
			P2->Position.y = P1->Position.y - 7;
			P2->Position.z = P1->Position.z;
		}
		else if (P2->Position.y <= -18660 && P2->Action > 90) {
			disableCol = false;
			CharObj2Ptrs[playerID]->Powerups &= 0x100u;
			P2->Action = 1;
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
			return;
		}
	}
}



int CharacterPetActionNumber[8] = { 72, 64, 64, 54, 54, 50, 56, 52 };

int isCharacterPetting() {
	return CharacterPetActionNumber[EntityData1Ptrs[0]->CharID];
}

bool isChaoPetByAI = false;
int chaoHappyTimer = 0;

void MakeAIPetChao(ObjectMaster* Chao) {
	EntityData1* data = Chao->Data1;
	EntityData1* p1 = EntityData1Ptrs[0];


	unsigned char ID = getAI_ID();

	if (!ID)
		return;

	EntityData1* p2 = EntityData1Ptrs[ID];

	if (p1->Action != isCharacterPetting()) {
		if (p2->Action > 40) {
			p2->Action = 1;
		}
		data->Index = 0;
		p2->Unknown = 0;
	}

	float dist = GetDistance(&p2->Position, &data->Position);

	switch (data->Index)
	{
	case 0:
		if (++p2->Unknown == 40)
		{
			data->InvulnerableTime = 0;
			data->Index = 1;
		}
		break;
	case 1:
		if (dist < 9) {
			chaoHappyTimer = 0;
			p2->Action = 64;
			CharObj2Ptrs[ID]->AnimationThing.Index = 133;
			isChaoPetByAI = true;
		}
		break;
	}
}

void CheckAndMakeAIPetChao(ObjectMaster* Chao) {

	unsigned char ID = getAI_ID();

	if (!ID)
		return;

	if (IsChaoGardenBanned || !EntityData1Ptrs[ID])
		return;

	if (CurrentLevel >= LevelIDs_SSGarden && CurrentLevel <= LevelIDs_MRGarden) {
		MakeAIPetChao(Chao);
	}
}


void Chao_Main_R(ObjectMaster* obj) {
	EntityData1* p1 = EntityData1Ptrs[0];


	if (IsPlayerInsideSphere(&obj->Data1->Position, 10)) {
		CheckAndMakeAIPetChao(obj);
	}

	if (p1->Action != isCharacterPetting() && isChaoPetByAI) {
		if (++chaoHappyTimer == 120) {
			Chao_SetBehavior(obj, (long*)Chao_Pleasure);
			isChaoPetByAI = false;
			chaoHappyTimer = 0;
		}
	}

	ObjectFunc(origin, Chao_Main_t->Target());
	origin(obj);
}

void MilesAI_VictoryPose(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;

	if (!EntityData1Ptrs[0] || !EntityData1Ptrs[data->CharIndex] || !isAIActive || EntityData1Ptrs[data->CharIndex]->CharID != Characters_Tails)
		return;

	EntityData1* p1 = EntityData1Ptrs[0];
	EntityData1* p2 = EntityData1Ptrs[data->CharIndex];


	switch (data->Action)
	{
	case 0:
		if (p1->NextAction == 0x13) {
			disableCol = true;
			p2->Position = UnitMatrix_GetPoint_Player(&p1->Position, &p2->Rotation, 0.0f, 0.0f, 7.0f);
			p2->Rotation = p1->Rotation;
			if (++data->InvulnerableTime == 10)
				data->Action = 1;
		}
		break;
	case 1:

		if (CurrentLevel == LevelIDs_IceCap && CurrentAct == 2)
			ForcePlayerAction(data->CharIndex, 24);

		if ((p2->Status & Status_Ground) == 0 && (p2->Status & Status_OnColli) == 0 || p2->Position.y > p1->Position.y + 2 || p2->Position.y < p1->Position.y - 2)
		{
			p2->Position = UnitMatrix_GetPoint_Player(&p1->Position, &p2->Rotation, 0.0f, 0.0f, 6.0f);  //fix floating victory pose
		}

		if (++data->Index == 5) {

			if ((p2->Status & Status_Ground) == 0 && (p2->Status & Status_OnColli) == 0) { //last failsafe
				p2->Position = UnitMatrix_GetPoint_Player(&p1->Position, &p2->Rotation, 0.0f, 0.0f, -6.0f); //try other side
			}
			data->Action = 2;
		}

		break;
	case 2:
		p2->Rotation = p1->Rotation;
		ForcePlayerAction(data->CharIndex, 19); //Force AI to Victory pose
		disableCol = false;
		break;
	}
}

//While load result: Teleport AI close to the player and Force Victory Pose.
void ScoreDisplayMain_R(ObjectMaster* obj) {

	unsigned char ID = getAI_ID();

	if (ID > 0) {

		if (obj->Data1->Action == 0) {
			ObjectMaster* victory = LoadObject((LoadObj)(LoadObj_Data1 | LoadObj_Data2), 2, MilesAI_VictoryPose);
			victory->Data1->CharIndex = ID;
		}
	}

	ObjectFunc(origin, ScoreDisplay_main_t->Target());
	origin(obj);
}

int copyDebugAction = 110;

void PreventTailsAIAction(unsigned char playerID) {

	if (EntityData1Ptrs[playerID]->CharID != Characters_Tails || !EntityData1Ptrs[playerID] || !CharObj2Ptrs[playerID])
		return;

	CharObj2* co2 = CharObj2Ptrs[playerID];
	EntityData1* data = EntityData1Ptrs[playerID];

	if (CurrentLevel == LevelIDs_MysticRuins && CurrentAct == 0)
	{
		if (data->Action == 38)
		{
			data->Action = 1;
			data->Position.x += 5.0f;
		}
	}

	if (DebugMode)
	{
		copyDebugAction = data->Action;
	}
	else {
		if (data->Action == copyDebugAction)
		{
			data->Status &= ~Status_Ball;
			data->Action = 1;
		}
	}
}

void FixCollision(EntityData1* entity) {

	if (disableCol)
		return;

	AddToCollisionList(entity);
}

void ResetMilesAI(char pnum, char action)
{
	EntityData1* milesData = EntityData1Ptrs[AIIndex];
	task* miles = (task*)PlayerPtrs[AIIndex];
	CharObj2* co2Miles = CharObj2Ptrs[AIIndex];

	if (action > 0)
		ForcePlayerAction(pnum, action);

	disableCol = false;
	EV_ClrAction(miles);
	milesData->Action = 1;
	co2Miles->AnimationThing.Index = 1;
}


void AI_SitInCart(EntityData1* p1, EntityData1* milesData, task* miles, CharObj2* co2Miles)
{
	char pnum = milesData->CharIndex;

	if (CurrentLevel == LevelIDs_TwinklePark && CurrentAct == 1 && p1->Action == 45)
		return;

	if (p1->Action == 45 && milesData->Action != passengerCart)
	{
		disableCol = true;
		EV_SetAction(miles, &action_m_m9002_miles, &MILES_TEXLIST, 1.0f, 3, 0);
		milesData->Action = passengerCart;
	}
	else if (milesData->Action == passengerCart)
	{
		if (p1->Action == 45)
		{
			milesData->Rotation = p1->Rotation;
			milesData->Position = UnitMatrix_GetPoint_Player(&p1->Position, &p1->Rotation, -4.5f, 4.5f, 2.0f);
		}
		else
		{
			ResetMilesAI(AIIndex, 28);
		}
	}

}

void AI_HubWorld_Vehicle(EntityData1* p1, EntityData1* milesData, task* miles, CharObj2* co2Miles)
{
	if (!isInHubWorld())
	{
		return;
	}

	if (p1->Action == 20)
	{
		moveAItoPlayer(AIIndex, -5.0, 0.0f);
		ForcePlayerAction(AIIndex, 12);
		milesData->Rotation = p1->Rotation;	
		EV_SetAction(miles, &action_m_m9002_miles, &MILES_TEXLIST, 1.0f, 3, 0);
		disableCol = true;
		milesData->Action = 18;
	}

	if (milesData->Action == 18)
	{
		milesData->Rotation = p1->Rotation;
		milesData->Position = UnitMatrix_GetPoint_Player(&p1->Position, &p1->Rotation, 2.0f, 1.0f, 5.0f);
	}

	char nextAction = p1->NextAction;
	if ( (nextAction == 24 || nextAction == 5) && milesData->Action == 18)
	{
		ResetMilesAI(AIIndex, nextAction);
	}

}

void MoveAI_Vehicle()
{
	if (!EntityData1Ptrs[0])
		return;

	EntityData1* milesData = EntityData1Ptrs[AIIndex];
	task* miles = (task*)PlayerPtrs[AIIndex];
	EntityData1* p1 = EntityData1Ptrs[0];
	CharObj2* co2Miles = CharObj2Ptrs[AIIndex];

	AI_SitInCart(p1, milesData, miles, co2Miles);
	AI_HubWorld_Vehicle(p1, milesData, miles, co2Miles);
}

//patch RC to make Miles able to use it
void __cdecl execTPCoaster_r(task* tp);
Trampoline execTPCoaster_t(0x61D6E0, 0x61D6E6, execTPCoaster_r);
void __cdecl execTPCoaster_r(task* tp)
{
	TARGET_STATIC(execTPCoaster)(tp);

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

void MilesFasterRespawn(EntityData1* p1, EntityData1* p2)
{
	float distance = getMilesDistance(p1, p2);

	if (distance > 150.0f)
	{
		timingRespawn++;
		if (timingRespawn > 300)
		{
			ForcePlayerAction(AIIndex, 48);
			p2->Position.y = p1->Position.y + 60.0f;
			p2->Position.x = p1->Position.x;
			p2->Position.z = p1->Position.z;
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
	if (!EntityData1Ptrs[0] || !EntityData1Ptrs[pnum])
		return;


	InvincibilityCheck(pnum);
	PreventTailsAIAction(pnum);
	speedHighwayBuilding_Follow(pnum);
	SpinDash_Check(pnum);

	MoveAI_Vehicle();

	MilesFasterRespawn(EntityData1Ptrs[0], EntityData1Ptrs[pnum]);
}

void AI_Improvement() {

	//Miles General Improvement
	WriteCall((void*)0x597b14, LoadAISnowBoard_R);  //Load AI Snowboard when playing Sand Hill
	WriteCall((void*)0x4ea091, LoadAISnowBoard_R);  //Load AI Snowboard when playing Ice Cap.
	WriteCall((void*)0x4e9664, LoadAISnowBoard_R);
	WriteCall((void*)0x462490, FixCollision);


	if (isFlyTravel)
		FlyTravel_Init();

	if (isRescueAllowed)
		Rescue_Init();

	Chao_Main_t = new Trampoline((int)Chao_Main, (int)Chao_Main + 0x6, Chao_Main_R);
	ScoreDisplay_main_t = new Trampoline((int)ScoreDisplay_Main, (int)ScoreDisplay_Main + 0x5, ScoreDisplayMain_R);
}