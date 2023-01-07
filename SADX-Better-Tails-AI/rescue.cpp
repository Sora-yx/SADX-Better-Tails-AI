#include "pch.h"

//This is the rescue page, the idea here is to edit the deathzones and the death function to add code so Tails can save you.

task* MilesRescueObj = nullptr;
task* TailsRescueLanding = nullptr;
task* MilesRescueTaskPtr = nullptr;

bool isRescued = false;
int rngDeathZoneRescue = 0;
int rngRegularDeathRescue = 0;

bool isRescue()
{
	return MilesRescueObj || TailsRescueLanding || MilesRescueTaskPtr;
}

void CameraEvent_MilesRescue(_OBJ_CAMERAPARAM* camparam) {
	unsigned char ID = getAI_ID();

	if (!ID)
		return;

	CameraTask.camxpos = playertwp[ID]->pos.x;
	CameraTask.camypos = playertwp[ID]->pos.y;
	CameraTask.camzpos = playertwp[ID]->pos.z;
	CameraTask.camxpos += 30;
	CameraTask.camypos += 30;
	CameraTask.camzpos += 30;
	CameraTask.tgtxpos = playertwp[ID]->pos.x;
	CameraTask.tgtypos = playertwp[ID]->pos.y;
	CameraTask.tgtzpos = playertwp[ID]->pos.z;
}

MilesAI_Fly RescueArray[]{
	{ LevelIDs_EggHornet, 0, 812, 226.58, 839 },
	{ LevelIDs_EggViper, 0, 260, 12.8, 0},
	{ LevelIDs_StationSquare, 0,  273.946, 93.7993, 345.299},
	{ LevelIDs_StationSquare, 1,  -14.823, 122.437, 1339.01},
	{ LevelIDs_StationSquare, 2,  399.721, -54.527, 994.5},
	{ LevelIDs_StationSquare, 3,  63.2726, 152.052, 1511.99},
	{ LevelIDs_StationSquare, 4,  -470.063, 105.65, 2034.67},
	{ LevelIDs_StationSquare, 5,  695.884, 87.0144, 1771.52},
	{ LevelIDs_MysticRuins, 0, -180.056, 255.684, 909.889},
	{ LevelIDs_MysticRuins, 2, -533.295, 127.576, -869.326},
	{ LevelIDs_Past, 0, 7.64868, 442.872, -739.94},
	{ LevelIDs_Past, 1, 0.782157, 138.21, 1358},
	{ LevelIDs_Past, 2, 0.782157, 138.21, 1358},
};

NJS_VECTOR CheckRescueArray() {
	for (int i = 0; i < LengthOfArray(RescueArray); i++) {
		if (CurrentLevel == RescueArray[i].level && CurrentAct == RescueArray[i].act)
		{
			return RescueArray[i].destination;
		}
	}

	return { -1, -1, -1 };
}

bool isMilesSaving() {
	if (MilesRescueObj || TailsRescueLanding)
		return true;

	return false;
}

//reset PTR when objects are deleted
void TailsAI_LandingRescueDelete(task* obj) {
	rngDeathZoneRescue = 0;
	TailsRescueLanding = nullptr;
	MilesRescueObj = nullptr;
}

void TailsAI_RescueDelete(task* obj) {
	MilesRescueObj = nullptr;
}

const char* const Sonic_message01[] = {
	"	 	 Thanks Tails!   \n",
	NULL,
};

void TailsAI_LandingRescue(task* obj) {
	auto data = obj->twp;
	auto p1 = playertwp[0];
	char pnum = data->counter.b[0];
	auto p2 = playertwp[pnum];

	auto co2p1 = playerpwp[0];
	auto co2p2 = playerpwp[pnum];

	if (!p1 || !p2 || !IsIngame()) {
		return;
	}

	LookAt(&p2->pos, &data->pos, nullptr, &p2->ang.y);

	switch (data->mode) {
	case 0:
	{
		isRescued = true;
		obj->dest = TailsAI_LandingRescueDelete;
		co2p2->mj.reqaction = 37;
		PlayCharacterGrabAnimation(p1, co2p1);
		FlySoundOnFrames(pnum);
		data->mode++;
	}
	break;
	case 1:
		UpdateP1Position(co2p1, co2p2, p1, p2);
		FlySoundOnFrames(pnum);

		if (++data->btimer == 30) {
			data->smode = rand() % 2;
			if (data->smode != 0) {
				if (p1->counter.b[1] == Characters_Sonic) {
					if (TextLanguage == 1)
						DisplayHintText(Sonic_message01, 100);

					if (VoiceLanguage)
						PlayVoice(64871);
				}
			}

			data->mode++;
		}

		break;
	case 2:
		FlySoundOnFrames(pnum);
		co2p2->spd.y = spdYFall;
		UpdateP1Position(co2p1, co2p2, p1, p2);

		if (++data->wtimer == 80 || ((p1->flag & 3))) {
			data->btimer = 0;
			data->mode++;
		}
		break;
	case 3:

		CharColliOn(p1);
		CharColliOn(p2);
		co2p2->item &= 0x100u;
		co2p1->item &= 0x100u;
		EnableController(0);
		PlayCharacterLeaveAnimation(p1, co2p1, pnum);
		RestoreAIControl(pnum);

		if (++data->btimer == 20) {
			FreeTask(obj);
		}
		break;
	}
}

