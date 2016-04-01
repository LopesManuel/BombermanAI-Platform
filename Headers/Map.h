#ifndef __MAP_H__
#define __MAP_H__

#include "Global_Vars.h"
#include "Player.h"
#include <cassert> // for assert()

class Map
{
public:
	Map(char * map);
	~Map();
    
    //Checks if the movement is valid
    bool can_Move(Player* player, int movement);
    //Subscripting [] operator overloading
    char &operator[](int i);

private:
	//Holds the world map
    char* world_Map;
    //Holds the size of the vector 
    int SIZE;
};

#endif