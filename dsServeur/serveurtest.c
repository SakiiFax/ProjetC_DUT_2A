#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>

#define PORT 6000

int PLACE_DISP;

void AjoutReservation(char*, char*, char*);
int nbReservation();
long createNumdoss();
int annulerReservation(char*);
int trouverDossier(char*);
void *execution(void*);

typedef struct {
    int fdSocketCommunication;
    char clientT[100];
    int nbRecu;
} var;

int main()
{
    int fdSocketAttente;
    int fdSocketCommunication;
    struct sockaddr_in coordonneesServeur;
    struct sockaddr_in coordonneesAppelant;
    char clientT[100];
    int nbRecu;
    int longueurAdresse;
    pthread_t td;
    fdSocketAttente = socket(PF_INET, SOCK_STREAM, 0);
    if(fdSocketAttente < 0)
    {
        printf("socket incorrecte\n");
        exit(-1);
    }


    // On prépare l’adresse d’attachement locale
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordonneesServeur, 0x00, longueurAdresse);
    coordonneesServeur.sin_family = PF_INET;
    // toutes les interfaces locales disponibles
    coordonneesServeur.sin_addr.s_addr = htonl(INADDR_ANY);
    // toutes les interfaces locales disponibles
    coordonneesServeur.sin_port = htons(PORT);

    if(bind(fdSocketAttente,(struct sockaddr*)&coordonneesServeur,sizeof(coordonneesServeur) ) == -1)
    {
        printf("erreur de bind\n");
        exit(-1);
    }
    if(listen(fdSocketAttente,10) == -1)
    {
        printf("erreur de listen\n");
        exit(-1);
    }
    socklen_t tailleCoord=sizeof(coordonneesAppelant);
    while((fdSocketCommunication=accept(fdSocketAttente,(struct sockaddr*)&coordonneesAppelant,&tailleCoord)) > 0)
    {
        var *V;
        V -> fdSocketCommunication = fdSocketCommunication;
        V -> clientT = clientT;
        V -> nbRecu = nbRecu;
        pthread_create(&td, NULL, execution, &V);
    }
    
}

void *execution(void *V) {
    var *temp=(var*)V;
    // Envoi du nombre de place
    PLACE_DISP = 100 - nbReservation();
    char placedisp[10];
    snprintf(placedisp, 10, "%i", PLACE_DISP);
    send(temp -> fdSocketCommunication,placedisp,strlen(placedisp),0);
    
    // Reception de l'identité du client
    char client[41], nom[20], prenom[20], d[]=" ";
    temp -> nbRecu=recv(temp -> fdSocketCommunication,temp -> clientT,99,0);
    if(temp -> nbRecu>0)
    {
        temp -> clientT[temp -> nbRecu]=0;
        printf("Connexion en tant que %s\n", temp -> clientT);
        strcpy(client, temp-> clientT);
    }

    char *p = strtok(client, d);
    strcpy(nom,p);
    p = strtok(NULL, d);
    strcpy(prenom,p);

    // Demande ce que le client veut faire sur le serveur
    char choix[100], noChoix[10], choixA[10], choixR[10];
    strcpy(choixA,"2");strcpy(choixR,"1");
    strcpy(choix,"\nQue voulez-vous faire ?\n1 - Réserver une place\n2 - Annuler une réservation");
    send(temp -> fdSocketCommunication,choix,strlen(choix),0);

    // Reception du choix du client
    temp -> nbRecu=recv(temp -> fdSocketCommunication,temp -> clientT,99,0);
    if(temp -> nbRecu>0)
    {
        temp -> clientT[temp -> nbRecu]=0;
        printf("Choix reçu : %s\n", temp -> clientT);
        strcpy(noChoix, temp -> clientT);
    } 

    // Séparation entre la réservation et l'annulation 
    char noDoss[100], annul[100];    
    if(strcmp(noChoix,choixR)==0){

        //Reservation donc envoi d'un numéro de dossier et la place
        snprintf(noDoss, 100, "%li", createNumdoss());

        AjoutReservation(noDoss, prenom, nom);

        send(temp -> fdSocketCommunication,noDoss,strlen(noDoss),0);
        
        close(temp -> fdSocketCommunication);
        return 0;
    }
    else if(strcmp(noChoix,choixA)==0){

        // Annulation donc demande du numéro de dossier à annuler
        strcpy(annul, "Veuillez saisir le numéro de dossier de la réservation que vous souhaitez annuler :");
        send(temp -> fdSocketCommunication,annul,strlen(annul),0);

        // Reception du numéro de dossier à supprimer
        temp -> nbRecu=recv(temp -> fdSocketCommunication,temp -> clientT,99,0);
        if(temp -> nbRecu>0)
        {
            clientT[temp -> nbRecu]=0;
            printf("Dossier reçu : %s\n", temp -> clientT);
            strcpy(noDoss, temp -> clientT);
        }
        int suppr;
        suppr = annulerReservation(noDoss);
        if(suppr == 0){
            strcpy(annul,"Echec de la suppression");
            send(temp -> fdSocketCommunication,annul,strlen(annul),0);
            close(temp -> fdSocketCommunication);
            return 0;
        } 

        // Confirmation de l'annulation
        strcpy(annul,"Dossier n°"); strcat(annul, noDoss); strcat(annul, " supprimé");
        send(temp -> fdSocketCommunication,annul,strlen(annul),0);

        close(temp -> fdSocketCommunication);
        return 0;
    }
    else{
        close(temp -> fdSocketCommunication);
        return 0;
    } 

}

