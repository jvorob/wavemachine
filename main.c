#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>


#include "sim.h"

#define pi 3.14159265358979323

SDL_Renderer *setupWindow();
Uint32 timercallback(Uint32 interval, void *param);
void cleanup();
void draw(SDL_Renderer *r, sim_Sim *s);


SDL_Renderer *ren;
int CustomTimerEvent;
int timerlock = 0;//if 1, dont send timerevents

int main(int argc, char **argv){
	ren = setupWindow();


	if((CustomTimerEvent = SDL_RegisterEvents(1)) == -1){
		printf("Failed to register customevent\n");
		exit(-1);
	}

	SDL_AddTimer(100, timercallback, NULL);

	int quit = 0;
	SDL_Event e;

	int WIDTH = 10, HEIGHT = 10;
	sim_Sim *mysim = sim_CreateSimulation(WIDTH, HEIGHT, 50.0);
	mysim->curr[0].height = 256;
	mysim->curr[1].height = 256;
	mysim->curr[2].height = 256;
	mysim->curr[3].height = 256;
	mysim->curr[4].height = 256;
	mysim->curr[5].height = 256;
	mysim->curr[8].height = 256;
	mysim->curr[WIDTH * HEIGHT - 1].height = 256;
	mysim->curr[WIDTH * HEIGHT - 2].height = 256;
	mysim->curr[WIDTH * HEIGHT - 3].height = 256;
	mysim->curr[WIDTH * HEIGHT - 4].height = 256;
	mysim->curr[WIDTH * HEIGHT - 5].height = 256;
	mysim->curr[WIDTH * HEIGHT - 6].height = 256;

	int temp;

	while (!quit) {
		while ((temp = SDL_PollEvent(&e)) != 0) {

			if(e.type == SDL_QUIT) {
				quit = 1;
				break;
			} else if(e.type == SDL_USEREVENT) {
				draw(ren, mysim);
				sim_step(mysim);
				timerlock = 0;
			}
		}
	}


	cleanup();
	return 0;
}

void draw(SDL_Renderer *r, sim_Sim *s) {
	int i, j;
	int rectwidth = 32, rectheight = 32;
	int debugprint = 1;
	int drawflow = 1;
	sim_Cell *cell;
	SDL_Rect rect;
	rect.w = rectwidth;
	rect.h = rectheight;

	SDL_SetRenderDrawColor(r, 200, 255, 200, 255);
	SDL_RenderClear(r);

	for(i = 0; i < s->h; i++) {
		for(j = 0; j < s->w; j++) {
			cell = &(cellAt(j,i,s));

			int rg = 255 - (int)cell->height;
			rg = rg < 0 ? 0 : rg;
			rect.x = j * rectwidth;
			rect.y = i * rectheight;

			SDL_SetRenderDrawColor(ren, rg, rg, 255, 255); 
			SDL_RenderFillRect(ren, &rect);

			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);


			if(debugprint) fprintf(stderr,"%4.0lf ", cell->height);
		}
		if(debugprint) fprintf(stderr,"\n");
	}	
	if(debugprint) fprintf(stderr, "\nAvgheight:%lf\n---\n\n", sim_avgheight(s));
	if(debugprint) fprintf(stderr, "\n---\n\n");

	if(drawflow)
		for(i = 0; i < s->h; i++) {
			for(j = 0; j < s->w; j++) {
				cell = &(cellAt(j,i,s));
				rect.x = j * rectwidth;
				rect.y = i * rectheight;
				int len;
				len = cell->flow[RT];
				SDL_RenderDrawLine(ren, 
					rect.x + rectwidth / 2,
					rect.y + rectheight / 2,
					rect.x + rectwidth / 2 + len,
					rect.y + rectheight / 2);
				len = cell->flow[LF];
				SDL_RenderDrawLine(ren, 
					rect.x + rectwidth / 2,
					rect.y + rectheight / 2,
					rect.x + rectwidth / 2 - len,
					rect.y + rectheight / 2);
				len = cell->flow[DN];
				SDL_RenderDrawLine(ren, 
					rect.x + rectwidth / 2,
					rect.y + rectheight / 2,
					rect.x + rectwidth / 2,
					rect.y + rectheight / 2 + len);
				len = cell->flow[UP];
				SDL_RenderDrawLine(ren, 
					rect.x + rectwidth / 2,
					rect.y + rectheight / 2,
					rect.x + rectwidth / 2,
					rect.y + rectheight / 2 - len);
			}
		}

	SDL_RenderPresent(r);
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
