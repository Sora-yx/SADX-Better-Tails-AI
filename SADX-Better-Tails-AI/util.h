#pragma once

extern unsigned char AIIndex;

void __cdecl DisableTailsAI_Controller(Uint8 index);
void __cdecl EnableTailsAI_Controller(Uint8 index);
bool isInHubWorld();

extern bool disableCol;

bool isInputModActive();

NJS_VECTOR UnitMatrix_GetPoint_Player(NJS_VECTOR* orig, Angle3* rot, float x, float y, float z);
float GetSquare(NJS_VECTOR* orig, NJS_VECTOR* dest);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
float CheckDistance(NJS_VECTOR* vec1, NJS_VECTOR* vec2);
void PlayerLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);

void PlayerMoveForward(taskwk* entity, float speed);
float GetCharacterPositionY(taskwk* p1);
void UpdateP1Position(playerwk* co2p1, playerwk* co2p2, taskwk* p1, taskwk* p2);

bool isCharSelActive();
bool isRandoActive();
bool isUIScale();
bool isMultiEnabled();
bool IsPathExist(const std::string& s);
void ResetPlayerLook(uint8_t pnum);

void MoveForward(taskwk* entity, float speed);

void FadeoutScreen(task* obj);
void SetTailsAILookAt(taskwk* AI, taskwk* twp);

uint16_t GetSitCartAction(uint8_t curChar);