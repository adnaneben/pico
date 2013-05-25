/*
 *
 */

//#define _GNU_SOURCE
#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include <search.h>
#include <string.h>

#include "parserExpr.h"
#include "print.h"
#include "plot.h"
#include "exprList.h"
#include "eval.h"
#include "simplify.h"

char* usage =
        "usage : COMMAND\n"
        "pico 1.0 : Projet de complitaion FIP/FIPA 17, fait par Audry Makampila et Adnane Aqartit\n"
        "\n"
        "\n"
        "COMMAND can be : func or print or eval or simplify or plot used as following...\n"
        "\n"
        "=> func IDENT : EXPR\n"
        "Saves an expression that can be used later.\n"
        "       where IDENT is a function identifier that starts with a letter and could be only of letters and digits.\n"
        "             EXPR is an arithmetic expression with only one variable of possible use : 'x', can only use '+' and '*' as operations.\n"
        "Exemples :\n"
        "       func F1 : (x+2.5)*(-1*x+3)\n"
        "       func F2 : 2*x*x+3*x+-1\n"
        "       func F3 : F1(x*x) + F1''(1 + F2'(x+2))\n"
        "       func F4 : F1'(x)\n"
        "\n"
        "=> print [fmt] IDENT [\"file\"]\n"
        "Prints an expression in a specific format, the exression should be added by 'func' command to be recognized by the 'print'.\n"
        "       where IDENT is a function identifier that starts with a letter and could be only of letters and digits.\n"
        "             EXPR is an arithmetic expression with only one variable of possible use : 'x', can only use '+' and '*' as operations.\n"
        "             fmt is the format of the print, by default (/e) :"
        "                       /e for arithmetic representation\n"
        "                       /a for dot format tree representation\n"
        "                       /p for arithmetic representation of a polynominal forme\n"
        "Exemples :\n"
        "       print F1\n"
        "       print /a F2\n"
        "       print F2 \"expression.txt\"\n"
        "\n"
        "=> eval IDENT VALUE\n"
        "Evaluates an expression with a fixed value, the exression should be added by 'func' command to be recognized by the 'eval'.\n"
        "       where IDENT is a function identifier that starts with a letter and could be only of letters and digits.\n"
        "             VALUE a numeric value to be used to evaluate the expression.\n"
        "Exemples :\n"
        "       eval F1 33\n"
        "       eval F2 0\n"
        "\n"
        "=> simplify IDENT [IDENT2]\n"
        "Simplify an expression by calculating derivation, replacing functions references with their arithmetic representation and doing calculs on possible elements and also trying to remove the parentheses.\n"
        "       where IDENT is a function identifier that starts with a letter and could be only of letters and digits.\n"
        "             IDENT2 is an optional identifier to be used to store the simplified expression, if none is provided the simplified expression replaces the old one.\n"
        "Exemples :\n"
        "       simplify F3 F33\n"
        "       simplify F4\n"
        "\n"
        "=> plot IDENT_LIST [min max]\n"
        "Plot an expression or a list of exressions, the exression should be added by 'func' command to be recognized by the 'plot'.\n"
        "       where IDENT_LIST the list of identifiers  of functions to be plot.\n"
        "             min max optional, defines the boundaries of the plot.\n"
        "Exemples :\n"
        "       plot F3 F33 F1 F2\n"
        "       plot F4 -5 5\n"
        "\n"
        "ENSIIE, FIPA/FIP 17 2013\n";

int main(int argc, char** argv) {
    Commande e;
    Expr * tmp;
    fprintf(stdout, "Interpreteur Arithmetique\n");

    // usage
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        fprintf(stdout, "%s", usage);
    } else {
        fprintf(stdout, "Rappeler ce programme avec le paramètre -h pour afficher l'usage!! => pico -h\n");
    }

    fprintf(stdout, "Tapez votre intruction -->\n");
    fflush(stdout);
    e.u.Plot.list = NULL;
    while (1) {
        int st = parsing(&e);
        //fprintf(stdout,"st = %d\n", st);
        if (st != 1) {
            fprintf(stderr, "    -- erreur dans l'instruction --\n");
        } else {
            switch (e.type) {
                case FUNC:
                    tmp = exprList_get(e.u.IntExpr.name);
                    if (tmp == NULL) {
                        exprList_add(e.u.IntExpr.name, e.u.IntExpr.expr);
                        fprintf(stderr, "l'expression est valide et a ete sauvegarde\n");
                    } else
                        fprintf(stderr, "l'expression existe déjà!\n");
                    break;
                case PRINT:
                    print_Expr(e);
                    break;
                case EVAL:
                    eval_Expr(e);
                    break;
                case SIMP:
                    tmp = exprList_get(e.u.Simplify.ident1);
                    if (tmp == NULL) {
                        fprintf(stderr, "la fonction avec l'identifiant [%s] n'existe pas!\n", e.u.Simplify.ident1);
                    } else {
                        if (strlen(e.u.Simplify.ident2) > 0) {
                            tmp = expr_simplify(tmp);
                            exprList_add(e.u.Simplify.ident2, tmp);
                            fprintf(stderr, "la fonction avec l'identifiant [%s] a été simlpifié et ajouté avec le nom [%s].\n", e.u.Simplify.ident1, e.u.Simplify.ident2);
                        } else {
                            tmp = expr_simplify(tmp);
                            exprList_update(e.u.Simplify.ident1, tmp);
                            fprintf(stderr, "la fonction avec l'identifiant [%s] a été simlpifié.\n", e.u.Simplify.ident1);
                        }
                    }
                    break;
                case PLOT:
                    expr_plot(e.u.Plot.list, e.u.Plot.min, e.u.Plot.max);
                    fprintf(stdout, "\n");
                    break;
                default:
                    fprintf(stderr, "inconnue \n");
            }
        }
    }
    return 0;
}
