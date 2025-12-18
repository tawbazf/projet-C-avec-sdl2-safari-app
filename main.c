#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "menu.h"
#include "pays.h"
#include "utilisateur.h"
#include <SDL_syswm.h>
#include "jeu.h"

// fct pour changer la couleur de la console
void setConsoleColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
//nom est valide
int est_nom_valide(const char* s) {
    if (strlen(s) == 0) return 0;

    for (int i = 0; s[i]; i++) {
        if (!isalpha((unsigned char)s[i]))
            return 0;
    }
    return 1;
}


int main() {
    // console windows
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    //initialisation sdl / ttf / mixer
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }
    if(TTF_Init() != 0) {
        printf("Erreur TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur SDL_mixer: %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    Mix_Music* musique = Mix_LoadMUS("assets/music.mp3");
    if(musique) Mix_PlayMusic(musique, -1);

    Mix_Chunk* sosSound = Mix_LoadWAV("assets/sos.wav");
    if(!sosSound) printf("Erreur chargement SOS: %s\n", Mix_GetError());

    SDL_Window* win = SDL_CreateWindow("Safari", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, SDL_WINDOW_SHOWN);
   SDL_Renderer* ren = SDL_CreateRenderer(
    win,
    -1,
    SDL_RENDERER_ACCELERATED |
    SDL_RENDERER_PRESENTVSYNC |
    SDL_RENDERER_TARGETTEXTURE
);
 /*SDL_RendererInfo info;
SDL_GetRendererInfo(ren, &info);

if (info.flags & SDL_RENDERER_TARGETTEXTURE)
    printf("Renderer supporte TARGETTEXTURE\n");
else
    printf(" Renderer NE supporte PAS TARGETTEXTURE\n");*/


    //initialisation des pays
    char* animauxMaroc[] = {"assets/elephant.jpg","assets/lion.jpg","assets/giraffe.jpg"};
    char* animauxKenya[] = {"assets/elephant.jpg","assets/lion.jpg","assets/giraffe.jpg"};
    char* animauxEgypte[] = {"assets/camel.jpg","assets/crocodile.jpg"};
    char* animauxSud[] = {"assets/lion.jpg","assets/elephant.jpg","assets/zebra.jpg"};

    Pays paysInfo[4] = {
        {"Maroc","assets/maroc.jpg","Le Maroc...",{animauxMaroc[0],animauxMaroc[1],animauxMaroc[2]},3},
        {"Kenya","assets/kenya.jpg","Le Kenya...",{animauxKenya[0],animauxKenya[1],animauxKenya[2]},3},
        {"Egypte","assets/egypte.jpg","L'Egypte...",{animauxEgypte[0],animauxEgypte[1]},2},
        {"Afrique du Sud","assets/afrique.jpg","Afrique du Sud...",{animauxSud[0],animauxSud[1],animauxSud[2]},3}
    };

   PaysActivites paysActivites[] = {
    {"Maroc", {
        {"Printemps", "Visite des souks", 0, 1000.0f},
        {"Ete", "Plage a Essaouira", 1, 1200.0f},
        {"Automne", "Trekking Atlas", 1, 1500.0f},
        {"Hiver", "Exploration Sahara", 0, 1300.0f}
    }, 4},
    {"Kenya", {
        {"Printemps", "Safari Maasai Mara", 1, 2500.0f},
        {"Ete", "Observation oiseaux", 0, 2000.0f},
        {"Automne", "Trek montagne", 1, 2700.0f},
        {"Hiver", "Safari Tsavo", 1, 3000.0f}
    }, 4},
    {"Egypte", {
        {"Printemps", "Croisiere Nil", 0, 1800.0f},
        {"Ete", "Plongee Mer Rouge", 1, 2200.0f},
        {"Automne", "Visite pyramides", 0, 2000.0f},
        {"Hiver", "Exploration desert", 1, 2500.0f}
    }, 4},
    {"Afrique du Sud", {
        {"Printemps", "Safari Kruger", 1, 2700.0f},
        {"Ete", "Plage et surf", 0, 2300.0f},
        {"Automne", "Randonnee vignobles", 0, 2100.0f},
        {"Hiver", "Sports extremes", 1, 3000.0f}
    }, 4}
};


       // affichage de  la map
    SDL_Texture* mapTex = IMG_LoadTexture(ren, "assets/africamap.png");
    if (!mapTex) {
        printf("Erreur chargement map: %s\n", IMG_GetError());
    } else {
SDL_Rect zones[4] = {
    {70, 40, 120, 80},    // maroc
   {580, 250, 120, 130}, // kenya
    {520, 50, 130, 90},   // egypte
    {600, 400, 160, 120}  // afrique du sud
};


        const char* nomsPaysMap[4] = {"Maroc","Kenya","Egypte","Afrique du Sud"};

        TTF_Font* fontMap = TTF_OpenFont("fonts/arialbd.ttf", 28);
        TTF_Font* fontInfo = TTF_OpenFont("fonts/arialbd.ttf", 32);
        SDL_Color blanc = {255, 255, 255, 255};
        SDL_Color gris = {200, 200, 200, 255};


        char message[100] = "Cliquez sur un pays pour commencer...";
        Uint32 messageTime = 0; // timestamp du dernier clic
        const Uint32 fadeDuration = 4000; // message visible 4 secondes

        int runningMap = 1;
        SDL_Event eMap;

        // Message au lancement
        printf("\nLa carte d Afrique est affichee. Cliquez sur les pays !\n");
        printf("Fermez la fenetre pour continuer le programme.\n\n");
        fflush(stdout);

        while(runningMap) {
            Uint32 currentTime = SDL_GetTicks();

            while(SDL_PollEvent(&eMap)) {
                if(eMap.type == SDL_QUIT) runningMap = 0;
                else if(eMap.type == SDL_MOUSEBUTTONDOWN && eMap.button.button == SDL_BUTTON_LEFT) {
                    int mx = eMap.button.x, my = eMap.button.y;
                    int trouve = 0;
                    for(int i = 0; i < 4; i++) {
                        if(mx >= zones[i].x && mx <= zones[i].x + zones[i].w &&
                           my >= zones[i].y && my <= zones[i].y + zones[i].h) {
                            sprintf(message, "Dernier pays clique : %s", nomsPaysMap[i]);
                            messageTime = currentTime;
                            trouve = 1;
                            break;
                        }
                    }
                    if(!trouve) {
                        strcpy(message, "Cliquez sur une zone rouge !");
                        messageTime = currentTime;
                    }
                }
            }

            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            SDL_RenderClear(ren);
            SDL_RenderCopy(ren, mapTex, NULL, NULL);

            // dessin des rectangles rouges
            for(int i = 0; i < 4; i++) {
                SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
                SDL_RenderDrawRect(ren, &zones[i]);

                SDL_Surface* surfNom = TTF_RenderText_Solid(fontMap, nomsPaysMap[i], blanc);
                if(surfNom) {
                    SDL_Texture* texNom = SDL_CreateTextureFromSurface(ren, surfNom);
                    SDL_Rect rNom = {zones[i].x + (zones[i].w - surfNom->w)/2, zones[i].y - 40, surfNom->w, surfNom->h};
                    SDL_RenderCopy(ren, texNom, NULL, &rNom);
                    SDL_FreeSurface(surfNom);
                    SDL_DestroyTexture(texNom);
                }
            }

            // affichage de mssg en bas
            if (currentTime - messageTime < fadeDuration) {

                SDL_Rect fondMsg = {20, 500, 860, 80};
                SDL_SetRenderDrawColor(ren, 0, 0, 0, 180);
                SDL_RenderFillRect(ren, &fondMsg);
                SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
                SDL_RenderDrawRect(ren, &fondMsg);
                SDL_Surface* surfMsg = TTF_RenderText_Solid(fontInfo, message, blanc);
                if(surfMsg) {
                    SDL_Texture* texMsg = SDL_CreateTextureFromSurface(ren, surfMsg);
                    SDL_Rect rMsg = {450 - surfMsg->w / 2, 530, surfMsg->w, surfMsg->h};
                    SDL_RenderCopy(ren, texMsg, NULL, &rMsg);
                    SDL_FreeSurface(surfMsg);
                    SDL_DestroyTexture(texMsg);
                }
            }

            SDL_RenderPresent(ren);
            SDL_Delay(16);
        }

        TTF_CloseFont(fontMap);
        TTF_CloseFont(fontInfo);
        SDL_DestroyTexture(mapTex);
    }

    //saisie de l utilisateur
 Utilisateur u;
char fam;

do {
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Nom: "); fflush(stdout);
    scanf("%49s", u.nom);

    if (!est_nom_valide(u.nom)) {
        setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf(" Nom invalide. Lettres uniquement.\n");
    }

} while (!est_nom_valide(u.nom));

do {
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Prenom: "); fflush(stdout);
    scanf("%49s", u.prenom);

    if (!est_nom_valide(u.prenom)) {
        setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf(" Prenom invalide. Lettres uniquement.\n");
    }

} while (!est_nom_valide(u.prenom));

do {
    setConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("Famille ? (o/n): "); fflush(stdout);
    scanf(" %c", &fam);

    fam = tolower(fam);

    if (fam != 'o' && fam != 'n') {
        setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("Tapez 'o' pour oui ou 'n' pour non.\n");
    }

} while (fam != 'o' && fam != 'n');

u.famille = (fam == 'o');

do {
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf("Choisir une saison :\n");

    setConsoleColor(FOREGROUND_GREEN);
    printf("1 : Printemps\n");

    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN);
    printf("2 : Ete\n");

    setConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE);
    printf("3 : Automne\n");

    setConsoleColor(FOREGROUND_BLUE);
    printf("4 : Hiver\n");

    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Votre choix : "); fflush(stdout);

    if (scanf("%d", &u.saisonIndex) != 1) {
        setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf(" Entree invalide. Entrez un nombre.\n");

        while (getchar() != '\n'); // vider buffer
        u.saisonIndex = -1;
    } else {
        u.saisonIndex--; // 1–4 → 0–3
    }

} while (u.saisonIndex < 0 || u.saisonIndex > 3);


setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
printf("\n Utilisateur valide : %s %s\n", u.nom, u.prenom);

    // menu choix des pays
    Menu menuPays;
    TTF_Font* font = TTF_OpenFont("fonts/arialbd.ttf",26);
    SDL_Color blanc = {255,255,255};
    const char* nomsPays[] = {"Maroc","Kenya","Egypte","Afrique du Sud"};
    menu_creer(ren,&menuPays,nomsPays,4,font,blanc,50,100,300,50);

    int choixPays = -1;
    SDL_Event e;
    int runningMenu = 1;

    printf("\nChoisissez maintenant votre pays dans la fenetre SDL qui va reapparaitre...\n");
    fflush(stdout);

    // Ramener la fenetre au premier plan
    SDL_RaiseWindow(win);
    SDL_RestoreWindow(win);
    SDL_Delay(300);

    while(runningMenu){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT) { runningMenu = 0; choixPays = 0; }
            int sel = menu_selectionner(&e, &menuPays);
            if(sel >= 0){
                choixPays = sel;
                runningMenu = 0;
            }
        }
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);
        menu_afficher(ren, &menuPays);
        SDL_RenderPresent(ren);
    }

    u.pays = &paysInfo[choixPays];
    u.paysActivites = &paysActivites[choixPays];

    strcpy(u.saisonChoisie, u.paysActivites->saisons[u.saisonIndex].nom);
    Saison s = u.paysActivites->saisons[u.saisonIndex];

    printf("\n=== VOTRE VOYAGE ===\n");
    printf("Destination : %s\n", u.pays->nom);
    printf("Saison      : %s\n", u.saisonChoisie);
    printf("Activite    : %s\n", s.activite);

    if(s.danger){
        printf("!!! DANGER DETECTE !!! Activite en equipe obligatoire\n");
        Mix_PlayChannel(-1, sosSound, 0);
    }

    appliquer_reduction(&u);
