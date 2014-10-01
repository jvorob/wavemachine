#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>


#define pi 3.14159265358979323

SDL_Renderer *setupWindow();
Uint32 timercallback(Uint32 interval, void *param);
void cleanup();


SDL_Renderer *ren;
int CustomTimerEvent;
int timerlock = 0;//if 1, dont send timerevents

int main(int argc, char **argv){
	ren = setupWindow();


	if((CustomTimerEvent = SDL_RegisterEvents(1)) == -1){
		printf("Failed to register customevent\n");
		exit(-1);
	}

	SDL_AddTimer(10, timercallback, NULL);

	int quit = 0;
	SDL_Event e;

	SDL_Point point;
	point.x = 320;
	point.y = 240;
	double theta = 0;
	double pointerlength = 50;
	double dtheta = 0.5;

	int temp;

	while (!quit) {
		while ((temp = SDL_PollEvent(&e)) != 0) {

			if(e.type == SDL_QUIT) {
				quit = 1;
				break;
			} else if(e.type == SDL_USEREVENT) {

				SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
				SDL_RenderClear(ren);


				int dx = (int)(cos(theta * 2 * pi / 360) * pointerlength);
				int dy = (int)(sin(theta * 2 * pi / 360) * pointerlength);

				SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
				SDL_RenderDrawLine(ren, point.x, point.y, point.x + dx, point.y + dy);
				SDL_RenderPresent(ren);


				theta+= dtheta;
				timerlock = 0;
			}
		}
	}


	cleanup();
	return 0;
}

void cleanup() {
	SDL_DestroyRenderer(ren);
	SDL_Quit();
}

Uint32 timercallback(Uint32 interval, void *param) {

	SDL_Event event;
	SDL_zero(event);
	event.type = CustomTimerEvent;

	if(!timerlock){
		SDL_PushEvent(&event);
		timerlock = 1;
	}

	return interval;
}

SDL_Renderer *setupWindow() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("SDL_Init Error: %s",SDL_GetError());
		exit(-1);
	}

	SDL_Window *win = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == NULL){
		printf("SDL_CreateWindow Error: %s",SDL_GetError());
		SDL_Quit();
		exit(-1);
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL){
		SDL_DestroyWindow(win);
		printf("SDL_CreateRenderer Error: %s",SDL_GetError());
		SDL_Quit();
		exit(-1);
	}

	return ren;
}
