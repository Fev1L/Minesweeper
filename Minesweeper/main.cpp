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

int totalSafeCells = 0, revealedCells = 0,counter = 0;

struct Rectangle {
    SDL_FRect rect;
    SDL_Color color;
    float radius;
    std::string label;
};

struct Bavel {
    SDL_FRect rect;
    int index[2];
    std::string label;
    bool revealed = false;
    bool flag = false;
};

struct Text {
    SDL_FRect rect;
    SDL_Color color;
    std::string label;
    std::string textIn;
};

struct Triangle {
    SDL_FPoint p1;
    SDL_FPoint p2;
    SDL_FPoint p3;
    SDL_Color color;
};

struct Image {
    SDL_FRect rect;
    const char* label;
    std::string texture;
};

void drawRectangle(SDL_Renderer* renderer,const Rectangle &rct) {
    SDL_SetRenderDrawColor(renderer, rct.color.r, rct.color.g, rct.color.b, rct.color.a);

    SDL_FRect core = { rct.rect.x + rct.radius, rct.rect.y, rct.rect.w - 2 * rct.radius, rct.rect.h };
    SDL_RenderFillRect(renderer, &core);

    SDL_FRect left = { rct.rect.x, rct.rect.y + rct.radius, rct.radius, rct.rect.h - 2 * rct.radius };
    SDL_FRect right = { rct.rect.x + rct.rect.w - rct.radius, rct.rect.y + rct.radius, rct.radius, rct.rect.h - 2 * rct.radius };
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);

    for (float w = 0; w < rct.radius; ++w) {
        for (float h = 0; h < rct.radius; ++h) {
            if ((w * w + h * h) <= rct.radius * rct.radius) {
                SDL_RenderPoint(renderer, rct.rect.x + rct.radius - w, rct.rect.y + rct.radius - h);
                SDL_RenderPoint(renderer, rct.rect.x + rct.rect.w - rct.radius + w - 1, rct.rect.y + rct.radius - h);
                SDL_RenderPoint(renderer, rct.rect.x + rct.radius - w, rct.rect.y + rct.rect.h - rct.radius + h - 1);
                SDL_RenderPoint(renderer, rct.rect.x + rct.rect.w - rct.radius + w - 1, rct.rect.y + rct.rect.h - rct.radius + h - 1);
            }
        }
    }
}

bool isButtonClicked(const Bavel& button, float x, float y) {
    return (x >= button.rect.x && x <= button.rect.x + button.rect.w &&
            y >= button.rect.y && y <= button.rect.y + button.rect.h);
}

void drawText(SDL_Renderer* renderer,TTF_Font* font, const Text& text){
    float textW, textH;
    SDL_Color color = {text.color.r, text.color.g, text.color.b, 255};
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text.textIn.c_str(), 0, color, 375);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    SDL_GetTextureSize(texture, &textW, &textH);
    SDL_FRect dst = { text.rect.x, text.rect.y, textW, textH };
    SDL_RenderTexture(renderer, texture, nullptr, &dst);
}

void drawImage(SDL_Renderer* renderer, SDL_Texture* imageTexture, const Image& img){
    SDL_FRect imageRect = {img.rect.x, img.rect.y, img.rect.h, img.rect.w};
    SDL_RenderTexture(renderer, imageTexture, nullptr, &imageRect);
}

void drawTriangle(SDL_Renderer* r, const Triangle& t)
{
    SDL_SetRenderDrawColor(r, t.color.r, t.color.g, t.color.b, t.color.a);

    SDL_RenderLine(r, t.p1.x, t.p1.y, t.p2.x, t.p2.y);
    SDL_RenderLine(r, t.p2.x, t.p2.y, t.p3.x, t.p3.y);
    SDL_RenderLine(r, t.p3.x, t.p3.y, t.p1.x, t.p1.y);
}

void fillTriangle(SDL_Renderer* r, Triangle t)
{
    SDL_SetRenderDrawColor(r, t.color.r, t.color.g, t.color.b, t.color.a);

    if (t.p2.y < t.p1.y) std::swap(t.p1, t.p2);
    if (t.p3.y < t.p1.y) std::swap(t.p1, t.p3);
    if (t.p3.y < t.p2.y) std::swap(t.p2, t.p3);

    auto interp = [&](SDL_FPoint a, SDL_FPoint b){
        std::vector<float> vals;
        int dy = int(b.y - a.y);
        if (dy < 1) return vals;
        vals.reserve(dy);
        for (int i = 0; i <= dy; i++) {
            float tVal = float(i) / dy;
            vals.push_back(a.x + (b.x - a.x) * tVal);
        }
        return vals;
    };

    auto x12 = interp(t.p1, t.p2);
    auto x23 = interp(t.p2, t.p3);
    auto x13 = interp(t.p1, t.p3);

    x12.insert(x12.end(), x23.begin(), x23.end());

    int total = int(x13.size());
    for (int i = 0; i < total; i++) {
        float xL = x12[i];
        float xR = x13[i];
        if (xL > xR) std::swap(xL, xR);
        float y = t.p1.y + i;
        SDL_RenderLine(r, xL, y, xR, y);
    }
}

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

