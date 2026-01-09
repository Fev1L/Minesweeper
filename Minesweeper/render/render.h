//
//  render.h
//  Minesweeper
//
//  Created by Fev1L on 09.01.2026.
//

#pragma once
#include "types.h"

void drawRectangle(SDL_Renderer* renderer,const Rectangle &rct);

bool isButtonClicked(const Bavel& button, float x, float y);

void drawText(SDL_Renderer* renderer,TTF_Font* font, const Text& text);

void drawImage(SDL_Renderer* renderer, SDL_Texture* imageTexture, const Image& img);

void drawTriangle(SDL_Renderer* r, const Triangle& t);

void fillTriangle(SDL_Renderer* r, Triangle t);

void drawBevel(SDL_Renderer* renderer, Bavel bvl, bool pressed);

void drawMines(SDL_Renderer* renderer,TTF_Font* font ,Rectangle btn);
