#pragma once

extern unsigned char AIIndex;
void DeleteMilesAI();

void __cdecl DisableTailsAI_Controller(Uint8 index);
void __cdecl EnableTailsAI_Controller(Uint8 index);
bool isInHubWorld();

extern bool disableCol;
bool isTailsAI();