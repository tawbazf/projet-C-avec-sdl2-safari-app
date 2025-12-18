#include "menu.h"
#include <SDL.h>
#include <SDL_ttf.h>

void menu_creer(SDL_Renderer* ren, Menu* menu, const char* items[], int nbItems,
                TTF_Font* font, SDL_Color color, int x, int y, int w, int h) {
    menu->nbItems = nbItems;
    menu->items = items;
    menu->font = font;
    menu->color = color;
    menu->x = x;
    menu->y = y;
    menu->w = w;
    menu->h = h;
    menu->selected = 0;
}

int menu_selectionner(SDL_Event* e, Menu* menu) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:
                menu->selected = (menu->selected - 1 + menu->nbItems) % menu->nbItems;
                return -1;
            case SDLK_DOWN:
                menu->selected = (menu->selected + 1) % menu->nbItems;
                return -1;
            case SDLK_RETURN:
                return menu->selected;
            default:
                break;
        }
    }
    return -1;
}

void menu_afficher(SDL_Renderer* ren, Menu* menu) {
    for (int i = 0; i < menu->nbItems; i++) {
        SDL_Surface* surf = TTF_RenderText_Solid(menu->font, menu->items[i], menu->color);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
        SDL_Rect rect = {menu->x, menu->y + i * (menu->h + 10), surf->w, surf->h};
        SDL_RenderCopy(ren, tex, NULL, &rect);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);

        if (i == menu->selected) {
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            SDL_RenderDrawRect(ren, &rect);
        }
    }
}
