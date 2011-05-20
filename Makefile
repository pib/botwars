CFLAGS=-I./includes `sdl-config --cflags` -g -c -Wall
#LIBS=-L./libs libs/SDL_prim.o ./libs/SDL_ttf.o `freetype-config --libs` `sdl-config --libs` -lm
# set libs to the following for non-precompiled libs...
LIBS=-L./libs libs/SDL_prim.o -lSDL_ttf `freetype-config --libs` `sdl-config --libs` -lm

all: server client_dumb client_stupid client_notsmart

run: all
	./server ./fonts/FreeMonoBold.ttf 12

server: server.o servermain.o libs/SDL_prim.o robot.o servernet.o serverdraw.o
	gcc -g ${LIBS} servermain.o serverdraw.o servernet.o server.o robot.o -o server

client_dumb: client_dumb.o
	gcc -g client_dumb.o -o client_dumb

client_dumb.o: client_dumb.c includes/client.h
	gcc client_dumb.c ${CFLAGS}

client_notsmart: client_notsmart.o clientnet.o
	gcc -g client_notsmart.o clientnet.o -o client_notsmart

client_notsmart.o: client_notsmart.c includes/client.h
	gcc client_notsmart.c ${CFLAGS}

client_stupid: client_stupid.o clientnet.o
	gcc -g client_stupid.o clientnet.o -o client_stupid

client_stupid.o: client_stupid.c includes/client.h
	gcc client_stupid.c ${CFLAGS}

clientnet.o: clientnet.c includes/client.h
	gcc clientnet.c ${CFLAGS}

server.o: server.c includes/server.h includes/mytypes.h includes/robonet.h
	gcc server.c ${CFLAGS} 

servermain.o: servermain.c includes/server.h includes/mytypes.h includes/robonet.h
	gcc servermain.c ${CFLAGS} 

serverdraw.o: serverdraw.c includes/server.h includes/mytypes.h includes/robonet.h
	gcc serverdraw.c ${CFLAGS} 

servernet.o: servernet.c includes/server.h includes/mytypes.h includes/robonet.h
	gcc servernet.c ${CFLAGS} 

robot.o: robot.c includes/robot.h
	gcc robot.c ${CFLAGS}

libs/SDL_prim.o:
	gcc libs/SDL_prim.c -o libs/SDL_prim.o ${CFLAGS}

clean:
	rm -f *.o server client_dumb client_notsmart client_stupid

tags: *.c includes/*.h
	ctags -R .
	ctags -R -a tags /usr/include/SDL
