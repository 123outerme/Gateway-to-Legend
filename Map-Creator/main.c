#include "outermeSDL.h"

#define checkSKUp keyStates[26]
#define checkSKDown keyStates[2]
#define checkSKLeft keyStates[4]
#define checkSKRight keyStates[7]
#define checkSKInteract keyStates[44]
#define checkSKMenu keyStates[41]
//SDL_SCANCODE_W
//SDL_SCANCODE_S
//SDL_SCANCODE_A
//SDL_SCANCODE_D
//SDL_SCANCODE_SPACE
//SDL_SCANCODE_ESCAPE

#define PIXELS_MOVED 48

typedef struct {
    sprite spr;  //?
    char name[8 + 1];  //9 bytes
    int level;  //
    int experience;  //
    int money;  //
    int HP;  //
    int maxHP;  //
    int worldNum;  //
    int mapScreen;  //8 bytes
    int lastScreen;  //8 bytes
    int overworldX;  //
    int overworldY;  //
    SDL_RendererFlip flip;  //
    bool movementLocked;  // 1 byte
} player;

#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)

char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);
void loadMapFile(char* filePath, int* tilemapData[], const int lineNum, const int y, const int x);
void mainLoop(player* playerSprite);
void initPlayer(player* player, int x, int y, int size, int tileIndex);
void writeTileData();

int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];

int main(int argc, char* argv[])
{
    char* mainFilePath = calloc(200 + 1, sizeof(char));
    char mapFilePath[200];
    char tileFilePath[200];
	printf("Enter a filepath: ");
	scanf("%s", mainFilePath);
	if (!checkFile(mainFilePath, 1))
	{
	    printf("Invalid file.\n");
        return 1;
	}
    uniqueReadLine(&mapFilePath, 200, mainFilePath, 1);
    uniqueReadLine(&tileFilePath, 200, mainFilePath, 2);
    printf("Load this line: ");
    int loadLine = 0;
    scanf("%d", &loadLine);
    if (!checkFile(mapFilePath, loadLine) || loadLine < 0)
    {
        printf("Invalid line number.");
        return 2;
    }
    loadMapFile(mapFilePath, tilemap, loadLine, HEIGHT_IN_TILES, WIDTH_IN_TILES);
    initSDL(tileFilePath);
    player creator;
    initPlayer(&creator, 0, 0, TILE_SIZE, 0);
    mainLoop(&creator);
    char saveCheck[2];
    printf("Save? (y/n) ");
	scanf("%s", saveCheck);
	if (saveCheck[0] == 'y')
        writeTileData();
    printf("Ended at %d, %d", creator.spr.x, creator.spr.y);
    //waitForKey();
    closeSDL();
    SDL_Delay(1000);
    return 0;
}
//C:/Stephen/C/CodeBlocks/SDLSeekers/Map-Creator/map-packs/main.txt

char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum)
{
    char* dummy = "";
    readLine(filePath, lineNum, &dummy);
    strcpy(output, dummy);
    dummy = removeChar(output, '\n', outputLength, false);
    strcpy(output, dummy);
    return *output;
}

void loadMapFile(char* filePath, int* tilemapData[], const int lineNum, const int y, const int x)
{
    int numsC = 0, numsR = 0,  i, num;
    int sameArray[y][x];
    char thisLine[601], substring[3];
    strcpy(thisLine, readLine(filePath, lineNum, thisLine));
    //printf("%s\n", thisLine);
    for(i = 0; i < 600; i += 2)
    {
        sprintf(substring, "%.*s", 2, thisLine + i);
        //*(array + numsR++ + numsC * x)
        num = (int)strtol(substring, NULL, 16);
        sameArray[numsC][numsR++] = num;
        //printf("nums[%d][%d] = %d = %d (%s)\n", numsC, numsR - 1, num, sameArray[numsC][numsR - 1], substring);
        if (numsR > x - 1)
        {
            numsC++;
            numsR = 0;
        }
        //printf("%d\n", num);
    }
    for(int dy = 0; dy < y; dy++)
    {
        for(int dx = 0; dx < x; dx++)
            *(tilemapData + dx + dy * x) = sameArray[dy][dx];
    }
}

void mainLoop(player* playerSprite)
{
    bool quit = false, editingTiles = true;
    SDL_Event e;
    SDL_Keycode keycode;
    SDL_RenderClear(mainRenderer);
    drawTilemap(0, 0, 20, 15, false);
    drawTile(playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, playerSprite->flip);
    SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
    SDL_RenderPresent(mainRenderer);
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        drawTilemap(0, 0, 20, 15, false);
        keycode = waitForKey();
        if (!playerSprite->movementLocked && (keycode == SDLK_w || keycode == SDLK_s || keycode == SDLK_a || keycode == SDLK_d))
        {
                if (playerSprite->spr.y > 0 && keycode == SDLK_w)
                    playerSprite->spr.y -= PIXELS_MOVED;
                if (playerSprite->spr.y < SCREEN_HEIGHT - playerSprite->spr.h && keycode == SDLK_s)
                    playerSprite->spr.y += PIXELS_MOVED;
                if (playerSprite->spr.x > 0 && keycode == SDLK_a)
                    playerSprite->spr.x -= PIXELS_MOVED;
                if (playerSprite->spr.x < SCREEN_WIDTH - playerSprite->spr.w && keycode == SDLK_d)
                    playerSprite->spr.x += PIXELS_MOVED;
        }
        if (keycode == SDLK_ESCAPE)
            quit = true;
        if (keycode == SDLK_q && playerSprite->spr.tileIndex > 0)
            playerSprite->spr.tileIndex--;
        if (keycode == SDLK_e && playerSprite->spr.tileIndex < 127)
            playerSprite->spr.tileIndex++;
        if (keycode == SDLK_SPACE && editingTiles)
            tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
        if (keycode == SDLK_SPACE && !editingTiles)
            ;
        drawTile(playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, playerSprite->flip);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
        SDL_RenderPresent(mainRenderer);
    }
}

void initPlayer(player* player, int x, int y, int size, int tileIndex)
{
    //inputName(player);  //custom text input routine to get player->name
    initSprite(&(player->spr), x, y, size, tileIndex, (entityType) type_player);
	player->level = 1;
	player->experience = 0;
	player->money = 0;
	player->HP = 50;
	player->maxHP = 50;
	player->worldNum = 1;
	player->mapScreen = 10;
	player->lastScreen = 10;
	player->overworldX = x;
	player->overworldY = y;
	player->flip = SDL_FLIP_NONE;
	player->movementLocked = false;
    SDL_Delay(300);
    //name, x, y, w, level, HP, maxHP, attack, speed, statPts, move1 - move4, steps, worldNum, mapScreen, lastScreen, overworldX, overworldY
}

void writeTileData()
{
    char* outputFile = "output/output.txt";
    createFile(outputFile);
    char input[2];
    char output[601];
    input[0] = 0;
    output[0] = 0;
    for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
    {
        for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
        {
            sprintf(input, "%.2X", tilemap[dy][dx]);
            //printf("<>%s\n", input);
            strcat(output, input);
        }
    }
    //printf(">%s\n", output);
    appendLine("output/output.txt", output);
    printf("outputted to output/output.txt\n");
}
