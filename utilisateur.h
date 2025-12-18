#ifndef UTILISATEUR_H
#define UTILISATEUR_H
#include "pays.h"

typedef struct {
    char nom[50];
    char prenom[50];
    int famille;
    Pays* pays;
    PaysActivites* paysActivites;
    int saisonIndex;
    char saisonChoisie[20];
    int score;
    char badge[20];
    float prix;
    float reduction;
} Utilisateur;

void appliquer_reduction(Utilisateur* u);
void generer_ticket(Utilisateur u);
void calculer_score(Utilisateur* u, Saison s);
void attribuer_badge(Utilisateur* u);


#endif
