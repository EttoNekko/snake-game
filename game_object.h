class Head
{
    private:
    //The X and Y offsets of the head
    int mPosX, mPosY;
    //The velocity of the head
    int mVelX, mVelY;
    //The dimensions of the head
    const int HEAD_WIDTH = 50;
    const int HEAD_HEIGHT = 50;
    double degrees;
    SDL_RendererFlip headFlip;
    //velocity of the head
    const int HEAD_VEL = 50;
    public:
    Head()
    {
    //Initialize the offsets
    mPosX = (SCREEN_WIDTH - HEAD_WIDTH)/2 ;
    mPosY = (SCREEN_HEIGHT - HEAD_HEIGHT)/2 ;
    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
    degrees = 0;
    headFlip = SDL_FLIP_NONE;
    };
    //Takes key presses and change the head direction
    void headEvent( SDL_Event& e )
    {
        //If a key was pressed
        if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
        {
            switch( e.key.keysym.sym )
            {
                //moving for head
                case SDLK_UP: mVelY = -HEAD_VEL; mVelX = 0; degrees = 270; headFlip = SDL_FLIP_NONE; break;
                case SDLK_DOWN: mVelY = HEAD_VEL; mVelX = 0; degrees = 90; headFlip = SDL_FLIP_VERTICAL; break;
                case SDLK_LEFT: mVelX = -HEAD_VEL; mVelY = 0; degrees = 0; headFlip = SDL_FLIP_HORIZONTAL; break;
                case SDLK_RIGHT: mVelX = HEAD_VEL; mVelY = 0; degrees = 0; headFlip = SDL_FLIP_NONE; break;
            }
        }
    };
    //Moves the head
    void moveHead()
    {
        //Move the head left or right
        mPosX += mVelX;
        //Move the head up or down
        mPosY += mVelY;
        if(game.gameMode == wall)
        {
            //If the head went too far to the left or right
            if( (mPosX < 0) || (mPosX + HEAD_WIDTH > SCREEN_WIDTH) )
            {
                game.gameOver = true;
            }
            //If the head went too far up or down
            if( (mPosY < 0) || (mPosY + HEAD_HEIGHT > SCREEN_HEIGHT) )
            {
                game.gameOver = true;
            }
        }
        if(game.gameMode == noWall)
        {
            if(mPosX < 0)
            {
                mPosX = SCREEN_WIDTH-HEAD_WIDTH;
            };
            if(mPosX + HEAD_WIDTH > SCREEN_WIDTH)
            {
                mPosX = 0;
            };
            if(mPosY < 0)
            {
                mPosY = SCREEN_HEIGHT - HEAD_WIDTH;
            };
            if(mPosY + HEAD_HEIGHT > SCREEN_HEIGHT)
            {
                mPosY = 0;
            };
        }
    };
    void reposition()
    {
        if(mPosX < 0)
        {
            mPosX+=HEAD_VEL;
        };
        if(mPosX + HEAD_WIDTH > SCREEN_WIDTH)
        {
            mPosX-=HEAD_VEL;
        };
        if(mPosY < 0)
        {
            mPosY+=HEAD_VEL;
        };
        if(mPosY + HEAD_HEIGHT > SCREEN_HEIGHT)
        {
            mPosY-=HEAD_VEL;
        };
    }
    void minusY ()
    {
        mPosY-=HEAD_VEL;
    }
    void plusY ()
    {
        mPosY+=HEAD_VEL;
    }
    int getX() {return mPosX;};
    int getY() {return mPosY;};
    void render()
    {
    //Show the head
    gHeadSprite.render( mPosX, mPosY, currentHeadClip, NULL, degrees, NULL, headFlip );
    };
};

struct Body
{
    //The X and Y offsets of the body;
    int mPosX, mPosY;
    //The dimensions of the body
    static const int BODY_WIDTH = 50;
    static const int BODY_HEIGHT = 50;
    void render()
		{
        //Show the body
        gBody.render( mPosX, mPosY );
        };
    void checkCollision(Head a)
    {
        if( (a.getX() == mPosX && a.getY() == mPosY) )
        {
            game.gameOver = true;
        }
    }
};

class Fruit
{
    //The X and Y offsets of the fruit;
    int mPosX, mPosY;
    //The dimensions of the dot
    static const int FRUIT_WIDTH = 50;
    static const int FRUIT_HEIGHT = 50;
    public:
    Fruit()
    {
        mPosX = (rand() % (SCREEN_WIDTH/FRUIT_WIDTH))*50;
        mPosY = (rand() % (SCREEN_HEIGHT/FRUIT_HEIGHT))*50;
    }
    void renderNormal()
    {
        //Show the fruit
        gFruit.render( mPosX, mPosY );
    };
    void renderLightning()
    {
        //show the lightning fruit
        gLightningFruit.render( mPosX, mPosY );
    }
    bool checkCollision(Head a,Body b[], Fruit* c = NULL)
    {
    if( (a.getX() == mPosX && a.getY() == mPosY) )
        {
            //reset the fruit position
            mPosX = (rand() % (SCREEN_WIDTH/FRUIT_WIDTH))*50;
            mPosY = (rand() % (SCREEN_HEIGHT/FRUIT_HEIGHT))*50;
            // check to make sure fruit dont spawn on top of others
            bool same = true;
            while(same)
            {
                same = false;
                if(mPosX == c->getX() && mPosY == c->getY())
                {
                    mPosX = (rand() % (SCREEN_WIDTH/FRUIT_WIDTH))*50;
                    mPosY = (rand() % (SCREEN_HEIGHT/FRUIT_HEIGHT))*50;
                    same = true;
                }
                for(int i=0; i<=game.score; i++) {
                    if(b[i].mPosX==mPosX&&b[i].mPosY==mPosY)
                    {
                        mPosX = (rand() % (SCREEN_WIDTH/FRUIT_WIDTH))*50;
                        mPosY = (rand() % (SCREEN_HEIGHT/FRUIT_HEIGHT))*50;
                        same = true;
                        break;
                    };
				}
            }
            game.score++;
            return true;
        }
    }
    int getX() {return mPosX;}
    int getY() {return mPosY;}
};
