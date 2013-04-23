
#include<stdio.h>
#include<stdlib.h>

#include "parser.h"

/*
 * 
 */
int main(int argc, char** argv) {
    Commande e;
    fprintf(stdout, "Intruction -->\n");
    fflush(stdout);
    while (1) {
        int st = parsing(&e);
        printf("st = %d\n", st);
        if (st != 1) {
            fprintf(stderr, "Erreur\n");
        } else {
            switch (e.type) {
                case FUNC:
                    fprintf(stdout, "ok FUNC\n");
                    break;
                case PRINT:
                    fprintf(stdout, "ok PRINT\n");
                    break;
                case EVAL:
                    fprintf(stdout, "ok EVAL\n");
                    break;
                case SIMP:
                    fprintf(stdout, "ok SIMP\n");
                    break;
                case PLOT:
                    fprintf(stdout, "ok PLOT\n");
                    break;
                default:
                    fprintf(stderr, "inconnue \n");
            }
        }
    }
    return 0;
}
