#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include "pays.h"

typedef struct {
    SDL_Rect zone;
    Pays* pays;
} ZonePays;

//fct affiche toutes les images des pays dans la fenetre SDL
void afficher_map(SDL_Renderer* ren, Pays paysInfo[], int nbPays);

#endif
