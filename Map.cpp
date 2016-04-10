#include "Map.h"


Map::Map(char* m )
{
    //Vector size is equal to num_cols * num_rows
    SIZE = NUM_COLS * NUM_ROWS;

    map = m;
    
    bombs = new std::deque<Bomb*>();
}

bool Map::can_Move(Player* player, int movement)
{
    int player_position = player->get_mPosition();
    if( player->is_Alive() && map[ player_position + movement] == EXPLOSION){
        player->die();
        num_Players--;
    }
    if( map[ player_position + movement] == GRASS || map[ player_position + movement] == BOMB_PUP ||  map[ player_position + movement] == RANGE_PUP)
        return true;
   
    return false;     
}



char &Map::operator[](int index)
{
    assert(index >= 0 && index < SIZE);
    return map[index];
}

void Map::update_Game(std::vector<Player*> players)
{    
	//Draws all bombs
    for ( Bomb* b : (*bombs))
    {
		if( b != NULL && b->is_Alive())
		{
            if(b->update()){               
                if( b->get_turns_2explosion() > 0 )
                {
                    map[b->get_mPosition()] = BOMB;
                }
                else if (  b->get_turns_2explosion() == 0)
                {
                    //Right
                    explode(b->get_mPosition(), b->get_range(), 1);
                    //Left
                    explode(b->get_mPosition(), b->get_range(), -1);
                    //Up
                    explode(b->get_mPosition(), b->get_range(), NUM_COLS);
                    //Down
                    explode(b->get_mPosition(), b->get_range(), -NUM_COLS);
                }
            }
            else
            {
                clear_explosion(b->get_mPosition(), b->get_range());
                bombs->pop_front();
            }
   		}
	}
    /*Checks if any player died this turn  */
    for ( Player* p : players)
    {
		if( p != NULL && p->is_Alive())
		{
            if( map[p->get_mPosition()] == EXPLOSION ){
                p->die();
                num_Players--;	
            }
            else if( map[p->get_mPosition()] == BOMB_PUP){
                p->increase_Max_Bombs();
                map[p->get_mPosition()] = GRASS;
            }
            else if (  map[p->get_mPosition()] == RANGE_PUP ){
                p->increase_Range();
                map[p->get_mPosition()] = GRASS;
            }
		}
	}  
}

int Map::who_Won(std::vector<Player*> players)
{
    for (int i; i < players.size(); i++) 
    {
        Player* p = players[i];
		if( p != NULL && p->is_Alive())
		{
            return  i;
		}
	}  
    return NULL;
}


void Map::explode(int pos, int range, int direction)
{
    map[pos] = EXPLOSION;

    for ( int i = 0; i < range; i++){
        if ( map[pos + i * direction] == GRASS)
            map[pos + i * direction] = EXPLOSION;
        else if ( map[pos + i * direction] == WALL || map[pos + i * direction] == BOMB ) 
            break; 
        else if( map[pos + i * direction] == STONE ) 
        {
            map[pos + i * direction] = EXPLOSION;
            break;
        }
        else if( map[pos + i * direction] == STONE_PUP)
        {

            int randpu = rand() % 2;
            switch (randpu)
            {
                case 0:
                    map[pos + i * direction] = BOMB_PUP;
                    break;
                case 1:
                    map[pos + i * direction] = RANGE_PUP;
                    break;
            }
            break;
        }
    }
}
void Map::clear_explosion(int pos, int range)
{
    map[pos] = GRASS;
    for ( int i = 0; i < range; i++)
    {
        if ( ( pos + i < NUM_COLS*NUM_ROWS ) &&  map[pos + i] == EXPLOSION )
            map[pos + i] = GRASS;
        if (  ( pos - i > 0 ) && map[pos - i] == EXPLOSION)
            map[pos - i] = GRASS;
        if (  ( pos + i*NUM_COLS < NUM_COLS*NUM_ROWS ) && map[pos + i*NUM_COLS] == EXPLOSION)
            map[pos + i*NUM_COLS] = GRASS;
        if ( ( pos - i *NUM_COLS > 0 )  && map[pos - i*NUM_COLS] == EXPLOSION) 
            map[pos - i*NUM_COLS] = GRASS;
    }
}