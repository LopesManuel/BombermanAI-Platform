#ifndef __BOMB_H__
#define __BOMB_H__

#include "Global_Vars.h"
#include "Player.h"

class Bomb
{
public:
	Bomb(Player* p);
	~Bomb();
    
  	//Returns Bomb position in the world vector
	int get_mPosition(){ return mIndex(map_x, map_y);};
	//Returns Bomb's position in screen matrix
	int get_X(){ return x;};
	int get_Y(){ return y;};
	//Returns Bomb's position on matrix map
	int get_mapX(){ return map_x;};
	int get_mapY(){ return map_y;};
    //Updates player position
	void move(int direction);
    //Returns Bomb life span
    int get_turns_2explosion(){return turns_2explode;};
    //Returns if bomb exploded 
    bool exploded(){return explode;};
    void trigger_explosion(){turns_2explode = 0; explode = true;father->bomb_Exploded();};
    //Returns the explosion range
    int get_range(){return range;};
    //Updates bomb's life and checks if it has exploded
    bool update();
    //Get if needs to draw bomb
    bool is_Alive(){return life;};
    //Set life to false
    void die(){ life = false;};
   	//Returns skin path file
	const char* get_Skin(); 
    //Returns planter's ID
    int get_PlanterID(){return player_id;};
    //Returns if it is sliding
    int is_Sliding(){return sliding;};
    void set_Slide(int movement){sliding = movement;};
    
private:
    //Player who placed the bomb
    Player *father; 
	//Position (x,y) on screen
	int x;
	int y;
    //Position (x,y) on map
    int map_x;
    int map_y;
    //Bomb slide velocity
    int VELOCITY = SPRITE_SIZE;
	//Bomb's life 1000 = 1s
	int turns_2explode = 15000;
	int turns_2disapear = 1000;
	//Path to skin file
	const char* skin = "bomb.bmp";
    //Path to explosion skin file
	const char* explosion_skin = "explosion.bmp";
    //Bomb's range ( number of squares it hits)
    int range;
    //To check if there is a need to draw the bomb
    bool life;
    //Bool to show explosion skin
    bool explode;
    // Player that planted the bomb
    int player_id;
    //If is sliding and in which direction
    int sliding = -1;
};

#endif