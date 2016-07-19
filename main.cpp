#include "Global_Vars.h"
#include "SDL_Handler.h"
#include "Communicator.h"
#include "Player.h"
#include "Logger.h"

//Screen dimension constants 
const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;
int SPRITE_SIZE   = 32;
int NUM_COLS = SCREEN_WIDTH / SPRITE_SIZE;
int NUM_ROWS = SCREEN_HEIGHT / SPRITE_SIZE;

int speed;
int seed = -1; 
// Global logic 
int gameover = 0;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Temporary events
SDL_Event event;

// to check if needs to draw
bool draw_screen = true;

//Players' holder
Player* player1 = new Player("Images/player.bmp" , 1, 1, 1, 1);
Player* player2 = new Player("Images/player2.bmp" , 18, 1, 2, 2);
Player* player3 = new Player("Images/player3.bmp" , 1, 13, 3, 3);
Player* player4 = new Player("Images/player4.bmp" , 18, 13, 4, 4);
std::vector<Player*> all_Players;

//Number of active players
int num_Players = 0;
//Total number of starting players
int num_SPlayers = 0;

//Manual player, receives input from keyboard
Player *manual_Player;
int manual_Player_id = -1;
//Number of AI agents connected
int connected = 0; 

//Holds world map vector
char* world_Map = (char*) malloc(sizeof(char) * ((NUM_COLS) * (NUM_ROWS) ));

//Holds initial level preset
char* level;

//Waits for map update before changing it
bool map_updated = true;
bool bomb_added = true; 
bool map_randomizer = false;

//Map object to control all events
Map* map;

//Group of threads to handle player's communication
std::thread* p_comm;
// Comands to execute the ai agent program
char executes[4][25];
// File descriptores 
int  fdread[4][2];
int  fdwrite[4][2];
// a global instance of std::mutex to protect global variable
std::mutex myMutex;

// Log communicator
bool keep_log = false;
int state = 0;
bool replaying = false;
Logger* lg =  new Logger();
std::fstream log_data;

//Reinforcement learning variables
bool learning = false;
int objective_x, objective_y;
int episode_count = 0;
int max_ep_count = 600;

//Parses the comands from shell
void cmdParse(int argc , char* argv[]);
//Initializes the players that are going to play
void init_Players();
void comm_thread(int i );


