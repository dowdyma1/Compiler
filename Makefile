program: driver.c lex_analyzer.c parser.c codegen.c vm.c
	gcc -o prog driver.c lex_analyzer.c parser.c codegen.c vm.c

clean:
	rm prog
