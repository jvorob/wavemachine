#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sim.h"

void _reset_buffer(sim_Sim *s);
void _swap_buffers(sim_Sim *s);


#define FLOWCONSTANT 10.0 //Multiplier for delta pressure -> flow
#define FLOWEQUALIZINGRATE 0.1 //Multiplier for how easy it is to change flow


void _createflow(sim_Sim *s, int x, int y, double time) {//time elapsed in seconds
	int i;
	sim_Cell *cc, *bc;//center (x,y)
	sim_Cell *currs[4]; //current up, down, left right
	sim_Cell *buffs[4]; //next tick up, down, left right
	double heightdiff[4]; //>0 means flowing in
	double netflow;

	cc = &cellAt			(x, y, s);
	bc = &cellAtBuff 		(x, y, s);
	currs[UP] = &cellAt		(x, y - 1, s);
	buffs[UP] = &cellAtBuff		(x, y - 1, s);
	currs[DN] = &cellAt		(x, y + 1, s);
	buffs[DN] = &cellAtBuff		(x, y + 1, s);
	currs[LF] = &cellAt		(x - 1, y, s);
	buffs[LF] = &cellAtBuff		(x - 1, y, s);
	currs[RT] = &cellAt		(x + 1, y, s);
	buffs[RT] = &cellAtBuff		(x + 1, y, s);

	if(x == 0)
		currs[LF] = NULL;
	if(x == s->w - 1)
		currs[RT] = NULL;
	if(y == 0)
		currs[UP] = NULL;
	if(y == s->h - 1)
		currs[DN] = NULL;

	for(i = 0; i < 4; i++) {
		if(currs[i]== NULL) {
			heightdiff[i] = 0;
			continue;
		}
		heightdiff[i] = currs[i]->height - cc->height;
	}

	//1 unit of flow = 1 height / second
	//zflow =~ flow
	netflow = 0;
	for(i = 0; i < 4; i++) {
		double temp = FLOWCONSTANT * sqrt(fabs(heightdiff[i])) * (heightdiff[i] < 0 ? 1 : -1);
		double flowdiff;
		flowdiff = temp - bc->flow[i];
		bc->flow[i] += time * FLOWEQUALIZINGRATE * flowdiff;
		netflow += bc->flow[i];
	}

}


void _equalizeflow(sim_Sim *s, int x, int y, double time) {
	int i;
	sim_Cell *cc, *bc;//center (x,y)
	sim_Cell *currs[4]; //current up, down, left right
	sim_Cell *buffs[4]; //next tick up, down, left right
	double heightdiff[4]; //>0 means flowing in
	double netflow;

	cc = &cellAt			(x, y, s);
	bc = &cellAtBuff 		(x, y, s);
	currs[UP] = &cellAt		(x, y - 1, s);
	buffs[UP] = &cellAtBuff		(x, y - 1, s);
	currs[DN] = &cellAt		(x, y + 1, s);
	buffs[DN] = &cellAtBuff		(x, y + 1, s);
	currs[LF] = &cellAt		(x - 1, y, s);
	buffs[LF] = &cellAtBuff		(x - 1, y, s);
	currs[RT] = &cellAt		(x + 1, y, s);
	buffs[RT] = &cellAtBuff		(x + 1, y, s);

	if(x == 0)
		currs[LF] = NULL;
	if(x == s->w - 1)
		currs[RT] = NULL;
	if(y == 0)
		currs[UP] = NULL;
	if(y == s->h - 1)
		currs[DN] = NULL;


	for(i = 0; i < 4; i++) {
		if(currs[i]== NULL)
			continue;
		
		double dirflow;//net flow in the given direction
		dirflow = cc->flow[i] - currs[i]->flow[(i+2)%4];
		dirflow /= 2;
		bc->flow[i] = dirflow;
		buffs[i]->flow[(i+2)%4] = -1 * dirflow;
	}
}

void _movewater(sim_Sim *s, int x, int y, double time) {
	int i;
	sim_Cell *cc, *bc;//center (x,y)
	sim_Cell *currs[4]; //current up, down, left right
	sim_Cell *buffs[4]; //next tick up, down, left right
	double heightdiff[4]; //>0 means flowing in
	double netflow;

	cc = &cellAt			(x, y, s);
	bc = &cellAtBuff 		(x, y, s);
	currs[UP] = &cellAt		(x, y - 1, s);
	buffs[UP] = &cellAtBuff		(x, y - 1, s);
	currs[DN] = &cellAt		(x, y + 1, s);
	buffs[DN] = &cellAtBuff		(x, y + 1, s);
	currs[LF] = &cellAt		(x - 1, y, s);
	buffs[LF] = &cellAtBuff		(x - 1, y, s);
	currs[RT] = &cellAt		(x + 1, y, s);
	buffs[RT] = &cellAtBuff		(x + 1, y, s);

	if(x == 0)
		currs[LF] = NULL;
	if(x == s->w - 1)
		currs[RT] = NULL;
	if(y == 0)
		currs[UP] = NULL;
	if(y == s->h - 1)
		currs[DN] = NULL;


	for(i = 0; i < 4; i++) {
		bc->height -= bc->flow[i] * time;
	}
}

sim_Sim *sim_CreateSimulation(int w, int h, double startheight){
	int i, j;
	sim_Sim *temp;

	temp = (sim_Sim *) malloc(sizeof(sim_Sim));

	if(temp == NULL) {
		fprintf(stderr, "Failed to malloc in sim_CreateSimulation\n");
		exit(-1);
	}

	temp->w = w;
	temp->h = h;
	temp->curr = (sim_Cell *) malloc(w * h * sizeof(sim_Cell));
	temp->buff = (sim_Cell *) malloc(w * h * sizeof(sim_Cell));

	if(temp->curr == NULL || temp->buff == NULL) {
		fprintf(stderr, "Failed to malloc in sim_CreateSimulation\n");
		exit(-1);
	}

	for(i = 0; i < w * h; i++){
		temp->curr[i].height = startheight;
		temp->curr[i].pressure = 0;
		temp->curr[i].zflow = 0;
		for(j = 0; j < 4; j++)
			temp->curr[i].flow[j] = 0;
	}

	return temp;
}

void sim_step(sim_Sim *s, double time){
	int i, j;

	_reset_buffer(s);

	for(i = 0; i < s->h; i++) {
		for(j = 0; j < s->w; j++) {
			_createflow(s, j, i, time);
		}
	}	

	_swap_buffers(s);
	_reset_buffer(s);

	for(i = 0; i < s->h; i++) {
		for(j = 0; j < s->w; j++) {
			_equalizeflow(s, j, i, time);
			_movewater(s, j, i, time);
		}
	}	

	_swap_buffers(s);
}

void _reset_buffer(sim_Sim *s) {
	memcpy(s->buff, s->curr, s->w * s->h * sizeof(sim_Cell));	
}

void _swap_buffers(sim_Sim *s){
	sim_Cell *temp;

	temp = s->curr;
	s->curr = s->buff;
	s->buff = temp;
}

double sim_avgheight(sim_Sim *s) {
	int i;
	double sum;

	for(i = 0; i < s->w * s->h; i++) {
		sum += s->curr[i].height;
	}

	return sum / (s->w * s->h);
}

