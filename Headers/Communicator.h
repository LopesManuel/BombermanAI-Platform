#ifndef __COMM_H__
#define __COMM_H__

#include "Global_Vars.h"
#include "Player.h"
#include "Map.h"

/* Starts the threads, and in each thread create a child process *
 * with the AI agents that are going to play. Then establishes a *
 * pipe connection with the created agents, and sends Width and H*/
void connect_thread(int i );

void send_Map();

int* get_Action(std::vector<Player*> players);

#endif