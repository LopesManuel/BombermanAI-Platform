#include "SDL_Handler.h"

/* Handles all input events*/
void handle_Events(SDL_Event event, Map *map, Player *manual_Player)
{
	switch (event.type) {
		/* close button clicked */
		case SDL_QUIT:
			gameover = 1;
			break;
			
		/* handle the keyboard */
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					gameover = 1;
					break;
				case SDLK_LEFT:
                    if(manual_Player != NULL && map->can_Move(manual_Player, -1) && manual_Player->is_Alive())
    					manual_Player->move(LEFT);
					break;
				case SDLK_RIGHT:
                    if(manual_Player != NULL && map->can_Move(manual_Player, 1) && manual_Player->is_Alive())
    					manual_Player->move(RIGHT);
					break;
				case SDLK_UP:
                    if(manual_Player != NULL && map->can_Move(manual_Player, -NUM_COLS) && manual_Player->is_Alive())
    					manual_Player->move(UP);
					break;
				case SDLK_DOWN:
                    if(manual_Player != NULL && map->can_Move(manual_Player, NUM_COLS) && manual_Player->is_Alive())
					   manual_Player->move(DOWN);
					break;
                case SDLK_SPACE:
                    if(manual_Player != NULL && map->can_Move(manual_Player, 0) && manual_Player->is_Alive() && manual_Player->can_Place()) 
                    {   // Can only plant 1 bomb per location 
                        Bomb* temp = new Bomb(manual_Player);
                        map->add_bomb(temp);
                    }
                    break;
                case SDLK_RETURN:
                    if(manual_Player != NULL && manual_Player->has_detonator() && manual_Player->is_Alive()) 
                    {   // Detonates all is bombs
                        map->detonate(manual_Player->get_Id());
                    }
                    break;
			}
			break;
	}
}

/* Initializes SDL windows surface*/
bool init()
{

	//Initialization flag
	bool success = true;
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "Bomberman Learning Platform", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	return success;
}

/* Loads the bithmap and converts it to fisplay format*/
SDL_Surface* bitmap_Loader(const char* path)
{
	// load bitmap to temp surface 
	SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", path, SDL_GetError() );
		gameover = 1;
		return NULL;
	}
	
	//Convert surface to screen format 
	SDL_Surface* optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, NULL );
	if( optimizedSurface == NULL ) 
	{ 
		printf( "Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError() );
    } 
    
    //Get rid of old loaded surface 
    SDL_FreeSurface( loadedSurface );
	
	return optimizedSurface;
}

/* Free resources and close SDL */
void close()
{
    //Deallocate all surfaces
    SDL_FreeSurface(grass);
	SDL_FreeSurface(wall);
	SDL_FreeSurface(stone);
	SDL_FreeSurface(bomb);
	SDL_FreeSurface(explosion);	
    SDL_FreeSurface(bomb_pu);	
    SDL_FreeSurface(range_pu);	
 	SDL_FreeSurface(ghost_pu);
	SDL_FreeSurface(speed_pu);	
    SDL_FreeSurface(switch_pu);	
    SDL_FreeSurface(slide_pu);
     
	//Deallocate surface
	SDL_FreeSurface( gScreenSurface );
	gScreenSurface = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}


bool load_Media(char* lvl)
{
	//Loading success flag
	bool success = true;
	if( !load_Map(lvl) )
	{
		printf("Error loading map!");
		success = false;
	}
    grass = bitmap_Loader("Images/grass.bmp");
	wall = bitmap_Loader("Images/brick_wall.bmp");
	stone = bitmap_Loader("Images/stone_wall.bmp");
    bomb = bitmap_Loader("Images/bomb.bmp");
    explosion = bitmap_Loader("Images/explosion.bmp");
    bomb_pu = bitmap_Loader("Images/bomb_powerup.bmp");
    range_pu = bitmap_Loader("Images/range_powerup.bmp");
    ghost_pu =  bitmap_Loader("Images/ghost_powerup.bmp");
	speed_pu =  bitmap_Loader("Images/speed_powerup.bmp");	
    switch_pu =  bitmap_Loader("Images/switch_powerup.bmp");	
    slide_pu =  bitmap_Loader("Images/slide_powerup.bmp");
    
    return success;
}
void draw_Player(std::vector<Player*> players)
{
    /*Draw all live players */
    for ( Player* p : players){
		if( p != NULL && p->is_Alive())
		{
            load_Object(p);
		}
	}    
}


