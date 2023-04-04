all:
	clang src/ap.c src/ast.c src/builder.c src/cleaner.c src/error.c src/lexer.c src/main.c src/parser.c src/print.c src/token.c src/util.c -Iinc
	lldb a.out -o "run" -o "exit"

clean:
	rm a.out