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
    if( map[ player_position + movement] == GRASS || 
        map[ player_position + movement] == RANGE_PUP ||
        map[ player_position + movement] == BOMB_PUP ||  
        map[ player_position + movement] == GHOST_PUP ||  
        map[ player_position + movement] == SPEED_PUP ||
        map[ player_position + movement] == SWITCH_PUP ||  
        map[ player_position + movement] == SLIDE_PUP 
      )
        return true;
    /* Can't bomb inside walls*/
    if( movement != 0 && (player->is_ghost() &&  (   map[ player_position + movement] == STONE
                                    || map[ player_position + movement] == STONE_PUP) ) )
        return true;
    
    if ( player->has_slider() &&  map[ player_position + movement] == BOMB ){
        for ( Bomb* b : (*bombs))
        {
            if ( b->get_mPosition() == player_position + movement )
            {
                if ( movement == -1 )
                    b->set_Slide(LEFT);
                else if ( movement == 1)
                    b->set_Slide(RIGHT);
                else if ( movement == -NUM_COLS)
                    b->set_Slide(UP);
                else if ( movement == NUM_COLS)
                    b->set_Slide(DOWN);
            }   
        }
    }
    
    return false;     
}



char &Map::operator[](int index)
{
    assert(index >= 0 && index < SIZE);
    return map[index];
}

bool Map::update_Game(std::vector<Player*> players)
{    
    bool changed = false;
    map_updated = false;
   // std::cout << "B" << std::endl;
	//Draws all bombs
    for ( int i = 0; i < (*bombs).size(); i++)
    {
        Bomb* b = (*bombs)[i];
		if( b != NULL && b->is_Alive())
		{   //Check if it exploded
            if(b->update()){               
                if( b->get_turns_2explosion() > 0 )
                {
                    map[b->get_mPosition()] = BOMB;
                    changed = true;
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
                    changed = true;                    
                }
            }
            else
            {
                clear_explosion(b->get_mPosition(), b->get_range());
                b->die();
                changed = true;
            }
            //Check if it is sliding
            if ( b->is_Sliding() != -1 &&  can_Slide(b, b->is_Sliding()))
            {
                b->move(b->is_Sliding());
                changed = true;
            }
   		}
        else{
            bombs->erase(bombs->begin()+i);
        }
	}
    //std::cout << "P" << std::endl;
    /*Checks if any player died this turn  */
    for ( Player* p : players)
    {
		if( p != NULL && p->is_Alive())
		{
            if( map[p->get_mPosition()] == EXPLOSION ){
                p->die();
                num_Players--;	
                changed = true;
            }
            else if( map[p->get_mPosition()] == BOMB_PUP){
                p->increase_Max_Bombs();
                map[p->get_mPosition()] = GRASS;
                changed = true;
            }
            else if (  map[p->get_mPosition()] == RANGE_PUP ){
                p->increase_Range();
                map[p->get_mPosition()] = GRASS;
                changed = true;
            }
            else if( map[p->get_mPosition()] == GHOST_PUP){
                p->ghost_mode(true);
                map[p->get_mPosition()] = GRASS;
                changed = true;
            }
            else if (  map[p->get_mPosition()] == SPEED_PUP ){
                p->increase_Speed();
                map[p->get_mPosition()] = GRASS;
                changed = true;
            }
            else if( map[p->get_mPosition()] == SWITCH_PUP){
                p->detonate_mode(true);
                map[p->get_mPosition()] = GRASS;
                changed = true;
            }
            else if (  map[p->get_mPosition()] == SLIDE_PUP ){
                p->slider_mode(true);
                map[p->get_mPosition()] = GRASS;
                changed = true;
            }
		}
	}  
    //std::cout << "U" << std::endl;
    map_updated = true;
    return changed;
}

int Map::who_Won(std::vector<Player*> players)
{
    for (int i=0; i < players.size(); i++) 
    {
		if( players[i]->is_Alive() )
		{
            return  i;
		}
	}  
    return -1;
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

            int randpu = rand() % 100;
            if( randpu < 30)
                map[pos + i * direction] = BOMB_PUP;
            else if( randpu < 60)
                map[pos + i * direction] = RANGE_PUP;
            else if ( randpu < 75)
                map[pos + i * direction] = SPEED_PUP;
            else if ( randpu < 80 )
                map[pos + i * direction] = GHOST_PUP;
            else if ( randpu < 90)
                map[pos + i * direction] = SWITCH_PUP;
            else if ( randpu < 100)
                map[pos + i * direction] = SLIDE_PUP;
            
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

void Map::detonate(int player_id)
{
    //Draws all bombs
    for ( Bomb* b : (*bombs))
    {
        if (player_id == b->get_PlanterID() )
        {
            b->trigger_explosion();  
        }
    }
}

bool Map::can_Slide(Bomb* bomb, int movement)
{
    int bomb_position = bomb->get_mPosition();
    
    switch (movement) 
    {
		case UP:
            if( map[ bomb_position - NUM_COLS] == GRASS ){
                map[  bomb_position ] = GRASS;
                return true;
            }
			break;
		case DOWN:
            if( map[ bomb_position + NUM_COLS] == GRASS ){
                map[  bomb_position ] = GRASS;
                return true;
            }
			break;
		case LEFT:
            if( map[ bomb_position - 1] == GRASS ){
                map[  bomb_position ] = GRASS;
                return true;
            }
			break;
		case RIGHT:
            if( map[ bomb_position + 1] == GRASS ){
                map[  bomb_position ] = GRASS;
                return true;
            }
			break;
	}        
    bomb->set_Slide(-1);
    return false;     
}