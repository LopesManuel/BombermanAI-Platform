#ifndef __DEFINITIONS__
#define __DEFINITIONS__

#include <SDL2/SDL.h> 

//Screen dimension constants 
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int SPRITE_SIZE;

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
  return ( y * ( SCREEN_WIDTH/SPRITE_SIZE )) + x;
}

//Number of players active
extern int num_Players;

// Player's possible actions
enum Actions
{
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3,
  FIRE = 4
};

#endif