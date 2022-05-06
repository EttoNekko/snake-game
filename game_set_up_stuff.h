//Screen dimension constants
const int SCREEN_WIDTH = 50*15;
const int SCREEN_HEIGHT = 50*15;
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Globally used font
TTF_Font *gFont = NULL;
//The sound effects that will be used
Mix_Music *gMusic1 = NULL;
Mix_Music *gMusic2 = NULL;
Mix_Music *gMusic3 = NULL;
Mix_Music *gMusic4 = NULL;
Mix_Chunk *gEat = NULL;
Mix_Chunk *gDeath = NULL;

enum Mode { wall = 1, noWall = 2};

struct Game
{
    //game data
    int score;
    int speed;
    int speedTimer;
    int endAnimationTimer;
    //Main loop flag
    bool gameStart;
    bool gameOver;
    bool deathAnimation;
    bool gameEnd;
    Mode gameMode;
    bool gameChange;
    Game()
    {
        score = 0;
        speed = 180;
        speedTimer = 0;
        endAnimationTimer = 0;
        gameMode = wall;
        gameChange = true;
        gameOver = false;
        gameStart = false;
        deathAnimation = false;
        gameEnd = false;
    }
    void gameEvent(SDL_Event& e)
    {
        //If a key was pressed
        if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
        {
            switch( e.key.keysym.sym )
            {
                //Play the music
                case SDLK_1: Mix_PlayMusic( gMusic1, -1 ); break;
                case SDLK_2: Mix_PlayMusic( gMusic2, -1 ); break;
                case SDLK_3: Mix_PlayMusic( gMusic3, -1 ); break;
                case SDLK_9:
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
                //decide game mode;
                case SDLK_o: if(gameChange) {gameMode = wall; gameChange = false;}; break;
                case SDLK_p: if(gameChange) {gameMode = noWall; gameChange = false;}; break;
            }
        }
    }

    void speedCheck()
    {
        if(speed!=170)
        {
            speedTimer++;
        }
        if(speedTimer>=15)
        {
            speed = 170;
            speedTimer = 0;
        }
    }

    void endAnimtionCheck()
    {
        if(endAnimationTimer>=3950)
        {
            endAnimationTimer = 0;
        }
    }
};

Game game;

//Texture wrapper class
class LTexture
{
    //The actual hardware texture
    SDL_Texture* mTexture;
    //Image dimensions
    int mWidth;
    int mHeight;
    public:
    //Initializes variables
    LTexture()
    {
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
    };
    //Deallocates memory
    ~LTexture()
    {
    //Deallocate
    free();
    };
    //Loads image at specified path
    bool loadFromFile( std::string path )
    {
        //Get rid of preexisting texture
        free();
        //The final texture
        SDL_Texture* newTexture = NULL;
        //Load image at specified path
        SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
        if( loadedSurface == NULL )
        {
            printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
        }
        else
        {
            //Color key image
            SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
            //Create texture from surface pixels
            newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
            if( newTexture == NULL )
            {
                printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
            }
            else
            {
                //Get image dimensions
                mWidth = loadedSurface->w;
                mHeight = loadedSurface->h;
            }
            //Get rid of old loaded surface
            SDL_FreeSurface( loadedSurface );
        }
        //Return success
        mTexture = newTexture;
        return mTexture != NULL;
    };
    //Creates image from font string
    bool loadFromRenderedText( string textureText, SDL_Color textColor = {0, 0, 0} )
    {
        //Get rid of preexisting texture
        free();
        //Render text surface
        SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
        if( textSurface == NULL )
        {
            printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
        }
        else
        {
            //Create texture from surface pixels
            mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
            if( mTexture == NULL )
            {
                printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
            }
            else
            {
                //Get image dimensions
                mWidth = textSurface->w;
                mHeight = textSurface->h;
            }
            //Get rid of old surface
            SDL_FreeSurface( textSurface );
        }
        //Return success
        return mTexture != NULL;
    };
    //Deallocates texture
    void free()
    {
            //Free texture if it exists
            if( mTexture != NULL )
            {
                SDL_DestroyTexture( mTexture );
                mTexture = NULL;
                mWidth = 0;
                mHeight = 0;
            }
    };
    //Renders texture at given point
    void render( int x, int y, SDL_Rect* clip = NULL, SDL_Rect* picSize = NULL, double angle = 0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        //Set rendering space and render to screen
        SDL_Rect renderQuad = { x, y, mWidth, mHeight };
        //Set clip rendering dimensions
        if( clip != NULL )
        {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        };
        if( picSize != NULL )
        {
            renderQuad.w = picSize->w;
            renderQuad.h = picSize->h;
        };
        //Render to screen
        SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
    };
    int getWidth() {return mWidth;}
    int getHeight() {return mHeight;}
};

