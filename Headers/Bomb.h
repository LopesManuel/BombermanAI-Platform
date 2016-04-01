#ifndef __BOMB_H__
#define __BOMB_H__

#include "Global_Vars.h"
#include <math.h>

class Bomb
{
public:
	Player(const char* s, int pos_x, int pos_y);
	~Bomb();
    
  	//Returns Bomb position in the world vector
	int get_mPosition(){ return mIndex(map_x, map_y);};
	//Returns Bomb X in the world matrix
	int get_X(){ return x;};
	//Returns Bomb Y in the world matrix
	int get_Y(){ return y;};  

private:
	//Position (x,y) on screen
	int x;
	int y;
    //Position (x,y) on map
    int map_x;
    int map_y;
	//Bomb's life
	int turns_2explode;
	//Path to skin file
	const char* skin;

};

#endif