/* 
 * File:   parser.h
 * Author: 
 *
 * Created on May 6, 2013, 12:43 AM
 */

#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

/*
 * structure qui represente le print format
 */
typedef enum _PrintFormat {
    EXPR_AR,
    ARB_AR,
    POLY_AR
} PrintFormat;

/**
 * enumération du type de commande
 */
typedef enum _KindCmd {
    FUNC,
    PRINT,
    EVAL,
    SIMP,
    PLOT
} KindCmd;

/**
 * enumération du type d'expression
 */
typedef enum _ExprKind {
    ELEMENT,
    OPBINAIRE,
    FUNCT,
    PAR
} ExprKind;

/**
 * enumération de l'opération
 */
typedef enum _Op {
    PLUS,
    MULT
} Op;

/**
 * enumération du type de constante
 */
typedef enum _CST {
    INT,
    REEL
} TypeCst;

/**
 * enumération du type d'element
 */
typedef enum _ElType {
    VAL,
    VAR,
    VALVAR
} ElType;

/**
 * strucutre de l'expression
 */
typedef struct _Expr {
    ExprKind type;

    union {

        struct _Element {
            TypeCst typecst;
            float value;
            int power;
            ElType elType;
        } Element;

        struct _OpBinaire {
            Op operateur;
            struct _Expr * left;
            struct _Expr * rigth;
        } OpBinaire;

        struct _Func {
            char* name;
            int derivative;
            struct _Expr * child;
        } Func;

        struct _Par {
            struct _Expr * single;
        } Par;
    } u;
} Expr;

/**
 * liste chainé d'identifiant
 */
typedef struct _Ident_list {
    char * ident;
    struct _Ident_list * suivant;
} Ident_list;

/**
 * strucutre d'une commande utilisateur
 */
typedef struct _Commande {
    KindCmd type;

    union {

        struct _IntructionExp {
            char* name;
            Expr * expr;
        } IntExpr;

        struct _InstructionPrint {
            char* name; // nom de la fonction
            PrintFormat fmt; // le format demande
            char* filename;
        } Print;

        struct _InstructionEval {
            char* name; // nom de la fonction
            TypeCst number;
            float valeur;
            int val;
        } Eval;

        struct _Plot {
            int min;
            int max;
            Ident_list * list;
        } Plot;

        struct _Simplify {
            char * ident1;
            char * ident2;
        } Simplify;
    } u;
} Commande;

/**
 * création d'une expression d'element
 * @param 
 * @param 
 * @param 
 * @param 
 * @return 
 */
extern Expr * new_Element(float, int, TypeCst, ElType);

/**
 * création d'une expression d'opération binaire
 * @param 
 * @param 
 * @param 
 * @return 
 */
extern Expr * new_OpBinaire(Expr *, char, Expr *);

/**
 * création d'une expression d'opération binaire avec l'énumération Op
 * @param 
 * @param 
 * @param 
 * @return 
 */
extern Expr * new_OpBinaire_enum(Expr *, Op, Expr *);

/**
 * création d'une expression d'element à partir d'une référence à une fonction
 * @param 
 * @param 
 * @return 
 */
extern Expr * new_Func(char *, Expr *);

/**
 * création d'une expression d'element à partir d'une référence à une fonction qui peut être dérivé
 * @param 
 * @param 
 * @return 
 */
extern Expr * new_Func_Deriv(char *, int, Expr *);

/**
 * création d'une expression d'element avec parenthèse
 * @param 
 * @return 
 */
extern Expr * new_Par(Expr *);

/**
 * création d'une liste d'identifiant en liste chainé
 * @param 
 * @param 
 * @return 
 */
extern Ident_list * new_ident_list(char *, Ident_list *);

/*
 * Function parsing
 */
int parsing(Commande * e);

/**
 * expr_dup duplique une expression
 * @param l'experssion a dupliquer
 * @return le résultat de la duplication
 */
Expr* expr_dup(Expr*);

/**
 * exprPow : retourne l'exponentiel aplatiné d'une expression
 * @param expr l'expression à traiter
 * @param ntimes la puissance
 * @return le résultat de la puissance
 */
Expr* exprPow(Expr*, int);

#endif // PARSER_H_INCLUDED
