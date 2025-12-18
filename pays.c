#include "pays.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>

// fction utilitaire pour creer du texte (declare dans pays.h)
SDL_Texture* creerTexte(SDL_Renderer* ren, TTF_Font* font, const char* texte, SDL_Color couleur) {
    if (!texte || strlen(texte) == 0) return NULL;
    SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, texte, couleur, 420);
    if (!surf) {
        printf("Erreur creation texte: %s\n", TTF_GetError());
        return NULL;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    return tex;
}

// Fonction utilitaire privee pour charger une image en texture
static SDL_Texture* chargerTexture(SDL_Renderer* ren, const char* path) {
    SDL_Surface* surf = IMG_Load(path);
    if (!surf) {
        printf("Erreur chargement image %s: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    return tex;
}

void afficher_pays(SDL_Renderer* ren, Pays* p) {
    // chargement de la police
    TTF_Font* font = TTF_OpenFont("fonts/arialbd.ttf", 26);
    if (!font) {
        printf("Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }

    SDL_Color blanc = {255, 255, 255, 255};

    // Chargement des textures une seule fois
    SDL_Texture* imgPays = chargerTexture(ren, p->image);
    SDL_Texture* txtNom  = creerTexte(ren, font, p->nom, blanc);
    SDL_Texture* txtDesc = creerTexte(ren, font, p->infos, blanc);

    // prechargement des textures des animaux
    SDL_Texture* animauxTex[10] = {NULL};
    for (int i = 0; i < p->nb_animaux; i++) {
        animauxTex[i] = chargerTexture(ren, p->animaux[i]);
    }

    // positions d affichage
    SDL_Rect rectImgPays = {20,   80, 400, 300};   // Image du pays à gauche
    SDL_Rect rectNom     = {450, 120, 420,  50};   // Nom du pays à droite
    SDL_Rect rectDesc    = {450, 180, 420, 350};   // Description longue à droite (plus d'espace)
    SDL_Rect rectAnimal  = {150,  80, 600, 450};   // Animal grand et centré

    // etats de linterface
    int modeAnimaux = 0;           // 0 = vue pays, 1 = diaporama animaux
    int indexAnimal = 0;
    int running = 1;

    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = 0;
                        break;

                    case SDLK_SPACE:
                        if (p->nb_animaux > 0) {
                            modeAnimaux = !modeAnimaux;     // Bascule vue pays ↔ animaux
                            if (modeAnimaux) indexAnimal = 0; // Recommence au premier animal
                        }
                        break;

                    case SDLK_RIGHT:
                        if (modeAnimaux && p->nb_animaux > 1) {
                            indexAnimal = (indexAnimal + 1) % p->nb_animaux;
                        }
                        break;

                    case SDLK_LEFT:
                        if (modeAnimaux && p->nb_animaux > 1) {
                            indexAnimal = (indexAnimal - 1 + p->nb_animaux) % p->nb_animaux;
                        }
                        break;
                }
            }
        }

        // Rendu de la scene
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        if (modeAnimaux && p->nb_animaux > 0) {
            // mode diaporama animaux
            if (animauxTex[indexAnimal]) {
                SDL_RenderCopy(ren, animauxTex[indexAnimal], NULL, &rectAnimal);
            }

            // indicateur en bas
            char indicateur[64];
            snprintf(indicateur, sizeof(indicateur), "Animal %d/%d  —  Appuyez sur ESPACE pour revenir",
                     indexAnimal + 1, p->nb_animaux);
            SDL_Texture* texInd = creerTexte(ren, font, indicateur, blanc);
            if (texInd) {
                SDL_Rect rInd = {20, 550, 800, 40};
                SDL_RenderCopy(ren, texInd, NULL, &rInd);
                SDL_DestroyTexture(texInd);
            }
        }
        else {
            //  Mode vue pays
            if (imgPays) SDL_RenderCopy(ren, imgPays, NULL, &rectImgPays);
            if (txtNom)  SDL_RenderCopy(ren, txtNom,  NULL, &rectNom);
            if (txtDesc) SDL_RenderCopy(ren, txtDesc, NULL, &rectDesc);

            // Message d'aide en bas
            SDL_Texture* texAide = creerTexte(ren, font, "Appuyez sur ESPACE pour voir les animaux", blanc);
            if (texAide) {
                SDL_Rect rAide = {20, 550, 800, 40};
                SDL_RenderCopy(ren, texAide, NULL, &rAide);
                SDL_DestroyTexture(texAide);
            }
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16); // ~60 FPS
    }

    // Nettoyage des ressources
    if (imgPays) SDL_DestroyTexture(imgPays);
    if (txtNom)  SDL_DestroyTexture(txtNom);
    if (txtDesc) SDL_DestroyTexture(txtDesc);
    for (int i = 0; i < p->nb_animaux; i++) {
        if (animauxTex[i]) SDL_DestroyTexture(animauxTex[i]);
    }
    TTF_CloseFont(font);
}
