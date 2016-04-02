#include "Bomb.h"

Bomb::Bomb(int pos_x, int pos_y, int r)
{   
	x = pos_x;
	y = pos_y;
    map_x = pos_x / SPRITE_SIZE;
    map_y = pos_y / SPRITE_SIZE;
	range = r;
    life = true;
    explode = false;
}
/* TODO:
    - Moving bombs, update shall update its' position 
*/
bool Bomb::update()
{
    
    if( !explode )
    {
        turns_2explode--;
        if ( turns_2explode <= 0 )
        {
            explode = true;
        }
    }
    else
    {
        turns_2disapear--;
        if ( turns_2disapear <= 0 )
        {
            life = false;
            return false;
        }
    }
    return true;
}

const char* Bomb::get_Skin()
{
    if ( explode )
        return explosion_skin;
    else
        return skin;
    
}