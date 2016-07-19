#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

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
#include <functional>
#include <fstream>
#include <string>

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
char pUps[] = "brgstl";

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
//Reinforcment learning objectives
int OBJECTIVE_X;
int OBJECTIVE_Y;

//To save RL q values
std::fstream log_data;

//Gets matrix index from vector 
inline int mIndex( int y, int x)
{ 
  return ( y * NUM_COLS) + x;
}

// Players' position
int *x;
int *y;
// Players' ranges
int *r; 
// Who is alive  1 - alive || 0 - dead
int *alive;
int *speed; 
int *teams;

/* Establishes a connection with the server */
bool connect_RL()
{

    char line[20];
    //Get line from pipe input
    //std::getline (std::cin,mesg);
    std::cin.getline (line,20);
    std::string mesg(line);
    //Break line into protocol NUM_COLS NUM_ROWS
    std::istringstream iss(mesg);
    char pmesg;
    iss >> pmesg;
    iss >> OBJECTIVE_X; 
    iss >> OBJECTIVE_Y; 
    iss >> NUM_COLS;
    iss >> NUM_ROWS;
    iss >> NUM_PLAYERS;
    iss >> PLAYER_ID;     
   // std::cout << NUM_COLS <<NUM_PLAYERS << std::endl;
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
       // std::cout << "AT " << x[PLAYER_ID] << "," << y[PLAYER_ID] <<std::endl;
       // std::cout <<  NUM_ROWS  << " "  << NUM_COLS  << std::endl;

        for(int i = 0; i < NUM_ROWS; i++)
        {
            for(int j = 0; j < NUM_COLS; j++)
            { 
                wordl_map[i][j] = world[mIndex(i,j)];
               // std::cout <<wordl_map[i][j] << " ";
            }
           // std::cout << std::endl;
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



#endif