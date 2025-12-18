#include "jeu.h"
#include <SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

#define LARGEUR 900
#define HAUTEUR 600

int collision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void afficher_instructions(SDL_Renderer* ren, TTF_Font* font) {
    SDL_Color blanc = {255, 255, 255, 255};
    const char* texte =
        "MINI-JEU : PROTECTEUR DE LA NATURE\n\n"
        "OBJECTIF : Sauvez les animaux et evitez les dangers\n"
        "COMMANDES : Fleches = Deplacer le carre vert, ESC = Quitter\n"
        "REGLES : +20 pts = attraper animal, +10 pts = dechet, -30 pts = toucher danger\n"
        "DUREE : 30 secondes\n\n"
        "Appuyez sur ENTREE pour commencer";

    SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(font, texte, blanc, 800);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect dst = {50, 50, surf->w, surf->h};

    SDL_SetRenderDrawColor(ren, 0, 100, 0, 255);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_RenderPresent(ren);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);

    SDL_Event e;
    int wait = 1;
    while(wait) {
        SDL_WaitEvent(&e);
        if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) wait = 0;
        if(e.type == SDL_QUIT) exit(0);
    }
}

int lancer_mini_jeu(SDL_Renderer* ren) {
    srand(time(NULL));
    int score = 0;
    Uint32 debut = SDL_GetTicks();
    SDL_Event e;
    int running = 1;

    SDL_Rect joueur = {400, 300, 40, 40};
    ObjetJeu animal = {{rand()%850, rand()%550, 30, 30}, 1};
    ObjetJeu dechet = {{rand()%850, rand()%550, 25, 25}, 1};
    ObjetJeu danger = {{rand()%850, rand()%550, 35, 35}, 1};

    while(running) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) return score;
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return score;
        }

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if(keys[SDL_SCANCODE_UP]) joueur.y -= 5;
        if(keys[SDL_SCANCODE_DOWN]) joueur.y += 5;
        if(keys[SDL_SCANCODE_LEFT]) joueur.x -= 5;
        if(keys[SDL_SCANCODE_RIGHT]) joueur.x += 5;

        if(animal.actif && collision(joueur, animal.rect)) { score += 20; animal.actif = 0; }
        if(dechet.actif && collision(joueur, dechet.rect)) { score += 10; dechet.actif = 0; }
        if(danger.actif && collision(joueur, danger.rect)) { score -= 30; danger.actif = 0; }

        if(SDL_GetTicks() - debut > 30000) running = 0;

        // Dessin
        SDL_SetRenderDrawColor(ren, 180, 220, 180, 255);
        SDL_RenderClear(ren);

        // Joueur (carre vert)
        SDL_SetRenderDrawColor(ren, 0, 200, 0, 255);
        SDL_RenderFillRect(ren, &joueur);

        if(animal.actif) { SDL_SetRenderDrawColor(ren, 200, 150, 50, 255); SDL_RenderFillRect(ren, &animal.rect); }
        if(dechet.actif) { SDL_SetRenderDrawColor(ren, 100, 100, 100, 255); SDL_RenderFillRect(ren, &dechet.rect); }
        if(danger.actif) { SDL_SetRenderDrawColor(ren, 200, 50, 50, 255); SDL_RenderFillRect(ren, &danger.rect); }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    return score;
}
