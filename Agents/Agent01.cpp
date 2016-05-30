#include "Functions.h"
#include <functional>
#include <fstream>
#include <iostream>
#include <string>

int holes [14][2] = { {3,2},{3,3},{3,4},{3,5},{3,6},{3,7},{3,8},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7},{1,8}};
int num_holes = 14;
// Values size
int num_actions = 4;
// Optimistic initialization
bool opt_init = true;
// Frequency with which we should explore
float epsilon = 0.2f; 
/* Saves the eligibility traces, same size as the 
    feature vector. */
float **eligibility;
/* Vector of integers of length N that tells 
    us how many times we’ve played each action*/
int *counts;
/* Discounted rewards we expect to receive if we start 
    at state s, take action a*/
float ***Q;
/* Parameter function vector*/
float **parameter_vector;
/* Discount factor for future rewards [0.0, 1.0]*/
float gama = 0.9f;  
float lambda = 0.5f;
/* Learning rate */
float alpha = 0.1f;

int s0[2];
int a0;
int r0;
int s1[2];
int a1; // not used for Q learning
int starting = 1;

//Helper functions
int next_action();
int start_episode();
int step();
int end_episode();
int get_max_value_index();
int e_greedy();
void save_log();

int main()
{
    bool gameover = false;
    clock_t t1,t2;
    float diff,seconds; 
    int action;
    int count = 1;
    
    if( connect() ) 
    {
        Q = (float***) std::malloc(sizeof(float**) * num_actions);
        for(int i = 0; i < num_actions; i++)
        {
            Q[i] = (float**) std::malloc(sizeof(float*) * NUM_ROWS);
            for(int ir = 0; ir < NUM_ROWS; ir++)
            {
                Q [i][ir] =  (float*) std::malloc(sizeof(float) * NUM_COLS);
            }
        }
        //Allocate space to world map
        wordl_map = (char**) std::malloc(sizeof(char*) * NUM_ROWS);
        eligibility =  (float**) std::malloc(sizeof(float*) * NUM_ROWS);
        parameter_vector =  (float**) std::malloc(sizeof(float*) * NUM_ROWS);
        for(int i = 0; i < NUM_ROWS; i++)
        {
            wordl_map[i] = (char*) std::malloc(sizeof(char) * NUM_COLS);
            eligibility[i] =  (float*) std::malloc(sizeof(float) * NUM_COLS);
            parameter_vector[i] =  (float*) std::malloc(sizeof(float) * NUM_COLS);
        }
        //Allocate space for players positions
        x = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        y = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        r = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        speed = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        alive = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        teams =  (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        counts =  (int*) std::malloc(sizeof(int) * num_actions);

        //Game loop
        while( !gameover )
        {
            gameover = update_Map();
            if ( !gameover )
            {  //Updates players' information
               gameover = update_Players();
            }
            t1=clock();
            // AI agents get next action 
            action = next_action();
            t2=clock();
            diff  = ((float)t2-(float)t1);
            seconds = diff / CLOCKS_PER_SEC;

            if ( seconds <= 1 ){
                /* 5 plays per second */
                usleep((CLOCKS_PER_SEC/speed[PLAYER_ID])-seconds*(CLOCKS_PER_SEC/5));
                /* Print action to send to server */
                std::cout << action;
            }
            else{
                /* Agent only have 1sec to give the next action*/
                std::cout << "TIMEOUT"<<std::endl;
            }
            count++;
            if ( count % 1000 )
            {
                count = 1;  
                save_log();
            }
        }
    }
    return 0;
}

/* RL agents next action */
int  next_action()
{
    int action = -1;
    if ( starting )
        action = start_episode();
    else 
    {
        for ( int i = 0; i < NUM_PLAYERS; i++)
        {
            r0 = 0;
         //   if ( i != PLAYER_ID && x[PLAYER_ID] == x[i] && y[PLAYER_ID] == y[i] )
            if ( x[PLAYER_ID] == 2 && y[PLAYER_ID] == 8 )
            {
                r0 = 3;
                break;
            }
   /*         for ( int h = 0; h < num_holes; h++)
            {
                if ( x[PLAYER_ID] == holes[h][0] && y[PLAYER_ID] == holes[h][1] )
                {
                    r0 = -1;
                    break;
                }
            }
            if ( r0 == -1 ) break; */
        }
        action = step();
    }
    return action;
}

int start_episode()
{
    starting = 0;
    for (int i = 0; i < NUM_ROWS; ++i)	
    {
        for (int j = 0; j < NUM_COLS; ++j)	
        {
            eligibility[i][j] = 0.0f; // 1. e = 0;
            if(!opt_init)
                parameter_vector[i][j] = 0.0f;
            else{
                parameter_vector[i][j] = 1.0f / (float)NUM_ROWS*NUM_COLS;
            }
        }
    }
    for (int a = 0; a < num_actions; a++)	
    {
        float Qoa = 0.0;
        counts[a] = 0;
        for (int i = 0; i < NUM_ROWS; ++i)	
        {
            for (int j = 0; j < NUM_COLS; ++j)	
            {
                Q[a][i][j] = 0.0f; 
            }
        }
    }
    //Update s0 & a0
    a0 = get_max_value_index();
    s1[0] = x[PLAYER_ID];
    s1[1] = y[PLAYER_ID];
    return a0;
}
/* calculate the target for Q(s,a)
Q learning target is Q(s0,a0) = r0 + gamma * max_a Q[s1,a] */
int step()
{
    // get next action a1
    int action = e_greedy();
    // update s0,s1,a0,a1
    s0[0] = s1[0];
    s0[1] = s1[1];
    s1[0] = x[PLAYER_ID];
    s1[1] = y[PLAYER_ID];
    //Q learning
    float qmax = Q[get_max_value_index()][x[PLAYER_ID]][y[PLAYER_ID]];
    float target = r0 + gama * qmax;
    // simpler and faster update without eligibility trace
    // update Q[sa] towards it with some step size
    float update = alpha * (target - Q[a0][s0[0]][s0[1]]);
    Q[a0][s0[0]][s0[1]] += update;
    
    a0 = action;
    return action;
}

int end_episode()
{
    
}

/* choose the action that maximizes the current value function with 
 * probability (1 - e) and a random action with the probability e */
int e_greedy( )
{
	float rnumb = (float) (rand()%100 + 1)/100;
	if( rnumb < epsilon)
    {
		return rand() % num_actions;
	}
	else
    {
		return get_max_value_index();
	}

}

int get_max_value_index()
{
    float qmax = 0.0f;
    int qmax_index = 0;
    for (int a = 0; a < num_actions; a++)	
    {
        if ( Q[a][x[PLAYER_ID]][y[PLAYER_ID]] > qmax ){
            qmax = Q[a][x[PLAYER_ID]][y[PLAYER_ID]];
            qmax_index = a;
        }
    }
	return qmax_index;
}

void save_log()
{
    std::fstream log_data;
    char seedstring[4];
    std::sprintf(seedstring,"%d",PLAYER_ID); 
    std::string path = "Q_values";
    path += "_";
    path += seedstring;
    log_data.open(path,std::fstream::out );   
    std::string current_log = ""; 
    for ( int a = 0; a < num_actions; a++ )
    {
        for(int i = 0; i < NUM_ROWS; i++)
        {
            for(int j = 0; j < NUM_COLS; j++)
            {
                current_log += std::to_string(Q[a][i][j]); 
                current_log += " ";	
            }
            current_log += "\n";
        }
        current_log += "-------------------------\n";
    }
    log_data << current_log;
}

void read_log()
{
    std::fstream log_data;
    char seedstring[4];
    std::sprintf(seedstring,"%d",PLAYER_ID); 
    std::string path = "Qvalues";
    path += "_";
    path += seedstring;
    log_data.open(path,std::fstream::out );   
    std::string current_log = ""; 
    for ( int a = 0; a < num_actions; a++ )
    {
        for(int i = 0; i < NUM_ROWS; i++)
        {
            for(int j = 0; j < NUM_COLS; j++)
            {
                current_log += std::to_string(Q[a][i][j]); 
                current_log += " ";	
            }
            current_log += "\n";
        }
        current_log += "-------------------------\n";
    }
    log_data << current_log;
}