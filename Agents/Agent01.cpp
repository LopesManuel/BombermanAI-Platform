#include "Functions.h"

//Tile node struct pos(x,y) & father
struct node{
    //position in map matrix
    int x;
    int y;
    //is a dangerous tile?
    bool danger;
    //has father
    bool has_father;
    //father 
    node *father;
};

//Last bomb's position
int last_bomb_pos[2];
//Last action
int last_action = 0;

std::deque<Actions> *plan;
// Next objectives
int mission_x;
int mission_y;
bool in_danger;
int largest_range; 
int turnsWaiting = 111;

//Helper functions
int next_action();
double distance_Calculate(int x1, int y1, int x2, int y2);
std::vector<Actions> get_Possible_Moves(int x, int y);
std::deque<node> get_Possible_Positions(node *father);
Actions get_What_Move_To_Make (int x, int y, int x2, int y2);
bool check_for_players();
void search_for_players();
void flee_bombs();
void search_for_walls();

int main()
{
    bool gameover = false;
    clock_t t1,t2;
    float diff,seconds; 
    int action;
    in_danger = false;
    
    if( connect() ) 
    {
        //Allocate space to world map
        wordl_map = (char**) std::malloc(sizeof(char*) * NUM_ROWS);
        for(int i = 0; i < NUM_ROWS; i++)
        {
            wordl_map[i] = (char*) std::malloc(sizeof(char) * NUM_COLS);
        }
        //Allocate space for players positions
        x = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        y = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        r = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        speed = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        alive = (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        teams =  (int*) std::malloc(sizeof(int) * NUM_PLAYERS);
        //Game loop
        while( !gameover )
        {
            gameover = update_Map();
            if ( !gameover )
            {  //Updates players' information
               gameover = update_Players();
            }
            t1=clock();
            // AI agents get next action 
            action = next_action();
            t2=clock();
            diff  = ((float)t2-(float)t1);
            seconds = diff / CLOCKS_PER_SEC;

            if ( seconds <= 1 ){
                /* 5 plays per second */
                usleep((CLOCKS_PER_SEC/speed[PLAYER_ID])-seconds*(CLOCKS_PER_SEC/5));
                /* Print action to send to server */
                std::cout << action;
            }
            else{
                /* Agent only have 1sec to give the next action*/
                std::cout << "TIMEOUT"<<std::endl;
            }
        }
    }
    return 0;
}

double distance_Calculate(int x1, int y1, int x2, int y2)
{
    int x = x1 - x2;
    int y = y1 - y2;
    double dist;

    dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
    dist = sqrt(dist);                  //sqrt is function in math.h

    return dist;
}

std::vector<Actions> get_Possible_Moves(int x, int y)
{
    std::vector<Actions> possible_moves;

    if( wordl_map[x + 1][y] != STONE &&  wordl_map[x + 1][y] != WALL &&  wordl_map[x + 1][y] != BOMB &&  wordl_map[x + 1][y] != EXPLOSION)
    {
        possible_moves.push_back(DOWN); //DOWN
    }
    if( wordl_map[x - 1][y] != STONE &&  wordl_map[x - 1][y] != WALL &&  wordl_map[x - 1][y] != BOMB &&  wordl_map[x - 1][y] != EXPLOSION)
    {
        possible_moves.push_back(UP); //UP
    }
    if( wordl_map[x][y + 1] != STONE &&  wordl_map[x][y + 1] != WALL &&  wordl_map[x][y + 1] != BOMB &&  wordl_map[x][y + 1] != EXPLOSION)
    {
        possible_moves.push_back(RIGHT); //RIGHT
    }
    if( wordl_map[x][y -1] != STONE &&  wordl_map[x][y - 1] != WALL &&  wordl_map[x][y - 1] != BOMB &&  wordl_map[x][y - 1] != EXPLOSION)
    {
        possible_moves.push_back(LEFT); //LEFT
    }

    return possible_moves;
}
Actions get_What_Move_To_Make (int x, int y, int x2, int y2)
{
    if( x < x2 )
    {
        return DOWN; //DOWN
    }
    if( x > x2 )
    {
        return UP;
    }
    else if( y < y2)
    {
        return RIGHT;
    }
    else if( y > y2)
    {
        return LEFT;
    }

    return STOP;
}

std::deque<node> get_Possible_Positions(node *father)
{
    int x = father->x; 
    int y = father->y;
    std::deque<node> possible_moves;
    /* Flees from bombs */
    if(  wordl_map[x + 1][y] != STONE && wordl_map[x + 1][y] != WALL  && wordl_map[x + 1][y] != BOMB && wordl_map[x + 1][y] != EXPLOSION)
    {   
        node pos = {x+1,y,false,true,father};
        possible_moves.push_back(pos); //DOWN
    }
    if( wordl_map[x - 1][y] != STONE &&  wordl_map[x - 1][y] != WALL &&   wordl_map[x - 1][y] != BOMB && wordl_map[x - 1][y] != EXPLOSION)
    {
        node pos = {x-1, y,false,true,father};
        possible_moves.push_back(pos); //DOWN
    }
    if( wordl_map[x][y +1] != STONE && wordl_map[x][y + 1] != WALL && wordl_map[x][y + 1] != BOMB &&  wordl_map[x][y + 1] != EXPLOSION)
    {
        node pos = { x, y+1,false,true,father};
        possible_moves.push_back(pos); //DOWN
    }
    if(  wordl_map[x][y -1] != STONE && wordl_map[x][y - 1] != WALL && wordl_map[x][y - 1] != BOMB &&  wordl_map[x][y - 1] != EXPLOSION)
    {
        node pos = { x, y-1,false,true,father};
        possible_moves.push_back(pos); //DOWN
    }
    return possible_moves;
} 

bool check_for_players(){
    /* Update largest range */
    double lr = -1;
    int lr_id = -1;
    for ( int i = 0; i < NUM_PLAYERS; i++)
    {
        if( alive[i])
        {
            if ( r[i] > lr )
            {
                lr = r[i];
                lr_id = i;
            }
        }
    }
    largest_range = r[lr_id];
    /* now check if anyplayer in range*/
    double min_dist = 9999.00;
    int min_dist_pid = -1;
    for ( int i = 0; i < NUM_PLAYERS; i++)
    {
        if( i != PLAYER_ID && alive[i])
        {
            double tmp_dist = distance_Calculate(x[PLAYER_ID], y[PLAYER_ID], x[i], y[i]);
            if ( tmp_dist - r[i] <= 0 && (tmp_dist - r[i]) < min_dist )
            {
                min_dist = tmp_dist;
                min_dist_pid = i;
            }
        }
    }
    if ( min_dist_pid != -1){
        mission_x = x[min_dist_pid];
        mission_y = y[min_dist_pid];
        return true;
    }
    return false;
}

void search_for_players()
{
    bool seen;
    bool stop_exploration = false;  
    /* Go to player 1 */
    node root = {x[PLAYER_ID], y[PLAYER_ID],false ,NULL};
    std::deque<node> visited;
    visited.push_front(root);
    std::deque<node> explore = get_Possible_Positions(&root);
    for( int i = 0; i < explore.size(); i++)
    {
        /* Check if it has a pathway to anyplayer */
        for ( int p = 0; p < NUM_PLAYERS; p++)
        {
            if( p != PLAYER_ID && alive[p])
            {
                if ( (explore[i].x == x[p] &&  explore[i].y == y[p]))
                {
                    if ( distance_Calculate(explore[i].x, explore[i].y, x[PLAYER_ID], y[PLAYER_ID]) < r[PLAYER_ID]  )
                    {
                        plan->push_front(FIRE);
                        stop_exploration = true;
                        break;
                    }
                    node* fat = explore[i].father;
                    plan->push_front(get_What_Move_To_Make(fat->x, fat->y, explore[i].x, explore[i].y));
                    while ( fat->has_father )
                    {
                        plan->push_front(get_What_Move_To_Make(fat->father->x, fat->father->y, fat->x, fat->y));
                        fat = fat->father;
                    }
                    if ( !plan->empty() )
                    {
                        explore.clear();
                        stop_exploration = true;
                        break;
                    }
                }
            }
        } 
    }
    while( !explore.empty() ) 
    {
        visited.push_front(explore.front());
        explore.pop_front();
        
        std::deque<node> tmp = get_Possible_Positions(&visited.front());
        for( int i = 0; i < tmp.size(); i++)
        {
            seen = false;
            //Checks if it has been already visited
            for ( int j = 0; j < visited.size(); j++)
            {
                if ( tmp[i].x == visited[j].x &&  tmp[i].y == visited[j].y ){
                    seen = true;
                    break;
                }
            }
            //Checks if it has been pinned to explore
            for ( int j = 0; j < explore.size(); j++)
            {
                if ( tmp[i].x == explore[j].x &&  tmp[i].y == explore[j].y ){
                    seen = true;
                    break;
                }
            }
            if ( !seen )
            {

                explore.push_back(tmp[i]);
                /* Check if it has a pathway to anyplayer */
                for ( int p = 0; p < NUM_PLAYERS; p++)
                {
                    if( p != PLAYER_ID && alive[p])
                    {
                        if ( (tmp[i].x == x[p] &&  tmp[i].y == y[p]))
                        {
                            if ( distance_Calculate(tmp[i].x, tmp[i].y, x[PLAYER_ID], y[PLAYER_ID]) < r[PLAYER_ID]  )
                            {
                                plan->push_front(FIRE);
                                stop_exploration = true;
                                break;
                            }
                            node* fat = tmp[i].father;
                            plan->push_front(get_What_Move_To_Make(fat->x, fat->y, tmp[i].x, tmp[i].y));
                            while ( fat->has_father )
                            {
                                plan->push_front(get_What_Move_To_Make(fat->father->x, fat->father->y, fat->x, fat->y));
                                fat = fat->father;
                            }
                            if ( !plan->empty() )
                            {
                                explore.clear();
                                stop_exploration = true;
                                break;
                            }
                        }
                    }
                } 
                if ( stop_exploration ) 
                    break;
            }
        }
    }
}

void flee_bombs(){
    bool seen;
    bool stop_exploration = false;
    int bomb_x = -1;
    int bomb_y = -1;

    /* Check for bombs */
    for ( int i = -largest_range; i < largest_range; i++)
    {
        if( x[PLAYER_ID]+i >= 0 && x[PLAYER_ID]+i < NUM_ROWS && wordl_map[x[PLAYER_ID] + i][y[PLAYER_ID]] == BOMB )
        {   
            bomb_x = x[PLAYER_ID]+i;
            bomb_y = y[PLAYER_ID];
            break;
        }
        if(  y[PLAYER_ID]+i >= 0 && y[PLAYER_ID]+i < NUM_COLS &&  wordl_map[x[PLAYER_ID] ][y[PLAYER_ID] + i] == BOMB )
        {   
            bomb_x = x[PLAYER_ID];
            bomb_y = y[PLAYER_ID]+i;
            break;
        }
    } 
    if ( bomb_x != -1 && bomb_y != -1) 
    {
        node root = {x[PLAYER_ID], y[PLAYER_ID],false ,NULL};
        std::deque<node> visited;
        visited.push_front(root);
        std::deque<node> explore = get_Possible_Positions(&root);
        for( int i = 0; i < explore.size(); i++)
        {
            if ( distance_Calculate(explore[i].x, explore[i].y, bomb_x, bomb_y) > largest_range 
                || (explore[i].x !=  bomb_x  && explore[i].y !=  bomb_y) )
            {
                in_danger = true;
                node* fat = explore[i].father;
                plan->push_front(get_What_Move_To_Make(fat->x, fat->y, explore[i].x, explore[i].y));
                while ( fat->has_father )
                {
                    plan->push_front(get_What_Move_To_Make(fat->father->x, fat->father->y, fat->x, fat->y));
                    fat = fat->father;
                }
                if ( !plan->empty() )
                {
                    explore.clear();
                    stop_exploration = true;
                    break;
                }
            }
        }
        while( !explore.empty() ) 
        {
            visited.push_front(explore.front());
            explore.pop_front();
            
            std::deque<node> tmp = get_Possible_Positions(&visited.front());
            for( int i = 0; i < tmp.size(); i++)
            {
                seen = false;
                //Checks if it has been already visited
                for ( int j = 0; j < visited.size(); j++)
                {
                    if ( tmp[i].x == visited[j].x &&  tmp[i].y == visited[j].y ){
                        seen = true;
                        break;
                    }
                }
                //Checks if it has been pinned to explore
                for ( int j = 0; j < explore.size(); j++)
                {
                    if ( tmp[i].x == explore[j].x &&  tmp[i].y == explore[j].y ){
                        seen = true;
                        break;
                    }
                }
                if ( !seen )
                {
                    explore.push_back(tmp[i]);
                    
                    if ( distance_Calculate(tmp[i].x, tmp[i].y, bomb_x, bomb_y) > largest_range 
                        || (tmp[i].x !=  bomb_x  && tmp[i].y !=  bomb_y) )
                    {
                        in_danger = true;
                        node* fat = tmp[i].father;
                        plan->push_front(get_What_Move_To_Make(fat->x, fat->y, tmp[i].x, tmp[i].y));
                        while ( fat->has_father )
                        {
                            plan->push_front(get_What_Move_To_Make(fat->father->x, fat->father->y, fat->x, fat->y));
                            fat = fat->father;
                        }
                        if ( !plan->empty() )
                        {
                            explore.clear();
                            stop_exploration = true;
                            break;
                        }
                    }
                    if ( stop_exploration ) 
                        break;
                }
            }
        }
    }
}

void search_for_walls(){
    bool seen;
    bool stop_exploration = false;
    
    if ( wordl_map[x[PLAYER_ID]+1][y[PLAYER_ID]] == STONE || wordl_map[x[PLAYER_ID]-1][y[PLAYER_ID]] == STONE ||
        wordl_map[x[PLAYER_ID]][y[PLAYER_ID] -1] == STONE || wordl_map[x[PLAYER_ID]][y[PLAYER_ID] +1] == STONE)
    {
        plan->push_front(FIRE);
    }
    else
    {
        node root = {x[PLAYER_ID], y[PLAYER_ID],false ,NULL};
        std::deque<node> visited;
        visited.push_front(root);
        std::deque<node> explore = get_Possible_Positions(&root);
        for( int i = 0; i < explore.size(); i++)
        {
            if ( wordl_map[explore[i].x+1][explore[i].y] == STONE || wordl_map[explore[i].x-1][explore[i].y] == STONE ||
                wordl_map[explore[i].x][explore[i].y -1] == STONE || wordl_map[explore[i].x][explore[i].y +1] == STONE)
            {
                node* fat = explore[i].father;
                plan->push_front(get_What_Move_To_Make(fat->x, fat->y, explore[i].x, explore[i].y));
                while ( fat->has_father )
                {
                    plan->push_front(get_What_Move_To_Make(fat->father->x, fat->father->y, fat->x, fat->y));
                    fat = fat->father;
                }
                if ( !plan->empty() )
                {
                    explore.clear();
                    stop_exploration = true;
                    break;
                }
            }
        }
        while( !explore.empty() ) 
        {
            visited.push_front(explore.front());
            explore.pop_front();
            std::deque<node> tmp = get_Possible_Positions(&visited.front());
            for( int i = 0; i < tmp.size(); i++)
            {
                seen = false;
                //Checks if it has been already visited
                for ( int j = 0; j < visited.size(); j++)
                {
                    if ( tmp[i].x == visited[j].x &&  tmp[i].y == visited[j].y ){
                        seen = true;
                        break;
                    }
                }
                //Checks if it has been pinned to explore
                for ( int j = 0; j < explore.size(); j++)
                {
                    if ( tmp[i].x == explore[j].x &&  tmp[i].y == explore[j].y ){
                        seen = true;
                        break;
                    }
                }
                if ( !seen )
                {
                    explore.push_back(tmp[i]);
                    
                    if ( wordl_map[tmp[i].x+1][tmp[i].y] == STONE || wordl_map[tmp[i].x-1][tmp[i].y] == STONE ||
                        wordl_map[tmp[i].x][tmp[i].y -1] == STONE || wordl_map[tmp[i].x][tmp[i].y +1] == STONE)
                    {
                        node* fat = tmp[i].father;
                        plan->push_front(get_What_Move_To_Make(fat->x, fat->y, tmp[i].x, tmp[i].y));
                        while ( fat->has_father )
                        {
                            plan->push_front(get_What_Move_To_Make(fat->father->x, fat->father->y, fat->x, fat->y));
                            fat = fat->father;
                        }
                        if ( !plan->empty() )
                        {
                            explore.clear();
                            stop_exploration = true;
                            break;
                        }
                    }
                    if ( stop_exploration ) 
                        break;
                }
            }
        }
    }
}
/* AI agents next action */
int  next_action()
{
    int action = -1;
    turnsWaiting++;
    //Updates mission objective position
    bool cfp = check_for_players();
    
    plan = new std::deque<Actions>();
    if ( turnsWaiting > 6){
        turnsWaiting = 7;
        /*Survival above all */
        flee_bombs();
        if ( plan->empty() && in_danger){
            in_danger = false;
            turnsWaiting = 0;
            plan->push_front(STOP);
        }
        /*If it's not in danger go after players*/
        if ( plan->empty())
            search_for_players();        
        /*Ppen pathways to players */
        if ( plan->empty())
            search_for_walls();
            
        if ( !plan->empty() )
        {
            action = plan->front();
            plan->pop_front();
        }

    }

    return action;
}