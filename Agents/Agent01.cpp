#include <stdio.h> 
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <math.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <deque>

// Communication protocol
enum Protocol
{
    CONNECT   = 'C', // State of the connection
    MAP       = 'M', // World map
    LIFE      = 'L', // State of the players life
    POSITIONS = 'P' // Every players positions 
};
// Player's possible actions
enum Actions
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FIRE,
    STOP
};
// Game tiles
enum Tiles
{
  EXPLOSION = 'e',
  WALL      = '*',
  STONE     = '+',
  BOMB      = 'x',
  GRASS     = '0',
  PUP       = 'p'
};

// Game power up tiles
enum PowerUps
{
    BOMB_PUP  = 'b',
    RANGE_PUP = 'r',
    GHOST_PUP = 'g',
    SPEED_PUP = 's',
    SWITCH_PUP= 't',
    SLIDE_PUP = 'l'
};

//Holds world map matrix
char **wordl_map;
// World map width and height
int NUM_COLS; //width
int NUM_ROWS; //height
// Number of active players
int NUM_PLAYERS;
// Id of the current agent
int PLAYER_ID;
int bomb_range = 4; 

// Players' position
int *x;
int *y;
// Players' ranges
int *r; 
// Who is alive  1 - alive || 0 - dead
int *alive;
int *speed; 
int *teams;

//Gets matrix index from vector 
inline int mIndex( int y, int x)
{ 
  return ( y * NUM_COLS) + x;
}

//Last bomb's position
int last_bomb_pos[2];
//Last action
int last_action = 0;

std::deque<Actions> *plan;

//Helper functions
bool connect();
bool update_Map();
bool update_Players();
int next_action();
double distance_Calculate(int x1, int y1, int x2, int y2);
std::vector<Actions> get_Possible_Moves(int x, int y);
int flee_bombs();
std::vector<int*> get_Possible_Positions(int x, int y);

