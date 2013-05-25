/* 
 * File:   exprList.c
 * Author: 
 * le module qui gère la liste d'expression stocké dans un hashtable
 * Created on April 24, 2013, 12:07 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>

#include "exprList.h"

#define MAXSIZE 100

/**
 * initialize le hashtable
 */
char* listKeys[MAXSIZE];
Expr* listExpr[MAXSIZE];
int count;

/**
 * ajouter un element au hastable
 */
void exprList_add(char* name, Expr* expr) {
    /* there should be no failures */
    if (count == MAXSIZE) {
        fprintf(stderr, "entry add failed\n");
        exit(EXIT_FAILURE);
    }
    listKeys[count] = strdup(name);
    listExpr[count] = expr;
    count++;
}

/**
 * met à jour un element dans le hashtable
 */
Expr* exprList_update(char* name, Expr* expr) {
    int position = exprList_getNameIndex(name);
    /* item not found */
    if (position == -1) {
        return NULL;
    }
    //free(listExpr[position]);
    return listExpr[position] = expr;
}

/**
 * récupère un element dans le hashtable depuis sont identifiant
 */
Expr* exprList_get(char* name) {
    int position = exprList_getNameIndex(name);
    /* item not found */
    if (position == -1) {
        return NULL;
    }
    return listExpr[position];
}

/**
 * exprList_getNameIndex : get the key index
 * @param name the key
 * @return the index of the key 
 */
int exprList_getNameIndex(char* name) {
    int i = 0;
    for (; i < count; i++) {
        if (strcmp(listKeys[i], name) == 0)
            return i;
    }
    return -1;
}

/**
 * détruit le hashtable et libère les ressources mémoire utilisé par ce dernier
 */
void exprList_destroy() {
    int i = 0;
    for (; i < count; i++) {
        free(listExpr[i]);
    }
    count = 0;
}