// charger la font minijeu
TTF_Font* fontJeu = TTF_OpenFont("fonts/arialbd.ttf", 28);
if(!fontJeu) printf("Erreur ouverture font jeu: %s\n", TTF_GetError());
else afficher_instructions(ren, fontJeu);  // affiche les instructions

int scoreNature = lancer_mini_jeu(ren);       // lance le minijeu
u.score += scoreNature;

printf("\n Score Nature obtenu : %d pts\n", scoreNature);
u.score += scoreNature;
attribuer_badge(&u);
generer_ticket(u);

    // affichage de final pays choisi
    afficher_pays(ren, u.pays);
    SDL_Delay(5000);
    //pic souvenir

SDL_Texture* mapTexSouvenir = IMG_LoadTexture(ren, "assets/africamap.png");
SDL_Texture* photoPays = IMG_LoadTexture(ren, u.pays->image);

if (!mapTexSouvenir || !photoPays) {
    printf("erreur chargement images souvenir: %s\n", IMG_GetError());
} else {

    SDL_Texture* target = SDL_CreateTexture(
        ren,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_TARGET,
        900, 600
    );

    if (!target) {
        printf("Impossible de creer la texture cible\n");
    } else {

        SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(ren, target);

        // Background noiiiire
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // dessiner map
        SDL_RenderCopy(ren, mapTexSouvenir, NULL, NULL);

        // Country zones (tweaked for visual correctness)
        SDL_Rect zones[4] = {
            {70, 50, 120, 80},    // Maroc
             {580, 250, 120, 130}, // Kenya
            {520, 50, 130, 90},   // Egypte
            {600, 400, 160, 120}  // Afrique du Sud
        };

        // cadre dore pour le pays selecte
        for (int i = 0; i < 6; i++) {
            SDL_SetRenderDrawColor(ren, 255, 215, 0, 255 - i*30); // fading effect
            SDL_Rect r = {
                zones[choixPays].x - i,
                zones[choixPays].y - i,
                zones[choixPays].w + 2*i,
                zones[choixPays].h + 2*i
            };
            SDL_RenderDrawRect(ren, &r);
        }

        // photo de pays avec cadre
        SDL_Rect dstPhoto = {560, 30, 300, 200};
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawRect(ren, &dstPhoto);
        SDL_RenderCopy(ren, photoPays, NULL, &dstPhoto);

        // Texte
        TTF_Font* font = TTF_OpenFont("fonts/arialbd.ttf", 32);
        if (font) {
            SDL_Color noir = {0, 0, 0, 255};
            SDL_Color blanc = {255, 255, 255, 255};
            char txt[200];
            sprintf(txt, "Voyage de %s %s en %s",
                    u.prenom, u.nom, u.pays->nom);


            SDL_Surface* surf = TTF_RenderText_Blended(font, txt, noir);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
            SDL_Rect dst = {450 - surf->w / 2 + 2, 450 + 2, surf->w, surf->h};
            SDL_RenderCopy(ren, tex, NULL, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);

            // Main texte
            surf = TTF_RenderText_Blended(font, txt, blanc);
            tex = SDL_CreateTextureFromSurface(ren, surf);
            dst.x -= 2;
            dst.y -= 2;
            SDL_RenderCopy(ren, tex, NULL, &dst);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);

            TTF_CloseFont(font);
        }

        // Sauv  PNG
        SDL_Surface* save = SDL_CreateRGBSurfaceWithFormat(0, 900, 600, 32, SDL_PIXELFORMAT_ARGB8888);
        SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_ARGB8888, save->pixels, save->pitch);
        IMG_SavePNG(save, "souvenir.png");
        SDL_FreeSurface(save);

        printf("\nPHOTO SOUVENIR CREE : souvenir.png\n");

        // Showfinal
        SDL_SetRenderTarget(ren, NULL);
        SDL_RenderCopy(ren, target, NULL, NULL);
        SDL_RenderPresent(ren);

        SDL_DestroyTexture(target);
    }
}

