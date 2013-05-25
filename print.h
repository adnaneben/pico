/* 
 * File:   print.h
 * Author: 
 *
 * Created on May 6, 2013, 12:43 AM
 */

#include <search.h>
#include "parserExpr.h"

#ifndef PRINT_H
#define	PRINT_H

#ifdef	__cplusplus
extern "C" {
#endif

	

    /*
     * afficher l'expression en fonction de l'instruction  de l'utilisateur
     */
    void print_Expr(Commande);

    /*
     * afficher l'expression au format arithmétique
     */
    extern void print_Arith(Expr *, FILE *);

    /*
     * afficher l'expression arithmétique au format dot
     */
	 
    extern void print_Arb(Expr *, FILE *);

    /*
     * afficher l'expression arithmétique sous forme polynomiale
     */
    extern void print_Pol(Expr *, FILE *);

    /**
     * print_ArithStr : imprime l'expression en forme arithmétique dans la chaine de caractère stringOut
     * @param expr l'expression à imprimer
     * @param stringOut la chaine de caractère de sortie qui représente l'expression
     */
    void print_ArithStr(Expr*, char*);
	
	
	
    
#ifdef	__cplusplus
}
#endif

#endif	/* PRINT_H */
