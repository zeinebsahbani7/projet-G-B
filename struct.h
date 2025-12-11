
#ifndef STRUCT_H
#define STRUCT_H

#define NB_SALLES 10

typedef struct Salle {
    char nom[50];
    int capacite;
    float tarif_horaire;
    char equipements[5][50];
} Salle;

typedef struct Reservation {
    int id;
    char client[50];
    char nom_salle[50];
    char date[11];
    char debut[6];
    char fin[6];
    int nb_personnes;
    float tarif;
    char statut[20];
} Reservation;

typedef struct NoeudReservation {
    Reservation res;
    struct NoeudReservation *suiv;
    struct NoeudReservation *prec;
} NoeudReservation;

typedef struct Classement {
    char nom[50];
    int reservations;
} Classement;
typedef struct { char nom[50]; int nb_reservations; } ClientStats;
#endif /* STRUCT_H */
