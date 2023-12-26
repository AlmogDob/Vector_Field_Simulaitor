build: simulaitor.c
	gcc -Wall -Wextra ./simulaitor.c -lSDL2 -lSDL2_ttf -lm -o simulaitor

run:
	./simulaitor

clean:
	rm simulaitor

test:
	gcc -Wall -Wextra ./test.c -lSDL2 -lSDL2_ttf -lm -o test
	./test
	rm test
