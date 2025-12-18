#include "utilisateur.h"
#include <stdio.h>
#include <string.h>

void appliquer_reduction(Utilisateur* u) {
    Saison s = u->paysActivites->saisons[u->saisonIndex];

    // reduction familiale
    float reduction = (u->famille) ? 20.0f : 0.0f;

    // ajouter red si activite dangereuse
    if (s.danger) reduction += 10.0f;

    u->reduction = reduction;
    u->prix = s.prix * (1.0f - reduction / 100.0f);
}

void calculer_score(Utilisateur* u, Saison s) {
    int points = 50;  // base
    if (s.danger == 0) points += 20;  // activite sure
    if (u->famille) points += 30;     // voyage famille
    u->score = points;
}

void attribuer_badge(Utilisateur* u) {
    if (u->score >= 10)
        strcpy(u->badge, "Expert Safari");
    else if (u->score >= 20)
        strcpy(u->badge, "Aventurier");
    else
        strcpy(u->badge, "Explorateur");
}

void generer_ticket(Utilisateur u) {
    Saison s = u.paysActivites->saisons[u.saisonIndex];

    printf("\n========== TICKET SAFARI ==========\n");
    printf("Voyageur      : %s %s\n", u.prenom, u.nom);
    printf("Destination   : %s\n", u.pays->nom);
    printf("Saison        : %s\n", u.saisonChoisie);
    printf("Activite      : %s\n", s.activite);
    if (u.famille) printf("Voyage en famille : OUI\n");
    printf("Prix          : %.2f € (reduction %.1f%%)\n", u.prix, u.reduction);
    printf("Score         : %d points\n", u.score);
    printf("Badge         : %s\n", u.badge);
    printf("===================================\n\n");

    // Sauvegarde dans fichier
    FILE* f = fopen("ticket.txt", "w");
    if(f) {
        fprintf(f, "========== TICKET SAFARI ==========\n");
        fprintf(f, "Voyageur : %s %s\n", u.prenom, u.nom);
        fprintf(f, "Destination : %s\n", u.pays->nom);
        fprintf(f, "Saison : %s\n", u.saisonChoisie);
        fprintf(f, "Activite : %s\n", s.activite);
        fprintf(f, "Prix : %.2f (reduction %.1f%%)\n", u.prix, u.reduction);
        fprintf(f, "Score : %d points\n", u.score);
        fprintf(f, "Badge : %s\n", u.badge);
        fprintf(f, "===================================\n");
        fclose(f);
    }
}
