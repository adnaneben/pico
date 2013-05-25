%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parserExpr.h"

static Commande cmd;
%}

%union
{
    char* string;
    PrintFormat fmt;
    Expr* e;
    Ident_list* list;
    int level;
}

%token TK_FUNC
%token TK_PRINT
%token TK_EVAL
%token TK_SIMPLIFY
%token TK_PLOT

%token <fmt> TK_PrintFormat
%token <string> TK_STRING TK_IDENT TK_REEL TK_INTEGER

%token TK_X
%token TK_COLON
%token TK_QUOTE
%token TK_LPAREN
%token TK_RPAREN
%token TK_PLUS
%token TK_MULTIPLY
%token TK_SEP

%type<e> expr element
%type<string>  string_optional number ident_optional
%type<fmt>  printFormat_Optional
%type<list> ident_list
%type<level> quoteList_optional TK_QUOTE

%left TK_PLUS TK_MULTIPLY

%start debut

%%
debut : script ;

script
    : script commande
    |
;

commande
    : TK_FUNC TK_IDENT TK_COLON expr TK_SEP
        {
            cmd.type=FUNC;
            cmd.u.IntExpr.name=$2;
            cmd.u.IntExpr.expr = $4;
            return 0;
        }
    | TK_PRINT printFormat_Optional TK_IDENT string_optional TK_SEP
        {
            cmd.type=PRINT;
            cmd.u.Print.name=$3;
            cmd.u.Print.fmt = $2;
            cmd.u.Print.filename=$4;
            return 0;
	
        }
    | TK_EVAL TK_IDENT number TK_SEP
        {
            cmd.type=EVAL;
            cmd.u.Eval.name=$2;
            if(cmd.u.Eval.number == REEL)
                cmd.u.Eval.valeur=atof($3);
            else cmd.u.Eval.val=atoi($3);
            return 0;
        }
    | TK_SIMPLIFY TK_IDENT ident_optional TK_SEP
        {
            cmd.type= SIMP;
            cmd.u.Simplify.ident1 = $2;
            cmd.u.Simplify.ident2 = $3;
            return 0;
        }
    | TK_PLOT ident_list minMax_optional TK_SEP
        {
            cmd.type=PLOT;
            cmd.u.Plot.list=$2;
            return 0;
        }
;

number
    :TK_REEL  {$$=$1 ; cmd.u.Eval.number=REEL;}
    |TK_INTEGER {$$=$1 ; cmd.u.Eval.number=INT;}
;

minMax_optional
    : TK_INTEGER TK_INTEGER {cmd.u.Plot.min = atof($1) ;cmd.u.Plot.max = atof($2);}
    |                       {cmd.u.Plot.min = -100 ;cmd.u.Plot.max =100;}
;

ident_list
    : ident_list TK_IDENT {$$ = new_ident_list($2,$1);}
    | TK_IDENT { $$ = new_ident_list($1,NULL);  }
;

ident_optional
    : TK_COLON TK_IDENT { $$ = $2 ;}
    |                   { $$ = "";}
;

printFormat_Optional
    : TK_PrintFormat {$$=$1;}
    |                {$$=EXPR_AR;}
;

string_optional
    : TK_STRING {$$=$1;}
    |           {$$="default";}
;

expr 
    : TK_LPAREN expr TK_RPAREN {$$ = new_Par($2); }
    | expr TK_MULTIPLY expr  {$$ = new_OpBinaire($1,'*',$3); }
    | expr TK_PLUS expr {$$ = new_OpBinaire($1,'+',$3);}
    | TK_IDENT quoteList_optional TK_LPAREN expr TK_RPAREN {$$ = new_Func_Deriv($1,$2,$4);}
    | element { $$ = $1;}
;

quoteList_optional
    : quoteList_optional TK_QUOTE { $$ = ($1 + 1); }
    | { $$ = 0; }
;

element  
    : TK_REEL {$$=new_Element(atof($1),0,REEL,VAL);}
    | TK_INTEGER { $$=new_Element(atof($1),0,INT,VAL);}
    | TK_X { $$=new_Element((float)1,1,INT,VAR); }
;

%%

#include "lex.yy.c"
int parsing(Commande * c)
{
    int ret = yyparse();
    if(ret == 0)
    {
        *c = cmd;
        return 1;
    }
    else if(ret == -1)
    {
        return -1;
    }
    else 
        return 0;
}