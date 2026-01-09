//
//  game.h
//  Minesweeper
//
//  Created by Fev1L on 09.01.2026.
//

#pragma once
#include "../types.h"

void fillField(int field[16][16]);
void calculateNumbers(int width, int height, int field[16][16], Game* game);
void revealCell(int gx, int gy, std::vector<Bavel>& buttons, int field[16][16], std::vector<Rectangle>& mines, int& revealedCells);
