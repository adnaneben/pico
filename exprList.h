/* 
 * File:   ExprList.h
 * Author: 
 * l'intéreface du module qui gère la liste d'expression stocké dans un hashtable
 * Created on April 24, 2013, 12:05 AM
 */

#include "parserExpr.h"

#ifndef EXPRLIST_H
#define	EXPRLIST_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * initialize le hashtable
     * @param  la taille max du hashtable
     * @return 
     */
    int exprList_init(int);

    /**
     * ajouter un element au hastable
     * @param l'identifiant de l'element
     * @param l'expression à ajouter dans le hashtable
     */
    void exprList_add(char*, Expr*);

    /**
     * met à jour un element dans le hashtable
     * @param l'identifiant de l'element
     * @param l'element nouveau
     * @return l'element mis à jours avec le nouveau
     */
    Expr* exprList_update(char*, Expr*);

    /**
     * récupère un element dans le hashtable depuis sont identifiant
     * @param l'identifiant de l'element à chercehr
     * @return l'element recherché sinon NULL
     */
    Expr* exprList_get(char*);

    /**
     * détruit le hashtable et libère les ressources mémoire utilisé par ce dernier
     */
    void exprList_destroy();

#ifdef	__cplusplus
}
#endif

#endif	/* EXPRLIST_H */

