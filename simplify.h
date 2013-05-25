/* 
 * File:   simplify.h
 * Author: 
 * l'interface des méthodes de simplification
 * Created on May 6, 2013, 12:43 AM
 */

#include "parserExpr.h"

#ifndef SIMPLIFY_H
#define	SIMPLIFY_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * expr_derivate : calcule la dérivé au niveau demandé d'une expression
     * @param expr l'expression pour laquel calculé la dérivé
     * @param level le niveau de dérivation demandé
     * @return l'expression résultante de la dérivation
     */
    Expr* expr_derivate(Expr*, int);

    /**
     * expr_replaceVariable : replace les variables d'une expression par une expression de replacement passé en paramètre varReplacemenet
     * @param expr l'expression à traité
     * @param varReplacemenet l'expression de remplacement
     * @return l'expresssion résultante du replacement
     */
    Expr* expr_replaceVariable(Expr*, Expr*);

    /**
     * expr_simplify :  l'opération de la simplification qui consiste à regrouper les chiffres de même ordre pour en calculer le résultat si possible, ainsi que 
     *      le remplacement des références de fonction appelé par l'expression de ces fonctions avec le bon paramètre.
     *      s'il y'a des dérivation ils vont être calculé
     * @param expr l'expresssion à simplifier
     * @return l'experession sous forme simplifié
     */
    Expr* expr_simplify(Expr*);

    /**
     * expr_canPermute : si on peut faire une permutation pour mieux simplifier l'experssion ou pas
     * @param expr l'experssion à tester
     * @return > 0 si on peut permuter et 0 sinon (exceptionnellement 2 si la permutation doit se faire dans un sous-noeud)
     */
    int expr_canPermute(Expr*);

    /**
     * expr_doPermute : permet de permuter deux noeud pour rapprocher des noeuds qu'ont pourra ensuite simplifier, 
     *      elle permet aussi de restructurer un arbre  pour rapprocher des noeuds orphelins qu'ont pourra ensuite simplifier, 
     * @param expr l'expression à permuter
     * @return l'experession résultant de la permutation
     */
    Expr* expr_doPermute(Expr*);

#ifdef	__cplusplus
}
#endif

#endif	/* SIMPLIFY_H */

