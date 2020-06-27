CC = gcc
EXEC = dumsh

dumsh:./sources/main.o ./sources/dumsh.o ./sources/string_utils.o
	$(CC) -o $(EXEC) ./sources/main.o ./sources/dumsh.o ./sources/string_utils.o
	./$(EXEC)

./sources/main.o:./sources/main.c ./headers/dumsh.h ./headers/string_utils.h
	$(CC) -o ./sources/main.o -c ./sources/main.c

./sources/dumsh.o:./sources/dumsh.c ./headers/dumsh.h ./headers/string_utils.h
	$(CC) -o ./sources/dumsh.o -c ./sources/dumsh.c

./sources/string_utils.o:./sources/string_utils.c ./headers/string_utils.h
	$(CC) -o ./sources/string_utils.o -c ./sources/string_utils.c

clean:
	rm -rf ./sources/*.o

mrproper:clean
	rm -rf $(EXEC)