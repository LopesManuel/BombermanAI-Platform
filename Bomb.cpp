#include "Bomb.h"

Bomb::Bomb(Player *p)
{   
    father = p;
	x = p->get_X();
	y = p->get_Y();
    map_x = x / SPRITE_SIZE;
    map_y = y / SPRITE_SIZE;
	range = p->get_Range();
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
        if ( turns_2explode < 0 )
        {
            explode = true;
            father->bomb_Exploded();
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