#ifndef MENU_H
#define MENU_H
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct {
    int nbItems;
    const char** items;
    TTF_Font* font;
    SDL_Color color;
    int x, y, w, h;
    int selected;
} Menu;

void menu_creer(SDL_Renderer* ren, Menu* menu, const char* items[], int nbItems,
                TTF_Font* font, SDL_Color color, int x, int y, int w, int h);
int menu_selectionner(SDL_Event* e, Menu* menu);
void menu_afficher(SDL_Renderer* ren, Menu* menu);

#endif
