#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    
    
    
    FILE* fp = NULL;
    FILE* fp1 = NULL;
    char str;
    int ligne = 3;
    int curseur = 1;

    
    fp = fopen("listreserv.txt", "r");
    fp1 = fopen("file.txt", "w");

    while(ligne != curseur) {
        str = getc(fp);
        putc(str, fp1);
        if (str == '\n') {
            curseur++;
        }
    }
    curseur = 0;
    while(curseur < 3) {
        str = getc(fp);
        if (str == '\n') {
            curseur++;
        }
    }
    while((str = getc(fp)) != EOF) {
        putc(str, fp1);
    }
    fclose(fp);
    fclose(fp1);
    remove("listreserv.txt");
    rename("file.txt", "listreserv.txt");
    
    
/*  
    FILE* fp = NULL;


    char* numdoss = "9277617812";
    char* prenom = "testprenom";
    char* nom = "testnom";

    fp = fopen("listreserv.txt", "a");
    if (fp != NULL) {
        fputc('\n', fp);
        fputs(numdoss, fp);
        fputc('\n', fp);
        fputs(prenom, fp);
        fputc('\n', fp);
        fputs(nom, fp);

        fclose(fp);
    }
*/




    
/*
    int trouver = 0;
    int c;
    char* str = NULL;
    char* numdoss = "0748028647";

    fp = fopen("listreserv.txt", "r");
    if (fp != NULL) {
        while((c = fgetc(fp)) != EOF) {
            while(c != '\n') {

            }
        }
    }
    if (trouver == 1) {
        printf("trouvée");
    }
    else {
        printf("pas trouvée");
    }
    fclose(fp);
*/    
    
    
    
    /*
    fp = fopen("listreserv.txt", "r");
    if (fp != NULL) {
        int c;
        int nLignes = 1;
        while((c=fgetc(fp)) != EOF)
        {
            if(c=='\n')
                nLignes++;
        }
        nLignes /= 3;
        printf("%d", nLignes);
        fclose(fp);
    }*/

    return 0;
}