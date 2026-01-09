//
//  game.cpp
//  Minesweeper
//
//  Created by Fev1L on 09.01.2026.
//

#include "game.h"

void fillField(int field[16][16]){
    int chanse;
    for(int i = 0;i<16;i++){
        for(int j = 0;j<16;j++){
            chanse = rand() % 100;
            if(chanse > 80){
                field[i][j] = -1;
            }else{
                field[i][j] = 0;
            }
        }
    }
}

void calculateNumbers(int width, int height, int field[16][16], Game* game) {
    fillField(field);
    int dx[8] = {-1,-1,-1,0,0,1,1,1};
    int dy[8] = {-1,0,1,-1,1,-1,0,1};

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if(field[y][x] == -1)
                continue;

            int count = 0;
            for(int i = 0; i < 8; i++){
                int nx = x + dx[i];
                int ny = y + dy[i];

                if(nx >= 0 && nx < width && ny >= 0 && ny < height){
                    if(field[ny][nx] == -1){
                        count++;
                    }
                }
            }
            field[y][x] = count;
        }
    }
    int mineCount = 0;
    for(int i = 0;i<16;i++){
        for(int j = 0;j<16;j++){
            if(field[i][j] == -1){
                mineCount++;
            }
        }
    }
    game->totalSafeCells = 16 * 16 - mineCount;
    mineCount = 0;
}

void revealCell(int gx, int gy, std::vector<Bavel>& buttons, int field[16][16],
                std::vector<Rectangle>& mines, int& revealedCells) {

    Bavel& btn = buttons[gy * 16 + gx];

    if (btn.revealed) return;

    btn.revealed = true;
    revealedCells++;

    SDL_Color color = {191,191,191,255};
    mines.push_back({btn.rect, color, 0.0f, std::to_string(field[gy][gx])});

    if (field[gy][gx] == 0) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = gx + dx;
                int ny = gy + dy;
                if (nx >= 0 && nx < 16 && ny >= 0 && ny < 16) {
                    revealCell(nx, ny, buttons, field, mines, revealedCells);
                }
            }
        }
    }
}
