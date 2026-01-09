//
//  types.h
//  Minesweeper
//
//  Created by Fev1L on 09.01.2026.
//

#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

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

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;

    int field[16][16];
    std::vector<Bavel> buttons;
    std::vector<Rectangle> mines;
    std::vector<Image> flags;
    std::vector<SDL_Texture*> imageTexture;

    std::string endGame = "";
    int totalSafeCells = 0;
    int revealedCells = 0;
    int counter = 0;
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
