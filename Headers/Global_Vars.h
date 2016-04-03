#ifndef __DEFINITIONS__
#define __DEFINITIONS__

#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>    
#include <deque>
#include <cassert> 
//Using SDL and standard IO 
#include <SDL2/SDL.h> 
#include <stdio.h> 

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

//Gets matrix index from vector 
inline int mIndex( int x, int y)
{ 
  return ( y * NUM_COLS) + x;
}

//Number of players active
extern int num_Players;

// Player's possible actions
enum Actions
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
  FIRE
};

// Tiles
enum Tiles
{
  EXPLOSION = 'e',
  WALL = '*',
  STONE = '+',
  BOMB = 'x',
  GRASS = '0'
};
#endif