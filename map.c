#include "map.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>

static SDL_Texture* chargerTexture(SDL_Renderer* ren, const char* path) {
    SDL_Surface* s = IMG_Load(path);
    if (!s) return NULL;
    SDL_Texture* t = SDL_CreateTextureFromSurface(ren, s);
    SDL_FreeSurface(s);
    return t;
}

void afficher_map(SDL_Renderer* ren, Pays paysInfo[], int nbPays) {
    SDL_RenderClear(ren);

    int x = 0, y = 0;
    int w = 400, h = 300; // largeur et hauteur des images

    for(int i = 0; i < nbPays; i++) {
        SDL_Surface* surf = IMG_Load(paysInfo[i].image);
        if(!surf) {
            printf("Erreur chargement image %s: %s\n", paysInfo[i].image, IMG_GetError());
            continue;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
        SDL_FreeSurface(surf);

        SDL_Rect dest = { x, y, w, h };
        SDL_RenderCopy(ren, tex, NULL, &dest);
        SDL_DestroyTexture(tex);

        x += w;
        if(x + w > 900) { // si depasse largeur fenetre, passer a la ligne suivante
            x = 0;
            y += h;
        }
    }

    SDL_RenderPresent(ren);

    // laisser la fenetre ouverte jusqua ce que utilisateur ferme
    int running = 1;
    SDL_Event e;
    while(running) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
        }
        SDL_Delay(16);
    }
}
