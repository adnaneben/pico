/* 
 * File:   expr_simplify.c
 * Author: 
 * L'implémentation de la méthode de simplification d'expressions et des méthodes nécessaire pour ce traitement
 * Created on May 6, 2013, 12:43 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include "simplify.h"
#include "print.h"
#include "eval.h"
#include "parserExpr.h"
#include "exprList.h"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

/**
 * expr_derivate : calcule la dérivé au niveau demandé d'une expression
 */
Expr* expr_derivate(Expr* expr, int level) {
    if (level < 1) {
        return expr;
    }
    Expr* subExpr = derv_ExprExt(expr);
    return expr_derivate(subExpr, --level);
}

/**
 * expr_replaceVariable : replace les variables d'une expression par une expression de replacement passé en paramètre varReplacemenet
 */
Expr* expr_replaceVariable(Expr* expr, Expr* varReplacemenet) {
    if (expr->type == ELEMENT && expr->u.Element.power >= 1) {
        Expr* exprPower;
        if (varReplacemenet->type == OPBINAIRE || varReplacemenet->type == FUNCT) {
            exprPower = exprPow(new_Par(varReplacemenet), expr->u.Element.power);
        } else {
            exprPower = exprPow(varReplacemenet, expr->u.Element.power);
        }
        return expr->u.Element.elType == VALVAR
                ? new_OpBinaire_enum(exprPower, MULT, new_Element(expr->u.Element.value, 0, expr->u.Element.typecst, VAR))
                : exprPower;
    } else if (expr->type == ELEMENT) {
        return expr;
    } else if (expr->type == PAR) {
        return expr_replaceVariable(expr->u.Par.single, varReplacemenet);
    } else if (expr->type == FUNCT) {
        Expr* identFunc = exprList_get(expr->u.Func.name);
        if (identFunc == NULL) {
            fprintf(stderr, "ERROR dans la simplification, l'identifiant de la fonction [%s] n'existe pas!\n", expr->u.Func.name);
            exit(EXIT_FAILURE);
        }
        identFunc = expr_replaceVariable(identFunc, expr_replaceVariable(expr->u.Func.child, varReplacemenet));
        return new_Par(identFunc);
    } else if (expr->type == OPBINAIRE) {
        expr->u.OpBinaire.left = expr_replaceVariable(expr->u.OpBinaire.left, varReplacemenet);
        expr->u.OpBinaire.rigth = expr_replaceVariable(expr->u.OpBinaire.rigth, varReplacemenet);
        return expr;
    }
}

/**
 * expr_simplify :  l'opération de la simplification qui consiste à regrouper les chiffres de même ordre pour en calculer le résultat si possible, ainsi que 
 *      le remplacement des références de fonction appelé par l'expression de ces fonctions avec le bon paramètre.
 *      s'il y'a des dérivation ils vont être calculé
 */
