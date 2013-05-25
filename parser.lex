%option noyywrap

DIGIT       [0-9]+

%%
 
func                        { return TK_FUNC; }
print                       { return TK_PRINT; }
eval                        { return TK_EVAL; }
simplify                    { return TK_SIMPLIFY; }
plot                        { return TK_PLOT; }
x                           { return TK_X; }
[']                         { return TK_QUOTE; }
[a-zA-Z][a-zA-Z0-9]*        { yylval.string = strdup(yytext); return TK_IDENT; }
\"[^\n]*\"                  { yytext[yyleng-1] = 0; yylval.string = strdup(yytext+1); return TK_STRING; }
("-")?{DIGIT}               { yylval.string = strdup(yytext); return TK_INTEGER;}
("-")?{DIGIT}(\.{DIGIT})    { yylval.string = strdup(yytext);  return TK_REEL; }
"*"                         { return TK_MULTIPLY; }
"+"                         { return TK_PLUS; }
"("                         { return TK_LPAREN; }
")"                         { return TK_RPAREN; }
":"                         { return TK_COLON; }
"/e"                        { yylval.fmt = EXPR_AR;  return TK_PrintFormat; }
"/a"                        { yylval.fmt = ARB_AR ; return TK_PrintFormat; }
"/p"                        { yylval.fmt = POLY_AR ; return TK_PrintFormat;  }
[\n]                        { return TK_SEP; }
[ \t]*                      { }

%%

int yyerror(const char *msg) {
    fprintf(stderr,"Error:%s (near <%s>)\n",msg,yytext); 
    return 0;
}
