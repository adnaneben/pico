/* 
 * File:   parser.c
 * Author: 
 *
 * Created on May 6, 2013, 12:43 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parserExpr.h"

/*
 *         créer un nouvel Element dans une expression
 *         Ex :  2x^2 
 *         power = 2
 *         integer = 2
 *         typecst = INT	
 */
extern Expr * new_Element(float value, int power, TypeCst cst, ElType elType) {
    Expr * ret = (Expr *) malloc(sizeof (*ret));
    ret->type = ELEMENT;
    ret->u.Element.power = power;
    ret->u.Element.value = value;
    ret->u.Element.typecst = cst;
    ret->u.Element.elType = elType;
    return ret;
}

/*
 *         création une operation dans une expression
 */
extern Expr * new_OpBinaire_enum(Expr * l, Op op, Expr * r) {
    return new_OpBinaire(l, op == MULT ? '*' : '+', r);
}

/*
 *         création une operation dans une expression
 */

extern Expr * new_OpBinaire(Expr * l, char op, Expr * r) {
    Expr * ret = (Expr *) malloc(sizeof (*ret));
    Expr * tmp;
    Expr * tmp1;
    switch (op) {

        case '+':
            ret->type = OPBINAIRE;
            ret->u.OpBinaire.operateur = PLUS;
            ret->u.OpBinaire.left = l;
            ret->u.OpBinaire.rigth = r;
            break;
        case '*':
            if (l->type == ELEMENT && r->type == ELEMENT) {
                ret->type = ELEMENT;
                ret->u.Element.power = r->u.Element.power + l->u.Element.power;
                if (ret->u.Element.power == 0)
                    ret->u.Element.elType = VAL;
                else
                    ret->u.Element.elType = VALVAR;

                if (l->u.Element.typecst == INT && r->u.Element.typecst == INT) {
                    ret->u.Element.typecst = INT;
                    //ret->u.Element.value = r->u.Element.value * l->u.Element.value;
                } else
                    ret->u.Element.typecst = REEL;


                if (r->u.Element.value != 0 && l->u.Element.value != 0)
                    ret->u.Element.value = r->u.Element.value * l->u.Element.value;
                else
                    ret->u.Element.value = r->u.Element.value + l->u.Element.value;

                if (ret->u.Element.value == 1)
                    ret->u.Element.elType = VAR;
                //                free(r);
                //                free(l);
            }
            else if (r->type == ELEMENT) {
                tmp1 = (Expr *) malloc(sizeof (*tmp1));
                tmp1 = l->u.OpBinaire.rigth;
                if (l->type == OPBINAIRE && tmp1->type == ELEMENT) {
                    ret->type = OPBINAIRE;
                    ret->u.OpBinaire.left = l->u.OpBinaire.left;
                    tmp = (Expr *) malloc(sizeof (*tmp));
                    tmp->type = ELEMENT;
                    tmp->u.Element.power = r->u.Element.power + tmp1->u.Element.power;
                    if (tmp->u.Element.power == 0)
                        tmp->u.Element.elType = VAL;
                    else
                        tmp->u.Element.elType = VALVAR;

                    if (tmp1->u.Element.typecst == INT && r->u.Element.typecst == INT) {
                        tmp->u.Element.typecst = INT;
                        // tmp->u.Element.value = l->u.OpBinaire.rigth.u.Element.value * r->u.Element.value;
                    } else
                        tmp->u.Element.typecst = REEL;

                    if (tmp1->u.Element.value != 0 && r->u.Element.value != 0)
                        tmp->u.Element.value = tmp1->u.Element.value * r->u.Element.value;
                    else
                        tmp->u.Element.value = tmp1->u.Element.value + r->u.Element.value;
                    if (tmp->u.Element.value == 1)
                        tmp->u.Element.elType = VAR;
                    ret->u.OpBinaire.rigth = tmp;
                    ret->u.OpBinaire.operateur = l->u.OpBinaire.operateur;
                } else {
                    ret->type = OPBINAIRE;
                    ret->u.OpBinaire.left = l;
                    ret->u.OpBinaire.rigth = r;
                    ret->u.OpBinaire.operateur = MULT;

                }
            } else {
                ret->type = OPBINAIRE;
                ret->u.OpBinaire.left = l;
                ret->u.OpBinaire.rigth = r;
                ret->u.OpBinaire.operateur = MULT;
            }
            return ret;
            break;
    }
    return ret;
}

