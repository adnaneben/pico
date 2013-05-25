/* 
 * File:   eval.c
 * Author: 
 *
 * Created on May 6, 2013, 12:43 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <math.h>

#include "parserExpr.h"
#include "exprList.h"
#include "simplify.h"
#include "print.h"
/*
extern Expr * derv_Expr(Expr * expr) {
    Expr * ret;
    ENTRY * search;
    Expr * tmp;
    ret = (Expr *) malloc(sizeof (*ret));
    switch (expr->type) {
        case ELEMENT:
			switch (expr->u.Element.elType) 
			{
                case VAL:
                    return new_Element(0, 0, INT, VAL);
                case VAR:
                    return new_Element(expr->u.Element.power, expr->u.Element.power - 1, INT, (expr->u.Element.power - 1) == 0 ? VAL : VALVAR);
                case VALVAR:
                    return new_Element(
                            expr->u.Element.power * expr->u.Element.value, expr->u.Element.power - 1, 
                            expr->u.Element.typecst == REEL ? REEL : INT, 
                            (expr->u.Element.power - 1) == 0 ? VAL : VALVAR);
            }

            break;
        case FUNCT:
            search = search_Func(expr->u.Func.name);
            if (search == NULL) {
                fprintf(stderr, "la fonction %s n'existe pas \n", expr->u.Func.name);
                exit(1);
            }
            tmp = (Expr *) search->data;
			
			ret = putExprIn(expr->u.Func.child,tmp);
            ret = derv_Expr(ret);
            
            return ret;
            break;
        case PAR:
            return new_Par(derv_Expr(expr->u.Par.single));
            break;
        case OPBINAIRE:
            switch (expr->u.OpBinaire.operateur) {
                case PLUS:
                    return new_OpBinaire(derv_Expr(expr->u.OpBinaire.left), '+', derv_Expr(expr->u.OpBinaire.rigth));
                    break;
                case MULT:
                    tmp = new_OpBinaire(derv_Expr(expr->u.OpBinaire.left), '*', expr->u.OpBinaire.rigth);
                    ret = new_OpBinaire(expr->u.OpBinaire.left, '*', derv_Expr(expr->u.OpBinaire.rigth));
                    return new_OpBinaire(tmp, '+', ret);
                    break;
            }
            break;
    }
}
*/
/**
 * derv_ExprExt : calcule la dérivé première d'une expression
 * @param expr l'expression pour laquel calculer la dérivé
 * @return l'expresssion dérivé
 */
extern Expr * derv_ExprExt(Expr * expr) {
    switch (expr->type) {
        case ELEMENT:
            switch (expr->u.Element.elType) {
                case VAL:
                    return new_Element(0, 0, INT, VAL);
                case VAR:
                    return new_Element(expr->u.Element.power, expr->u.Element.power - 1, INT, (expr->u.Element.power - 1) == 0 ? VAL : VALVAR);
                case VALVAR:
                    return new_Element(
                            expr->u.Element.power * expr->u.Element.value, expr->u.Element.power - 1, 
                            expr->u.Element.typecst == REEL ? REEL : INT, 
                            (expr->u.Element.power - 1) == 0 ? VAL : VALVAR);
            }
        case OPBINAIRE:
            switch (expr->u.OpBinaire.operateur) {
                case PLUS:
                    return new_OpBinaire(derv_ExprExt(expr->u.OpBinaire.left), '+', derv_ExprExt(expr->u.OpBinaire.rigth));
                case MULT:
                    return new_OpBinaire(
                           new_Par(new_OpBinaire(new_Par(derv_ExprExt(expr->u.OpBinaire.left)), '*', expr->u.OpBinaire.rigth)),
                            '+',
                           new_Par(new_OpBinaire(expr->u.OpBinaire.left, '*', new_Par(derv_ExprExt(expr->u.OpBinaire.rigth)))));
            }
        case FUNCT:
        {
            Expr* identFunc = exprList_get(expr->u.Func.name);
            if (identFunc == NULL) {
                fprintf(stderr, "ERROR dans la simplification, l'identifiant de la fonction [%s] n'existe pas!\n", expr->u.Func.name);
                exit(EXIT_FAILURE);
            }
            Expr* simplified = expr_simplify(identFunc);
            Expr* derivated = expr_derivate(simplified, expr->u.Func.derivative);
            Expr* paramExpr = expr_simplify(expr->u.Func.child);
            identFunc = expr_replaceVariable(derivated, paramExpr);
            return derv_ExprExt(identFunc);
        }
        case PAR:
            return derv_ExprExt(expr->u.Par.single);
    }
}




extern float calc_eval_Expr(float val, Expr * expr) {
    int i = 0;
    int compt;
    float v;
    Expr * expr1;
    Expr * tmp;
    int exposant;
    switch (expr->type) 
	{
        case ELEMENT:
            if (expr->u.Element.elType == VAR) {
                return pow(val, expr->u.Element.power) * 1;
            } else if (expr->u.Element.elType == VALVAR) {
                return pow(val, expr->u.Element.power) * expr->u.Element.value;
            } else {
                return expr->u.Element.value;
            }
        case FUNCT:
            expr1 = exprList_get(expr->u.Func.name);
            if (expr1 == NULL) {
                fprintf(stderr, "la fonction %s n'existe pas \n", expr->u.Func.name);
                exit(1);
            }

            v = calc_eval_Expr(val, expr->u.Func.child);
            i = expr->u.Func.derivative;

            if (i > 0) {
                tmp = derv_ExprExt(expr1);
                //printf("expr derive \n");
                //print_Expr(tmp,stdout);
                //printf("\n");
				//print_Arith(tmp,stdout);
				//printf("\n");
                for (compt = 1; compt < i; compt++) {
                    tmp = derv_ExprExt(tmp);
					//print_Arith(tmp,stdout);
					//printf("\n");
                }
                v = calc_eval_Expr(v, tmp);
            } else
                v = calc_eval_Expr(v, expr1);
            ///float v = derv_Func(exp);
            return v;
            break;
        case PAR:
            return calc_eval_Expr(val, expr->u.Par.single);
            break;
        case OPBINAIRE:
            switch (expr->u.OpBinaire.operateur) {
                case PLUS:
                    //printf("%.f + %.f = %.f", eval_Expr(val ,exp->u.OpBinaire.left),eval_Expr(val ,exp->u.OpBinaire.left),eval_Expr(val ,exp->u.OpBinaire.left) + eval_Expr(val ,exp->u.OpBinaire.left));
                    return calc_eval_Expr(val, expr->u.OpBinaire.left) + calc_eval_Expr(val, expr->u.OpBinaire.rigth);
                    break;
                case MULT:
                    return calc_eval_Expr(val, expr->u.OpBinaire.left) * calc_eval_Expr(val, expr->u.OpBinaire.rigth);
                    break;
            }
            break;
    }
}

extern void eval_Expr(Commande e) {
    Expr * expr;
    float val;
    expr = exprList_get(e.u.Eval.name);
    if (expr == NULL) {
        fprintf(stderr, "la fonction n'existe pas \n");
        return;
    }
    switch (e.u.Eval.number) {
        case INT:
            val = calc_eval_Expr((float) e.u.Eval.val, expr);
            break;
        case REEL:
            val = calc_eval_Expr(e.u.Eval.valeur, expr);
            break;
    }
    fprintf(stdout, "la fonction %s = %.1f \n", e.u.Eval.name, val);
}