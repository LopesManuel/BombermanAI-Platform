#include <stdio.h> 
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>

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
  FIRE
};
// Game tiles
enum Tiles
{
  EXPLOSION = 'e',
  WALL      = '*',
  STONE     = '+',
  BOMB      = 'x',
  GRASS     = '0'
};
//Holds world map matrix
char **world_Map;
// World map width and height
int NUM_COLS; //width
int NUM_ROWS; //height

//Helper functions
bool connect();
bool update_Map();

int main()
{
    bool gameover = false;
    if( connect() ) 
    {
        //Allocate space to world map
        world_Map = (char**) std::malloc(sizeof(char*) * NUM_ROWS);
        for(int i = 0; i < NUM_ROWS; i++)
        {
            world_Map[i] = (char*) std::malloc(sizeof(char) * NUM_COLS);
        }
        //Game loop
        if(! update_Map() )
        {
            std::cout << "RECEIVED";    
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
    //Break line into protocol  world_map
    std::istringstream iss(mesg);
    char pmesg;
    std::string world;
    iss >> pmesg;
    iss >> world;

    if (pmesg == MAP){
        for(int i = 0; i < NUM_ROWS; i++)
        {
            for(int j = 0; j < NUM_COLS; j++)
            { 
                world_Map[i][j] = world[i+j];
            }
        } 
    }
    else
    {
        std::cout << "ERROR UPDATING MAP" << std::endl; 
        return true; // gameover ? 
    }
    return false; // gameover ? 
}