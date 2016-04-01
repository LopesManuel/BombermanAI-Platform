#include "SDL.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SPRITE_SIZE    32

int main(int argc, char* argv[])
{
	SDL_Surface *screen, *temp, *sprite, *grass;
	SDL_Rect rcSprite, rcGrass;
	SDL_Event event;
	Uint8 *keystate;
	int colorkey, gameover;

	/* initialize video system */
	SDL_Init(SDL_INIT_VIDEO);

	/* set the title bar */
	SDL_WM_SetCaption("SDL Move", "SDL Move");

	/* create window */
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

	/* load sprite */
	temp   = SDL_LoadBMP("sprite.bmp");
	sprite = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	/* setup sprite colorkey and turn on RLE */
	colorkey = SDL_MapRGB(screen->format, 255, 0, 255);
	SDL_SetColorKey(sprite, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);

	/* load grass */
	temp  = SDL_LoadBMP("grass.bmp");
	grass = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	/* set sprite position */
	rcSprite.x = 0;
	rcSprite.y = 0;

	gameover = 0;

	/* message pump */
	while (!gameover)
	{
		/* look for an event */
		if (SDL_PollEvent(&event)) {
			/* an event was found */
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
					}
					break;
			}
		}

		/* handle sprite movement */
		keystate = SDL_GetKeyState(NULL);

		if (keystate[SDLK_LEFT] ) {
			rcSprite.x -= 2;
		}
		if (keystate[SDLK_RIGHT] ) {
			rcSprite.x += 2;
		}
		if (keystate[SDLK_UP] ) {
			rcSprite.y -= 2;
		}
		if (keystate[SDLK_DOWN] ) {
			rcSprite.y += 2;
		}

		/* collide with edges of screen */
		if ( rcSprite.x < 0 ) {
			rcSprite.x = 0;
		}
		else if ( rcSprite.x > SCREEN_WIDTH-SPRITE_SIZE ) {
			rcSprite.x = SCREEN_WIDTH-SPRITE_SIZE;
		}
		if ( rcSprite.y < 0 ) {
			rcSprite.y = 0;
		}
		else if ( rcSprite.y > SCREEN_HEIGHT-SPRITE_SIZE ) {
			rcSprite.y = SCREEN_HEIGHT-SPRITE_SIZE;
		}

		/* draw the grass */
		for (int x = 0; x < SCREEN_WIDTH/SPRITE_SIZE; x++) {
			for (int y = 0; y < SCREEN_HEIGHT/SPRITE_SIZE; y++) {
				rcGrass.x = x * SPRITE_SIZE;
				rcGrass.y = y * SPRITE_SIZE;
				SDL_BlitSurface(grass, NULL, screen, &rcGrass);
			}
		}

		/* draw the sprite */
		SDL_BlitSurface(sprite, NULL, screen, &rcSprite);

		/* update the screen */
		SDL_UpdateRect(screen,0,0,0,0);
	}

	/* clean up */
	SDL_FreeSurface(sprite);
	SDL_FreeSurface(grass);
	SDL_Quit();

	return 0;
}