Expr* expr_simplify(Expr* expr) {
    if (expr->type == FUNCT) {
        Expr* identFunc = exprList_get(expr->u.Func.name);
        if (identFunc == NULL) {
            fprintf(stderr, "ERROR dans la simplification, l'identifiant de la fonction [%s] n'existe pas!\n", expr->u.Func.name);
            exit(EXIT_FAILURE);
        }
        Expr* simplified = expr_simplify(identFunc);
        Expr* derivated = expr_derivate(simplified, expr->u.Func.derivative);
        simplified = expr_simplify(derivated);
        Expr* paramExpr = expr_simplify(expr->u.Func.child);
        identFunc = expr_replaceVariable(simplified, paramExpr);
        return expr_simplify(identFunc); // no need to expr_simplify here
    } else if (expr->type == PAR) {
        Expr* simplified = expr_simplify(expr->u.Par.single); // , expr->u.Func.derivative
        return simplified;
    } else if (expr->type == OPBINAIRE) {
        if (expr->u.OpBinaire.left->type == ELEMENT
                && expr->u.OpBinaire.rigth ->type == ELEMENT) {
            if (expr->u.OpBinaire.operateur == MULT) {
                Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                newExpr->type = ELEMENT;
                newExpr->u.Element.power = expr->u.OpBinaire.rigth->u.Element.power + expr->u.OpBinaire.left->u.Element.power;
                newExpr->u.Element.typecst = (expr->u.OpBinaire.rigth->u.Element.typecst == REEL || expr->u.OpBinaire.left->u.Element.typecst == REEL)
                        && expr->u.OpBinaire.rigth->u.Element.value != 0 && expr->u.OpBinaire.left->u.Element.value != 0
                        ? REEL : INT;
                if (expr->u.OpBinaire.rigth->u.Element.elType == VAL && expr->u.OpBinaire.left->u.Element.elType == VAL) {
                    newExpr->u.Element.value = expr->u.OpBinaire.rigth->u.Element.value * expr->u.OpBinaire.left->u.Element.value;
                    newExpr->u.Element.elType = VAL;
                } else if (expr->u.OpBinaire.rigth->u.Element.elType == VAR && expr->u.OpBinaire.left->u.Element.elType == VAR) {
                    newExpr->u.Element.elType = VAR;
                } else if (expr->u.OpBinaire.rigth->u.Element.elType == VAR || expr->u.OpBinaire.left->u.Element.elType == VAR) {
                    newExpr->u.Element.value = expr->u.OpBinaire.left->u.Element.elType == VAR ? expr->u.OpBinaire.rigth->u.Element.value : expr->u.OpBinaire.left->u.Element.value;
                    if (newExpr->u.Element.value == 1) {
                        newExpr->u.Element.elType = VAR;
                        newExpr->u.Element.value = 0;
                    } else {
                        newExpr->u.Element.elType = VALVAR;
                    }
                } else {
                    newExpr->u.Element.value = expr->u.OpBinaire.rigth->u.Element.value * expr->u.OpBinaire.left->u.Element.value;
                    newExpr->u.Element.elType = VALVAR;
                }
                return newExpr;
            } else {
                if (expr->u.OpBinaire.rigth->u.Element.elType == VAL && expr->u.OpBinaire.left->u.Element.elType == VAL) {
                    Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                    newExpr->type = ELEMENT;
                    newExpr->u.Element.typecst = (expr->u.OpBinaire.rigth->u.Element.typecst == REEL || expr->u.OpBinaire.left->u.Element.typecst == REEL)
                            ? REEL : INT;
                    newExpr->u.Element.value = expr->u.OpBinaire.rigth->u.Element.value + expr->u.OpBinaire.left->u.Element.value;
                    newExpr->u.Element.elType = VAL;
                    return newExpr;
                } else if ((expr->u.OpBinaire.rigth->u.Element.elType == VAR || expr->u.OpBinaire.left->u.Element.elType == VALVAR)
                        && (expr->u.OpBinaire.rigth->u.Element.elType == VAR || expr->u.OpBinaire.left->u.Element.elType == VALVAR)
                        && expr->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.Element.power) {
                    float leftVal = expr->u.OpBinaire.left->u.Element.elType == VAR ? 1 : expr->
                            u.OpBinaire.left->u.Element.value;
                    float rightVal = expr->u.OpBinaire.rigth->u.Element.elType == VAR ? 1 : expr->u.OpBinaire.rigth->u.Element.value;
                    Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                    newExpr->type = ELEMENT;
                    newExpr->u.Element.typecst = (expr->u.OpBinaire.rigth->u.Element.typecst == REEL || expr->u.OpBinaire.left->u.Element.typecst == REEL)
                            ? REEL : INT;
                    newExpr->u.Element.power = expr->u.OpBinaire.rigth->u.Element.power;
                    newExpr->u.Element.value = leftVal + rightVal;
                    newExpr->u.Element.elType = VALVAR;
                    return newExpr;
                } else if (expr->u.OpBinaire.rigth->u.Element.elType == VALVAR && expr->u.OpBinaire.left->u.Element.elType == VALVAR
                        && expr->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.Element.power) {
                    float leftVal = expr->u.OpBinaire.left->u.Element.value;
                    float rightVal = expr->u.OpBinaire.rigth->u.Element.value;
                    Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                    newExpr->type = ELEMENT;
                    newExpr->u.Element.typecst = (expr->u.OpBinaire.rigth->u.Element.typecst == REEL || expr->u.OpBinaire.left->u.Element.typecst == REEL)
                            ? REEL : INT;
                    newExpr->u.Element.power = expr->u.OpBinaire.rigth->u.Element.power;
                    newExpr->u.Element.value = leftVal + rightVal;
                    newExpr->u.Element.elType = VALVAR;
                    return newExpr;
                }
            }
            return expr;
        } else if ((expr->u.OpBinaire.left->type == ELEMENT
                && expr->u.OpBinaire.rigth->type == PAR)
                || (expr->u.OpBinaire.rigth->type == ELEMENT
                && expr->u.OpBinaire.left->type == PAR)) {
            Expr *parenthPart = expr->u.OpBinaire.rigth->type == ELEMENT ? expr->u.OpBinaire.left : expr->u.OpBinaire.rigth;
            Expr *elePart = expr->u.OpBinaire.rigth->type == ELEMENT ? expr->u.OpBinaire.rigth : expr->u.OpBinaire.left;
            parenthPart = expr_simplify(parenthPart->u.Par.single);
            if (parenthPart->type == OPBINAIRE) {
                if (expr->u.OpBinaire.operateur == MULT) {
                    Expr* newExprLeft = (Expr*) malloc(sizeof (Expr*));
                    newExprLeft->type = OPBINAIRE;
                    newExprLeft->u.OpBinaire.left = expr_dup(elePart);
                    newExprLeft->u.OpBinaire.operateur = MULT;
                    newExprLeft->u.OpBinaire.rigth = parenthPart->u.OpBinaire.left;
                    // try permute
                    if (expr_canPermute(newExprLeft->u.OpBinaire.rigth)) {
                        newExprLeft->u.OpBinaire.rigth = expr_doPermute(newExprLeft->u.OpBinaire.rigth);
                    }
                    newExprLeft->u.OpBinaire.rigth = newExprLeft->u.OpBinaire.rigth->type == OPBINAIRE
                            ? new_Par(expr_dup(newExprLeft->u.OpBinaire.rigth))
                            : expr_dup(newExprLeft->u.OpBinaire.rigth);

                    Expr* newExprRight = (Expr*) malloc(sizeof (Expr*));
                    newExprRight->type = OPBINAIRE;
                    newExprRight->u.OpBinaire.left = expr_dup(elePart);
                    newExprRight->u.OpBinaire.operateur = MULT;
                    newExprRight->u.OpBinaire.rigth = parenthPart->u.OpBinaire.rigth;
                    // try permute
                    if (expr_canPermute(newExprRight->u.OpBinaire.rigth)) {
                        newExprRight->u.OpBinaire.rigth = expr_doPermute(newExprRight->u.OpBinaire.rigth);
                    }
                    newExprRight->u.OpBinaire.rigth = newExprRight->u.OpBinaire.rigth->type == OPBINAIRE
                            ? new_Par(expr_dup(newExprRight->u.OpBinaire.rigth))
                            : expr_dup(newExprRight->u.OpBinaire.rigth);

                    Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                    newExpr->type = OPBINAIRE;
                    newExpr->u.OpBinaire.left = expr_simplify(expr_dup(newExprLeft));
                    newExpr->u.OpBinaire.operateur = parenthPart->u.OpBinaire.operateur;
                    newExpr->u.OpBinaire.rigth = expr_simplify(expr_dup(newExprRight));
                    // recursive call
                    Expr* simplified = expr_simplify(newExpr);
                    if (expr_canPermute(simplified)) {
                        newExpr = expr_doPermute(simplified);
                        simplified = expr_simplify(newExpr);
                    }
                    return simplified;
                } else {
                    Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                    newExpr->type = OPBINAIRE;
                    newExpr->u.OpBinaire.left = expr_dup(elePart);
                    newExpr->u.OpBinaire.operateur = PLUS;
                    newExpr->u.OpBinaire.rigth = expr_dup(parenthPart);
                    // recursive call
                    Expr* simplified = expr_simplify(newExpr);
                    if (expr_canPermute(simplified)) {
                        newExpr = expr_doPermute(simplified);
                        simplified = expr_simplify(newExpr);
                    }
                    return simplified;
                }
            } else {
                Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                newExpr->type = OPBINAIRE;
                newExpr->u.OpBinaire.left = expr->u.OpBinaire.left->type == ELEMENT ? expr_dup(elePart) : expr_dup(parenthPart);
                newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
                newExpr->u.OpBinaire.rigth = expr->u.OpBinaire.rigth->type == ELEMENT ? expr_dup(elePart) : expr_dup(parenthPart);
                // recursive call
                Expr* simplified = expr_simplify(newExpr);
                if (expr_canPermute(simplified)) {
                    newExpr = expr_doPermute(simplified);
                    simplified = expr_simplify(newExpr);
                }
                return simplified;
            }
        } else if (expr->u.OpBinaire.left->type == PAR
                && expr->u.OpBinaire.rigth->type == PAR) {
            Expr *leftParenthPart = expr_simplify(expr->u.OpBinaire.left->u.Par.single);
            Expr *rightParenthPart = expr_simplify(expr->u.OpBinaire.rigth->u.Par.single);
            if (leftParenthPart->type == OPBINAIRE || rightParenthPart->type == OPBINAIRE) {
                if (leftParenthPart->type == OPBINAIRE && rightParenthPart->type == OPBINAIRE) {
                    if (leftParenthPart->u.OpBinaire.operateur == rightParenthPart->u.OpBinaire.operateur
                            && leftParenthPart->u.OpBinaire.operateur == expr->u.OpBinaire.operateur) {
                        Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                        newExpr->type = OPBINAIRE;
                        newExpr->u.OpBinaire.left = expr_dup(leftParenthPart);
                        newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
                        newExpr->u.OpBinaire.rigth = expr_dup(rightParenthPart);
                        // try permutation
                        if (expr_canPermute(newExpr)) {
                            Expr* exprPerm = expr_doPermute(newExpr);
                            newExpr = expr_simplify(exprPerm);
                        }
                        return newExpr;
                    } else { // PLUS
                        if (expr->u.OpBinaire.operateur == MULT) {
                            Expr* newExprLeft = (Expr*) malloc(sizeof (Expr*));
                            newExprLeft->type = OPBINAIRE;
                            newExprLeft->u.OpBinaire.left = expr_dup(leftParenthPart->u.OpBinaire.left);
                            newExprLeft->u.OpBinaire.operateur = MULT;
                            newExprLeft->u.OpBinaire.rigth = new_Par(expr_dup(rightParenthPart));

                            Expr* newExprRight = (Expr*) malloc(sizeof (Expr*));
                            newExprRight->type = OPBINAIRE;
                            newExprRight->u.OpBinaire.left = expr_dup(leftParenthPart->u.OpBinaire.rigth);
                            newExprRight->u.OpBinaire.operateur = MULT;
                            newExprRight->u.OpBinaire.rigth = new_Par(expr_dup(rightParenthPart));

                            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                            newExpr->type = OPBINAIRE;
                            newExpr->u.OpBinaire.left = expr_dup(expr_simplify(newExprLeft));
                            newExpr->u.OpBinaire.operateur = PLUS;
                            newExpr->u.OpBinaire.rigth = expr_dup(expr_simplify(newExprRight));

                            // try permutation
                            Expr* simplified = expr_simplify(newExpr);
                            if (expr_canPermute(simplified)) {
                                Expr* exprPerm = expr_doPermute(simplified);
                                simplified = expr_simplify(exprPerm);
                            }
                            return simplified;
                        } else { // PLUS
                            // try permutation
                            if (expr_canPermute(expr)) {
                                Expr* newExpr = expr_doPermute(expr);
                                expr = expr_simplify(newExpr);
                            }
                            return expr;
                        }
                    }
                } else {
                    Expr *opBinPart = leftParenthPart->type == OPBINAIRE ? leftParenthPart : rightParenthPart;
                    Expr *otherPart = leftParenthPart->type == OPBINAIRE ? rightParenthPart : leftParenthPart;
                    if (otherPart->type == ELEMENT) {
                        if (expr->u.OpBinaire.operateur == MULT) {
                            Expr* newExprLeft = (Expr*) malloc(sizeof (Expr*));
                            newExprLeft->type = OPBINAIRE;
                            newExprLeft->u.OpBinaire.left = expr_dup(opBinPart->u.OpBinaire.left);
                            newExprLeft->u.OpBinaire.operateur = MULT;
                            newExprLeft->u.OpBinaire.rigth = expr_dup(otherPart);

                            Expr* newExprRight = (Expr*) malloc(sizeof (Expr*));
                            newExprRight->type = OPBINAIRE;
                            newExprRight->u.OpBinaire.left = expr_dup(opBinPart->u.OpBinaire.rigth);
                            newExprRight->u.OpBinaire.operateur = MULT;
                            newExprRight->u.OpBinaire.rigth = expr_dup(otherPart);

                            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                            newExpr->type = OPBINAIRE;
                            newExpr->u.OpBinaire.left = expr_dup(expr_simplify(newExprLeft));
                            newExpr->u.OpBinaire.operateur = opBinPart->u.OpBinaire.operateur; // supposed to be PLUS
                            newExpr->u.OpBinaire.rigth = expr_dup(expr_simplify(newExprRight));

                            // try permutation
                            Expr* simplified = expr_simplify(newExpr);
                            if (expr_canPermute(simplified)) {
                                newExpr = expr_doPermute(simplified);
                                simplified = expr_simplify(newExpr);
                            }
                            return simplified;
                        } else {
                            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                            newExpr->type = OPBINAIRE;
                            newExpr->u.OpBinaire.left = expr_dup(otherPart);
                            newExpr->u.OpBinaire.operateur = opBinPart->u.OpBinaire.operateur; // supposed to be PLUS
                            newExpr->u.OpBinaire.rigth = expr_dup(opBinPart);

                            // try permutation
                            Expr* simplified = expr_simplify(newExpr);
                            // try permutation
                            if (expr_canPermute(simplified)) {
                                Expr* newExpr = expr_doPermute(simplified);
                                simplified = expr_simplify(newExpr);
                            }
                            return simplified;
                        }
                    } else if (otherPart->type == PAR) {
                        Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                        Expr* tmp = expr_simplify(otherPart->u.Par.single);
                        newExpr->type = OPBINAIRE;
                        newExpr->u.OpBinaire.left = tmp->type == OPBINAIRE ? new_Par(expr_dup(tmp)) : expr_dup(tmp);
                        newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
                        newExpr->u.OpBinaire.rigth = tmp->type == OPBINAIRE ? new_Par(expr_dup(opBinPart)) : expr_dup(opBinPart);
                        return expr_simplify(newExpr);
                    } else if (otherPart->type == FUNCT) {
                        Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
                        newExpr->type = OPBINAIRE;
                        newExpr->u.OpBinaire.left = expr_dup(otherPart);
                        newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
                        newExpr->u.OpBinaire.rigth = new_Par(expr_dup(opBinPart));
                        return expr_simplify(newExpr);
                    }
                }
            }
        } else if ((expr->u.OpBinaire.left->type == FUNCT
                && expr->u.OpBinaire.rigth->type == PAR)
                || (expr->u.OpBinaire.rigth->type == FUNCT
                && expr->u.OpBinaire.left->type == PAR)) {
            Expr *funcPart = expr->u.OpBinaire.left->type == FUNCT ? expr_simplify(expr->u.OpBinaire.left) : expr_simplify(expr->u.OpBinaire.rigth);
            Expr *parenthPart = expr->u.OpBinaire.left->type == PAR ? expr_simplify(expr->u.OpBinaire.left->u.Par.single) : expr_simplify(expr->u.OpBinaire.rigth->u.Par.single);
            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
            newExpr->type = OPBINAIRE;
            newExpr->u.OpBinaire.left = expr->u.OpBinaire.operateur == PLUS ? expr_dup(funcPart) : new_Par(expr_dup(funcPart));
            newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
            newExpr->u.OpBinaire.rigth = expr->u.OpBinaire.operateur == PLUS ? expr_dup(parenthPart) : new_Par(expr_dup(parenthPart));
            return expr_simplify(newExpr);
        } else if (expr->u.OpBinaire.left->type == FUNCT
                && expr->u.OpBinaire.rigth->type == FUNCT) {
            Expr *leftFuncPart = expr_simplify(expr->u.OpBinaire.left);
            Expr *rightFuncPart = expr_simplify(expr->u.OpBinaire.rigth);
            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
            newExpr->type = OPBINAIRE;
            newExpr->u.OpBinaire.left = new_Par(expr_dup(leftFuncPart));
            newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
            newExpr->u.OpBinaire.rigth = new_Par(expr_dup(rightFuncPart));
            return expr_simplify(newExpr);
        } else if ((expr->u.OpBinaire.left->type == FUNCT
                && expr->u.OpBinaire.rigth->type == ELEMENT)
                || (expr->u.OpBinaire.rigth->type == FUNCT
                && expr->u.OpBinaire.left->type == ELEMENT)) {
            Expr *funcPart = expr->u.OpBinaire.rigth->type == FUNCT ? expr->u.OpBinaire.rigth : expr->u.OpBinaire.left;
            Expr *elePart = expr->u.OpBinaire.rigth->type == ELEMENT ? expr->u.OpBinaire.rigth : expr->u.OpBinaire.left;
            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
            newExpr->type = OPBINAIRE;
            newExpr->u.OpBinaire.left = expr_dup(elePart);
            newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
            newExpr->u.OpBinaire.rigth = new_Par(expr_dup(funcPart));
            return expr_simplify(newExpr);
        } else if ((expr->u.OpBinaire.left->type == OPBINAIRE
                && expr->u.OpBinaire.rigth->type == ELEMENT)
                || (expr->u.OpBinaire.rigth->type == OPBINAIRE
                && expr->u.OpBinaire.left->type == ELEMENT)) {
            Expr *opBinPart = expr->u.OpBinaire.rigth->type == OPBINAIRE ? expr_simplify(expr->u.OpBinaire.rigth) : expr_simplify(expr->u.OpBinaire.left);
            Expr *elePart = expr->u.OpBinaire.rigth->type == ELEMENT ? expr->u.OpBinaire.rigth : expr->u.OpBinaire.left;
            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
            newExpr->type = OPBINAIRE;
            newExpr->u.OpBinaire.left = expr_dup(elePart);
            newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
            newExpr->u.OpBinaire.rigth = expr_dup(opBinPart);
            // try permutation
            if (expr_canPermute(newExpr)) {
                Expr* exprPerm = expr_doPermute(newExpr);
                return expr_simplify(exprPerm);
            }
            return newExpr;
        } else if (expr->u.OpBinaire.left->type == OPBINAIRE
                && expr->u.OpBinaire.rigth->type == OPBINAIRE) {
            Expr *leftPart = expr_simplify(expr->u.OpBinaire.left);
            Expr *rightPart = expr_simplify(expr->u.OpBinaire.rigth);
            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
            newExpr->type = OPBINAIRE;
            newExpr->u.OpBinaire.left = expr_dup(leftPart);
            newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
            newExpr->u.OpBinaire.rigth = expr_dup(rightPart);
            // try permutation
            if (expr_canPermute(newExpr)) {
                Expr* exprPerm = expr_doPermute(newExpr);
                return expr_simplify(exprPerm);
            }
            return expr_simplify(newExpr);
        } else if ((expr->u.OpBinaire.left->type == OPBINAIRE
                && expr->u.OpBinaire.rigth->type == PAR)
                || (expr->u.OpBinaire.rigth->type == OPBINAIRE
                && expr->u.OpBinaire.left->type == PAR)) {
            Expr *opBinPart = expr->u.OpBinaire.rigth->type == OPBINAIRE ? expr_simplify(expr->u.OpBinaire.rigth) : expr_simplify(expr->u.OpBinaire.left);
            Expr *parPart = expr->u.OpBinaire.rigth->type == PAR ? expr_simplify(expr->u.OpBinaire.rigth) : expr_simplify(expr->u.OpBinaire.left);
            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
            newExpr->type = OPBINAIRE;
            newExpr->u.OpBinaire.left = new_Par(expr_dup(opBinPart));
            newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
            newExpr->u.OpBinaire.rigth = new_Par(expr_dup(parPart));
            return expr_simplify(newExpr);
        } else if ((expr->u.OpBinaire.left->type == OPBINAIRE
                && expr->u.OpBinaire.rigth->type == FUNCT)
                || (expr->u.OpBinaire.rigth->type == OPBINAIRE
                && expr->u.OpBinaire.left->type == FUNCT)) {
            Expr *opBinPart = expr->u.OpBinaire.rigth->type == OPBINAIRE ? expr_simplify(expr->u.OpBinaire.rigth) : expr_simplify(expr->u.OpBinaire.left);
            Expr *functPart = expr->u.OpBinaire.rigth->type == FUNCT ? expr_simplify(expr->u.OpBinaire.rigth) : expr_simplify(expr->u.OpBinaire.left);
            Expr* newExpr = (Expr*) malloc(sizeof (Expr*));
            newExpr->type = OPBINAIRE;
            newExpr->u.OpBinaire.left = new_Par(expr_dup(opBinPart));
            newExpr->u.OpBinaire.operateur = expr->u.OpBinaire.operateur;
            newExpr->u.OpBinaire.rigth = new_Par(expr_dup(functPart));
            return expr_simplify(newExpr);
        }
    }
    return expr;
}

