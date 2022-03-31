class Head
{
    public:
		//The X and Y offsets of the head
		int mPosX, mPosY;
		//The velocity of the head
		int mVelX, mVelY;
        //Head's collision box
		SDL_Rect mCollider;
		//The dimensions of the head
		static const int HEAD_WIDTH = 50;
		static const int HEAD_HEIGHT = 50;
		double degrees;
		//Maximum axis velocity of the head
		static const int HEAD_VEL = 50;
		//Initializes the variables
		Head()
		{
        //Initialize the offsets
        mPosX = (SCREEN_WIDTH - HEAD_WIDTH)/2 ;
        mPosY = (SCREEN_HEIGHT - HEAD_HEIGHT)/2 ;
        //Initialize the velocity
        mVelX = 0;
        mVelY = 0;
        //Set collision box dimension
        mCollider.w = HEAD_WIDTH;
        mCollider.h = HEAD_HEIGHT;
        };
		//Takes key presses and adjusts the head's velocity
		void handleEvent( SDL_Event& e )
		{
            //If a key was pressed
            if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
            {
                //Adjust the velocity
                switch( e.key.keysym.sym )
                {
                //Play the music
                case SDLK_1: Mix_PlayMusic( gMusic1, -1 ); break;
                case SDLK_2: Mix_PlayMusic( gMusic2, -1 ); break;
                case SDLK_3: Mix_PlayMusic( gMusic3, -1 ); break;
                case SDLK_9:
                        //If music is being played
                            //If the music is paused
                            if( Mix_PausedMusic() == 1 )
                            {
                                //Resume the music
                                Mix_ResumeMusic();
                            }
                            //If the music is playing
                            else
                            {
                                //Pause the music
                                Mix_PauseMusic();
                            }
                        break;
                    //Stop the music
                    case SDLK_0: Mix_HaltMusic(); break;
                    //moving for head
                    case SDLK_UP: mVelY = -HEAD_VEL; mVelX = 0; degrees = 270; headFlip = SDL_FLIP_NONE; break;
                    case SDLK_DOWN: mVelY = HEAD_VEL; mVelX = 0; degrees = 90; headFlip = SDL_FLIP_VERTICAL; break;
                    case SDLK_LEFT: mVelX = -HEAD_VEL; mVelY = 0; degrees = 0; headFlip = SDL_FLIP_HORIZONTAL; break;
                    case SDLK_RIGHT: mVelX = HEAD_VEL; mVelY = 0; degrees = 0; headFlip = SDL_FLIP_NONE; break;
                }
            }
        };
		//Moves the head
		void move()
		{
            //Move the head left or right
            mPosX += mVelX;
            mCollider.x = mPosX;
            //If the head went too far to the left or right
            if( (mPosX < 0) || (mPosX + HEAD_WIDTH > SCREEN_WIDTH) )
            {
                //Move back
                game.gameOver = true;
            }
            //Move the head up or down
            mPosY += mVelY;
            mCollider.y = mPosY;
            //If the head went too far up or down
            if( (mPosY < 0) || (mPosY + HEAD_HEIGHT > SCREEN_HEIGHT) )
            {
                //Move back
                game.gameOver = true;
            }
        };
		//Shows the head on the screen
		void render()
		{
        //Show the head
        gHeadSprite.render( mPosX, mPosY, currentHeadClip, NULL, degrees, NULL, headFlip );
        };
};

class Body
{
        public:
		//The X and Y offsets of the body;
		int mPosX, mPosY;
        SDL_Rect mCollider;
		//The dimensions of the body
		static const int BODY_WIDTH = 50;
		static const int BODY_HEIGHT = 50;
    Body()
    {
        //set collider box dimension
        mCollider.w = BODY_WIDTH;
        mCollider.h = BODY_HEIGHT;
    }
    void render()
		{
        //Show the body
        gBody.render( mPosX, mPosY );
        };
    void checkCollision(SDL_Rect a)
    {
        //The sides of the rectangles
        int leftA, leftB;
        int rightA, rightB;
        int topA, topB;
        int bottomA, bottomB;
        //Calculate the sides of rect A
        leftA = a.x;
        rightA = a.x + a.w;
        topA = a.y;
        bottomA = a.y + a.h;
        //Calculate the sides of fruit
        leftB = mCollider.x;
        rightB = mCollider.x + mCollider.w;
        topB = mCollider.y;
        bottomB = mCollider.y + mCollider.h;
        if( (rightA==rightB&&leftA==leftB&&topA==topB&&bottomA==bottomB) )
        {
            game.gameOver = true;
        }
    }
};

class Fruit
{
    private:
		//The X and Y offsets of the fruit;
		int mPosX, mPosY;
    public:
        SDL_Rect mCollider;
		//The dimensions of the dot
		static const int FRUIT_WIDTH = 50;
		static const int FRUIT_HEIGHT = 50;
    Fruit()
    {
        mPosX = (rand() % (SCREEN_WIDTH/FRUIT_WIDTH))*50;
        mPosY = (rand() % (SCREEN_HEIGHT/FRUIT_HEIGHT))*50;
        //Set collision box dimension
        mCollider.x = mPosX;
        mCollider.y = mPosY;
        mCollider.w = FRUIT_WIDTH;
        mCollider.h = FRUIT_HEIGHT;
    }
    void render()
		{
        //Show the fruit
        gFruit.render( mPosX, mPosY );
        };
    void checkCollision(SDL_Rect a,Body b[])
    {
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;
    //Calculate the sides of fruit
    leftB = mCollider.x;
    rightB = mCollider.x + mCollider.w;
    topB = mCollider.y;
    bottomB = mCollider.y + mCollider.h;

    if( (rightA==rightB&&leftA==leftB&&topA==topB&&bottomA==bottomB) )
        {
            //Play the sound effect
            Mix_PlayChannel( -1, gEat, 0 );
            //reset the fruit position
            mPosX = (rand() % (SCREEN_WIDTH/FRUIT_WIDTH))*50;
            mPosY = (rand() % (SCREEN_HEIGHT/FRUIT_HEIGHT))*50;
            // check to make sure fruit dont spawn on top of others
            bool same = true;
            while(same)
            {
                same = false;
                if(mPosX==a.x&&mPosY==a.y)
                {
                    mPosX = (rand() % (SCREEN_WIDTH/FRUIT_WIDTH))*50;
                    mPosY = (rand() % (SCREEN_HEIGHT/FRUIT_HEIGHT))*50;
                    same = true;
                };
                for(int i=1; i<=game.score; i++) {
                    if(b[i].mPosX==mPosX&&b[i].mPosY==mPosY)
                    {
                        mPosX = (rand() % (SCREEN_WIDTH/FRUIT_WIDTH))*50;
                        mPosY = (rand() % (SCREEN_HEIGHT/FRUIT_HEIGHT))*50;
                        same = true;
                        break;
                    };
				}
            }
            mCollider.x = mPosX;
            mCollider.y = mPosY;
            game.score++;
        }
    }
};
