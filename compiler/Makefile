all:
	clang src/ast.c src/lexer.c src/main.c src/parser.c src/print.c -Iinc
	lldb a.out -o "run" -o "exit"

clean:
	rm a.out