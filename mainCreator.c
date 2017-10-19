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

#define WINDOW_NAME "Gateway to Legend Map Creator"

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
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);
void mainLoop(player* playerSprite);
SDL_Keycode getKey();
void drawEventmap(int startX, int startY, int endX, int endY, bool drawHiddenTiles, bool updateScreen);
void drawEventTile(int id, int xCoord, int yCoord, int width, SDL_RendererFlip flip);
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
        printf("Enter the map-pack filepath: ");
        scanf("%s", mainFilePath);
        if (!checkFile(mainFilePath, 0))
        {
            printf("Invalid file.\n");
            return 1;
        }
        uniqueReadLine((char**) &mapFilePath, 200, mainFilePath, 1);
        uniqueReadLine((char**) &tileFilePath, 200, mainFilePath, 2);
        printf("Load this line: ");
        int loadLine = 0;
        scanf("%d", &loadLine);
        if (!checkFile(mapFilePath, loadLine) || loadLine < 0)
        {
            printf("Invalid line number.\n");
            return 2;
        }
        loadMapFile(mapFilePath, tilemap, eventmap, loadLine, HEIGHT_IN_TILES, WIDTH_IN_TILES);
    }
    else
    {
        strcpy(mainFilePath, "map-packs/a.txt");
        uniqueReadLine((char**) &mapFilePath, 200, mainFilePath, 1);
        uniqueReadLine((char**) &tileFilePath, 200, mainFilePath, 2);
        for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
        {
            for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
            {
                tilemap[dy][dx] = 0;
                eventmap[dy][dx] = 0;
            }
        }
    }
    initSDL(WINDOW_NAME, tileFilePath, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
    loadIMG("tileset/eventTile48.png", &eventTexture);
    player creator;
    initPlayer(&creator, 0, 0, TILE_SIZE, 0);
    SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    mainLoop(&creator);
    closeSDL();
    char saveCheck[2];
    printf("Save? (y/n) ");
	scanf("%s", saveCheck);
	if (saveCheck[0] == 'y')
        writeTileData();
    //waitForKey();
    SDL_DestroyTexture(eventTexture);
    //SDL_Delay(1000);
    return 0;
}
//C:/Stephen/C/CodeBlocks/Gateway-to-Legend/Map-Creator/map-packs/a.txt

char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum)
{
    char* dummy = "";
    readLine(filePath, lineNum, &dummy);
    strcpy((char*) output, dummy);
    dummy = removeChar((char*) output, '\n', outputLength, false);
    strcpy((char*) output, dummy);
    return *output;
}

void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x)
{
    int numsC = 0, numsR = 0,  i, num;
    bool writeToTilemap = false;
    char thisLine[1200], substring[2];
    strcpy(thisLine, readLine(filePath, lineNum, (char**) &thisLine));
    //printf("%s\n", thisLine);
    for(i = 0; i < 1200; i += 2)
    {
        sprintf(substring, "%.2s", thisLine + i);
        //*(array + numsR++ + numsC * x)
        num = (int)strtol(substring, NULL, 16);
        if (writeToTilemap)
            tilemapData[numsC][numsR++] = num;
        else
            eventmapData[numsC][numsR] = num;
        //printf(writeToTilemap ? "i = %d @ nums[%d][%d] = (%s)\n" : "i = %d @ eventArray[%d][%d] = (%s)\n", i, numsC, numsR - writeToTilemap, substring);
        writeToTilemap = !writeToTilemap;
        if (numsR > x - 1)
        {
            numsC++;
            numsR = 0;
        }
        //printf("%d\n", num);
    }
    /*for(int dy = 0; dy < y; dy++)
    {
        for(int dx = 0; dx < x; dx++)
        {
            *(tilemapData + dx + dy * x) = sameArray[dy][dx];
            *(eventmapData + dx + dy * x) = eventArray[dy][dx];
        }
    }*/
}

void mainLoop(player* playerSprite)
{
    bool quit = false, editingTiles = true;
    int frame = 0;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        drawTilemap(0, 0, 20, 15, false);
        if (!editingTiles)
        {
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0x58);
            SDL_RenderFillRect(mainRenderer, NULL);
            SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        }
        drawEventmap(0, 0, 20, 15, editingTiles, false);
        if (!editingTiles)
            drawEventTile(playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, playerSprite->flip);
        else
            drawTile(playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, playerSprite->flip);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
        SDL_RenderPresent(mainRenderer);
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        getKey();  //editor freezes without this
        if (++frame > 0)
        {
            if (playerSprite->spr.y > 0 && keyStates[SDL_SCANCODE_W])
                playerSprite->spr.y -= PIXELS_MOVED;
            if (playerSprite->spr.y < SCREEN_HEIGHT - playerSprite->spr.h && keyStates[SDL_SCANCODE_S])
                playerSprite->spr.y += PIXELS_MOVED;
            if (playerSprite->spr.x > 0 && keyStates[SDL_SCANCODE_A])
                playerSprite->spr.x -= PIXELS_MOVED;
            if (playerSprite->spr.x < SCREEN_WIDTH - playerSprite->spr.w && keyStates[SDL_SCANCODE_D])
                playerSprite->spr.x += PIXELS_MOVED;
            if (keyStates[SDL_SCANCODE_Q] && playerSprite->spr.tileIndex > 0)
                playerSprite->spr.tileIndex--;
            if (keyStates[SDL_SCANCODE_E] && playerSprite->spr.tileIndex < 127)
                playerSprite->spr.tileIndex++;
            if (keyStates[SDL_SCANCODE_SPACE] && editingTiles)
                tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
            if (keyStates[SDL_SCANCODE_SPACE] && !editingTiles)
                eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
            if (keyStates[SDL_SCANCODE_LSHIFT])
            {
                editingTiles = !editingTiles;
                if (!editingTiles)
                    playerSprite->spr.tileIndex = 1;
                else
                    playerSprite->spr.tileIndex = 0;
            }
            if (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_S] || keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_D])
                frame = -6;
            else
                frame = -12;
        }

        if (keyStates[SDL_SCANCODE_ESCAPE] || keyStates[SDL_SCANCODE_RETURN])
                quit = true;

        SDL_Delay(15);  //frame cap sorta
    }
}

SDL_Keycode getKey()
{
    SDL_Event e;
    SDL_Keycode keycode = 0;
    while(SDL_PollEvent(&e) != 0)
    {
        if(e.type == SDL_QUIT)
            keycode = -1;
        else
            if(e.type == SDL_KEYDOWN)
                keycode = e.key.keysym.sym;
    }
    return keycode;
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

void drawEventmap(int startX, int startY, int endX, int endY, bool drawHiddenTiles, bool updateScreen)
{
    for(int dy = startY; dy < endY; dy++)
        for(int dx = startX; dx < endX; dx++)
            drawEventTile(eventmap[dy][dx] == 1 && drawHiddenTiles ? 0 : eventmap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
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
    char* outputFile = "output/map.txt";
    createFile(outputFile);
    char input[2];
    char output[1201];
    input[0] = '\0';
    output[0] = '\0';  //these two lines are necessary for output being correct
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
    appendLine("output/map.txt", output);
    printf("%s\n", output);
    printf("outputted to output/map.txt\n");
}
