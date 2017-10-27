#include "outermeSDL.h"

#define PIXELS_MOVED TILE_SIZE

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

#define checkSKUp keyStates[26]
#define checkSKDown keyStates[22]
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
#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)
#define WINDOW_NAME "Gateway to Legend Map Creator"

//^map creator defines. v map-pack wizard defines

#define PICK_MESSAGES_ARRAY {"initial X", "initial Y", "Pick the main character tile.", "Pick the cursor.", "Pick the fully-transparent tile.", "Pick button 1.", "Pick button 2.", "Pick button 3.", "Pick door 1.", "Pick door 2.", "Pick door 3.", "Pick the teleporter.", "Pick the damaging hazard.", "Pick the warp gate."}
const int maxArraySize = 14;
#define MAX_MAP_PACK_DATA 6

int mainMapCreator();
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);
void mainMapCreatorLoop(player* playerSprite);
SDL_Keycode getKey();
void drawEventmap(int startX, int startY, int endX, int endY, bool drawHiddenTiles, bool updateScreen);
void drawEventTile(int id, int xCoord, int yCoord, int width, SDL_RendererFlip flip);
void initPlayer(player* player, int x, int y, int size, int tileIndex);
void writeTileData();
//^map creator functions. v map-pack wizard functions
int mainMapPackWizard();
void mainMapPackWizardLoop(sprite* playerSprite, int* numArray);
void strPrepend(char* input, const char* prepend);

int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* eventTexture;

int main(int argc, char* argv[])
{
    initSDL("Gateway to Legend Map Tools", "tileset/SeekersTile48.png", FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 24);
    drawText("Press 1 for Map Creator, 2 for Map-Pack Wizard.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0, 0, 0}, true);
    SDL_Keycode keycode = waitForKey();
    closeSDL();
    if (keycode == SDLK_1)
        mainMapCreator();
    if (keycode == SDLK_2)
        mainMapPackWizard();
    if (keycode != SDLK_1 && keycode != SDLK_2)
        printf("Invalid keypress.\n");
    return 0;
}

int mainMapCreator()
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
        printf("Enter the map-pack filepath: map-packs/");
        scanf("%s", mainFilePath);
        strPrepend(mainFilePath, "map-packs/");
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
    mainMapCreatorLoop(&creator);
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

void mainMapCreatorLoop(player* playerSprite)
{
    bool quit = false, editingTiles = true;
    int frame = 0, sleepFor = 0, targetTime = 1000 / 60, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = SDL_GetTicks();
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
        if (SDL_GetTicks() - lastKeypressTime) >= 16)
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
	    lastKeypressTime = SDL_GetTicks();
        }

        if (keyStates[SDL_SCANCODE_ESCAPE] || keyStates[SDL_SCANCODE_RETURN])
                quit = true;

        sleepFor = targetTime - (SDL_GetTicks() - lastFrame);  //FPS limiter; rests for (16 - time spent) ms per frame, effectively making each frame run for ~16 ms, or 60 FPS
        if (sleepFor > 0)
            SDL_Delay(sleepFor);
        lastFrame = SDL_GetTicks();
	frame++;
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
//end map creator code.

//start map-pack wizard code
#undef SCREEN_WIDTH
#undef SCREEN_HEIGHT
#define SCREEN_WIDTH TILE_SIZE * 16
#define SCREEN_HEIGHT TILE_SIZE * 9
#undef WIDTH_IN_TILES
#undef HEIGHT_IN_TILES
#define WIDTH_IN_TILES SCREEN_WIDTH / TILE_SIZE
#define HEIGHT_IN_TILES SCREEN_HEIGHT / TILE_SIZE