int main ( int argc, char *argv[] )
{
    //Parse comands
    cmdParse(argc, argv);
    init_Players();
	
    //Start up SDL and create window
	if( draw_screen && !init() )
	{
		printf( "Failed to initialize!\n" );
		exit(-1);
	}
	//Loads lvl map
	if( !load_Map(level) )
	{
		printf("Error loading map!");
        exit(-1);
	}
    //Load lvl and all images 
	if(draw_screen && !load_Media(level))
	{
        std::cout << "error : " << level << std::endl;
		printf( "Failed to load textures!\n" );
		exit(-1);
	}
	else
	{			
        map = new Map(world_Map);
        int count = 0;
            
        //Create a thread for each AI agent
        for ( int j = 1; j <= connected; j++)
        {
            //Sends map updates to AI agents
            p_comm[j-1] = std::thread(comm_thread, j-1);
        }
        if ( keep_log )
            lg->write_Log();
        /* Simulation time*/
        clock_t t1,t2;
        float diff,seconds;
        t1=clock(); 
		/* message pump */
		while (!replaying && !gameover  && num_Players > 1 )
		{
            count++;
			/* look for an event */
			if (SDL_PollEvent(&event) ){
				handle_Events(event, map, manual_Player);
			}
            /* If map changed write it to the log*/
             map->update_Game(all_Players);
            if (keep_log ){
                lg->write_state(all_Players);
            }   
            if ( draw_screen ){       
                draw_Map(map);
                draw_Player(all_Players);
                //Update the surface
                SDL_UpdateWindowSurface( gWindow );
            }
            //Update time counters
            if ( count % 10000 == 0 && !learning)
            {
                count = 1;
                t2=clock();
                diff  = ((float)t2-(float)t1);
                seconds = diff / CLOCKS_PER_SEC;
                //4 minutes s the simulation max time
                if ( seconds >= 60 ) 
                {
                    std::cout << "Simulation exceeded time limit!" << std::endl;
                    exit(-1);
                }
            }
            //Reset positions Reinforcement Learning agents
            if ( learning )
            {
                count = 1;
                map->check_Learners(all_Players);
                if ( episode_count >= max_ep_count)
                    gameover = 1;
            } 
		}
        lg->write_state(all_Players);
        /* Replays game from log file*/
        if( replaying ){
            int state = 1;
            while (!gameover)
		    {
                /* look for an event */
                if (SDL_PollEvent(&event) ){
                    handle_Replay_Controls(event, state, lg, all_Players);
                }
               map->update_Game(all_Players);
               draw_Map(map);
               draw_Player(all_Players);
               //Update the surface
			   SDL_UpdateWindowSurface( gWindow );
            }
        }
        else 
        {
             int winner =  map->who_Won(all_Players);
            if ( winner == -1){
                std::cout << "Draw!!" << std::endl;
                lg->write_winner(winner, NULL);
            }
            else
            {
                std::cout << "Player " << winner+1 << " won!! Congratulations! Agent:"<< executes[winner] << std::endl;
                lg->write_winner(winner, all_Players[winner]->get_agent());
            }
            int rank[all_Players.size()];
            for (int i=0; i < all_Players.size(); i++) 
            {
                rank[i] = all_Players[i]->turns;
            }  
            //Now we call the sort function
            std::sort(rank, rank + all_Players.size(),std::greater<int>());

            std::cout << "Rank:" << std::endl;
            for (size_t i = 0; i < all_Players.size(); ++i)
            {
                for (size_t j = 0; j < all_Players.size(); ++j)
                    if ( all_Players[j]->turns == rank[i]){
                         if ( (i == 0 || i==1) && winner == j )
                            std::cout << "1."<< executes[j] << " Turns: "<<rank[i] << " Bombs:" << all_Players[j]->num_bombs_planted<< std::endl;
                        else if ( (i == 0 ) && winner != j )
                            std::cout << "2."<< executes[j] << " Turns: "<<rank[i] << " Bombs:" << all_Players[j]->num_bombs_planted<< std::endl;
                        else
                            std::cout << i+1<<"."<< executes[j] << " Turns: "<<rank[i] << " Bombs:" << all_Players[j]->num_bombs_planted<< std::endl;
                        all_Players[j]->turns =-1;
                        break;
                    }
                    
            }
        }
    }

	close();
	return 0;
}


/*    Helper functions    */

