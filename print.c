/* 
 * File:   print.c
 * Author: 
 *
 * Created on May 6, 2013, 12:43 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>

#include "parserExpr.h"
#include "exprList.h"

/*
 * afficher l'expression en forme arithmétique
 */
void print_Arith(Expr * expr, FILE * stream) {
    int i;
    int compt;
    switch (expr->type) {
        case ELEMENT:
            switch (expr->u.Element.elType) {
                case VAL:
                    if (expr->u.Element.typecst == INT)
                        fprintf(stream, "%d", (int) expr->u.Element.value);
                    else
                        fprintf(stream, "%.1f", expr->u.Element.value);
                    break;
                case VAR:
                    if (expr->u.Element.power > 0)
                        fprintf(stream, "x");
                    for (i = 1; i < expr->u.Element.power; i++) {
                        fprintf(stream, " * ");
                        fprintf(stream, "x");

                    }
                    break;
                case VALVAR:
                    if (expr->u.Element.typecst == INT)
                        fprintf(stream, "%d", (int) expr->u.Element.value);
                    else
                        fprintf(stream, "%.1f", expr->u.Element.value);

                    for (i = 0; i < expr->u.Element.power; i++) {
                        fprintf(stream, " * ");
                        fprintf(stream, "x");
                    }
                    break;
            }
            break;
        case FUNCT:
            fprintf(stream, "%s", expr->u.Func.name);
            i = expr->u.Func.derivative;
            for (compt = 0; compt < i; compt++) {
                fprintf(stream, "'");
            }
            fprintf(stream, "( ");
            print_Arith(expr->u.Func.child, stream);
            fprintf(stream, " )");
            break;
        case PAR:
            fprintf(stream, "( ");
            print_Arith(expr->u.Par.single, stream);
            fprintf(stream, " )");
            break;
        case OPBINAIRE:
            print_Arith(expr->u.OpBinaire.left, stream);
            switch (expr->u.OpBinaire.operateur) {
                case PLUS:
                    fprintf(stream, " + ");
                    break;
                case MULT:
                    fprintf(stream, " * ");
                    break;
            }
            print_Arith(expr->u.OpBinaire.rigth, stream);
    }
}

int mult = 0;
int plus = 0;
int par = 0;

/*
 * afficher l'expression arithmétique au format dot
 */
extern void print_Arb(Expr * expr, FILE *stream) {
    int i = 0;
    int compt;
    switch (expr->type) {
        case ELEMENT:
            switch (expr->u.Element.elType) {
                case VAL:
                    if (expr->u.Element.typecst == INT)
                        fprintf(stream, "%d;\n", (int) expr->u.Element.value);
                    else
                        fprintf(stream, "\"%.1f\";\n", expr->u.Element.value);
                    break;
                case VAR:
                    fprintf(stream, "\"x^%d\";\n", expr->u.Element.power);
                    break;
                case VALVAR:
                    if (expr->u.Element.typecst == INT)
                        fprintf(stream, "\"%d", (int) expr->u.Element.value);
                    else
                        fprintf(stream, "\"%.1f", expr->u.Element.value);

                    fprintf(stream, "x^%d\";\n", expr->u.Element.power);
                    break;
            }
            break;
        case FUNCT:
            fprintf(stream, "\"%s", expr->u.Func.name);
            i = expr->u.Func.derivative;
            for (compt = 0; compt < i; compt++) {
                fprintf(stream, "'");
                //fprintf(stream,"->");
            }
            fprintf(stream, "\"");
            fprintf(stream, " -> ");
            print_Arb(expr->u.Func.child, stream);
            break;
        case PAR:
            fprintf(stream, " PAR%d -> ", par);
            par++;
            //plus++;
            //mult++;
            print_Arb(expr->u.Par.single, stream);
            plus++;
            mult++;
            //plus--;
            //mult--;
            break;
        case OPBINAIRE:
            switch (expr->u.OpBinaire.operateur) {
                case PLUS:
                    // fprintf(stream, " PLUS%d;\n", plus);
                    fprintf(stream, " PLUS%d -> ", plus);
                    //fprintf(stream, " PLUS -> ");
                    plus++;
                    print_Arb(expr->u.OpBinaire.left, stream);
                    plus--;
                    // fprintf(stream, " PLUS -> ");
                    fprintf(stream, " PLUS%d -> ", plus);
                    plus++;
                    plus++;
                    print_Arb(expr->u.OpBinaire.rigth, stream);
                    plus--;
                    plus--;
                    break;
                case MULT:
                    //fprintf(stream, " MULT%d;\n", mult);
                    fprintf(stream, " MULT%d -> ", mult);
                    mult++;
                    print_Arb(expr->u.OpBinaire.left, stream);
                    mult--;
                    fprintf(stream, " MULT%d -> ", mult);
                    //fprintf(stream, " MULT%d;\n", mult);
                    mult++;
                    mult++;
                    print_Arb(expr->u.OpBinaire.rigth, stream);
                    mult--;
                    mult--;
                    break;
            }
    }
}

