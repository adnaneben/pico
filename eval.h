/* 
 * File:   eval.h
 * Author: 
 *
 * Created on May 6, 2013, 12:43 AM
 */

#include <search.h>

#include "parserExpr.h"

#ifndef EVAL_H
#define	EVAL_H

#ifdef	__cplusplus
extern "C" {
#endif

    /* Fonction permetant d'evaluer une expession */
    extern void eval_Expr(Commande);

    /* calcul d'une expression en fonction d'une valeur  */
    extern float calc_eval_Expr(float, Expr *);

    /*derivé d'une expression */
    extern Expr * derv_Expr(Expr *);

    /**
     * derv_ExprExt : calcule la dérivé première d'une expression
     */
    extern Expr * derv_ExprExt(Expr *);

#ifdef	__cplusplus
}
#endif

#endif	/* EVAL_H */