void cmdParse(int argc , char* argv[])
{
    for(int i = 0; i < argc; i++)
    {
        if( strcmp(argv[i], "-replay") == 0 )
        {
            if(argv[i+1] == NULL)
            {
                printf( "Missing log file!\n" );
                exit(-1);   
            }
            else{
                log_data.open(argv[i+1],std::fstream::in );
                if (log_data.is_open())
                {
                    replaying = true;
                    lg->read_log_header();
                    break;
                }
                else
                {
                    std::cout << "Error opening file";
                    exit(-1);
                }
            }
        }   
        else if( strcmp(argv[i], "-learning") == 0 )
        {
            learning = true;
            if ( i+1 > argc || i+2 > argc )
            {
                printf( "Missing arguments!\n" );
                exit(-1);   
            }
            int ox = atoi(argv[i+1]);
            if( ox <= 0 || ox > NUM_COLS)
            {
                printf( "Objective X is not in reach!\n" );
                exit(-1);   
            }
            else{
                objective_x = ox - 1;
            }
            int oy = atoi(argv[i+2]);
            if( oy <= 0 || oy > NUM_ROWS)
            {
                printf( "Objective Y is not in reach!\n" );
                exit(-1);   
            }
            else{
                objective_y = oy - 1;
            }
            std::cout << "Reinforcement Learning agents learning! ";
            std::cout << "Objective: (" << objective_x  << "," << objective_y << ")"<< std::endl;
                
        }    
        else if( strcmp(argv[i], "-n") == 0 )
        {
            int np = atoi(argv[i+1]);
            if( np > 4 || np <= 0)
            {
                printf( "Max number of players is 4!\n" );
                exit(-1);   
            }
            p_comm = new std::thread[np];
            num_Players += np;
            connected = np;
        }
        else if( strcmp(argv[i], "-l") == 0 )
        {
            level = argv[i+1];
        }
        else if( strcmp(argv[i], "-maprand") == 0 )
        {
            map_randomizer = true;
        }
        else if( strcmp(argv[i], "-m") == 0 )
        {
            if ( connected != 0)
            {
                printf( "-m flag should come first than -p and -n!\n" );
                exit(-1);   
            }
            else{
                num_Players++;
            }
        }
        else if( strcmp(argv[i], "-s") == 0 )
        {
            speed = atoi(argv[i+1]);
        }
        else if ( strcmp(argv[i], "-p") == 0 )
        {
            //Create a thread for each AI agent
            for ( int j = 1; j <= connected; j++)
            {
                if(argv[i+j] == NULL)
                {
                    printf( "Missing artificial inteligence for some players!\n" );
                    exit(-1);   
                }
                else if( argv[i+j][0] == '-'){
                    printf( "Not enough AI agents passed!\n" );
                    exit(-1);   
                }
                int cx;
                cx = snprintf ( executes[j-1], 15, "./%s", argv[i+j]);

                p_comm[j-1] = std::thread(connect_thread, (j-1));
                p_comm[j-1].join(); 
            }
            //Check if the number of players is less than 4 
            if( num_Players > 4 )
            {
                printf( "Can't add manual player! Max number of players is 4!\n" );    
            }
            else
            {
                manual_Player_id = connected;    
            }
        }
        else if ( strcmp(argv[i], "-log") == 0 )
        {
            keep_log = true;
        } 
        else if ( strcmp(argv[i], "-seed") == 0 )
        {
            if(argv[i+1] == NULL)
            {
                printf( "Missing seed number!\n" );
                exit(-1);   
            }
            else if( atoi(argv[i+1]) > 0 ){
                seed = atoi(argv[i+1]);
                srand(seed);
            }
            else{
                printf( "Error on seed number!\n" );
                exit(-1);
            }
         }
        else if ( strcmp(argv[i], "-eps") == 0 )
        {
            if(argv[i+1] == NULL)
            {
                printf( "Missing total number of episodes!\n" );
                exit(-1);   
            }
            else if( atoi(argv[i+1]) > 0 ){
                max_ep_count = atoi(argv[i+1]);
            }
            else{
                printf( "Error on number of episodes!\n" );
                exit(-1);
            }
         }
         else if ( strcmp(argv[i], "-f") == 0 )
         {
             draw_screen = false;
         }
    }
    num_SPlayers = num_Players;
}

void init_Players()
{
    int tmp_num_Players = num_Players;
    if( tmp_num_Players == 4 ){
        player4->set_agent(executes[3]);
    	all_Players.insert(all_Players.begin(),player4);
        tmp_num_Players--;
    }
    if( tmp_num_Players == 3 ){
        player3->set_agent(executes[2]);
    	all_Players.insert(all_Players.begin(),player3);
        tmp_num_Players--;
    }
    if( tmp_num_Players == 2 ){
        player2->set_agent(executes[1]);
    	all_Players.insert(all_Players.begin(),player2);
        tmp_num_Players--;
    }

    if( tmp_num_Players == 1){
        player1->set_agent(executes[0]);
        all_Players.insert(all_Players.begin(),player1);
        tmp_num_Players--;
    }
    if ( manual_Player_id != -1){
         manual_Player = all_Players[manual_Player_id];
         if( all_Players[manual_Player_id] != NULL)
            all_Players[manual_Player_id]->set_agent("ManualPlayer");
    }

}


void comm_thread(int i ) 
{    
    while(!gameover  && num_Players > 1)
    {
        send_Map(i);
        get_Action(i, all_Players, map);
    }
}


