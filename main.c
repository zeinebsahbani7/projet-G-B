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
/*verifier*/
void sauvgarder_tarif(Salle *salles_arr, int nb){
    FILE *f=fopen("tarifs.txt","w"); if(!f){afficher_erreur("Impossible d'ouvrir tarifs.txt");return;}
    for(int i=0;i<nb;i++) fprintf(f,"%s;%f\n",salles_arr[i].nom,salles_arr[i].tarif_horaire);
    fclose(f);
}
/*verifier*/
void generer_facture(const Reservation *r){
    #ifdef _WIN32
      system("if not exist factures mkdir factures >nul 2>&1");
    #else
      system("mkdir -p factures");
    #endif
    char ticket[256]; snprintf(ticket,sizeof(ticket),"factures/facture_%d.txt",r->id);
    FILE *f=fopen(ticket,"w");
    fprintf(f,"========================================\n");
    fprintf(f,"           FACTURE DE RESERVATION        \n");
    fprintf(f,"========================================\n");
    fprintf(f,"ID: %d\nClient: %s\nSalle: %s\nDate: %s\n", r->id, r->client, r->nom_salle, r->date);
    fprintf(f,"Durée: %.2f heures\nMontant: %.2f DT\n", minutes_diff(r->debut,r->fin)/60.0, r->tarif);
    fprintf(f,"========================================\nMerci pour votre confiance !\n");
    fclose(f);
}
/*verifier*/
void afficher_menu(){
    #ifdef _WIN32
      system("cls");
    #else
      system("clear");
    #endif
    printf(BLUE "=========== MENU PRINCIPAL ===========\n" RESET);
    printf(" 1. Nouvelle réservation\n 2. Chiffre d'affaires\n 3. Réservations par mois\n");
    printf(" 4. Salles populaires\n 5. Supprimer réservation\n 6. Rechercher (client/salle/date)\n");
    printf(" 7. Filtrer par statut\n 8. Taux d'occupation\n 9. Revenus annuels\n");
    printf("10. Classement clients\n11. Quitter\n");

}
int dateValide(const char *date) {
    int jour, mois, annee;

    if (strlen(date) != 10 || date[2] != '/' || date[5] != '/')
        return 0;

    if (sscanf(date, "%2d/%2d/%4d", &jour, &mois, &annee) != 3)
        return 0;

    if (mois < 1 || mois > 12) return 0;
    if (annee <= 0) return 0;

    int joursParMois[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if (jour < 1 || jour > joursParMois[mois-1]) return 0;

    return 1; 
}
int heureValide(const char *heure) {
    int h, m;

    if (strlen(heure) != 5 || heure[2] != ':')
        return 0;

    if (sscanf(heure, "%2d:%2d", &h, &m) != 2)
        return 0;

    if (h < 0 || h > 23) return 0;
    if (m < 0 || m > 59) return 0;

    return 1; 
}




/*trouver struct salle a partir de nom/verifier*/
Salle* psalle(const char *nom){for(int i=0;i<nb_salles;i++)if(strcmp(salles[i].nom,nom)==0)return &salles[i];return NULL;}

/*verifier*/
void chiffre_affaires_par_salle(){
    printf(BLUE "\n=== CHIFFRE D'AFFAIRES PAR SALLE ===\n" RESET);
    for(int i=0;i<nb_salles;i++){
        float ca=0;
        for(NoeudReservation *p=tete_reservations;p;p=p->suiv)
            if(strcmp(p->res.nom_salle,salles[i].nom)==0 && strncmp(p->res.statut,"confirm",7)==0)
                ca+=p->res.tarif;
        printf(" %-12s : %.2f DT\n",salles[i].nom,ca);
    }
}

void reservations_par_mois_cette_annee(){
    printf(BLUE "\n=== RÉSERVATIONS PAR MOIS (ANNEE EN COURS) ===\n" RESET);
    int c[12]={0}; time_t t=time(NULL); struct tm tm=*localtime(&t); int an=tm.tm_year+1900;
    for(NoeudReservation *p=tete_reservations;p;p=p->suiv){
        int j,m,a; if(sscanf(p->res.date,"%d/%d/%d",&j,&m,&a)==3 && a==an && strcmp(p->res.statut,"confirmée")==0){
            if(m>=1&&m<=12) c[m-1]++; }
    }
    for(int i=0;i<12;i++) printf("Mois %2d : %d\n",i+1,c[i]);
}

void salles_les_plus_populaires(){
    Classement cl[NB_SALLES]; int nb=0;
    for(int i=0;i<nb_salles;i++){
        strncpy(cl[nb].nom,salles[i].nom,49); cl[nb].nom[49]='\0'; cl[nb].reservations=0;
        for(NoeudReservation *p=tete_reservations;p;p=p->suiv)
            if(strcmp(p->res.nom_salle,salles[i].nom)==0 && strcmp(p->res.statut,"confirmée")==0)
                cl[nb].reservations++;
        nb++;
    }
    for(int i=0;i<nb-1;i++) for(int j=i+1;j<nb;j++) if(cl[j].reservations>cl[i].reservations){Classement tmp=cl[i];cl[i]=cl[j];cl[j]=tmp;}
    printf(GREEN "\n🏆 Top salles:\n" RESET);
    for(int i=0;i<nb && i<5;i++) printf(" #%d %s -> %d réservations\n", i+1, cl[i].nom, cl[i].reservations);
}

void supprimer_reservation(int id){
    NoeudReservation *cur=tete_reservations;
    while(cur && cur->res.id!=id) cur=cur->suiv;
    if(!cur){afficher_erreur("Réservation introuvable.");return;}
    supprimer_noeud(cur); afficher_succes("Réservation supprimée.");
}

void rechercher_reservation(const char *critere,const char *valeur){
    printf(BLUE "\n=== Recherche (%s = %s) ===\n" RESET,critere,valeur);
    int found=0;
    for(NoeudReservation *p=tete_reservations;p;p=p->suiv){
        if ((strcmp(critere,"client")==0 && stricmp(p->res.client,valeur)==0) ||
            (strcmp(critere,"salle")==0  && stricmp(p->res.nom_salle,valeur)==0) ||
            (strcmp(critere,"date")==0   && strcmp(p->res.date,valeur)==0)) {
            printf("ID %d | %s | %s | %s | statut: %s\n", p->res.id, p->res.client, p->res.nom_salle, p->res.date, p->res.statut);
            found=1;
        }
    }
    if(!found) printf(YELLOW "Aucun résultat.\n" RESET);
}

void filtrer_reservations(const char *statut){
    printf(BLUE "\n=== Réservations (%s) ===\n" RESET,statut);
    int found=0;
    for(NoeudReservation *p=tete_reservations;p;p=p->suiv){
        if(strcmp(p->res.statut,statut)==0){
            printf("ID %d | Client: %s | Salle: %s | Date: %s | %s-%s\n",
                p->res.id,p->res.client,p->res.nom_salle,p->res.date,p->res.debut,p->res.fin);
            found=1;
        }
    }
    if(!found) printf(YELLOW "Aucun élément avec ce statut.\n" RESET);
}

void taux_occupation(){
    printf(BLUE "\n=== Taux d'occupation par salle ===\n" RESET);
    for(int i=0;i<nb_salles;i++){
        int total=0, occupe=0;
        for(NoeudReservation *p=tete_reservations;p;p=p->suiv){
            if(strcmp(p->res.nom_salle,salles[i].nom)==0)
            {
                total++; 
                if(strcmp(p->res.statut,"confirmée")==0) occupe++;
            }
        }
        float taux=(total>0)?(100.0f*occupe/total):0;/*melek fehmetha */
        printf("%-12s : %.2f%% (confirmées: %d / total: %d)\n", salles[i].nom, taux, occupe, total);
    }
}

void revenus_annuels(){
    printf(BLUE "\n=== Revenus mensuels et annuels (confirmées) ===\n" RESET);
    float mois[12]={0}, total=0; time_t t=time(NULL); struct tm tm=*localtime(&t); int an=tm.tm_year+1900;
    for(NoeudReservation *p=tete_reservations;p;p=p->suiv){
        int j,m,a; if(sscanf(p->res.date,"%d/%d/%d",&j,&m,&a)==3 && a==an && strcmp(p->res.statut,"confirmée")==0){
            if(m>=1&&m<=12){ mois[m-1]+=p->res.tarif; total+=p->res.tarif; }
        }
    }
    for(int i=0;i<12;i++) printf("Mois %2d : %.2f DT\n",i+1,mois[i]);
    printf(GREEN "\nTotal annuel : %.2f DT\n" RESET,total);
}


void classement_clients(){
    ClientStats stats[200]; int nb=0;
    for(NoeudReservation *p=tete_reservations;p;p=p->suiv){
        int found=0;
        for(int i=0;i<nb;i++) if(strcmp(stats[i].nom,p->res.client)==0){ stats[i].nb_reservations++; found=1; break; }
        if(!found && nb<200){ strncpy(stats[nb].nom,p->res.client,49); stats[nb].nom[49]='\0'; stats[nb].nb_reservations=1; nb++; }
    }
    for(int i=0;i<nb-1;i++) for(int j=i+1;j<nb;j++) if(stats[j].nb_reservations>stats[i].nb_reservations){ ClientStats tmp=stats[i]; stats[i]=stats[j]; stats[j]=tmp; }
    printf(GREEN "\n🏆 Clients les plus fréquents:\n" RESET);
    for(int i=0;i<nb && i<10;i++) printf(" #%d %-20s -> %d réservations\n", i+1, stats[i].nom, stats[i].nb_reservations);
}
