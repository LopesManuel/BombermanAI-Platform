#ifndef __DEFINITIONS__
#define __DEFINITIONS__

#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>    
#include <deque>
#include <string>
#include <sstream>
#include <cassert> 
//Using SDL and standard IO 
#include <SDL2/SDL.h> 
#include <stdio.h> 
//Using threads to handle communications
#include <thread>
//For using fork()
#include <unistd.h>

//Screen dimension constants 
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int SPRITE_SIZE;
extern const int NUM_COLS;
extern const int NUM_ROWS;


// Global logic 
extern int gameover;

//The window we'll be rendering to
extern SDL_Window* gWindow;
	
//The surface contained by the window
extern SDL_Surface* gScreenSurface;

//Temporary event to hold  poll the event queue to get the most recent event by calling SDL_PollEvent
extern SDL_Event event;

//Holds world map
extern char* world_Map;

//Holds path to the lvl map txt
extern char* level;

//Gets matrix index from vector 
inline int mIndex( int x, int y)
{ 
  return ( y * NUM_COLS) + x;
}

//Number of players active
extern int num_Players;
//Total number of starting players
extern int num_SPlayers;

// Player's possible actions
enum Actions
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
  FIRE
};

// Game tiles' sprite
enum Tiles
{
  EXPLOSION = 'e',
  WALL      = '*',
  STONE     = '+',
  BOMB      = 'x',
  GRASS     = '0',
  STONE_PUP = '-',
};
// Game power up tiles
enum PowerUps
{
    BOMB_PUP  = 'b',
    RANGE_PUP = 'r'  
};


/******************* COMMUNICATIONS **********************/
//Group of threads to handle player's communication
extern std::thread* p_comm;
//Comands to execute the ai agent program
extern char executes[4][15];
//File descriptores 
extern int  fdread[4][2];
extern int  fdwrite[4][2];
//Number of AI agents connected
extern int connected; 

// Communication protocol
enum Protocol
{
    CONNECT   = 'C', // State of the connection
    MAP       = 'M', // World map
    LIFE      = 'L', // State of the players life
    POSITIONS = 'P', // Every players' positions 
};



#endif