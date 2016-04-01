#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Global_Vars.h"
#include <math.h>

class Player
{
public:
	Player(const char* s, int pos_x, int pos_y);
	~Player();

	//Returns player position in the world vector
	int get_mPosition(){ return mIndex(map_x, map_y);};
	//Returns player X in the world matrix
	int get_X(){ return x;};
	//Returns player Y in the world matrix
	int get_Y(){ return y;};
	//Returns if the player is alive (dah)
	bool is_Alive(){ return life;};
	//Checks if it is connected
	bool is_Connected(){ return connected;};
	//Updates player position
	void move(int direction);
	//Returns skin path file
	const char* get_Skin(){ return skin;}; 
private:
	//Player velocity
	const int VELOCITY = SPRITE_SIZE;	 
	//Position (x,y) on screen
	int x;
	int y;
    //Position (x,y) on map
    int map_x;
    int map_y;
	//Player's life
	bool life;
	//Check if connected
	bool connected;
	//Path to skin file
	const char* skin;
};

#endif