/**
 * expr_canPermute : si on peut faire une permutation pour mieux simplifier l'experssion ou pas
 */
int expr_canPermute(Expr * expr) {
    int canSubPermute = 0;

    // les condition sous lesquels on peut effectuer une permutation
    if (expr->type == OPBINAIRE
            && ((expr->u.OpBinaire.left->type == OPBINAIRE
            && expr->u.OpBinaire.rigth->type == OPBINAIRE
            && expr->u.OpBinaire.rigth->u.OpBinaire.operateur == expr->u.OpBinaire.operateur
            && expr->u.OpBinaire.rigth->u.OpBinaire.left->type == ELEMENT
            && expr->u.OpBinaire.rigth->u.OpBinaire.rigth->type == ELEMENT
            && expr->u.OpBinaire.left->u.OpBinaire.left->type == ELEMENT
            && expr->u.OpBinaire.left->u.OpBinaire.rigth->type == ELEMENT
            && expr->u.OpBinaire.left->u.OpBinaire.operateur == expr->u.OpBinaire.operateur)
            || (expr->u.OpBinaire.left->type == OPBINAIRE
            && expr->u.OpBinaire.rigth->type == ELEMENT
            && (expr->u.OpBinaire.left->u.OpBinaire.left->type == ELEMENT
            || expr->u.OpBinaire.left->u.OpBinaire.rigth->type == ELEMENT)
            && expr->u.OpBinaire.left->u.OpBinaire.operateur == expr->u.OpBinaire.operateur)
            || (expr->u.OpBinaire.left->type == ELEMENT
            && expr->u.OpBinaire.rigth->type == OPBINAIRE
            && (expr->u.OpBinaire.rigth->u.OpBinaire.left->type == ELEMENT
            || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->type == ELEMENT)
            && expr->u.OpBinaire.rigth->u.OpBinaire.operateur == expr->u.OpBinaire.operateur)
            || (expr->u.OpBinaire.left->type == ELEMENT
            && expr->u.OpBinaire.rigth->type == OPBINAIRE
            && (canSubPermute = expr_canPermute(expr->u.OpBinaire.rigth)))
            || (expr->u.OpBinaire.rigth->type == ELEMENT
            && expr->u.OpBinaire.left->type == OPBINAIRE
            && (canSubPermute = expr_canPermute(expr->u.OpBinaire.left)))
            )) {
        if (expr->u.OpBinaire.operateur == MULT) {
            return 1; // can Permute
        } else if (expr->u.OpBinaire.left->type == OPBINAIRE
                && expr->u.OpBinaire.rigth->type == OPBINAIRE) {
            if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power
                    || expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power
                    || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power
                    || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {
                return 1; // can Permute    //    else if (exp) {
                //
                //
                //    }


            }
        } else if (expr->u.OpBinaire.left->type == OPBINAIRE) {
            if ((expr->u.OpBinaire.left->u.OpBinaire.left->type == ELEMENT
                    && expr->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power)
                    || (expr->u.OpBinaire.left->u.OpBinaire.rigth->type == ELEMENT
                    && expr->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power)) {
                return 1; // can Permute
            }
        } else if (expr->u.OpBinaire.rigth->type == OPBINAIRE) {
            if ((expr->u.OpBinaire.rigth->u.OpBinaire.left->type == ELEMENT
                    && expr->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power)
                    || (expr->u.OpBinaire.rigth->u.OpBinaire.rigth->type == ELEMENT
                    && expr->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power)) {
                return 1; // can Permute
            }
        } else if (canSubPermute) {

            return 2; // can Permute
        }
    }
    return 0; // can NOT Permute
}

