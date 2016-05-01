#include "Functions.h"

//Helper functions
int next_action();
double distanceCalculate(int x1, int y1, int x2, int y2);

int main()
{
    bool gameover = false;
    clock_t t1,t2;
    float diff,seconds; 
    int action;
    
    if( connect() ) 
    {
        //Allocate space to world map
        wordl_map = (char**) std::malloc(sizeof(char*) * NUM_ROWS);
        for(int i = 0; i < NUM_ROWS; i++)
        {
            wordl_map[i] = (char*) std::malloc(sizeof(char) * NUM_COLS);
        }
        //Allocate space for players positions
        x = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        y = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        r = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        speed = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        alive = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        teams =  (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        //Game loop
        while( !gameover )
        {
            gameover = update_Map();
            gameover = update_Players();
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
        }
    }
    return 0;
}

double distanceCalculate(int x1, int y1, int x2, int y2)
{
    int x = x1 - x2;
    int y = y1 - y2;
    double dist;

    dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
    dist = sqrt(dist);                  //sqrt is function in math.h

    return dist;
}

/* AI agents next action */
int  next_action(){
    /* generate random action: */
    int action = rand() % 4;
    return action;   
}