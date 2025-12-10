#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "struct.h"


#ifndef _WIN32
#define stricmp strcasecmp
#endif


#define BLUE   "\033[1;34m"
#define GREEN  "\033[1;32m"
#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"
#define RESET  "\033[0m"


Salle salles[NB_SALLES];
int nb_salles = 4;


NoeudReservation *tete_reservations = NULL;
NoeudReservation *queue_reservations = NULL;


/* messages/ verifier */
void afficher_erreur(const char *message) { printf(RED "✖ Erreur : %s\n" RESET, message); }
void afficher_succes(const char *message){ printf(GREEN "✔ %s\n" RESET, message); }

/* chevauchement/verifier */
int chevauchement(const char *d1,const char *f1,const char *d2,const char *f2){
    int h1,m1,h2,m2,a1,b1,a2,b2;
    if (sscanf(d1,"%d:%d",&h1,&m1)!=2 || sscanf(f1,"%d:%d",&a1,&b1)!=2) return 0;
    if (sscanf(d2,"%d:%d",&h2,&m2)!=2 || sscanf(f2,"%d:%d",&a2,&b2)!=2) return 0;
    int debut1=h1*60+m1, fin1=a1*60+b1, debut2=h2*60+m2, fin2=a2*60+b2;
    return (debut1<fin2 && debut2<fin1);
}

/* disponibilite /verifier */
int salle_disponible(const Reservation *r, NoeudReservation *tete){
    for(NoeudReservation *p=tete;p;p=p->suiv)
        if(strcmp(p->res.nom_salle,r->nom_salle)==0 && strcmp(p->res.date,r->date)==0
           && strcmp(p->res.statut,"confirmée")==0)
            if(chevauchement(r->debut,r->fin,p->res.debut,p->res.fin)) return 0;
    return 1;
}

/* capacite/verifier */
int capacite_ok(const Salle *salle,int pres){
    if(!salle){afficher_erreur("Salle inconnue.");return 0;}
    if(pres>salle->capacite){char buf[64];sprintf(buf,"Capacité max %d",salle->capacite);afficher_erreur(buf);return 0;}
    return 1;
}

/* tarif et duree/verifier */
float calcul_tarif(const Salle *s,const char *d,const char *f){
    int h1,m1,h2,m2; if (sscanf(d,"%d:%d",&h1,&m1)!=2 || sscanf(f,"%d:%d",&h2,&m2)!=2) return 0.0f;
    float duree=(h2-h1)+(float)(m2-m1)/60.0f; if(duree<0) duree=0;
    return s->tarif_horaire*duree;
}
/*verifier*/
int minutes_diff(const char d[6],const char f[6]){
    int h1,m1,h2,m2; if (sscanf(d,"%d:%d",&h1,&m1)!=2 || sscanf(f,"%d:%d",&h2,&m2)!=2) return 0;
    return (h2*60+m2)-(h1*60+m1);
}

/* Liste double/ verifier*/
void ajouter_en_tete(const Reservation *r){
    NoeudReservation *n=malloc(sizeof(NoeudReservation));
    n->res=*r; n->prec=NULL; n->suiv=tete_reservations;
    if(tete_reservations) tete_reservations->prec=n;
    tete_reservations=n; if(!queue_reservations) queue_reservations=n;
}
/*verifier*/
void supprimer_noeud(NoeudReservation *node){
    if(!node)return;
    if(node->prec) node->prec->suiv=node->suiv; else tete_reservations=node->suiv;
    if(node->suiv) node->suiv->prec=node->prec; else queue_reservations=node->prec;
    free(node);
}

/* Fichiers /verifier*/
void sauvegarder_reservation(NoeudReservation *p){
    FILE *f=fopen("data/reservations.txt","a"); if(!f){afficher_erreur("Impossible d'écrire data/reservations.txt");return;}
    fprintf(f,"\n%d;%s;%s;%s;%s;%s;%d;%.2f;%s",
    p->res.id,p->res.client,p->res.nom_salle,p->res.date,
    p->res.debut,p->res.fin,p->res.nb_personnes,p->res.tarif,p->res.statut);
    fclose(f);
}
/*verifier*/
void charger_reservations(int *Nbline){
    FILE *f=fopen("data/reservations.txt","r"); if(!f){afficher_erreur("Impossible d'ouvrir data/reservations.txt");return;}
    Reservation r;
    while(fscanf(f,"%d;%49[^;];%49[^;];%10[^;];%5[^;];%5[^;];%d;%f;%29[^\n]\n",
        &r.id,r.client,r.nom_salle,r.date,r.debut,r.fin,&r.nb_personnes,&r.tarif,r.statut)==9){
        ajouter_en_tete(&r); (*Nbline)++;
    }
    fclose(f); afficher_succes("Réservations chargées depuis le fichier.");
}
