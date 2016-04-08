#include "Player.h"

Player::Player(const char* s, int pos_x, int pos_y)
{
	skin = s;
	x = pos_x * SPRITE_SIZE;
	y = pos_y * SPRITE_SIZE;
    map_x = pos_x;
    map_y = pos_y;
	life = true;
	connected = false;
}

void Player::move( int direction )
{
	switch (direction) 
    {
		case UP:
			y -= VELOCITY;
			break;
		case DOWN:
			y += VELOCITY;
			break;
		case LEFT:
			x -= VELOCITY;
			break;
		case RIGHT:
			x += VELOCITY;
			break;
	}
    map_x = x / SPRITE_SIZE;
    map_y = y / SPRITE_SIZE;
}

bool Player::can_Place()
{
    if( num_bombs < max_num_boms)
    {
        num_bombs++;
        return true;
    }    
    return false;
}