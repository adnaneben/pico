/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>

#include "parserExpr.h"
#include "print.h"
#include "exprList.h"
#include "simplify.h"

/**
 * expr_plot :  permet de déssiner/tracer une liste de fonctions représenter par [*liste] en utilisant gnuplot.
 *      cette fonction se base sur la [simplify.h] pour la partie simplification pour permettre de représenter les foncitons
 *      complexe qui font appel à d'autres fonction ou utilise des dérivés
 * @param liste de type Ident_list, représente une liste chainé des noms de fonction à déssiner
 * @param min la borne inférieur du dessin, mettre 0 si facultatif
 * @param max la borne supérieur du dessin, mettre 0 si facultatif
 */
extern void expr_plot(Ident_list* liste, int min, int max) {
    if (min > max) {
        fprintf(stderr, "ERREUR, La borne inferieur [%d] doit etre strictement superieur à la borne superieur [%d]!\n", min, max);
        return;
    }

    char* command = malloc(sizeof (char)*1200);
    char* exprStr = malloc(sizeof (char)*255);
    char* expStrOrigin = malloc(sizeof (char)*255);
    char* gnuplotOutputName = malloc(sizeof (char)*255);
    //char listId[255];
    strcpy(command, "");
    strcpy(gnuplotOutputName, "");
    while (liste != NULL) {
        strcpy(exprStr, "");
        strcpy(expStrOrigin, "");

        Expr* expr = exprList_get(liste->ident);
        if (expr != NULL) {
            // get the string representation of the expression
            print_ArithStr(expr, expStrOrigin);
            // générer un nom pour l'image de l'output de gnuplot
            strcat(gnuplotOutputName, strdup(liste->ident));
            // simplifier l'expression pour faciliter l'impression
            expr = expr_simplify(expr);
            // get the simplified expression string respresentation
            print_ArithStr(expr, exprStr);
            // ajouter la chaine de caractère à la liste de commandes
            sprintf(exprStr, (liste->suivant != NULL) ? " %s title '%s'," : " %s title '%s'", strdup(exprStr), expStrOrigin);
            strcat(command, exprStr);
        } else {
            fprintf(stderr, "ERREUR, L'expression [%s] n'existe pas!\n", liste->ident);
        }
        liste = liste->suivant;
    }
    strcpy(exprStr, command);
    if (min == max && min == 0) {
        sprintf(command, "[%d:%d] %s", min, max, exprStr);
    }
    sprintf(command, "gnuplot -e \"set term jpeg; set out '%s.jpg'; plot %s\" && display '%s.jpg' &", gnuplotOutputName, exprStr, gnuplotOutputName);
    system(command);
    return;
}