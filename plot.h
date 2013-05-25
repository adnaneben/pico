/* 
 * File:   plot.h
 * Author: 
 *
 * Created on May 6, 2013, 12:43 AM
 */

#include <search.h>
#include "parserExpr.h"

#ifndef PLOT_H
#define	PLOT_H

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * expr_plot :  permet de déssiner/tracer une liste de fonctions représenter par [*liste] en utilisant gnuplot.
     *      cette fonction se base sur la [simplify.h] pour la partie simplification pour permettre de représenter les foncitons
     *      complexe qui font appel à d'autres fonction ou utilise des dérivés
     */
    extern void expr_plot(Ident_list *, int, int);

#ifdef	__cplusplus
}
#endif

#endif	/* PLOT_H */