/*
 *         créer une expression avec une fonction
 */
extern Expr * new_Func(char * name, Expr * exp) {
    Expr * ret = (Expr *) malloc(sizeof (*ret));
    ret->type = FUNCT;
    const char * ptr = strchr(name, '\'');
    if (ptr) {
        // get subName
        int index, len = strlen(name);
        for (index = 0; index < len; index++) {
            if (name[index] == '\'') break;
        }
        char subName[index + 1];
        memcpy(subName, name, index);
        subName[index] = '\0';
        ret->u.Func.name = strdup(subName);
        ret->u.Func.derivative = len - index;
        //printf("%s %d", ret->u.Func.name, ret->u.Func.derivative);
    } else {
        ret->u.Func.name = strdup(name);
        ret->u.Func.derivative = 0;
    }
    //ret->par = par;
    ret->u.Func.child = exp;
    return ret;
}

/*
 *         créer une expression avec de parenthèze
 */
extern Expr * new_Par(Expr * expr) {
    Expr * ret = (Expr *) malloc(sizeof (*ret));
    ret->type = PAR;
    ret->u.Par.single = expr;
    return ret;
}

/**
 * new_ident_list
 * @param f
 * @param list
 * @return 
 */
extern Ident_list * new_ident_list(char * f, Ident_list * list) {
    Ident_list * nouvelElement = malloc(sizeof (* nouvelElement));

    nouvelElement->ident = strdup(f);
    nouvelElement->suivant = NULL;

    if (list == NULL) {
        return nouvelElement;
    } else {

        Ident_list * tmp = list;
        while (tmp->suivant != NULL) {
            tmp = tmp->suivant;
        }
        tmp->suivant = nouvelElement;
        return list;
    }

}

/**
 * new_Func_Deriv
 * @param name
 * @param derivLevel
 * @param paramExpr
 * @return 
 */
extern Expr * new_Func_Deriv(char * name, int derivLevel, Expr * paramExpr) {
    Expr * ret = (Expr *) malloc(sizeof (*ret));
    ret->type = FUNCT;
    ret->u.Func.name = strdup(name);
    ret->u.Func.derivative = derivLevel;
    ret->u.Func.child = paramExpr;
    return ret;
}

/**
 * expr_dup duplique une expression
 */
Expr* expr_dup(Expr* expr) {
    switch (expr->type) {
        case ELEMENT:
            return new_Element(expr->u.Element.value, expr->u.Element.power, expr->u.Element.typecst, expr->u.Element.elType);
        case PAR:
        {
            Expr* newExpr = expr_dup(expr->u.Par.single);
            return new_Par(newExpr);
        }
        case FUNCT:
        {
            Expr* newExprParam = expr_dup(expr->u.Func.child);
            Expr* newExpr = new_Func(expr->u.Func.name, newExprParam);
            newExpr->u.Func.derivative = expr->u.Func.derivative;
            return newExpr;
        }
        case OPBINAIRE:
        {
            Expr* newExprL = expr_dup(expr->u.OpBinaire.left);
            Expr* newExprR = expr_dup(expr->u.OpBinaire.rigth);
            return new_OpBinaire_enum(newExprL, expr->u.OpBinaire.operateur, newExprR);
        }
    }
}

/**
 * exprPow : retourne l'exponentiel aplatiné d'une expression
 */
Expr* exprPow(Expr* expr, int power) {
    if (power > 1) {
        Expr* exprOpBin = exprPow(expr, power - 1);
        if (power % 4 == 0) { // equilibrer la disposition des expresions dans l'arbre à droite et à gauche
            if (exprOpBin->u.OpBinaire.left->type == ELEMENT) {
                exprOpBin = new_OpBinaire_enum(exprOpBin->u.OpBinaire.rigth, MULT, new_OpBinaire_enum(exprOpBin->u.OpBinaire.left, MULT, expr_dup(expr)));
            } else {
                exprOpBin = new_OpBinaire_enum(exprOpBin->u.OpBinaire.left, MULT, new_OpBinaire_enum(exprOpBin->u.OpBinaire.rigth, MULT, expr_dup(expr)));
            }
        } else if (power % 2 == 0) {
            exprOpBin = new_OpBinaire_enum(exprOpBin, MULT, expr_dup(expr));
        } else {
            exprOpBin = new_OpBinaire_enum(expr_dup(expr), MULT, exprOpBin);
        }
        return exprOpBin;
    }
    return expr_dup(expr);
}