int mainMapPackWizard()
{
    char getString[128], mapPackData[MAX_MAP_PACK_DATA][128], garbageData[128];
    int* numbers = calloc(maxArraySize, sizeof(int));
    int wizardState = 0;
    bool quit = false;
	while (!quit)
    {
        switch(wizardState)
        {
        case 0:
            printf("Create new file? (y/n) ");
            break;
        case 1:
        case 7:
            printf("File name? map-packs/");
            break;
        case 2:
            printf("Title of map pack? ");
            break;
        case 3:
            printf("Path for maps file? maps/");
            break;
        case 4:
            printf("Path for tileset file? tileset/");
            break;
        case 5:
            printf("Path for savefile? saves/");
            break;
        case 6:
            printf("Path for scripts? scripts/");
            break;
        case 8:
            printf("Initial X spawn-coordinate? ");
            break;
        case 9:
            printf("Initial Y spawn-coordinate? ");
            break;
        }
        scanf("%128[^\n]%*c", getString);
        switch(wizardState)
        {
        case 0:
            if (getString[0] == 'y')
                wizardState = 1;
            else
                wizardState = 7;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            strcpy(mapPackData[wizardState - 1], getString);
            if (wizardState == 1)
                strPrepend(mapPackData[0], "map-packs/");

            if (wizardState == 3)
                strPrepend((char*) mapPackData[2], "maps/");

            if (wizardState == 4)
                strPrepend((char*) mapPackData[3], "tileset/");

            //printf("%s\n", mapPackData[wizardState - 1]);
            if (wizardState == 5)
                strPrepend((char*) mapPackData[4], "saves/");

            if (wizardState == 6)
            {
                strPrepend((char*) mapPackData[5], "scripts/");
                wizardState++;  //gets us past loading
            }
            wizardState++;
            break;
        case 7:
            strcpy(mapPackData[0], getString);
            strPrepend(mapPackData[0], "map-packs/");
            if (!checkFile(mapPackData[0], 0))
            {
                perror("No such map-pack");
                return 1;
            }
            for(int i = 1; i < MAX_MAP_PACK_DATA; i++)
                uniqueReadLine((char**) &mapPackData[i], 128, mapPackData[0], i - 1);
            for(int i = 0; i < 2; i++)
            {
                uniqueReadLine((char**) &garbageData, 128, mapPackData[0], i + 5);
                numbers[i] = strtol(garbageData, NULL, 10);
            }
            quit = true;
            break;
        case 8:
        case 9:
            sscanf(getString, "%d", &(numbers[wizardState++ - 8]));
            if (wizardState == 10)  //since we did wizardState++ before this
                quit = true;
            break;
        }
    }
    initSDL("Gateway to Legend Map-Pack Wizard", mapPackData[3], FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 24);
    sprite chooser;
    initSprite(&chooser, 0, TILE_SIZE, TILE_SIZE, 0, type_player);
    mainMapPackWizardLoop(&chooser, numbers);
    if (!(numbers[0] == -1))
    {
        createFile(mapPackData[0]);
        for(int i = 1; i < MAX_MAP_PACK_DATA; i++)
        {
            appendLine(mapPackData[0], mapPackData[i]);
            //printf("%s\n", mapPackData[i]);
        }
        char* whoCares = "";
        for(int i = 0; i < maxArraySize; i++)
        {
            appendLine(mapPackData[0], intToString(numbers[i], whoCares));
            //printf("%d\n", numbers[i]);
        }
        printf("Outputted to your file.\n");
    }
    closeSDL();
    free(numbers);
    return 0;
}

void mainMapPackWizardLoop(sprite* playerSprite, int* numArray)
{
    int numArrayTracker = 2, frame = 0, sleepFor = 0, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = lastFrame + 1;
    char* text[] = PICK_MESSAGES_ARRAY;
    bool quit = false;
    while(numArrayTracker < maxArraySize && !quit)
    {
        SDL_RenderClear(mainRenderer);
        SDL_RenderCopy(mainRenderer, tilesetTexture, NULL, &((SDL_Rect) {.x = 0, .y = TILE_SIZE, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT - TILE_SIZE}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0x1C, 0xC6, 0xFF);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->x, .y= playerSprite->y, .w = playerSprite->w, .h = playerSprite->h}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        drawText(text[numArrayTracker], 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0x00, 0x0, 0x00}, true);
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        getKey();  //editor freezes without this
        if (SDL_GetTicks() - lastKeypressTime) >= 16)
        {
            if (playerSprite->y > TILE_SIZE && checkSKUp)
                playerSprite->y -= PIXELS_MOVED;
            if (playerSprite->y < SCREEN_HEIGHT - playerSprite->h && checkSKDown)
                playerSprite->y += PIXELS_MOVED;
            if (playerSprite->x > 0 && checkSKLeft)
                playerSprite->x -= PIXELS_MOVED;
            if (playerSprite->x < SCREEN_WIDTH - playerSprite->w && checkSKRight)
                playerSprite->x += PIXELS_MOVED;
            if (checkSKInteract)
                numArray[numArrayTracker++] = 8 * (playerSprite->x / TILE_SIZE) + playerSprite->y / TILE_SIZE - 1;  //-1 because we don't start at y=0
            lastKeypressTime = SDL_GetTicks();
        }

        if (checkSKMenu || keyStates[SDL_SCANCODE_RETURN])
                quit = true;

        sleepFor = targetTime - (SDL_GetTicks() - lastFrame);  //FPS limiter; rests for (16 - time spent) ms per frame, effectively making each frame run for ~16 ms, or 60 FPS
        if (sleepFor > 0)
            SDL_Delay(sleepFor);
        lastFrame = SDL_GetTicks();
	frame++;
        //SDL_RenderPresent(mainRenderer);
    }
    /*for(int i = 0; i < maxArraySize; i++)
        printf("%d\n", numArray[i]);*/
    //waitForKey();
    if (numArrayTracker < maxArraySize)
        numArray[0] = -1;
}

void strPrepend(char* input, const char* prepend)
{
    char* temp = calloc(128, sizeof(char));
    strcpy(temp, prepend);
    strcat(temp, input);
    strcpy(input, temp);
    free(temp);
}
//end map-pack wizard code

