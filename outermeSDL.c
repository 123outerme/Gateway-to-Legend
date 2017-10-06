#include "outermeSDL.h"
#define IMG_INIT_FLAGS IMG_INIT_PNG

int initSDL(const char* tilesetFilePath)
{
    int done = 0;
    SDL_Init(SDL_INIT_VIDEO || SDL_INIT_EVENTS);
    if( !( IMG_Init(IMG_INIT_PNG) & IMG_INIT_FLAGS))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
         done = -1;
    }
    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        done = -2;
    }
    mainWindow = NULL;
    mainScreen = NULL;
    tilesetTexture = NULL;
    mainRenderer = NULL;
    mainFont = NULL;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            return 1;
    }
    else
    {
        mainWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderSetLogicalSize(mainRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
                SDL_RenderClear(mainRenderer);
                loadTTFont(FONT_FILE_NAME, &mainFont, 48);
                //loadTTFont(FONT_FILE_NAME, &smallFont, 20);
                if (!mainFont)
                {
                    printf("%s could not be created! SDL Error: %s\n", !mainFont ? "mainFont" : "Nothing", TTF_GetError());
                    return -3;
                }
                else
                {
                    loadIMG(tilesetFilePath, &tilesetTexture);
                    if (!tilesetTexture)
                    {
                        printf("Tileset could not load! SDL Error: %s\n", SDL_GetError());
                        return 6;
                    }
                    /*else
                    {
                        srand((unsigned int) time(NULL));
                        if (checkFile(CONFIG_FILE_NAME, SIZE_OF_SCANCODE_ARRAY))
                        {
                            loadConfig(CONFIG_FILE_NAME);
                        }
                        else
                            initConfig(CONFIG_FILE_NAME);
                    }*/
                }
            }
        }
    }
    return done;
}

bool loadIMG(char* imgPath, SDL_Texture** dest)
{
    SDL_Surface* surf = IMG_Load(imgPath);
    SDL_SetColorKey(surf, 1, SDL_MapRGB(surf->format, 255, 28, 198));
    *dest = SDL_CreateTextureFromSurface(mainRenderer, surf);
    if (!*dest)
    {
        printf("Unable to load image/create texture for %s! SDL_Error: %s\n", imgPath, SDL_GetError());
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
        printf( "Font could not be loaded! SDL Error: %s\n", SDL_GetError() );
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

void initSprite(sprite* spr, int x, int y, int size, int tileIndex, entityType type)
{
    spr->x = x;
	spr->y = y;
	spr->w = size;
	spr->h = size;
	spr->tileIndex = tileIndex;
	spr->clipRect = &((SDL_Rect){.x = (tileIndex / 8) * size, .y = (tileIndex % 8) * size, .w = size, .h = size});
	spr->type = type;
}

void drawTilemap(int startX, int startY, int endX, int endY, bool updateScreen)
{
    for(int dy = startY; dy < endY; dy++)
        for(int dx = startX; dx < endX; dx++)
            /*drawTile(tilemap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE)*/;
    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}

void drawTile(int id, int xCoord, int yCoord, int width, SDL_RendererFlip flip)
{
    //printf("%d , %d\n", id  / 8, (id % 8));
    SDL_RenderCopyEx(mainRenderer, tilesetTexture, &((SDL_Rect) {.x = (id / 8) * width, .y = (id % 8) * width, .w = width, .h = width}), &((SDL_Rect) {.x = xCoord, .y = yCoord, .w = width, .h = width}), 0, &((SDL_Point) {.x = width / 2, .y = width / 2}), flip);
    //SDL_RenderPresent(mainRenderer);
}

void drawText(char* input, int x, int y, int maxW, int maxH, SDL_Color color, bool render)
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
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
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
		printf("Error opening file!\n");
		return -1;
	}
	else
    {
        fclose(filePtr);
		return 0;
    }
}

bool checkFile(char* filePath, int desiredLines)
{
    FILE* filePtr = fopen(filePath,"r");
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
    return lines >= desiredLines;
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
        static char thisLine[255];
        fseek(filePtr, 0, SEEK_SET);
        for(int p = 0; p <= lineNum; p++)
            fgets(thisLine, 255, filePtr);
        //printf("%s @ %d\n", thisLine, thisLine);
        *output = thisLine;
        //printf("%s @ %d\n", output, output);
        fclose(filePtr);
        return thisLine;
	}
}

char* intToString(int value, char * result)
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

void freeThisMem(int ** x)
{
	free(*x);
	*x = NULL;
}
