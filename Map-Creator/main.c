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
void loadMapFile(char* filePath, int* tilemapData[], int* eventmapData[], const int lineNum, const int y, const int x);
void mainLoop(player* playerSprite);
void drawEventmap(int startX, int startY, int endX, int endY, bool updateScreen);
void initPlayer(player* player, int x, int y, int size, int tileIndex);
void writeTileData();

int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* eventTexture;

int main(int argc, char* argv[])
{
    for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
        for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
            eventmap[dy][dx] = 0;
    char* mainFilePath = calloc(200 + 1, sizeof(char));
    char mapFilePath[200];
    char tileFilePath[200];
    char loadCheck[2];
    printf("Load? (y/n) ");
	scanf("%s", loadCheck);
	if (loadCheck[0] == 'y')
    {
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
        loadMapFile(mapFilePath, tilemap, eventmap, loadLine, HEIGHT_IN_TILES, WIDTH_IN_TILES);
    }
    else
    {
        strcpy(mainFilePath, "map-packs/main.txt");
        uniqueReadLine(&mapFilePath, 200, mainFilePath, 1);
        uniqueReadLine(&tileFilePath, 200, mainFilePath, 2);
        for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
        {
            for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
            {
                tilemap[dy][dx] = 0;
                eventmap[dy][dx] = 0;
            }
        }
    }
    initSDL(tileFilePath);
    loadIMG("tileset/eventTile48.png", &eventTexture);
    player creator;
    initPlayer(&creator, 0, 0, TILE_SIZE, 0);
    SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    mainLoop(&creator);
    char saveCheck[2];
    printf("Save? (y/n) ");
	scanf("%s", saveCheck);
	if (saveCheck[0] == 'y')
        writeTileData();
    //waitForKey();
    SDL_DestroyTexture(eventTexture);
    closeSDL();
    //SDL_Delay(1000);
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

void loadMapFile(char* filePath, int* tilemapData[], int* eventmapData[], const int lineNum, const int y, const int x)
{
    int numsC = 0, numsR = 0,  i, num;
    int sameArray[y][x], eventArray[y][x];
    bool writeToTilemap = false;
    char thisLine[1201], substring[2];
    strcpy(thisLine, readLine(filePath, lineNum, thisLine));
    printf("%s\n", thisLine);
    for(i = 0; i < 1200; i += 2)
    {
        sprintf(substring, "%.*s", 2, thisLine + i);
        //*(array + numsR++ + numsC * x)
        num = (int)strtol(substring, NULL, 16);
        if (writeToTilemap)
            sameArray[numsC][numsR++] = num;
        else
            eventArray[numsC][numsR] = num;
        printf(writeToTilemap ? "i = %d @ nums[%d][%d] = (%s)\n" : "i = %d @ eventArray[%d][%d] = (%s)\n", i, numsC, numsR - writeToTilemap, substring);
        writeToTilemap = !writeToTilemap;
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
        {
            *(tilemapData + dx + dy * x) = sameArray[dy][dx];
            *(eventmapData + dx + dy * x) = eventArray[dy][dx];
        }
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
        if (!editingTiles)
        {
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0x58);
            SDL_RenderFillRect(mainRenderer, NULL);
            drawEventmap(0, 0, 20, 15, false);
            SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            drawEventTile(playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, playerSprite->flip);
        }
        else
            drawTile(playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, playerSprite->flip);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
        SDL_RenderPresent(mainRenderer);
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
            eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
        if (keycode == SDLK_LSHIFT)
        {
            editingTiles = !editingTiles;
            if (!editingTiles)
                playerSprite->spr.tileIndex = 1;
            else
                playerSprite->spr.tileIndex = 0;
        }
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

void drawEventmap(int startX, int startY, int endX, int endY, bool updateScreen)
{
    for(int dy = startY; dy < endY; dy++)
        for(int dx = startX; dx < endX; dx++)
            drawEventTile(eventmap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}

void drawEventTile(int id, int xCoord, int yCoord, int width, SDL_RendererFlip flip)
{
    //printf("%d , %d\n", id  / 8, (id % 8));
    SDL_RenderCopyEx(mainRenderer, eventTexture, &((SDL_Rect) {.x = (id / 8) * width, .y = (id % 8) * width, .w = width, .h = width}), &((SDL_Rect) {.x = xCoord, .y = yCoord, .w = width, .h = width}), 0, &((SDL_Point) {.x = width / 2, .y = width / 2}), flip);
    //SDL_RenderPresent(mainRenderer);
}

void writeTileData()
{
    char* outputFile = "output/output.txt";
    createFile(outputFile);
    char input[2];
    char output[1201];
    input[0] = 0;
    output[0] = 0;
    for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
    {
        for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
        {
            sprintf(input, "%.2X", eventmap[dy][dx]);
            //printf("<>%s\n", input);
            strcat(output, input);
            sprintf(input, "%.2X", tilemap[dy][dx]);
            strcat(output, input);
        }
    }
    //printf(">%s\n", output);
    appendLine("output/output.txt", output);
    printf("%s", output);
    printf("outputted to output/output.txt\n");
}
