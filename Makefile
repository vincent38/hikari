all: hikari
	@echo "[SUCCESS] Program compiled."

hikari: ./src/hikari.o ./src/mc48.o
	gcc -o ./hikari ./src/hikari.o ./src/mc48.o -O3 -lm

hikari.o: ./src/hikari.c
	gcc -o ./src/hikari.o -c ./src/hikari.c -O3 -lm

mc48.o: ./src/mc48.c ./src/mc48.h
	gcc -o ./src/mc48.o -c ./src/mc48.c -O3 -lm

clean:
	rm ./src/*.o
	@echo "[SUCCESS] Sources deleted."

mrproper: clean
	rm hikari
	@echo "[SUCCESS] Program deleted."

.PHONY: all clean
