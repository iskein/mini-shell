main : console.o builtin.o nonbuiltin.o
	gcc -o main console.o builtin.o nonbuiltin.o
console.o : console.c builtin.h nonbuiltin.h
	gcc -c console.c
builtin.o : builtin.c builtin.h
	gcc -c builtin.c
nonbuiltin.o : nonbuiltin.c nonbuiltin.h
	gcc -c nonbuiltin.c

clean : 
	rm main *.o