//Scene sprites
LTexture gHeadSprite;
LTexture gFruit;
LTexture gLightningFruit;
LTexture gBody;
LTexture gScoreText;
LTexture gGameOverScoreText;
LTexture gGameOverSprite1;
LTexture gGameOverSprite2;
LTexture gGameOverSprite3;
LTexture gBackgroundSprite;
LTexture gStartScreenSprite;
//head animation
const int WALKING_ANIMATION_FRAMES = 3;
SDL_Rect gHeadSpriteClips[ WALKING_ANIMATION_FRAMES ];
SDL_Rect* currentHeadClip = NULL;
//resize value to fit screen
SDL_Rect sizeResize = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
SDL_Rect* textureResize = &sizeResize;
//start screen animation
const int STARTSCREEN_ANIMATION_FRAMES = 4;
SDL_Rect gStartScreenSpriteClips[ STARTSCREEN_ANIMATION_FRAMES ];
SDL_Rect* currentStartScreenClip = NULL;
//background animation
const int BACKGROUND_ANIMATION_FRAMES = 6;
SDL_Rect gBackgroundSpriteClips[ BACKGROUND_ANIMATION_FRAMES ];
SDL_Rect* currentBackgroundClip = NULL;
//game over animation
const int GAMEOVER_ANIMATION_FRAMES1 = 3;
const int GAMEOVER_ANIMATION_FRAMES2 = 12;
//shared gameover clip
SDL_Rect gGameOverSpriteClips[ GAMEOVER_ANIMATION_FRAMES2 ];
SDL_Rect* currentGameOverClip = NULL;

