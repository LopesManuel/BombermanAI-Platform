#ifndef __MAP_H__
#define __MAP_H__

#include "Global_Vars.h"
#include "Player.h"
#include "Bomb.h"

class Map
{
public:
	Map(char * m);
	~Map();
    
    //Checks if the movement is valid
    bool can_Move(Player* player, int movement);
    bool can_Slide(Bomb* bomb, int movement);
    //Subscripting [] operator overloading
    char &operator[](int i);
    // Updates game actions
    bool update_Game(std::vector<Player*> players);
    // Handle all events related with the bombs
    void add_bomb(Bomb* temp){bombs->push_back(temp);};
    void clear_explosion(int pos, int range);
    void explode(int pos, int range, int direction);
    void detonate(int player_id);
    //Returns the player that won    
    int who_Won(std::vector<Player*> players);

private:
	//Holds the world map
    char* map;
    //Holds the size of the vector 
    int SIZE;
    //Bombs
    std::deque<Bomb*> *bombs;
};

#endif