int main()
{
    bool gameover = false;
    clock_t t1,t2;
    float diff,seconds; 
    int action;
    plan = new std::deque<Actions>();
    
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

/* Establishes a connection with the server */
bool connect()
{
    std::string mesg;
    //Get line from pipe input
    std::getline (std::cin,mesg);
    //Break line into protocol NUM_COLS NUM_ROWS
    std::istringstream iss(mesg);
    char pmesg;
    iss >> pmesg;
    iss >> NUM_COLS;
    iss >> NUM_ROWS;
    iss >> NUM_PLAYERS;
    iss >> PLAYER_ID; 
    
    if (pmesg == CONNECT){
        std::cout << "CONNECTED"; 
    }
    else
    {
        std::cout << "ERROR CONNECTING" << std::endl; 
        return false;
    }
    return true;
}

/* Receives and updates world map */
bool update_Map()
{
    std::string mesg;
    //Get line from pipe input
    std::getline (std::cin,mesg);
    //Break line into protocol  wordl_map
    std::istringstream iss(mesg);
    char pmesg;
    std::string world;
    iss >> pmesg;
    iss >> world;

    if (pmesg == MAP)
    {
    //        std::cout << "AT " << x[PLAYER_ID] << "," << y[PLAYER_ID] <<std::endl;
            
        for(int i = 0; i < NUM_ROWS; i++)
        {
            for(int j = 0; j < NUM_COLS; j++)
            { 
                wordl_map[i][j] = world[mIndex(i,j)];
    //            std::cout <<"("<< i <<"," << j<<") " <<wordl_map[i][j] << " ";
            }
    //        std::cout << std::endl;
        } 
    }
    else
    {
        std::cout << "ERROR UPDATING MAP" << std::endl; 
        return true; // gameover ? 
    }
    //Inform the server that the map was successfully received
    std::cout << "RECEIVED";    
    return false; // gameover ? 
}

/* Receives and updates all players positions and ranges*/
bool update_Players()
{
    std::string mesg;
    //Get line from pipe input
    std::getline (std::cin,mesg);
    //Break line into protocol NUM_COLS NUM_ROWS
    std::istringstream iss(mesg);
    char pmesg;
    iss >> pmesg;

    if (pmesg == POSITIONS){
        for( int i = 0; i < NUM_PLAYERS; i++){
            iss >> y[i]; // x position 
            iss >> x[i]; // y position
            iss >> r[i]; // ranges
            iss >> alive[i]; // is player i alive?
            iss >> speed[i]; // player speed
            iss >> teams[i]; // player's team
        }
    }
    else
    {
        std::cout << "ERROR CONNECTING" << std::endl; 
        return true;// gameover ? 
    }
    return false;// gameover ? 
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

std::vector<Actions> get_Possible_Moves(int x, int y)
{
    std::vector<Actions> possible_moves;

    if( wordl_map[x + 1][y] != STONE &&  wordl_map[x + 1][y] != WALL &&  wordl_map[x + 1][y] != BOMB &&  wordl_map[x + 1][y] != EXPLOSION)
    {
        possible_moves.push_back(DOWN); //DOWN
    }
    if( wordl_map[x - 1][y] != STONE &&  wordl_map[x - 1][y] != WALL &&  wordl_map[x - 1][y] != BOMB &&  wordl_map[x - 1][y] != EXPLOSION)
    {
        possible_moves.push_back(UP); //UP
    }
    if( wordl_map[x][y + 1] != STONE &&  wordl_map[x][y + 1] != WALL &&  wordl_map[x][y + 1] != BOMB &&  wordl_map[x][y + 1] != EXPLOSION)
    {
        possible_moves.push_back(RIGHT); //RIGHT
    }
    if( wordl_map[x][y -1] != STONE &&  wordl_map[x][y - 1] != WALL &&  wordl_map[x][y - 1] != BOMB &&  wordl_map[x][y - 1] != EXPLOSION)
    {
        possible_moves.push_back(LEFT); //LEFT
    }

    return possible_moves;
}
int flee_bombs(){
   int range = 4;
   for ( int i = -3; i < range; i++)
    {
        for ( int j = -3; j < range; j++)
        {
            if ( (x[PLAYER_ID] + i >= 0) && (x[PLAYER_ID]+ i < NUM_ROWS) && (y[PLAYER_ID] +j >= 0) && ( y[PLAYER_ID] +j < NUM_COLS) )
            {
                if( wordl_map[ (x[PLAYER_ID] )  + i][y[PLAYER_ID] +j] == BOMB || wordl_map[ (x[PLAYER_ID] )  + i][y[PLAYER_ID] +j] == EXPLOSION)
                {
                    std::vector<Actions> actions =  get_Possible_Moves(x[PLAYER_ID],y[PLAYER_ID] );
                    std::vector<Actions>::iterator position  = std::find(actions.begin(), actions.end(), 4);
                    if ( i < 0)
                        position = std::find(actions.begin(), actions.end(), UP);
                    else if ( i > 0)
                        position = std::find(actions.begin(), actions.end(), DOWN);
                    if (position != actions.end() && position != actions.end()) // == myVector.end() means the element was not found
                        actions.erase(position);    
                        
                    if (j > 0)
                        position = std::find(actions.begin(), actions.end(), RIGHT);
                    else if (j < 0)
                        position = std::find(actions.begin(), actions.end(), LEFT);
                        
                    if (position != actions.end() && position != actions.end()) // == myVector.end() means the element was not found
                        actions.erase(position);
                    
                    if ( actions.empty())
                    {
                        last_action = STOP;
                        return STOP;
                    }    
                    std::vector<int*> act_pos =  get_Possible_Positions(x[PLAYER_ID],y[PLAYER_ID] );
                    /* check if action gets him way from explosion*/
                    for ( int c = 0; c < act_pos.size() ; c++)
                    {
                        if ( act_pos[c][0] > 0 && act_pos[c][1] >0 && act_pos[c][0] < NUM_ROWS && act_pos[c][1] < NUM_COLS){
                            std::vector<Actions> tmp_pos_actions =  get_Possible_Moves(act_pos[c][0],act_pos[c][1]);
                            if( tmp_pos_actions.empty() )
                            {
                                int l = x[PLAYER_ID] - act_pos[c][0];
                                int o = y[PLAYER_ID] - act_pos[c][1];
                                if ( l > 0)
                                    position = std::find(actions.begin(), actions.end(), UP);
                                else if ( l < 0)
                                    position = std::find(actions.begin(), actions.end(), DOWN);
                                if (position != actions.end() && position != actions.end()) // == myVector.end() means the element was not found
                                    actions.erase(position);    
                                    
                                if (o < 0)
                                    position = std::find(actions.begin(), actions.end(), RIGHT);
                                else if (o > 0)
                                    position = std::find(actions.begin(), actions.end(), LEFT);
                                    
                                if (position != actions.end() && position != actions.end()) // == myVector.end() means the element was not found
                                    actions.erase(position);
                            }       
                        }
                    }
                    int randomIndex = rand() % actions.size(); 
                    last_action = actions[randomIndex];                    

                    return last_action;
                } 
            }
        }  
    }
    return -1;
}

std::vector<int*> get_Possible_Positions(int x, int y)
{

    std::vector<int*> possible_moves;
    /* Flees from bombs */
    if( wordl_map[x + 1][y] != STONE &&  wordl_map[x + 1][y] != WALL &&  wordl_map[x + 1][y] != BOMB &&  wordl_map[x + 1][y] != EXPLOSION)
    {   
        int pos [2];
        pos[0] = x+1;
        pos[1] = y;
        possible_moves.push_back(pos); //DOWN
    }
    if( wordl_map[x - 1][y] != STONE &&  wordl_map[x - 1][y] != WALL &&  wordl_map[x - 1][y] != BOMB &&  wordl_map[x - 1][y] != EXPLOSION)
    {
        int pos [2];
        pos[0] = x-1;
        pos[1] = y;
        possible_moves.push_back(pos); //DOWN
    }
    if(  wordl_map[x][y +1] != STONE &&  wordl_map[x][y + 1] != WALL &&  wordl_map[x][y + 1] != BOMB &&  wordl_map[x][y + 1] != EXPLOSION)
    {
        int pos [2];
        pos[0] = x;
        pos[1] = y+1;
        possible_moves.push_back(pos); //DOWN
    }
    if( wordl_map[x][y -1] != STONE &&  wordl_map[x][y - 1] != WALL &&  wordl_map[x][y - 1] != BOMB &&  wordl_map[x][y - 1] != EXPLOSION)
    {
        int pos [2];
        pos[0] = x;
        pos[1] = y-1;
        possible_moves.push_back(pos); //DOWN
    }
    return possible_moves;
} 

/* AI agents next action */
int  next_action()
{
    int action = -1;
    /* Flees from bombs */
    action = flee_bombs();
    if ( action == -1)
    {
        /* Destroys wall */
        if( (wordl_map[x[PLAYER_ID] + 1][y[PLAYER_ID]] == STONE || 
            wordl_map[x[PLAYER_ID] - 1][y[PLAYER_ID]] == STONE ||
            wordl_map[x[PLAYER_ID]][y[PLAYER_ID] + 1] == STONE ||
            wordl_map[x[PLAYER_ID]][y[PLAYER_ID] -1] == STONE) &&  
             (wordl_map[x[PLAYER_ID]][y[PLAYER_ID]] != BOMB ) )
        {
            action = FIRE;
        }
        else /*Gets closest enemy and moves towards him*/
        {
            std::vector<Actions> actions =  get_Possible_Moves(x[PLAYER_ID],y[PLAYER_ID] );
            double min_dist = 9999.00;
            int min_dist_pid = -1;
            for ( int i = 0; i < NUM_PLAYERS; i++)
            {
                if( i != PLAYER_ID && alive[i])
                {
                    double tmp_dist = distance_Calculate(x[PLAYER_ID], y[PLAYER_ID], x[i], y[i]);
                    if ( tmp_dist < min_dist )
                    {
                        min_dist = tmp_dist;
                        min_dist_pid = i;
                    }
                }
            }
            
            if ( min_dist < bomb_range )
            {
                 action = FIRE;
            }
            else
            {
                if ( y[min_dist_pid] < y[PLAYER_ID])
                    actions.push_back(UP);
                else if ( y[min_dist_pid] > y[PLAYER_ID])
                    actions.push_back(DOWN);
                if ( x[min_dist_pid] < x[PLAYER_ID])
                    actions.push_back(LEFT);
                else if ( x[min_dist_pid] > x[PLAYER_ID])
                    actions.push_back(RIGHT);  
                     
                int randomIndex = rand() % actions.size(); 
                action = actions[randomIndex];  
            }
        }
    }
    return action;
}