/**
 * expr_doPermute : permet de permuter deux noeud pour rapprocher des noeuds qu'ont pourra ensuite simplifier, 
 *      elle permet aussi de restructurer un arbre  pour rapprocher des noeuds orphelins qu'ont pourra ensuite simplifier, 
 */
Expr * expr_doPermute(Expr * expr) {
    int canSubPermute = 0;
    if (canSubPermute = expr_canPermute(expr)) {
        // on traite séparément la mutliplication et l'addition
        if (expr->u.OpBinaire.operateur == MULT) {
            if (expr->u.OpBinaire.left->type == ELEMENT || expr->u.OpBinaire.rigth->type == ELEMENT) {
                Expr* exprPermL = expr->u.OpBinaire.left->type == ELEMENT ? expr_dup(expr->u.OpBinaire.left) : expr_dup(expr->u.OpBinaire.rigth);

                Expr* exprPermR = expr->u.OpBinaire.left->type == OPBINAIRE ? expr_dup(expr->u.OpBinaire.left) : expr_dup(expr->u.OpBinaire.rigth);
                Expr* exprPermR_Ele = exprPermR->u.OpBinaire.left == ELEMENT ? expr_dup(exprPermR->u.OpBinaire.left) : expr_dup(exprPermR->u.OpBinaire.rigth);
                Expr* exprPermR_Other = exprPermR->u.OpBinaire.left == ELEMENT ? expr_dup(exprPermR->u.OpBinaire.rigth) : expr_dup(exprPermR->u.OpBinaire.left);

                if (exprPermR_Ele->u.Element.power > exprPermL->u.Element.power) {
                    exprPermL = new_Par(new_OpBinaire_enum(exprPermR_Ele, MULT, exprPermL));
                } else {
                    exprPermL = new_Par(new_OpBinaire_enum(exprPermL, MULT, exprPermL));
                }
                return new_OpBinaire_enum(expr_simplify(exprPermL), MULT, expr_simplify(exprPermR_Other));
            } else {
                if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power
                        || expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power
                        || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power
                        || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {
                    if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power
                            || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {

                        Expr* exprPermL;
                        if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power) {
                            exprPermL = new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.left, MULT, expr->u.OpBinaire.left->u.OpBinaire.left);
                        } else {
                            exprPermL = new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.left, MULT, expr->u.OpBinaire.rigth->u.OpBinaire.left);
                        }

                        Expr* exprPermR;
                        if (expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {
                            exprPermR = new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.rigth, MULT, expr->u.OpBinaire.left->u.OpBinaire.rigth);
                        } else {
                            exprPermR = new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.rigth, MULT, expr->u.OpBinaire.rigth->u.OpBinaire.rigth);
                        }

                        if ((expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power
                                && expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power)
                                || (expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power
                                && expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power)) {
                            return new_Par(new_OpBinaire_enum(expr_simplify(exprPermL), MULT, expr_simplify(exprPermR)));
                        } else {
                            return new_Par(new_OpBinaire_enum(expr_simplify(exprPermR), MULT, expr_simplify(exprPermL)));
                        }
                    } else if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power
                            || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power) {

                        Expr* exprPermL;
                        if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {
                            exprPermL = new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.left, MULT, expr->u.OpBinaire.left->u.OpBinaire.rigth);
                        } else {
                            exprPermL = new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.rigth, MULT, expr->u.OpBinaire.rigth->u.OpBinaire.left);
                        }

                        Expr* exprPermR;
                        if (expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power) {
                            exprPermR = new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.rigth, MULT, expr->u.OpBinaire.left->u.OpBinaire.left);
                        } else {
                            exprPermR = new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.left, MULT, expr->u.OpBinaire.rigth->u.OpBinaire.rigth);
                        }

                        if ((expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power
                                && expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power)
                                || (expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power
                                && expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power)) {
                            return new_Par(new_OpBinaire_enum(expr_simplify(exprPermL), MULT, expr_simplify(exprPermR)));
                        } else {
                            return new_Par(new_OpBinaire_enum(expr_simplify(exprPermR), MULT, expr_simplify(exprPermL)));
                        }
                    }
                }
            }
        } else if (expr->u.OpBinaire.left->type == OPBINAIRE
                && expr->u.OpBinaire.rigth->type == OPBINAIRE) {
            if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power
                    || expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power
                    || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power
                    || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {
                if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power
                        || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {

                    Expr* exprPermL;
                    if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power) {
                        exprPermL = new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.left, PLUS, expr->u.OpBinaire.left->u.OpBinaire.left);
                    } else {
                        exprPermL = new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.left, PLUS, expr->u.OpBinaire.rigth->u.OpBinaire.left);
                    }

                    Expr* exprPermR;
                    if (expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {
                        exprPermR = new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.rigth, PLUS, expr->u.OpBinaire.left->u.OpBinaire.rigth);
                    } else {
                        exprPermR = new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.rigth, PLUS, expr->u.OpBinaire.rigth->u.OpBinaire.rigth);
                    }

                    if ((expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power
                            && expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power)
                            || (expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power
                            && expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power)) {
                        return new_Par(new_OpBinaire_enum(expr_simplify(exprPermL), PLUS, expr_simplify(exprPermR)));
                    } else {
                        return new_Par(new_OpBinaire_enum(expr_simplify(exprPermR), PLUS, expr_simplify(exprPermL)));
                    }
                } else if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power
                        || expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power) {

                    Expr* exprPermL;
                    if (expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {
                        exprPermL = new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.left, PLUS, expr->u.OpBinaire.left->u.OpBinaire.rigth);
                    } else {
                        exprPermL = new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.rigth, PLUS, expr->u.OpBinaire.rigth->u.OpBinaire.left);
                    }

                    Expr* exprPermR;
                    if (expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power) {
                        exprPermR = new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.rigth, PLUS, expr->u.OpBinaire.left->u.OpBinaire.left);
                    } else {
                        exprPermR = new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.left, PLUS, expr->u.OpBinaire.rigth->u.OpBinaire.rigth);
                    }

                    if ((expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power
                            && expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power)
                            || (expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power
                            && expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power)) {
                        return new_Par(new_OpBinaire_enum(expr_simplify(exprPermL), PLUS, expr_simplify(exprPermR)));
                    } else {
                        return new_Par(new_OpBinaire_enum(expr_simplify(exprPermR), PLUS, expr_simplify(exprPermL)));
                    }
                }
            }
        } else if (expr->u.OpBinaire.left->type == OPBINAIRE) {
            if (expr->u.OpBinaire.left->u.OpBinaire.left->type == ELEMENT
                    && expr->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power) {

                Expr* exprPermL;
                if (expr->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power) {
                    exprPermL = new_Par(new_OpBinaire_enum(expr->u.OpBinaire.rigth, PLUS, expr->u.OpBinaire.left->u.OpBinaire.left));
                } else {
                    exprPermL = new_Par(new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.left, PLUS, expr->u.OpBinaire.rigth));
                }

                Expr* exprPermR = expr_dup(expr->u.OpBinaire.left->u.OpBinaire.rigth);

                if (exprPermR->u.Element.power > max(expr->u.OpBinaire.rigth->u.Element.power, expr->u.OpBinaire.left->u.OpBinaire.left->u.Element.power)) {
                    return new_OpBinaire_enum(exprPermR, PLUS, expr_simplify(exprPermL));
                } else {
                    return new_OpBinaire_enum(expr_simplify(exprPermL), PLUS, exprPermR);
                }
            } else if (expr->u.OpBinaire.left->u.OpBinaire.rigth->type == ELEMENT
                    && expr->u.OpBinaire.rigth->u.Element.power == expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {

                Expr* exprPermL = expr_dup(expr->u.OpBinaire.left->u.OpBinaire.left);
                Expr* exprPermR;
                if (expr->u.OpBinaire.rigth->u.Element.power > expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power) {
                    exprPermR = new_Par(new_OpBinaire_enum(expr->u.OpBinaire.rigth, PLUS, expr->u.OpBinaire.left->u.OpBinaire.rigth));
                } else {
                    exprPermR = new_Par(new_OpBinaire_enum(expr->u.OpBinaire.left->u.OpBinaire.rigth, PLUS, expr->u.OpBinaire.rigth));
                }

                if (exprPermL->u.Element.power > max(expr->u.OpBinaire.left->u.OpBinaire.rigth->u.Element.power, expr->u.OpBinaire.rigth->u.Element.power)) {
                    return new_OpBinaire_enum(exprPermL, PLUS, expr_simplify(exprPermR));
                } else {
                    return new_OpBinaire_enum(expr_simplify(exprPermR), PLUS, exprPermL);
                }
            }
        } else if (expr->u.OpBinaire.rigth->type == OPBINAIRE) {
            if (expr->u.OpBinaire.rigth->u.OpBinaire.left->type == ELEMENT
                    && expr->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power) {

                Expr* exprPermL;
                if (expr->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power) {
                    exprPermL = new_Par(new_OpBinaire_enum(expr->u.OpBinaire.left, PLUS, expr->u.OpBinaire.rigth->u.OpBinaire.left));
                } else {
                    exprPermL = new_Par(new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.left, PLUS, expr->u.OpBinaire.left));
                }
                Expr* exprPermR = expr_dup(expr->u.OpBinaire.rigth->u.OpBinaire.rigth);

                if (exprPermR->u.Element.power > max(expr->u.OpBinaire.rigth->u.OpBinaire.left->u.Element.power, expr->u.OpBinaire.left->u.Element.power)) {
                    return new_OpBinaire_enum(exprPermR, PLUS, expr_simplify(exprPermL));
                } else {
                    return new_OpBinaire_enum(expr_simplify(exprPermL), PLUS, exprPermR);
                }
            } else if (expr->u.OpBinaire.rigth->u.OpBinaire.rigth->type == ELEMENT
                    && expr->u.OpBinaire.left->u.Element.power == expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power) {

                Expr* exprPermL = expr_dup(expr->u.OpBinaire.rigth->u.OpBinaire.left);
                Expr* exprPermR;
                if (expr->u.OpBinaire.left->u.Element.power > expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power) {
                    exprPermR = new_Par(new_OpBinaire_enum(expr->u.OpBinaire.left, PLUS, expr->u.OpBinaire.rigth->u.OpBinaire.rigth));
                } else {
                    exprPermR = new_Par(new_OpBinaire_enum(expr->u.OpBinaire.rigth->u.OpBinaire.rigth, PLUS, expr->u.OpBinaire.left));
                }

                if (exprPermL->u.Element.power > max(expr->u.OpBinaire.rigth->u.OpBinaire.rigth->u.Element.power, expr->u.OpBinaire.left->u.Element.power)) {
                    return new_OpBinaire_enum(exprPermL, PLUS, expr_simplify(exprPermR));
                } else {
                    return new_OpBinaire_enum(expr_simplify(exprPermR), PLUS, exprPermL);
                }
            }
        } else if (canSubPermute == 2) {
            if (expr->u.OpBinaire.left->type == OPBINAIRE) {
                expr->u.OpBinaire.left = expr_doPermute(expr->u.OpBinaire.left);
                return expr;
            } else if (expr->u.OpBinaire.rigth->type == OPBINAIRE) {
                expr->u.OpBinaire.rigth = expr_doPermute(expr->u.OpBinaire.rigth);
                return expr;
            }
        }
    }
    return NULL; // can NOT Permute
}
