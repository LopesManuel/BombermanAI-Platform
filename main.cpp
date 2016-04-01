//Using SDL and standard IO 
#include <SDL2/SDL.h> 
#include <stdio.h> 
#include "Global_Vars.h"
#include "SDL_Handler.h"
#include "Player.h"

//Screen dimension constants 
const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;
const int SPRITE_SIZE   = 32;
const int NUM_COLS = SCREEN_WIDTH / SPRITE_SIZE;
const int NUM_ROWS = SCREEN_HEIGHT / SPRITE_SIZE;

// Global logic 
int gameover = 0;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Temporary events
SDL_Event event;

//Players' holder
Player* player1 = new Player("player.bmp" , SPRITE_SIZE, SPRITE_SIZE);
Player* player2;
Player* player3;
Player* player4;
Player** all_Players = (Player**) malloc(sizeof(Player*) * 4);

//Number of active players
int num_Players = 1;

//Holds world map vector
char* world_Map = (char*) malloc(sizeof(char) * ((NUM_COLS) * (NUM_ROWS) ));


int main ( int argc, char *argv[] )
{
	all_Players[0] = player1;
	all_Players[1] = player2;
	all_Players[2] = player3;
	all_Players[3] = player4;

	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
		exit(-1);
	}
		//Load all images 
	if(!load_Media())
	{
		printf( "Failed to load textures!\n" );
		exit(-1);
	}
	else
	{				
		/* message pump */
		while (!gameover)
		{
			/* look for an event */
			if (SDL_PollEvent(&event)) {
				handle_Events(event);
			}
			if(!update_Game(all_Players))
			{
				printf( "Failed to update game!\n" );
				exit(-1);
			}
			//Update the surface
			SDL_UpdateWindowSurface( gWindow );
		}
	}

	close();
	return 0;
}
