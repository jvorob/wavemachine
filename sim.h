#ifndef SIM_H
#define SIM_H


typedef struct _sim_cell {
	double height;
	double pressure;//1 is neutral, 0-1 is low, >1 is high
	double zflow;
	double flow[4];//positive means flowing out
} sim_Cell;

typedef struct _sim_sim {
	int w; 
	int h;	//like screen coordinate system: 0,0 at top left
	sim_Cell *curr; //Double buffered for simulation purposes
	sim_Cell *buff;
} sim_Sim;


enum dirs {
	UP, RT, DN, LF
};

//sim is a sim_Sim* 
#define cellAt(x,y,sim) ((sim)->curr[(x) + (y) * (sim)->w])
#define cellAtBuff(x,y,sim) ((sim)->buff[(x) + (y) * (sim)->w])


sim_Sim *sim_CreateSimulation(int w, int h, double startheight);
void sim_step(sim_Sim *s, double time);
double sim_avgheight(sim_Sim *s);

/*
High level idea:
	dheight + zflow -> pressure (completely recalculated)
	dpressure -> flow
	flows averaged, zflows adjusted
	drag to flow, drag to zflow
	flow->heights
*/

#endif
