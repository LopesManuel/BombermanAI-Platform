#include "SDL_Handler.h"

/* Handles all input events*/
void handle_Events(SDL_Event event)
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
                    if(map->can_Move(manual_Player, -1))
    					manual_Player->move(LEFT);
					break;
				case SDLK_RIGHT:
                    if(map->can_Move(manual_Player, 1))
    					manual_Player->move(RIGHT);
					break;
				case SDLK_UP:
                    if(map->can_Move(manual_Player, -NUM_COLS))
    					manual_Player->move(UP);
					break;
				case SDLK_DOWN:
                    if(map->can_Move(manual_Player, NUM_COLS))
					   manual_Player->move(DOWN);
					break;
                case SDLK_SPACE:
                    if(map->can_Move(manual_Player, 0)) 
                    {   // Can only plant 1 bomb per location 
                        Bomb* temp = new Bomb(manual_Player->get_X(), manual_Player->get_Y(), 3);
                        bombs.push_back(temp);
                        std::cout << "Bombs:" << bombs.size()<< std::endl;
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
	//Deallocate surface
	SDL_FreeSurface( gScreenSurface );
	gScreenSurface = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}


bool load_Media()
{
	//Loading success flag
	bool success = true;
	if( !load_Map("lvl1.txt") )
	{
		printf("Error loading map!");
		success = false;
	}
    map = new Map(world_Map);
	return success;
}
bool update_Game(std::vector<Player*> players)
{
	//Function success flag
	bool success = false;
    manual_Player = players.front();
    //Draws map
	refresh_Map();
	//Draws all bombs
    for ( Bomb* b : bombs){
		if( b != NULL && b->is_Alive())
		{

            if(b->update()){               
                if( b->get_turns_2explosion() > 0 )
                {
                    world_Map[b->get_mPosition()] = BOMB;
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
                bombs.pop_front();
            }
   		}
	}
    /*Checks if any player died this turn
        And draws all live players */
    for ( Player* p : players){
		if( p != NULL && p->is_Alive())
		{
            if( world_Map[p->get_mPosition()] == EXPLOSION )
                p->die();			
            else
                load_Object(p);
           success = true;
		}
	}
    return success;
}

void explode(int pos, int range, int direction){
    world_Map[pos] = EXPLOSION;

    for ( int i = 0; i < range; i++){
        if ( world_Map[pos + i * direction] == GRASS)
            world_Map[pos + i * direction] = EXPLOSION;
        if ( world_Map[pos + i * direction] == WALL || world_Map[pos + i * direction] == BOMB ) 
            break; 
        if( world_Map[pos + i * direction] == STONE ) 
        {
            world_Map[pos + i * direction] = EXPLOSION;
            break;
        }
    }
}
void clear_explosion(int pos, int range){
    world_Map[pos] = GRASS;
    for ( int i = 0; i < range; i++){
        if ( world_Map[pos + i] == EXPLOSION)
            world_Map[pos + i] = GRASS;
        if ( world_Map[pos - i] == EXPLOSION)
            world_Map[pos - i] = GRASS;
        if ( world_Map[pos + i*NUM_COLS] == EXPLOSION)
            world_Map[pos + i*NUM_COLS] = GRASS;
        if ( world_Map[pos - i*NUM_COLS] == EXPLOSION)
            world_Map[pos - i*NUM_COLS] = GRASS;
    }
}
void refresh_Map(){
	SDL_Rect rcPosition;
		/* Textures */
	SDL_Surface* grass = bitmap_Loader("grass.bmp");
	SDL_Surface* wall = bitmap_Loader("brick_wall.bmp");
	SDL_Surface* stone = bitmap_Loader("stone_wall.bmp");
    SDL_Surface* bomb = bitmap_Loader("bomb.bmp");
    SDL_Surface* explosion = bitmap_Loader("explosion.bmp");
	
    char ch;
	
	for (int y = 0; y < NUM_ROWS; y++) 
	{
		for (int x = 0; x < NUM_COLS; x++) 
		{
			char ch = world_Map[ mIndex(x,y)];
			rcPosition.x = x * SPRITE_SIZE;
			rcPosition.y = y * SPRITE_SIZE;
			if(ch == WALL)
				SDL_BlitSurface(wall, NULL, gScreenSurface, &rcPosition);
			else if (ch == GRASS)
				SDL_BlitSurface(grass, NULL, gScreenSurface, &rcPosition);
			else if (ch == STONE)
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
		}
	}
	SDL_FreeSurface(grass);
	SDL_FreeSurface(wall);
	SDL_FreeSurface(stone);
	SDL_FreeSurface(bomb);
	SDL_FreeSurface(explosion);		
}

bool load_Map(const char* path)
{
	SDL_Rect rcPosition;
	/* Textures */
	SDL_Surface* grass = bitmap_Loader("grass.bmp");
	SDL_Surface* wall = bitmap_Loader("brick_wall.bmp");
	SDL_Surface* stone = bitmap_Loader("stone_wall.bmp");

	
    /* Check if they were all loaded correctly*/
	if(grass == NULL || wall == NULL || stone == NULL) 
	{
		printf("Error loading map textures!");
		return false;
	}
	char ch;
	std::fstream fin( path, std::fstream::in);

	/* draw the map from the file */
	for (int y = 0; y < NUM_ROWS; y++) 
	{
		for (int x = 0; x < NUM_COLS; x++) 
		{
			fin >> std::noskipws >> ch;
			if(ch == '\n')
				fin >> std::noskipws >> ch;

			world_Map[ mIndex(x,y)] = ch;
			rcPosition.x = x * SPRITE_SIZE;
			rcPosition.y = y * SPRITE_SIZE;
			if(ch == '*')
				SDL_BlitSurface(wall, NULL, gScreenSurface, &rcPosition);
			else if (ch == '0')
				SDL_BlitSurface(grass, NULL, gScreenSurface, &rcPosition);
			else if (ch == '+')
				SDL_BlitSurface(stone, NULL, gScreenSurface, &rcPosition);
	
		}
	}
	SDL_FreeSurface(grass);
	SDL_FreeSurface(wall);
	SDL_FreeSurface(stone);
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