bool init()
{
	//Initialization flag
	bool success = true;
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 159, 159, 159, 255 );
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				//Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
				//Initialize SDL_mixer
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
				Mix_VolumeMusic(70);
			}
		}
	}
	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;
	//Load music
	gMusic1 = Mix_LoadMUS( "nyan_cat_song.MP3" );
	if( gMusic1 == NULL )
	{
		printf( "Failed to load nyan music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gMusic2 = Mix_LoadMUS( "hq_boss_theme.MP3" );
	if( gMusic2 == NULL )
	{
		printf( "Failed to load hq boss music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gMusic3 = Mix_LoadMUS( "woman_DojaCat.MP3" );
	if( gMusic3 == NULL )
	{
		printf( "Failed to load cat music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gMusic4 = Mix_LoadMUS( "chamhetroi.MP3" );
	if( gMusic4 == NULL )
	{
		printf( "Failed to load cham het music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	//Load sound effects
	gEat = Mix_LoadWAV( "yoshi_tongue.wav" );
	if( gEat == NULL )
	{
		printf( "Failed to load eat sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gDeath = Mix_LoadWAV("slow_oof.wav");
	if( gDeath == NULL )
	{
		printf( "Failed to load death sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	//Open the font
	gFont = TTF_OpenFont( "CHERI.ttf", 26 );
	if( gFont == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Render text
		if( !gScoreText.loadFromRenderedText( "Score: "+to_string(game.score) ) )
		{
			printf( "Failed to load text texture!\n" );
			success = false;
		}
		//Render game over text
		if( !gGameOverScoreText.loadFromRenderedText( "Your score is: "+ to_string(game.score), {255, 255, 51} ) )
		{
			printf( "Failed to load  gameOver text texture!\n" );
			success = false;
		}
	}
	//Load Start screen texture
	if( !gStartScreenSprite.loadFromFile( "nyan_start_screen.png" ) )
	{
		printf( "Failed to load start screen texture!\n" );
		success = false;
	}
	//Load background texture
	if( !gBackgroundSprite.loadFromFile( "background_sprite.png" ) )
	{
		printf( "Failed to load background texture!\n" );
		success = false;
	}
	//Load game over texture
	if( !gGameOverSprite1.loadFromFile( "nyan_gameOver_screen1.png" ) )
	{
		printf( "Failed to load game over 1 texture!\n" );
		success = false;
	}
	if( !gGameOverSprite2.loadFromFile( "nyan_gameOver_screen2.png" ) )
	{
		printf( "Failed to load game over 2 texture!\n" );
		success = false;
	}
	if( !gGameOverSprite3.loadFromFile( "nyan_gameOver_screen3.png" ) )
	{
		printf( "Failed to load game over 3 texture!\n" );
		success = false;
	}
	//Load sprite sheet texture
	if( !gHeadSprite.loadFromFile( "head.png" ) )
	{
		printf( "Failed to load head texture!\n" );
		success = false;
	}
    if( !gFruit.loadFromFile( "pickelFruit.png" ) )
	{
		printf( "Failed to load fruit texture!\n" );
		success = false;
	}
	if( !gLightningFruit.loadFromFile( "lightningFruit.png" ) )
	{
		printf( "Failed to load lightning fruit texture!\n" );
		success = false;
	}
	if( !gBody.loadFromFile( "body.png" ) )
	{
		printf( "Failed to load body texture!\n" );
		success = false;
	}
	else
	{
	    //Set head sprite clips
		gHeadSpriteClips[ 0 ] = {0, 0, 50 , 50};
		gHeadSpriteClips[ 1] = {50, 0, 50 , 50};
		gHeadSpriteClips[ 2 ] = {100, 0, 50 , 50};
		//Set start screen sprite clips
		gStartScreenSpriteClips[0] = {0 , 0, 550, 550};
		gStartScreenSpriteClips[1] = {550 , 0, 550, 550};
		gStartScreenSpriteClips[2] = {550*2 , 0, 550, 550};
		gStartScreenSpriteClips[3] = {550*3 , 0, 550, 550};
        //set background sprite clip
        gBackgroundSpriteClips[0] = {0, 0, 100, 100};
        gBackgroundSpriteClips[1] = {100, 0, 100, 100};
        gBackgroundSpriteClips[2] = {200, 0, 100, 100};
        gBackgroundSpriteClips[3] = {300, 0, 100, 100};
        gBackgroundSpriteClips[4] = {400, 0, 100, 100};
        gBackgroundSpriteClips[5] = {500, 0, 100, 100};
        //set gameover sprite clip 2
        gGameOverSpriteClips[0] = {0, 0, 550, 550};
        gGameOverSpriteClips[1] = {550*1, 0, 550, 550};
        gGameOverSpriteClips[2] = {550*2, 0, 550, 550};
        gGameOverSpriteClips[3] = {550*3, 0, 550, 550};
        gGameOverSpriteClips[4] = {550*4, 0, 550, 550};
        gGameOverSpriteClips[5] = {550*5, 0, 550, 550};
        gGameOverSpriteClips[6] = {550*6, 0, 550, 550};
        gGameOverSpriteClips[7] = {550*7, 0, 550, 550};
        gGameOverSpriteClips[8] = {550*8, 0, 550, 550};
        gGameOverSpriteClips[9] = {550*9, 0, 550, 550};
        gGameOverSpriteClips[10] = {550*10, 0, 550, 550};
        gGameOverSpriteClips[11] = {550*11, 0, 550, 550};
	}
	return success;
}

void close()
{
    //Free the sound effects
	Mix_FreeChunk( gEat );
	gEat = NULL;
	Mix_FreeChunk( gDeath );
	//Free the music
	Mix_FreeMusic( gMusic1 );
	Mix_FreeMusic( gMusic2 );
	Mix_FreeMusic( gMusic3 );
	Mix_FreeMusic( gMusic4 );
	gMusic1 = NULL; gMusic2 = NULL; gMusic3 = NULL; gMusic4 = NULL;
	//Free loaded image
	gHeadSprite.free();
	gFruit.free();
	gBody.free();
	gScoreText.free();
	gGameOverScoreText.free();
	gGameOverSprite1.free();
	gGameOverSprite2.free();
	gGameOverSprite3.free();
	gBackgroundSprite.free();
	gStartScreenSprite.free();
	currentHeadClip = NULL;
	currentBackgroundClip = NULL;
	currentStartScreenClip = NULL;
	currentGameOverClip = NULL;
	//Free global font
	TTF_CloseFont( gFont );
	gFont = NULL;
	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	Mix_Quit();
	TTF_Quit();
}
