#include "Functions.h"

//Last bomb's position
int last_bomb_pos[2];
//Last action
int last_action;

//Helper functions
int next_action();
double distance_Calculate(int x1, int y1, int x2, int y2);
std::vector<Actions> get_Possible_Moves();

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
        }
    }
    return 0;
}


double distance_Calculate(int x1, int y1, int x2, int y2)
{
    int x = x1 - x2;
    int y = y1 - y2;
    double dist;

    dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
    dist = sqrt(dist);                  //sqrt is function in math.h

    return dist;
}

std::vector<Actions> get_Possible_Moves()
{
    std::vector<Actions> possible_moves;
    /* Flees from bombs */
    if( wordl_map[ (x[PLAYER_ID] )  + 1][y[PLAYER_ID]] == GRASS)
    {
        possible_moves.push_back(DOWN); //DOWN
    }
    if( wordl_map[ (x[PLAYER_ID] )  - 1][y[PLAYER_ID]] == GRASS)
    {
        possible_moves.push_back(UP); //UP
    }
    if( wordl_map[ (x[PLAYER_ID] )][y[PLAYER_ID] + 1] == GRASS)
    {
        possible_moves.push_back(RIGHT); //RIGHT
    }
    if( wordl_map[ (x[PLAYER_ID] )][y[PLAYER_ID] -1] == GRASS)
    {
        possible_moves.push_back(LEFT); //LEFT
    }
    return possible_moves;
}

/* AI agents next action */
int  next_action()
{
    int range = 4;

    /* Flees from bombs */
               // std::cout <<" ------- " <<std::endl;
    for ( int i = -3; i < range; i++)
    {
        for ( int j = -3; j < range; j++)
        {
               // std::cout << "("<<x[PLAYER_ID]+ i << ", "<<y[PLAYER_ID] +j <<")"<<std::endl;
            if ( (x[PLAYER_ID] + i >= 0) && (x[PLAYER_ID]+ i < NUM_ROWS) && (y[PLAYER_ID] +j >= 0) && ( y[PLAYER_ID] +j < NUM_COLS) )
            {
                //std::cout <<" ------- " <<std::endl;
                //std::cout << "("<<x[PLAYER_ID]+ i << ", "<<y[PLAYER_ID] +j <<") =" << wordl_map[ (x[PLAYER_ID] )  + i][y[PLAYER_ID] +j]  <<std::endl;
                if( wordl_map[ (x[PLAYER_ID] )  + i][y[PLAYER_ID] +j] == BOMB || wordl_map[ (x[PLAYER_ID] )  + i][y[PLAYER_ID] +j] == EXPLOSION)
                {
                    std::vector<Actions> actions =  get_Possible_Moves();
                    std::vector<Actions>::iterator position  = std::find(actions.begin(), actions.end(), 4);
                    if ( i < 0)
                        position = std::find(actions.begin(), actions.end(), UP);
                    else if ( i > 0)
                        position = std::find(actions.begin(), actions.end(), DOWN);
                    if (j > 0)
                        position = std::find(actions.begin(), actions.end(), RIGHT);
                    else if (j < 0)
                        position = std::find(actions.begin(), actions.end(), LEFT);
                        
                    if (position != actions.end() && position != actions.end()) // == myVector.end() means the element was not found
                        actions.erase(position);
                    
                    if ( actions.empty())
                    {
                        last_action = -1;
                        return -1;
                    }    
                    int randomIndex = rand() % actions.size(); 
                    if ( last_bomb_pos[0] != x[PLAYER_ID] + i  || last_bomb_pos[1] != y[PLAYER_ID] +j)
                    {
                        last_bomb_pos[0] = x[PLAYER_ID] + i;
                        last_bomb_pos[1] = y[PLAYER_ID] +j;
                        last_action = actions[randomIndex];
                    }
                    return last_action;
                } 
            }
        }  
    }

    /* generate random action: */ 
    return 0;
}