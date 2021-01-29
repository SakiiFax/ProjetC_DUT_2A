#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define PORT 6000
int main()
{
    int fdSocket;
    int nbRecu;
    struct sockaddr_in coordonneesServeur;
    int longueurAdresse;
    char tampon[100];
    fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(fdSocket < 0)
    {
        printf("socket incorrecte\n");
        exit(-1);
    }


    // On prépare les coordonnées du serveur
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordonneesServeur, 0x00, longueurAdresse);
    coordonneesServeur.sin_family = PF_INET;
    // adresse du serveur
    inet_aton("127.0.0.1", &coordonneesServeur.sin_addr);
    // toutes les interfaces locales disponibles
    coordonneesServeur.sin_port = htons(PORT);
    if(connect(fdSocket,(struct sockaddr*)&coordonneesServeur,sizeof(coordonneesServeur))== -1)
    {
        printf("connexion impossible\n");
        exit(-1);
    }
    printf("connexion ok\n");
    nbRecu=recv(fdSocket,tampon,100,0);
    char salleComp[100];
    if(nbRecu>0){
        tampon[nbRecu]=0;
        if(strcmp(tampon,"0")==0) {
            printf("Il n'y a plus de place\n");
            strcpy(salleComp, tampon);
        } 
        else printf("Place disponibles : %s\n", tampon); 
    } 

    char nom[20], prenom[20];
    printf("Nom : ");
    scanf("%s", nom);
    printf("Prénom : ");
    scanf("%s", prenom);
    
    strcat(nom, " ");
    strcat(nom, prenom);
    send(fdSocket,nom,strlen(nom),0);
    nbRecu=recv(fdSocket,tampon,100,0);
    
    if(nbRecu>0)
    {
        tampon[nbRecu]=0;
        printf("%s\n",tampon);
    }

    char choix[10], choixA[10];
    strcpy(choixA,"2");
    printf("Choix : ");
    scanf("%s", choix);
    send(fdSocket,choix, strlen(choix),0);

    nbRecu=recv(fdSocket,tampon,100,0);
    
    if(nbRecu>0)
    {
        tampon[nbRecu]=0;
        printf("%s\n",tampon);
    }

    if(strcmp(choix,choixA)==0){
        char noDoss[100];
        printf("Dossier : ");
        scanf("%s", noDoss);
        send(fdSocket,noDoss, strlen(noDoss),0);

        nbRecu=recv(fdSocket,tampon,100,0);
    
        if(nbRecu>0)
        {
            tampon[nbRecu]=0;
            printf("%s\n",tampon);
        }

        close(fdSocket);        
    }
    else {
        close(fdSocket);
    } 

    return 0;
    
}