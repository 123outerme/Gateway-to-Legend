#include "outermeSDL.h"
#define IMG_INIT_FLAGS IMG_INIT_PNG
// ++ outermeSDL version 1.2 - last update 1/1/2018 ++


int initSDL(char* windowName, char* tilesetFilePath, char* fontFilePath, int windowWidth, int windowHeight, int fontSize)
{
    int done = 0;
    mainWindow = NULL;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    else
    {
        if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_FLAGS))
        {
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
            return -1;
        }
        //Initialize SDL_ttf
        if(TTF_Init() == -1)
        {
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            return -2;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        {
            printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError());
            return -3;
        }
        else
        //Mix_Init(MIX_INIT_OGG);  //deprecated?
        soundVolume = MIX_MAX_VOLUME;
        Mix_AllocateChannels(32);
        Mix_Volume(-1, soundVolume);  //sets all channels to the sound level soundVolume
        musicVolume = MIX_MAX_VOLUME;
        Mix_VolumeMusic(musicVolume);
        mainScreen = NULL;
        tilesetTexture = NULL;
        mainRenderer = NULL;
        mainFont = NULL;
        canDrawText = true;
        canDrawTiles = true;
        mainWindow = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (!mainWindow)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return 2;
        }
        else
        {
            mainScreen = SDL_GetWindowSurface(mainWindow);
            mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
            if(!mainRenderer)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                return 3;
            }
            else
            {
                SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_NONE);
                SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderSetLogicalSize(mainRenderer, windowWidth, windowHeight);
                SDL_RenderClear(mainRenderer);
                loadTTFont(fontFilePath, &mainFont, fontSize);
                //loadTTFont(FONT_FILE_NAME, &smallFont, 20);
                if (!mainFont)
                {
                    printf("%s could not be created! SDL Error: %s\n", !mainFont ? "mainFont" : "Nothing", TTF_GetError());
                    canDrawText = false;
                    done = -4;
                }
                loadIMG(tilesetFilePath, &tilesetTexture);
                if (!tilesetTexture)
                {
                    printf("Tileset could not load! SDL Error: %s\n", SDL_GetError());
                    canDrawTiles = false;
                    done = 6;
                }
                else
                {
                    srand((unsigned int) time(NULL));
                    /*if (checkFile(CONFIG_FILE_NAME, SIZE_OF_SCANCODE_ARRAY))
                    {
                        loadConfig(CONFIG_FILE_NAME);
                    }
                    else
                        initConfig(CONFIG_FILE_NAME);*/
                }
            }
        }
    }
    return done;
}

bool loadIMG(char* imgPath, SDL_Texture** dest)
{
    SDL_Surface* surf = IMG_Load(imgPath);
    if (!surf)
    {
	printf("Unable to load image for %s! SDL_Error: %s\n", imgPath, SDL_GetError());
        return false;
    }
    SDL_SetColorKey(surf, 1, SDL_MapRGB(surf->format, 255, 28, 198));
    *dest = SDL_CreateTextureFromSurface(mainRenderer, surf);
    if (!(*dest))
    {
        printf("Unable to create texture for %s! SDL_Error: %s\n", imgPath, SDL_GetError());
        return false;
    }
    SDL_FreeSurface(surf);
    return true;
}

