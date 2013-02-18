# requires SDL libs

all: NbNetController

NbNetController:
	gcc `sdl-config --cflags`  `sdl-config --libs` -o NbNetController NbNetController.c

clean:
	-rm NbNetController

