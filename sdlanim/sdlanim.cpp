
#include "SDL.h"

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SPRITE_SIZE     32

int gameover;

/* source and destination rectangles */
SDL_Rect rcSrc, rcSprite;

void HandleEvent(SDL_Event event)
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
					if ( rcSrc.x == 192 )
						rcSrc.x = 224;
					else
						rcSrc.x = 192;
					rcSprite.x -= 5;
					break;
				case SDLK_RIGHT:
					if ( rcSrc.x == 64 )
						rcSrc.x = 96;
					else
						rcSrc.x = 64;
					rcSprite.x += 5;
					break;
				case SDLK_UP:
					if ( rcSrc.x == 0 )
						rcSrc.x = 32;
					else
						rcSrc.x = 0;
					rcSprite.y -= 5;
					break;
				case SDLK_DOWN:
					if ( rcSrc.x == 128 )
						rcSrc.x = 160;
					else
						rcSrc.x = 128;
					rcSprite.y += 5;
					break;
			}
			break;
	}
}

int main(int argc, char* argv[])
{
	SDL_Surface *screen, *temp, *sprite, *grass;
	SDL_Rect rcGrass;
	int colorkey;

	/* initialize SDL */
	SDL_Init(SDL_INIT_VIDEO);

	/* set the title bar */
	SDL_WM_SetCaption("SDL Animation", "SDL Animation");

	/* create window */
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

	/* set keyboard repeat */
	SDL_EnableKeyRepeat(70, 70);

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
	rcSprite.x = 150;
	rcSprite.y = 150;

	/* set animation frame */
	rcSrc.x = 128;
	rcSrc.y = 0;
	rcSrc.w = SPRITE_SIZE;
	rcSrc.h = SPRITE_SIZE;

	gameover = 0;

	/* message pump */
	while (!gameover)
	{
		SDL_Event event;
		
		/* look for an event */
		if (SDL_PollEvent(&event)) {
			HandleEvent(event);
		}

		/* collide with edges of screen */
		if (rcSprite.x <= 0)
			rcSprite.x = 0;
		if (rcSprite.x >= SCREEN_WIDTH - SPRITE_SIZE) 
			rcSprite.x = SCREEN_WIDTH - SPRITE_SIZE;

		if (rcSprite.y <= 0)
			rcSprite.y = 0;
		if (rcSprite.y >= SCREEN_HEIGHT - SPRITE_SIZE) 
			rcSprite.y = SCREEN_HEIGHT - SPRITE_SIZE;

		/* draw the grass */
		for (int x = 0; x < SCREEN_WIDTH / SPRITE_SIZE; x++) {
			for (int y = 0; y < SCREEN_HEIGHT / SPRITE_SIZE; y++) {
				rcGrass.x = x * SPRITE_SIZE;
				rcGrass.y = y * SPRITE_SIZE;
				SDL_BlitSurface(grass, NULL, screen, &rcGrass);
			}
		}

		/* draw the sprite */
		SDL_BlitSurface(sprite, &rcSrc, screen, &rcSprite);

		/* update the screen */
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}

	/* clean up */
	SDL_FreeSurface(sprite);
	SDL_FreeSurface(grass);
	SDL_Quit();

	return 0;
}
