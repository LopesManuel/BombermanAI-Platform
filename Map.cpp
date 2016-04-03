#include "Map.h"


Map::Map(char* map )
{
    //Vector size is equal to num_cols * num_rows
    SIZE = NUM_COLS * NUM_ROWS;
    // Allocate space to hold world map
    world_Map = map;
    
}

bool Map::can_Move(Player* player, int movement)
{
    int player_position = player->get_mPosition();
    if( world_Map[ player_position + movement] == EXPLOSION)
        player->die();
    if( world_Map[ player_position + movement] == GRASS)
        return true;
   
    return false;     
}



char& Map::operator[](int i)
{
    assert(i >= 0 && i < SIZE);
    return world_Map[i];
}