bool loadTTFont(char* filePath, TTF_Font** dest, int sizeInPts)
{
    *dest = TTF_OpenFont(filePath, sizeInPts);
    if (!*dest)
    {
        printf("Font could not be loaded! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

int* loadTextTexture(char* text, SDL_Texture** dest, int maxW, SDL_Color color, int isBlended)
{
    static int wh[] = {0, 0};
    SDL_Surface* txtSurface = NULL;
    if (isBlended)
        txtSurface = TTF_RenderText_Blended_Wrapped(mainFont, text, color, maxW);
//    else
//        txtSurface = TTF_RenderText(smallFont, text, color, ((SDL_Color) {181, 182, 173}));
    *dest = SDL_CreateTextureFromSurface(mainRenderer, txtSurface);
    if (!*dest)
    {
        printf("Text texture could not be loaded! SDL Error: %s\n", SDL_GetError());
    }
    else
    {
        wh[0] = txtSurface->w;
        wh[1] = txtSurface->h;
    }
    SDL_FreeSurface(txtSurface);
    return wh;
}

void initSprite(sprite* spr, int x, int y, int size, int tileIndex, int angle, SDL_RendererFlip flip, entityType type)
{
    spr->x = x;
	spr->y = y;
	spr->w = size;
	spr->h = size;
	spr->tileIndex = tileIndex;
	spr->angle = angle;
	spr->flip = flip;
	spr->clipRect = &((SDL_Rect){.x = (tileIndex / 8) * size, .y = (tileIndex % 8) * size, .w = size, .h = size});
	spr->type = type;
}

void drawTilemap(int startX, int startY, int endX, int endY, bool updateScreen)
{
    for(int dy = startY; dy < endY; dy++)
        for(int dx = startX; dx < endX; dx++)
            drawTile(tilemap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}

void drawTile(int id, int xCoord, int yCoord, int width, int angle, SDL_RendererFlip flip)
{
    //printf("%d , %d\n", id  / 8, (id % 8));
    if (canDrawTiles)
        SDL_RenderCopyEx(mainRenderer, tilesetTexture, &((SDL_Rect) {.x = (id / 8) * width, .y = (id % 8) * width, .w = width, .h = width}), &((SDL_Rect) {.x = xCoord, .y = yCoord, .w = width, .h = width}), angle, &((SDL_Point) {.x = width / 2, .y = width / 2}), flip);
    //SDL_RenderPresent(mainRenderer);
}

void drawATilemap(SDL_Texture* texture, int map[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, int hideTileNumOf, bool updateScreen)
{
    for(int dy = startY; dy < endY; dy++)
        for(int dx = startX; dx < endX; dx++)
            drawATile(texture, map[dy][dx] == hideTileNumOf ? 0 : map[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}

void drawATile(SDL_Texture* texture, int id, int xCoord, int yCoord, int width, int height, int angle, SDL_RendererFlip flip)
{
    SDL_RenderCopyEx(mainRenderer, texture, &((SDL_Rect) {.x = (id / 8) * TILE_SIZE, .y = (id % 8) * TILE_SIZE, .w = width, .h = height}),
                     &((SDL_Rect) {.x = xCoord, .y = yCoord, .w = width, .h = height}), angle,
                     &((SDL_Point) {.x = width / 2, .y = height / 2}), flip);
}

void drawText(char* input, int x, int y, int maxW, int maxH, SDL_Color color, bool render)
{
    if (canDrawText)
    {
        SDL_Texture* txtTexture = NULL;
        int* wh;
        wh = loadTextTexture(input, &txtTexture, maxW, color, true);
        SDL_RenderCopy(mainRenderer, txtTexture, &((SDL_Rect){.w = *wh > maxW ? maxW : *wh, .h = *(wh + 1) > maxH ? maxH : *(wh + 1)}),
                                                 &((SDL_Rect){.x =  x, .y = y, .w = *wh > maxW ? maxW : *wh, .h = *(wh + 1) > maxH ? maxH : *(wh + 1)}));
        if (render)
            SDL_RenderPresent(mainRenderer);
        SDL_DestroyTexture(txtTexture);
    }
}

SDL_Keycode waitForKey()
{
    SDL_Event e;
    bool quit = false;
    SDL_Keycode keycode = SDLK_ESCAPE;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
                quit = true;
            else
                if(e.type == SDL_KEYDOWN)
                {
                    keycode = e.key.keysym.sym;
                    quit = true;
                }
        }
    }
    return keycode;
}

void closeSDL()
{
    TTF_CloseFont(mainFont);
    //TTF_CloseFont(smallFont);
	SDL_DestroyTexture(tilesetTexture);
	SDL_FreeSurface(mainScreen);
    SDL_DestroyWindow(mainWindow);
    SDL_DestroyRenderer(mainRenderer);
    for(int i = 0; i < MAX_SOUNDS; i++)
        Mix_FreeChunk(audioArray[i]);

    for(int i = 0; i < MAX_MUSIC; i++)
        Mix_FreeMusic(musicArray[i]);

    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    SDL_Quit();
}

int checkArrayForIVal(int value, int array[], size_t arraySize)
{
    int found = -1;
    for(int i = 0; i < arraySize; i++)
    {
        if (value == array[i])
        {
            found = i;
            break;
        }
    }
    return found;
}

char* removeChar(char input[], char removing, size_t length, bool foreToBack)
{
    static char sansChar[255];
    int i;
    length = strlen(input);
    if (foreToBack)
    {
        for(i = 0; i < length; i++)
        {
            //printf("%c at %d\n", input[i], i);
            if (input[i] != removing)
                break;
        }
        if (i == 0)
            return input;
        int y = 0;
        for(int x = i; x < length; x++)
            sansChar[y++] = input[x];
        sansChar[length] = '\0';
    }
    else
    {
        for(i = length - 1; i >= 0; i--)
        {
            if (input[i] != removing)
                break;
            //printf("%c\n", input[i]);
        }
        for(int x = 0; x < i + 1; x++)
            sansChar[x] = input[x];
        sansChar[i + 1 < length ? i + 1 : length] = '\0';
    }
    //printf("%s at %d\n", sansChar, sansChar);'
    return sansChar;
}

int createFile(char* filePath)
{
	FILE* filePtr;
	filePtr = fopen(filePath,"w");
	if (!filePtr)
	{
		printf("Error opening/creating file!\n");
		return -1;
	}
	else
    {
        fclose(filePtr);
		return 0;
    }
}

/** Checks if a file exists and if it has certain number of lines.
*
* Checks if a file exists and/or if it has the desired number of lines. desiredLines < 0 -> get number of lines instead.
*/
bool checkFile(char* filePath, int desiredLines)
{
    FILE* filePtr = fopen(filePath, "r");
	if (!filePtr)
		return false;
    char ch;
    int lines = 0;
    while(!feof(filePtr))
    {
      ch = fgetc(filePtr);
      if(ch == '\n')
      {
        lines++;
      }
    }
    fclose(filePtr);
    return desiredLines >= 0 ? lines >= desiredLines : lines;
}

int appendLine(char* filePath, char* stuff)
{
	FILE* filePtr;
	filePtr = fopen(filePath,"a");
	if (!filePtr)
	{
		printf("Error opening file!\n");
		return -1;
	}
	else
	{
		fprintf(filePtr, "%s\n", stuff);
		fclose(filePtr);
		return 0;
	}
}

char* readLine(char* filePath, int lineNum, char** output)
{
	FILE* filePtr = fopen(filePath,"r");
	if (!filePtr)
		return NULL;
	else
	{
        char* thisLine = calloc(2048, sizeof(char));
        fseek(filePtr, 0, SEEK_SET);
        for(int p = 0; p <= lineNum; p++)
            fgets(thisLine, 2048, filePtr);
        //printf("%s @ %d\n", thisLine, thisLine);
        *output = thisLine;
        //printf("%s @ %d\n", output, output);
        fclose(filePtr);
        return *output;
	}
}

char* intToString(int value, char* result)
{
    if (value == 0)
        return "0";
    bool negFlag = false;
    if (value < 0)
    {
        negFlag = true;
        value *= -1;
        //printf("new value = %d\n", value);
    }
	int digit = digits(value);
	//printf("digit = %d\n", digit);
	result = calloc(digit + 1 + negFlag, sizeof(char));
	result[digit + negFlag] = '\0';
	int usedVal = 0;
	for (int i = digit; i > 0; i--)
	{
		int x = (value - usedVal) / pwrOf10(i - 1);
		result[digit - i] = (char) x + '0';
		//printf("result[%d] = (%d) / %d = %d = character %c\n", digit - i, value - usedVal, pwrOf10(i - 1), x, result[digit - i]);
		usedVal = usedVal + (result[digit - i] - '0') * pwrOf10(i - 1);
		//printf("usedVal = itself + %d * %d = %d\n", (int) result[digit - i] - '0', pwrOf10(i - 1), usedVal);
	}
	if (negFlag)
    {
        char negative[1];
        strcpy(negative, "-");
        strcat(negative, result);
        strcpy(result, negative);
    }
	//printf("%s\n",result);
	return result;
}
int digits(int num)
{
    if (num < 0)
        num *= -1;
	return 1 + log10(num);
}
int pwrOf10(int power)
{
	int val = 1;
	int i = power;
	while (i > 0)
	{
		val *= 10;
		i--;
	}
	return val;
}

void* freeThisMem(void* x)
{
	free(x);
	return NULL;
}
