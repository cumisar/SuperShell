run: exec
	./exec
	
shell_main.o: shell_main.c lexer.h tokens.h nodes.h ast.h parse.h
	gcc shell_main.c -o shell_main.o -c -O0 -g3 -ggdb -Wall -Wextra
	
lexer.o: lexer.c lexer.h tokens.h nodes.h ast.h parse.h
	gcc lexer.c -o lexer.o -c -O0 -g3 -ggdb -Wall -Wextra
	
parse.o: parse.c lexer.h tokens.h nodes.h ast.h parse.h
	gcc parse.c -o parse.o -c -O0 -g3 -ggdb -Wall -Wextra
	
ast.o: ast.c lexer.h tokens.h nodes.h ast.h parse.h
	gcc ast.c -o ast.o -c -O0 -g3 -ggdb -Wall -Wextra

exec.o: exec.c nodes.h
	gcc exec.c -o exec.o -c -O0 -g3 -ggdb -Wall -Wextra
	
exec: shell_main.o lexer.o parse.o ast.o exec.o
	gcc shell_main.o lexer.o parse.o ast.o exec.o -o exec -O0 -g3 -ggdb -Wall -Wextra -lm

clean:
	-rm -f ./*.o ./exec

.PHONY: exec clean