void MilesRescuesCharacterFall(task* obj) {
	auto data = obj->twp;
	auto p1 = playertwp[0];
	char pnum = data->counter.b[0];
	auto p2 = playertwp[pnum];
	auto p1co2 = playerpwp[0];
	auto p2co2 = playerpwp[pnum];

	if (data->mode >= playerGrabbed)
		FlySoundOnFrames(pnum);

	switch (data->mode) {
	case initMilesFalling: //Set tails above player
		CharColliOff(p1);
		CharColliOff(p2);
		p1co2->spd = { 0, 0, 0 };
		p1co2->item |= Powerups_Invincibility;
		p2co2->item |= Powerups_Invincibility;
		obj->dest = TailsAI_RescueDelete;
		p2->pos = p1->pos;
		p2->pos.y += 60.0f;
		p2->mode = 6;
		data->mode++;
		break;
	case catchPlayer:
		if (p2->pos.y - p1->pos.y <= GetCharacterPositionY(p1) + 5.0f) {
			p1co2->spd.y = 0.0f;
			p2->flag &= ~(Status_Attack | Status_Ball | Status_LightDash);
			p2->mode = 15;
			p2co2->mj.reqaction = 37;
			p1->flag &= ~(Status_Attack | Status_Ball | Status_LightDash);

			PlayCharacterGrabAnimation(p1, p1co2);
			data->mode++;
		}
		else { //let AI Fall until he catches player 1.
			p2->flag |= Status_Ball;
			p2co2->mj.reqaction = 15;
			p1co2->spd.y = -2.5f;
			p2co2->spd.y = -4.5f;
		}
		break;
	case playerGrabbed:
		p2->mode = 15;
		p2co2->spd.y += 0.07f;

		if (++data->counter.w[1] == 140) {
			LoadObject(LoadObj_Data1, 1, FadeoutScreen);
		}

		if (++data->wtimer == 200) //get Altitude
		{
			NJS_VECTOR getPos = CheckRescueArray(); //move player to safe point (ie: last grabbed checkpoint or specific spot in hub world)
			if (getPos.x != -1 && getPos.y != -1 && getPos.z != -1) {
				p1->pos = getPos;
			}
			else {
				p1->pos = RestartLevel.Position;
			}
			p1->pos.y += 50.0f;
			p2->pos = p1->pos;
			p1co2->spd = { 0, 0, 0 };
			data->mode++;
		}
		else {
			UpdateP1Position(p1co2, p2co2, p1, p2);
		}
		break;
	case landingTransition:

		if (!TailsRescueLanding) {
			TailsRescueLanding = CreateElementalTask((LoadObj)2, 1, TailsAI_LandingRescue);
		}

		if (TailsRescueLanding) {
			TailsRescueLanding->twp->counter.b[0] = pnum;
			FreeTask(obj);
		}

		break;
	}
}

void CheckAndCallMilesRescue(int playerID) {
	DisableController(0);

	if (!MilesRescueObj) {
		MilesRescueObj = CreateElementalTask((LoadObj)2, 1, MilesRescuesCharacterFall);
		MilesRescueObj->twp->counter.b[0] = playerID;
	}
}

bool BannedRescueLevel() {
	if (CurrentLevel == LevelIDs_SpeedHighway && CurrentAct == 1 || CurrentLevel == LevelIDs_SkyDeck && CurrentAct == 1 || CurrentLevel == LevelIDs_IceCap && CurrentAct == 2
		|| CurrentLevel == LevelIDs_TwinklePark && CurrentAct == 0 || CurrentLevel >= LevelIDs_TwinkleCircuit) {
		return true;
	}

	return false;
}

void PlayCharacterDeathSound_r(ObjectMaster* a1, int pid) {
	unsigned char ID = getAI_ID();

	if (ID > 0 && (!playertwp[ID] || playertwp[ID]->counter.b[1] != Characters_Tails)) {
		PlayCharacterDeathSound(a1, pid); //kill the player
		return;
	}

	if (isMilesSaving() || rngDeathZoneRescue) {
		return;
	}

	if (!MilesRescueObj && !TailsRescueLanding && !rngDeathZoneRescue) {
		rngDeathZoneRescue = rand() % 100 + 1;

		if (BannedRescueLevel() || isRescued && CurrentLevel < LevelIDs_StationSquare || rngDeathZoneRescue < 100 - rescueChance) {
			PlayCharacterDeathSound(a1, pid); //kill the player
			return;
		}
	}

	if (ID > 0)
		CheckAndCallMilesRescue(ID);

	return;
}

