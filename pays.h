#ifndef PAYS_H
#define PAYS_H

#include <SDL.h>
#include <SDL_ttf.h>

typedef struct {
    char nom[20];
    char activite[100];
    int danger; // 0 ou 1
    float prix;
} Saison;

typedef struct {
    char nom[50];
    Saison saisons[4];
    int nbSaisons;
} PaysActivites;

typedef struct {
    char nom[50];
    char image[100];
    char infos[2000];
    char* animaux[10];
    int nb_animaux;
} Pays;

void afficher_pays(SDL_Renderer* ren, Pays* p);

SDL_Texture* creerTexte(SDL_Renderer* ren, TTF_Font* font, const char* texte, SDL_Color couleur);

#endif