void calculateNumbers(int width, int height, int field[16][16]) {
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
    totalSafeCells = 16 * 16 - mineCount;
    mineCount = 0;
}

void drawBevel(SDL_Renderer* renderer, Bavel bvl, bool pressed)
{
    SDL_Color light = {255, 255, 255, 255};
    SDL_Color dark  = {129, 129, 129, 255};

    SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
    SDL_RenderFillRect(renderer, &bvl.rect);

    SDL_Color topLeft  = pressed ? light : dark;
    SDL_Color bottomRight = pressed ? dark : light;

    Rectangle rectangleSides[] = {
        {{bvl.rect.x, bvl.rect.y, 4, bvl.rect.h}, topLeft, 0.0f, "left"},
        {{bvl.rect.x, bvl.rect.y, bvl.rect.w, 4}, topLeft, 0.0f, "top"},
        {{bvl.rect.x, bvl.rect.y + bvl.rect.h - 4, bvl.rect.w, 4}, bottomRight, 0.0f, "bottom"},
        {{bvl.rect.x + bvl.rect.w - 4, bvl.rect.y, 4, bvl.rect.h}, bottomRight, 0.0f, "right"}
    };

    Triangle tri[] = {
        {
            {bvl.rect.x, bvl.rect.y + bvl.rect.h - 4},
            {bvl.rect.x + 4, bvl.rect.y + bvl.rect.h - 4},
            {bvl.rect.x, bvl.rect.y + bvl.rect.h},
            bottomRight
        },
        {
            {bvl.rect.x + bvl.rect.w - 4, bvl.rect.y},
            {bvl.rect.x + bvl.rect.w, bvl.rect.y},
            {bvl.rect.x + bvl.rect.w - 4, bvl.rect.y + 4},
            topLeft
        }
    };

    for (Rectangle rct : rectangleSides)
        drawRectangle(renderer, rct);

    for (Triangle t : tri) {
        drawTriangle(renderer, t);
        fillTriangle(renderer, t);
    }
}

void drawMines(SDL_Renderer* renderer,TTF_Font* font ,Rectangle btn){
    SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
    SDL_FRect core = {btn.rect.x,btn.rect.y,32,32};
    SDL_RenderFillRect(renderer, &core);
    SDL_Color colors[] = {
        {11,36,251,255},
        {14,122,17,255},
        {252,13,27,255},
        {2,11,121,255},
        {133,33,25,255},
        {39,133,134,255},
        {0,0,0,255},
        {135,135,135,255}
    };
    Text txt;
    if(stoi(btn.label) > 0 && stoi(btn.label) < 9){
        txt = {{btn.rect.x + 6,btn.rect.y + 6},colors[stoi(btn.label)-1],"Text",btn.label};
    }else if(btn.label == "-1"){
        txt = {{btn.rect.x + 6,btn.rect.y + 6},{0,0,0,255},"Text","M"};
    }
    
    drawText(renderer, font, txt);
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

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;

    int field[16][16];
    std::vector<Bavel> buttons;
    std::vector<Rectangle> mines;
    std::vector<Image> flags;
    std::vector<SDL_Texture*> imageTexture;

    std::string endGame = "";
    bool running;
};

struct AppState {
    Rectangle rectangleScreen;
    Text textScreen10;
    Bavel bavelBackground;
    Bavel bavelMenu;
    Bavel bavelGame;
};

struct Fonts {
    TTF_Font* fontText;
    TTF_Font* fontMines;
    TTF_Font* fontButtons;
};

struct App {
    Game* game;
    AppState* state;
    Fonts* fonts;
};

//=================================================================
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]){
    srand(time(0));
    App* app = new App();
    app->game = new Game();
    app->state = new AppState();
    app->fonts = new Fonts();
    
    calculateNumbers(16, 16,app->game->field);
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
    
    app->game->running = true;
    *appstate = app;
    return SDL_APP_CONTINUE;
}
//=================================================================
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event){
    App* app = (App*)appstate;
    Game* game = app->game;
    bool running = true;
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

                revealCell(gx, gy, game->buttons, game->field, game->mines, revealedCells);
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
            revealedCells = 0;
            game->buttons.clear();
            game->flags.clear();
            game->endGame = "";
            calculateNumbers(16, 16, game->field);
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
    
    counter = totalSafeCells - revealedCells;
    Text timerText = {{403,46},{255,0,0,255},"Text",std::to_string(counter)};
    
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
    
    if (revealedCells == totalSafeCells) {
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
