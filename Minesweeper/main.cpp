//
//  main.cpp
//  Minesweeper
//
//  Created by Fev1L on 18.11.2025.
//

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <cstdlib>

#include "types.h"
#include "render/render.h"
#include "game/game.h"

//=================================================================
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]){
    srand(time(0));
    App* app = new App();
    app->game = new Game();
    app->state = new AppState();
    app->fonts = new Fonts();
    
    calculateNumbers(16, 16,app->game->field, app->game);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failure!");
        return SDL_APP_FAILURE;
    }
    
    if (!TTF_Init()) {
        SDL_Log("TTF_Init Failure!");
        SDL_Quit();
        return SDL_APP_FAILURE;
    }
    app->game->window = SDL_CreateWindow("Minesweeper by Fev1L v.1", 550, 700, SDL_WINDOW_OPENGL );
    app->game->renderer = SDL_CreateRenderer(app->game->window, nullptr);
    
    std::string basePath = SDL_GetBasePath();
    
    std::string iconPath = basePath + "Assets/icon.bmp";
    std::string minePath = basePath + "Assets/mine.bmp";
    std::string flagPath = basePath + "Assets/flag.bmp";
    
    SDL_Surface* icon = SDL_LoadBMP(iconPath.c_str());
    SDL_Surface* mine = SDL_LoadBMP(minePath.c_str());
    app->game->imageTexture.push_back(SDL_CreateTextureFromSurface(app->game->renderer, mine));
    SDL_Surface* flag = SDL_LoadBMP(flagPath.c_str());
    app->game->imageTexture.push_back(SDL_CreateTextureFromSurface(app->game->renderer, flag));
    if (icon) {
        SDL_SetWindowIcon(app->game->window, icon);
        SDL_DestroySurface(icon);
    } else {
        SDL_Log("Icon not found: %s", SDL_GetError());
    }
    
    std::string fontPath = basePath + "Assets/PressStart2P-Regular.ttf";
    app->fonts->fontText = TTF_OpenFont(fontPath.c_str(), 15);
    app->fonts->fontMines = TTF_OpenFont(fontPath.c_str(), 32);
    app->fonts->fontButtons = TTF_OpenFont(fontPath.c_str(), 20);
    if (!app->fonts->fontText && !app->fonts->fontMines && !app->fonts->fontButtons) {
        SDL_Log("FONT Failure!");
        SDL_DestroyRenderer(app->game->renderer);
        SDL_DestroyWindow(app->game->window);
        TTF_Quit();
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    app->state->textScreen10 = {{30,36},{88,86,88,255},"Text","Whilst hovering over a block, press ‘space’ to place a flag. Press ‘r’ to restart your game."};
    app->state->rectangleScreen = {{398,42,104,39},{0,0,0,255},0.0f,"Timer"};
    app->state->bavelBackground = {0,0,550,700};
    app->state->bavelMenu = {15,20,520,93};
    app->state->bavelGame = {15,140,520,520};
    
    for(int i = 0;i<16;i++){
        for(int j = 0;j<16;j++){
            app->game->buttons.push_back({{static_cast<float>(19 + (32 * i)),static_cast<float>(144 + (32 * j)),32,32},{i,j},"Btn",false});
        }
    }
    
    *appstate = app;
    return SDL_APP_CONTINUE;
}
//=================================================================
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event){
    App* app = (App*)appstate;
    Game* game = app->game;
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && game->endGame.empty()) {
        for(Bavel& btn : game->buttons){
            if (isButtonClicked(btn, mouseX, mouseY) && !btn.revealed) {
                int gy = btn.index[0];
                int gx = btn.index[1];

                if (game->field[gy][gx] == -1) {
                    game->endGame = "lose";
                    break;
                }

                revealCell(gx, gy, game->buttons, game->field, game->mines, game->revealedCells);
                break;
            }
        }
    }else if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_Keycode key = event->key.key;
        if (key == SDLK_SPACE && game->endGame.empty()) {
            for(Bavel& btn : game->buttons){
                if (isButtonClicked(btn, mouseX, mouseY)) {
                    if(btn.revealed == false){
                        game->flags.push_back({btn.rect.x,btn.rect.y,32,32});
                        btn.revealed = true;
                        btn.flag = true;
                        break;
                    }else if(btn.flag == true){
                        btn.revealed = false;
                        btn.flag = false;

                        for (int f = 0; f < game->flags.size(); f++) {
                            if (game->flags[f].rect.x == btn.rect.x && game->flags[f].rect.y == btn.rect.y) {
                                game->flags.erase(game->flags.begin() + f);
                                break;
                            }
                        }
                    }
                }
            }
        }else if(key == SDLK_R){
            game->mines.clear();
            game->revealedCells = 0;
            game->buttons.clear();
            game->flags.clear();
            game->endGame = "";
            calculateNumbers(16, 16, game->field, game);
            for(int i = 0;i<16;i++){
                for(int j = 0;j<16;j++){
                    game->buttons.push_back({{static_cast<float>(19 + (32 * i)),static_cast<float>(144 + (32 * j)),32,32},{i,j},"Btn"});
                }
            }
        }
    }
    return SDL_APP_CONTINUE;
}
//=================================================================
SDL_AppResult SDL_AppIterate(void* appstate){
    App* app = (App*)appstate;
    Game* game = app->game;
    AppState* state = app->state;
    Fonts* fonts = app->fonts;
    
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_SetRenderDrawColor(game->renderer, 191, 191, 191, 255);
    SDL_RenderClear(game->renderer);
    
    game->counter = game->totalSafeCells - game->revealedCells;
    Text timerText = {{403,46},{255,0,0,255},"Text",std::to_string(game->counter)};
    
    drawBevel(game->renderer, state->bavelBackground,true);
    drawBevel(game->renderer, state->bavelMenu, false);
    drawBevel(game->renderer, state->bavelGame, false);
    drawRectangle(game->renderer, state->rectangleScreen);
    drawText(game->renderer,fonts->fontMines, timerText);
    for(Bavel btn : game->buttons){
        if(mouseX >= btn.rect.x && mouseY >= btn.rect.y && mouseX <  btn.rect.x + btn.rect.w && mouseY <  btn.rect.y + btn.rect.h){
            drawBevel(game->renderer, btn, false);
        }else{
            drawBevel(game->renderer, btn, true);
        }
    }
    for(Rectangle rec : game->mines){
        drawMines(game->renderer,fonts->fontButtons, rec);
    }
    for(Image img : game->flags){
        drawImage(game->renderer, game->imageTexture[1], img);
    }
    drawText(game->renderer, fonts->fontText, state->textScreen10);
    
    if (game->revealedCells == game->totalSafeCells) {
        std::cout << "YOU WIN!" << std::endl;
        Bavel bavelEnd = {15,304,520,97};
        Text textEnd = {{143,335},{0,255,0,255},"Text","You Win!"};
        drawBevel(game->renderer, bavelEnd, true);
        drawText(game->renderer, fonts->fontMines, textEnd);
    }else if(game->endGame == "lose"){
        for(int y = 0; y < 16; y++){
            for(int x = 0; x < 16; x++){
                if(game->field[y][x] == -1){
                    int idx = y * 16 + x;
                    const Bavel& b = game->buttons[idx];
                    drawImage(game->renderer, game->imageTexture[0], {b.rect.x, b.rect.y, 32, 32});
                }
            }
        }
        Bavel bavelEnd = {15,304,520,97};
        Text textEnd = {{143,335},{255,0,0,255},"Text","You Lose!"};
        drawBevel(game->renderer, bavelEnd, true);
        drawText(game->renderer, fonts->fontMines, textEnd);
    }
    
    SDL_RenderPresent(game->renderer);
    SDL_Delay(32);
    return SDL_APP_CONTINUE;
}
//=================================================================
void SDL_AppQuit(void* appstate, SDL_AppResult result){
    App* app = (App*)appstate;
    Game* game = app->game;
    Fonts* fonts = app->fonts;
    TTF_CloseFont(fonts->fontText);
    TTF_CloseFont(fonts->fontMines);
    TTF_CloseFont(fonts->fontButtons);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    TTF_Quit();
    SDL_Quit();
    
    delete app->game;
    delete app->state;
    delete app->fonts;
    delete app;
}
//=================================================================
