#ifndef JEU_H
#define JEU_H

#include <SDL.h>
#include <SDL_ttf.h>

typedef struct {
    SDL_Rect rect;
    int actif;
} ObjetJeu;

int lancer_mini_jeu(SDL_Renderer* ren);
void afficher_instructions(SDL_Renderer* ren, TTF_Font* font);

#endif
