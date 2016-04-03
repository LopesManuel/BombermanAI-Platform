#ifndef __SDL_HELPER__
#define __SDL_HELPER__

#include <SDL2/SDL.h> 
#include "Global_Vars.h"
#include "Player.h"
#include "Map.h"
#include "Bomb.h"
#include <fstream>
#include <iostream>
#include <vector>    
#include <deque>

/* Handles all input events*/
void handle_Events(SDL_Event event);

/* Initializes SDL windows surface*/
bool init();

/* Loads the bithmap and converts it to fisplay format*/
SDL_Surface* bitmap_Loader(const char* path);

/* Free resources and close SDL */
void close();

/* Loads all media files 
   Called in the beginning to load all media */
bool load_Media();

void explode(int pos, int range, int direction);

void clear_explosion(int pos, int range);

template <typename Type> void load_Object(Type object);

bool update_Game(std::vector<Player*> players);

void refresh_Map();

/* Loads level map */
bool load_Map(const char*);

//Manual player
static Player* manual_Player;
//Map object to control all actions
static Map* map;

//Bombs
static std::deque<Bomb*> bombs;


#endif