SDL_DestroyTexture(mapTexSouvenir);
SDL_DestroyTexture(photoPays);
// BILLET SAFARI DELUXE
SDL_SetRenderTarget(ren, NULL);
SDL_SetRenderDrawColor(ren, 240, 220, 180, 255);
SDL_RenderClear(ren);


SDL_Texture* ticketTex = IMG_LoadTexture(ren, "assets/ticket_fond.jpeg");
if(ticketTex) {
    SDL_RenderCopy(ren, ticketTex, NULL, NULL);
    SDL_DestroyTexture(ticketTex);
} else {
    printf(" Image ticket_fond.png non trouvee, fond beige utilisee.\n");
}


TTF_Font* fontTitre = TTF_OpenFont("fonts/arialbd.ttf", 48);
TTF_Font* fontGrand  = TTF_OpenFont("fonts/arialbd.ttf", 22);
SDL_Color noir = {20, 20, 20, 255};
SDL_Color orColor = {200, 150, 0, 255};


void drawTextCenter(SDL_Renderer* r, const char* text, TTF_Font* f, SDL_Color color, int y){
    SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(f, text, color, 800);
    if(surf){
        SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
        SDL_Rect dst = {450 - surf->w / 2, y, surf->w, surf->h};
        SDL_RenderCopy(r, tex, NULL, &dst);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }
}


