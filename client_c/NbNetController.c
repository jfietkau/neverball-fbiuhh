/**
 * This is a client for the FBIUHH network control patch for Neverball 1.5.4,
 * which allows the game to be controlled over a TCP socket. This is a
 * graphical client implemented in C using the SDL library. It connects to
 * Neverball and sends values for x and z tilting that can be changed via the
 * cursor keys.
 * 
 * (C) 2012 Julian Fietkau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*---------------------------------------------------------------------------*/

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

/* Each message is 8 bytes long (actually contains 4 signed shorts, little endian). */
#define FBIUHH_MSG_SIZE 8

#ifdef WIN32
#include <sleep.h>
#endif

#ifndef WIN32
#include <unistd.h>
#define closesocket close
#define sleep(i) usleep(i * 1000)
#endif

int sign(int v)
{
    return v > 0 ? 1 : (v < 0 ? -1 : 0);
}

int main(int argc, char *argv[])
{
    /* Initialise variables. */
    int socket_c, port;
    char message[FBIUHH_MSG_SIZE];
    port = 33333;

    int x, z, r, c;
    x = z = r = c = 0;

    /* Create network socket. */
    if ((socket_c = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failure to initialize network socket.\n");
        return 0;
    }

    /* Create socket as non-blocking to prevent congestion, might create other issues. */
/*
    int flags;
    flags = fcntl(socket_c, F_GETFL, 0);
    fcntl(socket_c, F_SETFL, flags | O_NONBLOCK);
*/

    /* Initialise socket connection. */
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (argc > 1)
    {
        /* Server name may be provided via command line. */
        server = gethostbyname(argv[1]);
    }
    else
    {
        server = gethostbyname("localhost");
    }
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host.\n");
        return 0;
    }

    /* Initialize connection details. */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    signal(SIGPIPE, SIG_IGN);

    /* Create SDL context. */
    SDL_Event event;
    SDL_Surface* screen; 
    SDL_Init( SDL_INIT_EVERYTHING );
    screen = SDL_SetVideoMode(300, 100, 32, SDL_SWSURFACE);
    SDL_WM_SetCaption("FBIUHH Neverball Controller", NULL);
    SDL_EnableKeyRepeat(5, 5);

    /* Run the main loop. */
    int result;
    int counter;
    counter = 0;
    int running = 1;
    int connection_lost = 1;
    while (running)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_UP:    x += 1000; break;
                    case SDLK_DOWN:  x -= 1000; break;
                    case SDLK_LEFT:  z -= 1000; break;
                    case SDLK_RIGHT: z += 1000; break;
                }
                if(x > 32000) x = 32000;
                if(x < -32000) x = -32000;
                if(z > 32000) z = 32000;
                if(z < -32000) z = -32000;
            }
            if(event.type == SDL_QUIT)
            {
                running = 0;
            }
        }
        bzero(message, FBIUHH_MSG_SIZE);
        message[0] = x % 256;
        message[1] = x >> 8;
        message[2] = z % 256;
        message[3] = z >> 8;
        // TODO: message[4] and message[5] provide camera rotation,
        //       message[6] and message[7] contain further camera settings (undocumented).
        counter++;
        if(connection_lost == 1)
        {
            if (connect(socket_c, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            {
                fprintf(stderr,"ERROR establishing connection (Error %d)\n", errno);
            }
            else
            {
                connection_lost = 0;
            }
        }
        if(counter >= 200)
        {
            result = write(socket_c, message, FBIUHH_MSG_SIZE);
            if (result < 0)
            {
                fprintf(stderr, "Connection lost.\n");
                connection_lost = 1;
            }
            counter = 0;
        }
        sleep(50);
    }

    /* Finalize. */
    closesocket(socket_c);
    SDL_Quit();
    return 0;
}

/*---------------------------------------------------------------------------*/

