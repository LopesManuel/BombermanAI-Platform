#ifndef __SDL_HELPER__
#define __SDL_HELPER__


#include "Global_Vars.h"
#include "Player.h"
#include "Map.h"
#include "Logger.h"

/* Handles all in game input events*/
void handle_Events(SDL_Event event, Map* map, Player* mp);
/* Handles all in replay input events*/
void handle_Replay_Controls( SDL_Event event , int &state, Logger *lg, std::vector<Player*> &ap);

/* Initializes SDL windows surface*/
bool init();

/* Loads the bithmap and converts it to fisplay format*/
SDL_Surface* bitmap_Loader(const char* path);

/* Free resources and close SDL */
void close();

/* Loads all media files 
   Called in the beginning to load all media */
bool load_Media(char* lvl);

void explode(int pos, int range, int direction);

void clear_explosion(int pos, int range);

template <typename Type> void load_Object(Type object);

void draw_Map(Map *map);

void draw_Player(std::vector<Player*> players);

/* Loads level map */
bool load_Map(const char*);

//Textures
static SDL_Surface* grass;
static SDL_Surface* wall;
static SDL_Surface* stone;
static SDL_Surface* bomb;
static SDL_Surface* explosion;
static SDL_Surface* bomb_pu;
static SDL_Surface* range_pu;
static SDL_Surface* ghost_pu;
static SDL_Surface* speed_pu;
static SDL_Surface* switch_pu;
static SDL_Surface* slide_pu;

#endif