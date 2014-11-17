
sc : ./sharechat.c 
	gcc -o sc ./sharechat.c -lncurses -lform -lpanel -lpthread
clean :
	rm -v sc
