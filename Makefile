
sc : ./sharechat.c 
	gcc -o sc ./sharechat.c -lncurses -lform
clean :
	rm -v sc