void draw_Map(Map *map)
{
	SDL_Rect rcPosition;
		/* Textures */	
    char ch;
    std::cout << "d1" << std::endl;
	for (int y = 0; y < NUM_ROWS; y++) 
	{
		for (int x = 0; x < NUM_COLS; x++) 
		{
			char ch = (*map)[ mIndex(x,y)];
			rcPosition.x = x * SPRITE_SIZE;
			rcPosition.y = y * SPRITE_SIZE;
			if(ch == WALL)
				SDL_BlitSurface(wall, NULL, gScreenSurface, &rcPosition);
			else if (ch == GRASS)
				SDL_BlitSurface(grass, NULL, gScreenSurface, &rcPosition);
			else if (ch == STONE || ch == STONE_PUP)
				SDL_BlitSurface(stone, NULL, gScreenSurface, &rcPosition);
            else if (ch == BOMB){
   				SDL_BlitSurface(grass, NULL, gScreenSurface, &rcPosition);
                /* setup sprite colorkey and turn on RLE */
	            int colorkey = SDL_MapRGB(gScreenSurface->format, 255, 0, 255);
	            SDL_SetColorKey(bomb, SDL_TRUE | SDL_RLEACCEL, colorkey);
            	SDL_BlitSurface(bomb, NULL, gScreenSurface, &rcPosition);
            }
	        else if (ch == EXPLOSION)
            { 
			     SDL_BlitSurface(grass, NULL, gScreenSurface, &rcPosition);         
                /* setup sprite colorkey and turn on RLE */
	           int colorkey = SDL_MapRGB(gScreenSurface->format, 255, 0, 255);
	           SDL_SetColorKey(explosion, SDL_TRUE | SDL_RLEACCEL, colorkey);
            	SDL_BlitSurface(explosion, NULL, gScreenSurface, &rcPosition);
            }
	        else if (ch == BOMB_PUP)
            { 
			     SDL_BlitSurface(bomb_pu, NULL, gScreenSurface, &rcPosition);         
            }
	        else if (ch == RANGE_PUP)
            { 
			     SDL_BlitSurface(range_pu, NULL, gScreenSurface, &rcPosition);         
            }
	        else if (ch == GHOST_PUP)
            { 
			     SDL_BlitSurface(ghost_pu, NULL, gScreenSurface, &rcPosition);         
            }
	        else if (ch == SPEED_PUP)
            { 
			     SDL_BlitSurface(speed_pu, NULL, gScreenSurface, &rcPosition);         
            }
	        else if (ch == SWITCH_PUP)
            { 
			     SDL_BlitSurface(switch_pu, NULL, gScreenSurface, &rcPosition);         
            }
	        else if (ch == SLIDE_PUP)
            { 
			     SDL_BlitSurface(slide_pu, NULL, gScreenSurface, &rcPosition);         
            }
		}
	}	
    std::cout << "d2" << std::endl;
}

bool load_Map(const char* path)
{
	char ch;
	std::fstream fin( path, std::fstream::in);
    if ( fin == NULL)
        return false;
	/* Gets the map from the input */
	for (int y = 0; y < NUM_ROWS; y++) 
	{
		for (int x = 0; x < NUM_COLS; x++) 
		{
			fin >> std::noskipws >> ch;
			if(ch == '\n')
				fin >> std::noskipws >> ch;
                
			world_Map[ mIndex(x,y)] = ch;
		}
	}
	return true;
}


template <typename Type> 
void load_Object(Type object)
{
	SDL_Rect rcobject;
	rcobject.x = object->get_X();
	rcobject.y = object->get_Y();
	SDL_Surface* object_surface = bitmap_Loader(object->get_Skin());

	/* setup sprite colorkey and turn on RLE */
	int colorkey = SDL_MapRGB(gScreenSurface->format, 255, 0, 255);
	SDL_SetColorKey(object_surface, SDL_TRUE | SDL_RLEACCEL, colorkey);

	SDL_BlitSurface(object_surface, NULL, gScreenSurface, &rcobject);
	SDL_FreeSurface(object_surface);
    
}