int numLines = 8; // number of ticket lines
int spacing = 50;
int totalHeight = numLines * spacing;
int startY = 300 - totalHeight / 2;

int y = startY; // position verticale de départ

drawTextCenter(ren, "BILLET SAFARI DELUXE", fontTitre, orColor, y);
y += spacing;

char buf[256];

sprintf(buf, "Voyageur      : %s %s", u.prenom, u.nom);
drawTextCenter(ren, buf, fontGrand, noir, y);
y += spacing;

sprintf(buf, "Destination   : %s", u.pays->nom);
drawTextCenter(ren, buf, fontGrand, noir, y);
y += spacing;

sprintf(buf, "Saison        : %s", u.saisonChoisie);
drawTextCenter(ren, buf, fontGrand, noir, y);
y += spacing;

sprintf(buf, "Activite      : %s", u.paysActivites->saisons[u.saisonIndex].activite);
drawTextCenter(ren, buf, fontGrand, noir, y);
y += spacing;

sprintf(buf, "Prix          : %.2f a (reduction %.1f%%)", u.prix, u.reduction);
drawTextCenter(ren, buf, fontGrand, noir, y);
y += spacing;

sprintf(buf, "Score         : %d points", u.score);
drawTextCenter(ren, buf, fontGrand, noir, y);
y += spacing;

sprintf(buf, "Badge         : %s", u.badge[0]? u.badge : "Aucun");
drawTextCenter(ren, buf, fontGrand, noir, y);
y += spacing;

sprintf(buf, "Score Nature  : %d pts", scoreNature);
drawTextCenter(ren, buf, fontGrand, noir, y);
y += spacing;

SDL_RenderPresent(ren);
SDL_Event eTicket;
int ticketRunning = 1;

while(ticketRunning) {
    while(SDL_PollEvent(&eTicket)) {
        if(eTicket.type == SDL_QUIT) {
            ticketRunning = 0;
        }
        else if(eTicket.type == SDL_KEYDOWN) {
            ticketRunning = 0;
        }
        else if(eTicket.type == SDL_MOUSEBUTTONDOWN) {
            if(eTicket.button.button == SDL_BUTTON_LEFT) {
                // sauvgarder PNG quand cliquer
                SDL_Surface* save = SDL_CreateRGBSurfaceWithFormat(
                    0, 900, 600, 32, SDL_PIXELFORMAT_ARGB8888
                );
                if(save) {
                    SDL_RenderReadPixels(
                        ren, NULL,
                        SDL_PIXELFORMAT_ARGB8888,
                        save->pixels,
                        save->pitch
                    );

                    // Sauvgarder avc un nom unique
                    char filename[128];
                    sprintf(filename, "ticket_%lu.png", SDL_GetTicks());
                    if(IMG_SavePNG(save, filename) == 0)
                        printf(" Ticket sauvegarde en PNG : %s\n", filename);
                    else
                        printf(" Erreur lors de la sauvegarde du ticket\n");

                    SDL_FreeSurface(save);
                }
            }
        }
    }
    SDL_Delay(16);
}


// nettoyer les fonts
TTF_CloseFont(fontTitre);
TTF_CloseFont(fontGrand);


    // liberation des  ressources
    if(musique) Mix_FreeMusic(musique);
    if(sosSound) Mix_FreeChunk(sosSound);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_CloseFont(font);
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
