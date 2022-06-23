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
			Fruit lightningFruit;
			//Current animation frame
			int headFrame = 0;
			int startScreenFrame = 0;
			int backgroundFrame = 0;
			int gameOverFrame = 0;
			//event count
			int i = 0;
			//While application is running
			//start screen loop
			while( !game.gameStart || !game.gameOver || !game.gameEnd )
            {
                //handle event if detected
                while(SDL_PollEvent( &e ) != 0)
                    {
                        //User requests quit
                        if( e.type == SDL_QUIT )
                        {
                            game.gameStart = true;
                            game.gameOver = true;
                            game.gameEnd = true;
                        };
                        //Handle input for the game
                        game.gameEvent( e );
                        if(e.type == SDL_KEYDOWN && !game.gameStart)
                        {
                            if(e.key.keysym.sym == SDLK_o || e.key.keysym.sym == SDLK_p)
                            {
                                game.gameStart = true;
                            };
                        };
                        if(e.type == SDL_KEYDOWN && !game.gameEnd && game.gameOver && game.deathAnimation)
                        {
                            game.gameEnd = true;
                        };
                        //move the head
                        head.headEvent( e );
                    }
                //start screen render
                if(!game.gameStart)
                {
                    SDL_RenderClear( gRenderer );
                    // start screen animation
                    currentStartScreenClip = &gStartScreenSpriteClips[startScreenFrame / 5];
                    startScreenFrame++;
                    //Cycle animation
                    if( startScreenFrame / 5 >= STARTSCREEN_ANIMATION_FRAMES )
                    {
                        startScreenFrame = 0;
                    };
                    gStartScreenSprite.render(0, 0, currentStartScreenClip, textureResize);
                    SDL_RenderPresent( gRenderer );
                }
                //actual game render
                if( !game.gameOver && game.gameStart )
                {
                    // head animation
                    currentHeadClip = &gHeadSpriteClips[headFrame / 1];
                    headFrame++;
                    //Cycle animation
                    if( headFrame / 1 >= WALKING_ANIMATION_FRAMES )
                    {
                        headFrame = 0;
                    }
                    //background animaton
                    currentBackgroundClip = &gBackgroundSpriteClips[backgroundFrame / 2];
                    backgroundFrame++;
                    //Cycle animation
                    if( backgroundFrame / 2 >= BACKGROUND_ANIMATION_FRAMES )
                    {
                        backgroundFrame = 0;
                    }
                    //Move the picture
                    body[0].mPosX = head.getX();
                    body[0].mPosY = head.getY();
                    int tempX = body[0].mPosX;
                    int tempY = body[0].mPosY;
                    head.moveHead();
                    //check if head eat fruit
                    //if there is space for 2 fruit
                    if(game.score < (SCREEN_HEIGHT/50)*(SCREEN_WIDTH/50)-2)
                    {
                        if( fruit.checkCollision(head, body, &lightningFruit) )
                        {
                            //Play eat sound effect
                            Mix_PlayChannel( -1, gEat, 0 );
                        };
                        if( lightningFruit.checkCollision(head, body, &fruit) )
                        {
                            game.speed = 100;
                            game.speedTimer = 0;
                            //Play fast eat sound effect
                            Mix_PlayChannel( -1, gFastEat, 0 );
                        };
                    } else if( fruit.checkCollision(head, body, &lightningFruit) )
                        {
                            //Play eat sound effect
                            Mix_PlayChannel( -1, gEat, 0 );
                        };
                    //redo speed after some time
                    game.speedCheck();
                    //generate the body cordinate=
                    for(int i=1; i<=game.score; i++) {
                        int temp2X = body[i].mPosX;
                        int temp2Y = body[i].mPosY;
                        body[i].mPosX = tempX;
                        body[i].mPosY = tempY;
                        tempX = temp2X;
                        tempY = temp2Y;
                    }
                    //check if head collide with body
                    for(int i=1; i<=game.score; i++)
                        {
                        body[i].checkCollision(head);
                        }
                    //Clear screen
                    SDL_RenderClear( gRenderer );
                    //Render texture to screen
                    gBackgroundSprite.render(0, 0, currentBackgroundClip, textureResize);
                    head.render();
                    for(int i=1; i<=game.score; i++)
                    {
                        body[i].render();
                    }
                    fruit.renderNormal();
                    if( game.score < (SCREEN_HEIGHT/50)*(SCREEN_WIDTH/50)-2 )lightningFruit.renderLightning();
                    gScoreText.loadFromRenderedText( "Score: "+ to_string(game.score), {255, 255, 51} );
                    gScoreText.render(0, 0);
                    //Update screen
                    SDL_RenderPresent( gRenderer );
                    SDL_Delay(game.speed);
                }
                //play death animation
                if( game.gameOver && !game.deathAnimation )
                {
                    head.reposition();
                    SDL_RenderClear( gRenderer );
                    //Render texture to screen
                    gBackgroundSprite.render(0, 0, currentBackgroundClip, textureResize);
                    for(int i=1; i<=game.score; i++) {
                        body[i].render();
                    }
                    fruit.renderNormal();
                    if( game.score < (SCREEN_HEIGHT/50)*(SCREEN_WIDTH/50)-2 ) lightningFruit.renderLightning();
                    if(i<1) {head.minusY(); i++; Mix_HaltMusic(); Mix_PlayChannel( -1, gDeath, 0 );} else head.plusY();
                    head.render();
                    gScoreText.loadFromRenderedText( "Score: "+ to_string(game.score), {255, 255, 51} );
                    gScoreText.render(0, 0);
                    //Update screen
                    SDL_RenderPresent( gRenderer );
                    if(head.getY() >= SCREEN_HEIGHT) game.deathAnimation = true;
                    SDL_Delay(130);
                }
                //end screen render
                if( game.gameOver && !game.gameEnd && game.deathAnimation)
                {
                    SDL_RenderClear( gRenderer );
                    if(game.endAnimationTimer <= 1000)
                    {
                        currentGameOverClip = &gGameOverSpriteClips[gameOverFrame / 25];
                        gameOverFrame++;
                        //Cycle animation
                        if( gameOverFrame / 25 >= GAMEOVER_ANIMATION_FRAMES1 )
                        {
                            gameOverFrame = 0;
                        };
                        gGameOverSprite1.render(0, 0, currentGameOverClip, textureResize);
                    }
                    if(game.endAnimationTimer > 1000 && game.endAnimationTimer <= 2100)
                    {
                        currentGameOverClip = &gGameOverSpriteClips[gameOverFrame / 2];
                        gameOverFrame++;
                        //Cycle animation
                        if( gameOverFrame / 2 >= GAMEOVER_ANIMATION_FRAMES2 )
                        {
                            gameOverFrame = 0;
                        };
                        gGameOverSprite2.render(0, 0, currentGameOverClip, textureResize);
                    }
                    if(game.endAnimationTimer > 2100)
                    {
                        currentGameOverClip = &gGameOverSpriteClips[gameOverFrame / 2];
                        gameOverFrame++;
                        //Cycle animation
                        if( gameOverFrame / 2 >= GAMEOVER_ANIMATION_FRAMES2 )
                        {
                            gameOverFrame = 0;
                        };
                        gGameOverSprite3.render(0, 0, currentGameOverClip, textureResize);
                    }
                    gGameOverScoreText.loadFromRenderedText("Your score is: " + to_string(game.score), {255, 255, 51} );
                    gGameOverScoreText.render( (SCREEN_WIDTH - gGameOverScoreText.getWidth())/2, (SCREEN_HEIGHT - gGameOverScoreText.getHeight())/2 - 50 );
                    SDL_RenderPresent( gRenderer );
                    if(i==1) {Mix_PlayMusic( gMusic4, -1 ); i++;};
                    game.endAnimtionCheck();
                    game.endAnimationTimer++;
                }

            }
        }
	}
	//Free resources and close SDL
	close();

return 0;
}
