threadmake: main.c my_pthread.c
	gcc -ggdb -lm -o threadmake main.c my_pthread.c -I.

