#pragma once

extern unsigned char AIIndex;
void DeleteMilesAI();

void __cdecl DisableTailsAI_Controller(Uint8 index);
void __cdecl EnableTailsAI_Controller(Uint8 index);
bool isInHubWorld();

extern bool disableCol;
bool isTailsAI();


NJS_VECTOR UnitMatrix_GetPoint_Player(NJS_VECTOR* orig, Angle3* rot, float x, float y, float z);
float GetSquare(NJS_VECTOR* orig, NJS_VECTOR* dest);
float GetDistance(NJS_VECTOR* orig, NJS_VECTOR* dest);
float CheckDistance(NJS_VECTOR* vec1, NJS_VECTOR* vec2);
void PlayerLookAt(NJS_VECTOR* from, NJS_VECTOR* to, Angle* outx, Angle* outy);
float GetCharacterPositionY(EntityData1* p1);
void UpdateP1Position(CharObj2* co2p1, CharObj2* co2p2, EntityData1* p1, EntityData1* p2);


bool isP1AndTailsAIEnabled(char playerID);