/*
 * afficher l'expression arithmétique sous forme polynomiale
 */
extern void print_Pol(Expr * expr, FILE *stream) {
    int i;
    int compt;
    switch (expr->type) {
        case ELEMENT:
            switch (expr->u.Element.elType) {

                case VAL:
                    if (expr->u.Element.typecst == INT)
                        fprintf(stream, "%d", (int) expr->u.Element.value);
                    else
                        fprintf(stream, "%.1f", expr->u.Element.value);
                    break;
                case VAR:
                    if (expr->u.Element.power > 1)
                        fprintf(stream, "x^%d", expr->u.Element.power);
                    else
                        fprintf(stream, "x");
                    break;
                case VALVAR:
                    if (expr->u.Element.typecst == INT)
                        fprintf(stream, "%d", (int) expr->u.Element.value);
                    else
                        fprintf(stream, "%.1f", expr->u.Element.value);

                    if (expr->u.Element.power > 1)
                        fprintf(stream, "x^%d", expr->u.Element.power);
                    else
                        fprintf(stream, "x");
                    break;
            }
            break;
        case FUNCT:
            fprintf(stream, "%s", expr->u.Func.name);
            i = expr->u.Func.derivative;
            for (compt = 0; compt < i; compt++) {
                fprintf(stream, "'");
            }
            fprintf(stream, "( ");
            print_Pol(expr->u.Func.child, stream);
            fprintf(stream, " )");
            break;
        case PAR:
            fprintf(stream, "( ");
            print_Pol(expr->u.Par.single, stream);
            fprintf(stream, " )");
            break;
        case OPBINAIRE:
            print_Pol(expr->u.OpBinaire.left, stream);
            switch (expr->u.OpBinaire.operateur) {
                case PLUS:
                    fprintf(stream, " + ");
                    break;
                case MULT:
                    fprintf(stream, " * ");
                    break;
            }
            print_Pol(expr->u.OpBinaire.rigth, stream);
            break;
    }
}

/*

afficher l'expression en fonction de l'instruction  de l'utilisateur
 */
void print_Expr(Commande e) {
    FILE *stream;
    int testFile;
    Expr* ex = exprList_get(e.u.Print.name);

    if (ex != NULL) {
        testFile = strcmp(e.u.Print.filename, "default");
        if (!testFile)
            stream = stdout;
        else
            stream = fopen(e.u.Print.filename, "w");

        if (stream == NULL)
            fprintf(stderr, "\n    -- probleme d'ouverture du fichier %s --", e.u.Print.filename);
        else {
            switch (e.u.Print.fmt) {
                case EXPR_AR:
                    print_Arith(ex, stream);
                    fprintf(stdout, "\n");
                    break;
                case ARB_AR:
                    fprintf(stream, "digraph G {\n");
                    print_Arb(ex, stream);
                    fprintf(stream, "\n}");
                    fprintf(stdout, "\n");
                    mult = 0;
                    plus = 0;
                    par = 0;
                    break;
                case POLY_AR:
                    print_Pol(ex, stream);
                    fprintf(stdout, "\n");
                    break;
            }
        }
        if (stream != stdout) {
            fclose(stream);
            fprintf(stdout, "l'expression a été écrit dans le fichier %s\n", e.u.Print.filename);
        }
    } else
        fprintf(stderr, "l'expression n'existe pas \n");
}

/*
 * imprime l'expression en forme arithmétique dans la chaine de caractère stringOut
 */
void print_ArithStr(Expr *expr, char* stringOut) {
    int i;
    int compt;
    switch (expr->type) {
        case ELEMENT:
            if (expr->u.Element.value != 0) {
                char numeric[10];
                if (expr->u.Element.typecst == INT)
                    sprintf(numeric, "%d", (int) expr->u.Element.value);
                else
                    sprintf(numeric, "%.1f", expr->u.Element.value);
                strcat(stringOut, numeric);
                if (expr->u.Element.power > 0)
                    strcat(stringOut, " * ");
            }
            for (i = 0; i < expr->u.Element.power; i++) {
                if (i != 0)
                    strcat(stringOut, " * ");
                strcat(stringOut, "x");
            }
            break;
        case FUNCT:
            strcat(stringOut, strdup(expr->u.Func.name));
            i = expr->u.Func.derivative;
            for (compt = 0; compt < i; compt++) {
                strcat(stringOut, "'");
            }
            strcat(stringOut, "( ");
            print_ArithStr(expr->u.Func.child, stringOut);
            strcat(stringOut, " )");
            break;
        case PAR:
            strcat(stringOut, "( ");
            print_ArithStr(expr->u.Par.single, stringOut);
            strcat(stringOut, " )");
            break;
        case OPBINAIRE:
            print_ArithStr(expr->u.OpBinaire.left, stringOut);
            switch (expr->u.OpBinaire.operateur) {
                case PLUS:
                    strcat(stringOut, " + ");
                    break;
                case MULT:
                    strcat(stringOut, " * ");
                    break;
            }
            print_ArithStr(expr->u.OpBinaire.rigth, stringOut);
    }
}