static void __declspec(naked) PlayCharacterDeathSoundAsm(ObjectMaster* eax, int pid)
{
	__asm
	{
		push[esp + 04h]
		push eax
		call PlayCharacterDeathSound_r
		pop eax
		add esp, 4
		retn
	}
}

void CheckMilesBossRescue(unsigned char ID) {
	if (CurrentLevel != LevelIDs_EggHornet && CurrentLevel != LevelIDs_EggViper || GameState != 15 || isMilesSaving() || rngDeathZoneRescue || isRescued && CurrentLevel < LevelIDs_StationSquare)
		return;

	if (CurrentLevel == LevelIDs_EggHornet && playertwp[0]->pos.y > 94.0f || CurrentLevel == LevelIDs_EggViper && (playertwp[0]->pos.y > -150.0f || EggViper_Health < 3))
		return;

	if (!MilesRescueObj && !TailsRescueLanding && !rngDeathZoneRescue)
	{
		rngDeathZoneRescue = rand() % 100 + 1;

		if (!playertwp[ID] || playertwp[ID]->counter.b[1] != Characters_Tails || rngDeathZoneRescue < 100 - rescueChance) {
			return;
		}
	}

	playertwp[0]->mode = 20;
	CheckAndCallMilesRescue(ID);
	return;
}

void MilesRescueEnemyDelete(task* obj) {
	rngDeathZoneRescue = 0;
	MilesRescueTaskPtr = nullptr;
	return;
}

void MilesRescueFromEnemy(task* obj) {
	auto data = obj->twp;
	auto p1 = playertwp[0];
	char pnum = data->counter.b[0];
	auto p2 = playertwp[pnum];
	auto co2p2 = playerpwp[pnum];
	auto co2p1 = playerpwp[0];

	switch (data->mode)
	{
	case 0:
		co2p1->item |= Powerups_Invincibility;
		obj->dest = MilesRescueEnemyDelete;
		if (CurrentLevel < LevelIDs_Chaos0)
			SetCameraEvent(CameraEvent_MilesRescue, CameraAdjustsIDs::None, CameraDirectIDs::Target);

		p1->mode = 136;
		p2->mode = 2;
		data->mode++;
		break;
	case 1:
		p2->pos = p1->pos;
		p2->pos.x = p1->pos.x - 100.0f;
		PlaySound(768, 0, 0, 0);
		data->mode++;
		break;
	case 2:
		LookAt(&p2->pos, &p1->pos, nullptr, &p2->ang.y);
		p2->flag |= Status_Ball;
		co2p2->mj.reqaction = 15;
		p2->pos.x += 4;

		if (GetCollidingEntityA((EntityData1*)p2) || ++data->wtimer == 50)
			data->mode++;
		break;
	case 3:
		p2->mode = 4;
		co2p2->spd = { 1.0f, 0.0f, 0.0f };
		HurtCharacter(pnum);
		PlaySound(33, 0, 0, 0);
		isRescued = true;
		EnemyBounceThing(0, 1.5f, 4.0f, 0.0f);
		co2p1->item &= 0x100u;
		data->mode++;
		break;
	default:

		if (CurrentLevel < LevelIDs_Chaos0)
			RemoveCameraEvent();

		ForcePlayerAction(0, 17);
		FreeTask(obj);
		return;
	}
}

void CheckPlayerDamage(unsigned __int8 player) {
	unsigned char ID = getAI_ID();

	if (ID == 0)
		return KillPlayer(player);

	if (isRescued || !playertwp[ID] || playertwp[ID]->counter.b[1] != Characters_Tails || player > 0 || BannedRescueLevel()) {
		return KillPlayer(player);
	}

	if (isMilesSaving() || rngRegularDeathRescue)
		return;

	if (!MilesRescueTaskPtr && !rngRegularDeathRescue && !isRescued) {
		rngRegularDeathRescue = rand() % 100 + 1;

		if (rngDeathZoneRescue < 100 - rescueChance) {
			MilesRescueTaskPtr = CreateElementalTask((LoadObj)2, 1, MilesRescueFromEnemy);
			MilesRescueTaskPtr->twp->counter.b[0] = ID;
		}
		else
		{
			return KillPlayer(player);
		}
	}
}

void Rescue_Init() {
	WriteCall((void*)0x44af87, PlayCharacterDeathSoundAsm);
	WriteCall((void*)0x44affe, PlayCharacterDeathSoundAsm);
	WriteCall((void*)0x450785, CheckPlayerDamage);
}