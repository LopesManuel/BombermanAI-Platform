#include "Functions.h"

//Node struct pos(x,y) & father
struct node{
    //position in map matrix
    int x;
    int y;
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

//Helper functions
int next_action();
double distance_Calculate(int x1, int y1, int x2, int y2);
std::vector<Actions> get_Possible_Moves(int x, int y);
std::deque<node> get_Possible_Positions(node *father);
Actions get_What_Move_To_Make (int x, int y, int x2, int y2);
void update_objectives();

int main()
{
    bool gameover = false;
    clock_t t1,t2;
    float diff,seconds; 
    int action;
    plan = new std::deque<Actions>();
    
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
    if(  wordl_map[x + 1][y] != STONE && wordl_map[x + 1][y] != WALL &&  wordl_map[x + 1][y] != BOMB &&  wordl_map[x + 1][y] != EXPLOSION)
    {   
        node pos = {x+1,y,true,father};
        possible_moves.push_back(pos); //DOWN
    }
    if( wordl_map[x - 1][y] != STONE &&  wordl_map[x - 1][y] != WALL &&  wordl_map[x - 1][y] != BOMB &&  wordl_map[x - 1][y] != EXPLOSION)
    {
        node pos = {x-1, y,true,father};
        possible_moves.push_back(pos); //DOWN
    }
    if( wordl_map[x][y +1] != STONE && wordl_map[x][y + 1] != WALL &&  wordl_map[x][y + 1] != BOMB &&  wordl_map[x][y + 1] != EXPLOSION)
    {
        node pos = { x, y+1,true,father};
        possible_moves.push_back(pos); //DOWN
    }
    if(  wordl_map[x][y -1] != STONE && wordl_map[x][y - 1] != WALL &&  wordl_map[x][y - 1] != BOMB &&  wordl_map[x][y - 1] != EXPLOSION)
    {
        node pos = { x, y-1,true,father};
        possible_moves.push_back(pos); //DOWN
    }
    return possible_moves;
} 

void update_objectives(){
    double min_dist = 9999.00;
    int min_dist_pid = -1;
    for ( int i = 0; i < NUM_PLAYERS; i++)
    {
        if( i != PLAYER_ID && alive[i])
        {
            double tmp_dist = distance_Calculate(x[PLAYER_ID], y[PLAYER_ID], x[i], y[i]);
            if ( (tmp_dist - r[i]) < min_dist )
            {
                min_dist = tmp_dist;
                min_dist_pid = i;
            }
        }
    }
    if ( min_dist_pid != -1){
        mission_x = x[min_dist_pid];
        mission_y = y[min_dist_pid];
    }
    
}


/* AI agents next action */
int  next_action()
{
    bool seen;
    int action = -1;
    //Updates mission objective position
    update_objectives();
    
    if ( x[PLAYER_ID] == mission_x && y[PLAYER_ID] == mission_y)
        return -1;
    if ( plan->empty() )
    {
        /* Go to player 1 */
        node root = {x[PLAYER_ID], y[PLAYER_ID],false ,NULL};
        std::deque<node> visited;
        visited.push_front(root);
        std::deque<node> explore = get_Possible_Positions(&root);
        
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
                     if ( (tmp[i].x == mission_x &&  tmp[i].y == mission_y))
                     {
                         node* fat = tmp[i].father;
                         plan->push_front(get_What_Move_To_Make(fat->x, fat->y, tmp[i].x, tmp[i].y));
                         while ( fat->has_father )
                         {
                            plan->push_front(get_What_Move_To_Make(fat->father->x, fat->father->y, fat->x, fat->y));
                            fat = fat->father;
                         }
                         explore.clear();
                         break;
                     }
                }
            }
        }
        action = plan->front();
        plan->pop_front();
    }
    else
    {
        action = plan->front();
        plan->pop_front();
    }
    return action;
}