// Créer un numéro de dossier random
long createNumdoss() {
    srand(time(NULL));
    long r;
    int existe = 0;
    do {
        r = rand() % 10000000000;
    } while (existe == 1);
    return r;
}

// Ecrit dans le fichier texte la nouvelle réservation
void AjoutReservation(char *numdoss, char *prenom, char *nom) {
    FILE* fp = NULL;

    fp = fopen("listreserv.txt", "a"); // On ouvre en ajout pour ne pas supprimer le contenu
    if (fp != NULL) {
        fputs(numdoss, fp);
        fputc(' ', fp);
        fputs(prenom, fp);
        fputc(' ', fp);
        fputs(nom, fp);
        fputc('\n', fp); // Il faut à chaque ajout revenir à la ligne

        fclose(fp);
        PLACE_DISP--; // On décrémente le nombre de place disponible
    }
}

// Retourne le nombre de reservations déjà faite
int nbReservation() {
    FILE* fp = NULL;
    int nbLignes = 0;

    fp = fopen("listreserv.txt", "r");
    if (fp != NULL) {
        int c;
        while((c = fgetc(fp)) != EOF) { // On lit chaque caractère jusqu'à la fin du fichier
            if(c == '\n') {
                nbLignes++; // On incrémente à chaque fin de ligne
            }
        }
        fclose(fp);
    }
    return nbLignes;
}

// Annule une réservation
int annulerReservation(char *noDoss) {
    FILE* fp = NULL;
    FILE* fp1 = NULL;
    char str;
    int ligne = trouverDossier(noDoss); // On utilise la fonction pour trouver le numero de ligne à ne pas recopier
    int curseur = 1;

    if(ligne == -1){
        printf("noDoss inexistant\n");
        return 0; // echec car inexistant
    } 
    
    fp = fopen("listreserv.txt", "r");
    fp1 = fopen("file.txt", "w");

    while(ligne != curseur) { // On lit en recopiant jusqu'à la ligne de la commande à annuler
        str = getc(fp);
        putc(str, fp1);
        if (str == '\n') {
            curseur++;
        }
    }
    curseur = 0;
    while(curseur < 1) { // On lit sans recopier
        str = getc(fp);
        if (str == '\n') {
            curseur++;
        }
    }
    while((str = getc(fp)) != EOF) { // On lit en recopiant jusqu'à la fin
        putc(str, fp1);
    }
    fclose(fp);
    fclose(fp1);
    rename("listreserv.txt", "oldfile.txt"); // Il faut d'abord renommer l'ancien sinon cela pose des problèmes même s'il est supprimer avant le renommage du nouveau
    remove("oldfile.txt");
    
    printf("%d\n", rename("file.txt", "listreserv.txt"));

    return 1; // succes
} 

// Retourne la ligne du dossier cherché
int trouverDossier(char *numdoss) {

    int existe = 0;
    int nLignes = 0;
    FILE* fp;
    char test[100];

    fp = fopen("listreserv.txt", "r");

    while(fgets(test,100,fp) != NULL)
    {
	    if(strstr(test,"\n")) { // On incrémente à chaque saut de ligne pour garder la position du curseur dans le fichier
		    nLignes++;
        }
        if (strstr(test, numdoss) != NULL) { // On compare si la ligne correspond au numéro de dossier
            existe=1;
            return nLignes; // On retourne la position du curseur dans le fichier
        }
    } 
    
    if(existe == 0) {
        nLignes = -1;
    } 

    fclose(fp);
    return nLignes;
}