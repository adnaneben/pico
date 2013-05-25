# 
# ce Make file du projet PICO propose les deux actions de [build] et [clean]
#

MKDIR=mkdir
RM=rm -f
BuildDir=bin
GccOptions=-g
FlexOptions=-v
BisonOptions=-v
GccComplie=gcc -c
GccLink=gcc

# commande de précomplication qui crée le répertoir bin où vont etre créer les objects compilé et le bin du projet
preCompile: 
	${MKDIR} -p ${BuildDir}
	$(info ************ ${BuildDir} )

# génération flex
lexyy: parser.lex
	flex ${FlexOptions} parser.lex
	$(info ************ lex.yy.c )

# génération bison
parsertab : lexyy parser.y
	bison ${BisonOptions} parser.y
	$(info ************ parser.tab.c )

# le module du parser lexical/grammatical
${BuildDir}/parser.tab.o: preCompile parsertab
	${GccComplie} ${GccOptions} parser.tab.c -o ${BuildDir}/parser.tab.o
	$(info ************ ${BuildDir}/parser.tab.o )


# le module parser qui expose les fonctions de création d'expression
${BuildDir}/parserExpr.o: preCompile lexyy parser.c parserExpr.h
	${GccComplie} ${GccOptions} parserExpr.c -o ${BuildDir}/parserExpr.o
	$(info ************ ${BuildDir}/parserExpr.o )

# le module print
${BuildDir}/print.o: preCompile print.h print.c
	${GccComplie} ${GccOptions} print.c -o ${BuildDir}/print.o
	$(info ************ ${BuildDir}/print.o )

# le module eval
${BuildDir}/eval.o: preCompile eval.h eval.c
	${GccComplie} ${GccOptions} eval.c -o ${BuildDir}/eval.o
	$(info ************ ${BuildDir}/eval.o )

# le module simplify
${BuildDir}/simplify.o: preCompile simplify.h simplify.c
	${GccComplie} -c ${GccOptions} simplify.c -o ${BuildDir}/simplify.o
	$(info ************ ${BuildDir}/simplify.o )

# le module plot
${BuildDir}/plot.o: preCompile plot.h plot.c
	${GccComplie} ${GccOptions} plot.c -o ${BuildDir}/plot.o
	$(info ************ ${BuildDir}/plot.o )

# le module exprList
${BuildDir}/exprList.o: preCompile exprList.h exprList.c
	${GccComplie} ${GccOptions} exprList.c -o ${BuildDir}/exprList.o
	$(info ************ ${BuildDir}/exprList.o )

# la commande du build
build: preCompile lexyy parsertab ${BuildDir}/parser.tab.o ${BuildDir}/parserExpr.o ${BuildDir}/print.o ${BuildDir}/eval.o ${BuildDir}/simplify.o ${BuildDir}/plot.o ${BuildDir}/exprList.o main.c
	${GccLink} ${GccOptions} -o ${BuildDir}/pico main.c ${BuildDir}/parserExpr.o ${BuildDir}/parser.tab.o ${BuildDir}/print.o ${BuildDir}/eval.o ${BuildDir}/simplify.o ${BuildDir}/plot.o ${BuildDir}/exprList.o -lm
	$(info ************ ${BuildDir}/pico )
	$(info ***************************** )
	$(info ************ Le projet vient d'etre compilé dans le répertoir [${BuildDir}] )
	$(info ************ pour lancer le binaire taper : ./${BuildDir}/pico )

# la commande du clean
clean: preCompile
	${RM} ${BuildDir}/pico ${BuildDir}/*.o parser.tab.c lex.yy.c parser.output *.o *.mk
	$(info ************ Répertoir [${BuildDir}] nettoyé )
