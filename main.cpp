#include "Global_Vars.h"
#include "SDL_Handler.h"
#include "Player.h"

//Screen dimension constants 
const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;
const int SPRITE_SIZE   = 32;
const int NUM_COLS = SCREEN_WIDTH / SPRITE_SIZE;
const int NUM_ROWS = SCREEN_HEIGHT / SPRITE_SIZE;

// Global logic 
int gameover = 0;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Temporary events
SDL_Event event;

//Players' holder
Player* player1 = new Player("Images/player.bmp" , 1, 1);
Player* player2 = new Player("Images/player2.bmp" , 18, 13);
Player* player3 = new Player("Images/player2.bmp" , 1, 13);
Player* player4 = new Player("Images/player2.bmp" , 18, 1);
std::vector<Player*> all_Players;

//Number of active players
int num_Players = 2;

//Manual player, receives input from keyboard
Player *manual_Player;

//Holds world map vector
char* world_Map = (char*) malloc(sizeof(char) * ((NUM_COLS) * (NUM_ROWS) ));

//Holds world map vector
char* level;

//Map object to control all events
Map* map;

//Group of threads to handle player's communication
std::thread *p_comm = new std::thread[num_Players - 1];

//Parses the comands from shell
void cmdParse(int argc , char* argv[]);

void init_Players();

int main ( int argc, char *argv[] )
{
    //Parse comands
    cmdParse(argc, argv);
    init_Players();
	
    //Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
		exit(-1);
	}
    //Load lvl and all images 
	if(!load_Media(level))
	{
		printf( "Failed to load textures!\n" );
		exit(-1);
	}
	else
	{			
        map = new Map(world_Map);	
		/* message pump */
		while (!gameover  && num_Players > 1 )
		{
			/* look for an event */
			if (SDL_PollEvent(&event) ){
				handle_Events(event, map, all_Players.front());
			}
            map->update_Game(all_Players);         
            draw_Map(map);
            draw_Player(all_Players);
			//Update the surface
			SDL_UpdateWindowSurface( gWindow );
		}
        const char * winner =  map->who_Won(all_Players);
        if ( winner == NULL)
            std::cout << "Draw!!" << std::endl;
        else
            std::cout << "Player " << winner << " won!! Congratualtions!" << std::endl;
	}

	close();
	return 0;
}

void cmdParse(int argc , char* argv[])
{
    for(int i = 0; i < argc; i++)
    {
        if( strcmp(argv[i], "-n") == 0 )
        {
            int np = atoi(argv[i+1]);
            if( np > 4 || np <= 0)
            {
                printf( "Max number of players is 4!\n" );
                exit(-1);   
            }
            else
                num_Players = np;
        }
        else if( strcmp(argv[i], "-l") == 0 )
        {
            level = argv[i+1];
        }
        else if ( strcmp(argv[i], "-p") == 0 )
        {
            for ( int j = 1; j <= num_Players; j++)
            {
                if(argv[i+j] == NULL)
                {
                    printf( "Missing artificial inteligence for some players!\n" );
                    exit(-1);   
                }
                char execute [80];
                sprintf(execute," g++ -o player%d Agents/%s && ./player%d",j,argv[i+j],j);
                std::cout << execute << std::endl;
                system(execute);
            }
        }
        else if( strcmp(argv[i], "-m") == 0 )
        {
            manual_Player = player1;
        }
    }
    std::cout << num_Players << std::endl;
}

void init_Players()
{
    int tmp_num_Players = num_Players;
    if( tmp_num_Players == 4 ){
    	all_Players.insert(all_Players.begin(),player4);
        tmp_num_Players--;
    }
    if( tmp_num_Players == 3 ){
    	all_Players.insert(all_Players.begin(),player3);
        tmp_num_Players--;
    }
    if( tmp_num_Players == 2 ){
    	all_Players.insert(all_Players.begin(),player2);
        tmp_num_Players--;
    }
    if( tmp_num_Players == 1){
        all_Players.insert(all_Players.begin(),player1);
        tmp_num_Players--;
    }
        std::cout << "Size:" << all_Players.size() << std::endl;
}
