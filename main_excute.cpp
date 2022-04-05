#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <cmath>
#include <ctime>
using namespace std;
#include "game_set_up_stuff.h"
#include "game_object.h"

int main(int argc, char* argv[]) {
srand(time(0));
//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Event handler
			SDL_Event e;
			//The picture that will be moving around on the screen
			Head head;
			Body body[(SCREEN_HEIGHT/50)*(SCREEN_WIDTH/50)];
			Fruit fruit;
			//Current animation frame
			int headFrame = 0;
			int startScreenFrame = 0;
			int backgroundFrame = 0;
			//While application is running
			//start screen loop
			while(!game.gameStart)
                    {
                        while(SDL_PollEvent( &e ) != 0)
                        {
                            //User requests quit
                            if( e.type == SDL_QUIT )
                            {
                                game.gameStart = true;
                                game.gameOver = true;
                            };
                            if(e.type == SDL_KEYDOWN) game.gameStart = true;
                        }
                        SDL_RenderClear( gRenderer );
                        // start screen animation
                        currentStartScreenClip = &gStartScreenSpriteClips[startScreenFrame / 10];
                        ++startScreenFrame;
                        //Cycle animation
                        if( startScreenFrame / 10 >= STARTSCREEN_ANIMATION_FRAMES )
                        {
                            startScreenFrame = 0;
                        };
                        gStartScreenSprite.render(0, 0, currentStartScreenClip, textureResize);
                        SDL_RenderPresent( gRenderer );
                    }
            //actual game loop
			while( !game.gameOver )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						game.gameOver = true;
					};
					//Handle input for the head
					head.handleEvent( e );
				}
				// head animation
				currentHeadClip = &gHeadSpriteClips[headFrame / 1];
				++headFrame;
				//Cycle animation
				if( headFrame / 1 >= WALKING_ANIMATION_FRAMES )
				{
					headFrame = 0;
				}
                //background animaton
                currentBackgroundClip = &gBackgroundSpriteClips[backgroundFrame / 2];
				++backgroundFrame;
				//Cycle animation
				if( backgroundFrame / 2 >= BACKGROUND_ANIMATION_FRAMES )
				{
					backgroundFrame = 0;
				}
				//Move the picture
				body[0].mPosX = head.mPosX;
				body[0].mPosY = head.mPosY;
				int tempX = body[0].mPosX;
                int tempY = body[0].mPosY;
				head.move();
				//check if head eat fruit
				fruit.checkCollision(head.mCollider, body);
				//generate the body
				for(int i=1; i<=game.score; i++) {
                    int temp2X = body[i].mPosX;
                    int temp2Y = body[i].mPosY;
                    body[i].mPosX = tempX;
                    body[i].mPosY = tempY;
                    tempX = temp2X;
                    tempY = temp2Y;
                    body[i].mCollider.x =  body[i].mPosX;
                    body[i].mCollider.y =  body[i].mPosY;
				}
				//check if head collide with body
				for(int i=1; i<=game.score; i++) {
                    body[i].checkCollision(head.mCollider);
				}
				//Clear screen
				//SDL_SetRenderDrawColor( gRenderer, 159, 159, 159, 255 );
				SDL_RenderClear( gRenderer );
				//Render texture to screen
				gBackgroundSprite.render(0, 0, currentBackgroundClip, textureResize);
				head.render();
				for(int i=1; i<=game.score; i++) {
                    body[i].render();
				}
				fruit.render();
				gScoreText.loadFromRenderedText( "Score: "+to_string(game.score) );
				gScoreText.render(0, 0);
				//Update screen
				SDL_RenderPresent( gRenderer );
				SDL_Delay(170);
            }
        }
    }
	//Free resources and close SDL
	close();

return 0;
}
