//#include "outermeSDL.h"
#include "SDLGateway.h"

#define checkSKUp keyStates[SC_UP]
#define checkSKDown keyStates[SC_DOWN]
#define checkSKLeft keyStates[SC_LEFT]
#define checkSKRight keyStates[SC_RIGHT]
#define checkSKInteract keyStates[SC_INTERACT]
#define checkSKMenu keyStates[SC_MENU]

#define TILE_ID_PLAYER 16
#define PIXELS_MOVED 6

#define CONFIG_FILEPATH "GatewayToLegend.cfg"
#define GLOBALTILES_FILEPATH "tileset/main.png"
#define GLOBALSAVE_FILEPATH "saves/GatewayMain.txt"
#define MAP_PACKS_SUBFOLDER "map-packs/"
#define MAX_LIST_OF_MAPS 30
#define MAX_CHAR_IN_FILEPATH 128

#define MAX_MAPPACKS_PER_PAGE 11

#define START_GAMECODE 0
#define OPTIONS_GAMECODE 1
#define PLAY_GAMECODE 2
#define MAINLOOP_GAMECODE 3
#define OVERWORLDMENU_GAMECODE 4

#define MAX_TILE_ID_ARRAY 11

#define drawASprite(tileset, spr, flip) drawATile(tileset, spr.tileIndex, spr.x, spr.y, spr.w, flip)

int mainLoop(player* playerSprite);
void checkCollision(player* player, int* outputData, int moveX, int moveY);
char* mapSelectLoop(char** listOfFilenames, int maxStrNum, bool* backFlag);
void drawOverTilemap(SDL_Texture* texture, int startX, int startY, int endX, int endY, bool rerender);

/*bool debug;
bool doDebugDraw;
SDL_Texture* eventTexture;  //eventmap layer is needed, this is just for debug, so when you're all done you can prob remove these*/

int tileIDArray[MAX_TILE_ID_ARRAY];
#define PLAYER_ID tileIDArray[0]
#define CURSOR_ID tileIDArray[1]

int main(int argc, char* argv[])
{
    //debug = true;
    {
        int initCode = initSDL(GLOBALTILES_FILEPATH);
        if (initCode != 0)
            return initCode;
    }
    //loading in map pack header files
    char mainFilePath[MAX_CHAR_IN_FILEPATH];
    char** listOfFilenames;
    int maxStrNum = 0;
    listOfFilenames = getListOfFiles(MAX_LIST_OF_MAPS, MAX_CHAR_IN_FILEPATH - 9, MAP_PACKS_SUBFOLDER, &maxStrNum);
    //done loading map pack header files
    if (checkFile(GLOBALSAVE_FILEPATH, 0))
        /*load global save file*/;
    else
        createFile(GLOBALSAVE_FILEPATH);
    if (checkFile(CONFIG_FILEPATH, 6))  //load config
        loadConfig(CONFIG_FILEPATH);
    else
        initConfig(CONFIG_FILEPATH);
    player person;
    /*if (debug)
        loadIMG("tileset/eventTile48.png", &eventTexture);*/
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mainRenderer);
    int gameState = 0;
    char* buffer = "";  //actually needed
    bool quitGame = false;
    while(!quitGame)
    {
        int choice = 0;
        switch(gameState)
        {
        case START_GAMECODE:  //start menu
            choice = aMenu(tilesetTexture, 17, "Gateway to Legend", "Play", "Options", "Quit", " ", "(Not final menu)", 3, 1, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xA5, 0xA5, 0xA5, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);
            if (choice == 1)
                gameState = PLAY_GAMECODE;
            if (choice == 2)
                gameState = OPTIONS_GAMECODE;
            if (choice == 3)
                quitGame = true;
            break;
        case OPTIONS_GAMECODE:
            gameState = START_GAMECODE;
            break;
        case PLAY_GAMECODE:  //main menu
            strcpy(mainFilePath, mapSelectLoop(listOfFilenames, maxStrNum, &quitGame));
            if (quitGame)
            {
                gameState = START_GAMECODE;
                quitGame = false;
                break;
            }
            //loading map pack stuff
            char mapFilePath[MAX_CHAR_IN_FILEPATH - 9];
            char tileFilePath[MAX_CHAR_IN_FILEPATH - 9];
            char saveFilePath[MAX_CHAR_IN_FILEPATH - 9];
            uniqueReadLine((char**) &mapFilePath, MAX_CHAR_IN_FILEPATH - 9, mainFilePath, 1);
            printf("%s\n", mapFilePath);
            uniqueReadLine((char**) &tileFilePath, MAX_CHAR_IN_FILEPATH - 9, mainFilePath, 2);
            printf("%s\n", tileFilePath);
            uniqueReadLine((char**) &saveFilePath, MAX_CHAR_IN_FILEPATH - 9, mainFilePath, 3);
            if (checkFile(saveFilePath, 0))
                /*load local save file*/;
            else
                createFile(saveFilePath);
            printf("%s\n", saveFilePath);
            for(int i = 0; i < MAX_TILE_ID_ARRAY; i++)
            {
                tileIDArray[i] = strtol(readLine(mainFilePath, 6 + i, &buffer), NULL, 10);
            }
            loadIMG(tileFilePath, &tilesTexture);
            initPlayer(&person, strtol(readLine(mainFilePath, 4, &buffer), NULL, 10), strtol(readLine(mainFilePath, 5, &buffer), NULL, 10), TILE_SIZE, PLAYER_ID);
            //done loading map-pack specific stuff
            gameState = MAINLOOP_GAMECODE;
            break;
        case MAINLOOP_GAMECODE:  //main game loop
            loadMapFile(mapFilePath, tilemap, eventmap, 2, HEIGHT_IN_TILES, WIDTH_IN_TILES);
            choice = mainLoop(&person);
            if (choice == ANYWHERE_QUIT)
                quitGame = true;
            if (choice == 1)
                gameState = OVERWORLDMENU_GAMECODE;
            break;
        case OVERWORLDMENU_GAMECODE:  //overworld menu
            choice = aMenu(tilesTexture, CURSOR_ID, "Overworld Menu", "Back", " ", "Quit", " ", " " , 3, 1, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);
            if (choice == 1)
                gameState = MAINLOOP_GAMECODE;
            if (choice == 3)
                gameState = START_GAMECODE;
            break;
        }
    }
    printf("Quit successfully\n");
    //SDL_DestroyTexture(eventTexture);  //once we delete eventTexture, you can remove this.
    SDL_DestroyTexture(tilesTexture);
    closeSDL();
}

char* mapSelectLoop(char** listOfFilenames, int maxStrNum, bool* backFlag)
{
    bool quitMenu = false;
    char junkArray[MAX_CHAR_IN_FILEPATH];
    SDL_Keycode menuKeycode;
    int menuPage = 0, selectItem = 0;
    char* mapPackName = (char*) malloc(MAX_CHAR_IN_FILEPATH * sizeof(char));  //find some way to free this please!
    while(!quitMenu)
    {
        SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(mainRenderer);
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        for(int i = 0; i < (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE); i++)  //11 can comfortably be max
            drawText(readLine((char*) strcat(strcpy(junkArray, MAP_PACKS_SUBFOLDER), listOfFilenames[i + (menuPage * 5)]),  /*concatting the path and one of the filenames together into one string*/
                          0, (char**) &junkArray), TILE_SIZE + 10, (i + 3) * TILE_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {0, 0, 0}, false);
        drawText("Back", TILE_SIZE + 10, 2 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {0, 0, 0}, false);
        menuKeycode = getKey();
        if ((menuKeycode == SDL_GetKeyFromScancode(SC_LEFT) && menuPage > 0) || (menuKeycode == SDL_GetKeyFromScancode(SC_RIGHT) && menuPage < maxStrNum / MAX_MAPPACKS_PER_PAGE))
        {
            menuPage += (menuKeycode == SDL_GetKeyFromScancode(SC_RIGHT)) - 1 * (menuKeycode == SDL_GetKeyFromScancode(SC_LEFT));
            selectItem = 0;
        }

        if ((menuKeycode == SDL_GetKeyFromScancode(SC_UP) && selectItem > 0) || (menuKeycode == SDL_GetKeyFromScancode(SC_DOWN) && selectItem < (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE)))
            selectItem += (menuKeycode == SDL_GetKeyFromScancode(SC_DOWN)) - 1 * (menuKeycode == SDL_GetKeyFromScancode(SC_UP));

        drawTile(17, 10, (selectItem + 2) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);

        if (menuKeycode == SDL_GetKeyFromScancode(SC_INTERACT))
        {
            if (selectItem != 0)
                selectItem = menuPage * MAX_MAPPACKS_PER_PAGE + selectItem - 1;
            else
                *backFlag = true;
                quitMenu = true;
        }
    }
    //loading map pack stuff
    strncat(strcpy(mapPackName, MAP_PACKS_SUBFOLDER), listOfFilenames[selectItem], MAX_CHAR_IN_FILEPATH - 9);
    return mapPackName;
}

int mainLoop(player* playerSprite)
{
    SDL_Event e;
    bool quit = false, drawFlag = true;
    bool doorFlags[3] = {true, true, true};
    int* collisionData = (int*) calloc(7, sizeof(int));
    //doDebugDraw = false;
    int frame = 0, framerate = 0;
    int exitCode = 0;
    char whatever[5] = "    \0";
    time_t startTime = time(NULL), lastTime = startTime - 1, now = startTime + 1;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        drawATilemap(tilesTexture, false, 0, 0, 20, 15, false);
        drawOverTilemap(tilesTexture, 0, 0, 20, 15, false);
        /*if (doDebugDraw)
            drawATilemap(eventTexture, true, 0, 0, 20, 15, false);*/
        //drawTile(tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE + 1 * (playerSprite->spr.x % TILE_SIZE > .5 * TILE_SIZE)], (playerSprite->spr.x / TILE_SIZE  + 1 * (playerSprite->spr.x % TILE_SIZE > .5 * TILE_SIZE)) * TILE_SIZE, (playerSprite->spr.y / TILE_SIZE) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                exitCode = ANYWHERE_QUIT;
            }
            /*if (e.key.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_g && debug)
                doDebugDraw = !doDebugDraw;*/
        }
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        if (!playerSprite->movementLocked && (checkSKUp || checkSKDown || checkSKLeft || checkSKRight) && frame % 22 == 0)
        {
            int lastY = playerSprite->spr.y;
            int lastX = playerSprite->spr.x;
                //SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
                //drawTile(tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE], (playerSprite->spr.x / TILE_SIZE) * TILE_SIZE, (playerSprite->spr.y / TILE_SIZE) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
                if (playerSprite->spr.y > 0 && checkSKUp)
                    playerSprite->spr.y -= PIXELS_MOVED;
                if (playerSprite->spr.y < SCREEN_HEIGHT - playerSprite->spr.h && checkSKDown)
                    playerSprite->spr.y += PIXELS_MOVED;
                if (playerSprite->spr.x > 0 && checkSKLeft)
                    playerSprite->spr.x -= PIXELS_MOVED;
                if (playerSprite->spr.x < SCREEN_WIDTH - playerSprite->spr.w && checkSKRight)
                    playerSprite->spr.x += PIXELS_MOVED;
                if (checkSKLeft)
                    playerSprite->flip = SDL_FLIP_HORIZONTAL;
                if (checkSKRight)
                    playerSprite->flip = SDL_FLIP_NONE;
                /*if (checkCollision(playerSprite, tilemap[playerSprite->spr.y / TILE_SIZE + checkSKDown][playerSprite->spr.x / TILE_SIZE + checkSKRight], 0, 0))
                {
                    if (SDL_HasIntersection(&((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}), &((SDL_Rect) {.x = (playerSprite->spr.x / TILE_SIZE) * TILE_SIZE + checkSKRight, .y = (playerSprite->spr.y / TILE_SIZE) * TILE_SIZE + checkSKDown, .w = TILE_SIZE, .h = TILE_SIZE})))
                    {
                        playerSprite->spr.y = lastY;
                        playerSprite->spr.x = lastX;
                    }
                }*/
                checkCollision(playerSprite, collisionData, checkSKRight + -1 * checkSKLeft, checkSKDown + -1 * checkSKUp);
                if (collisionData[0] || ((collisionData[4] && doorFlags[0] == true) || (collisionData[5] && doorFlags[1] == true) || (collisionData[6] && doorFlags[2] == true)))
                {
                    playerSprite->spr.y = lastY;
                    playerSprite->spr.x = lastX;
                    //printf("%d\n", exitCode);
                }
                if (collisionData[1] || collisionData[2] || collisionData[3])
                {
                    for(int i = 0; i < 3; i++)
                    {
                        if (collisionData[i + 1])
                            doorFlags[i] = false;
                    }
                }
        }
        if (checkSKMenu)
        {
            quit = true;
            exitCode = 1;
        }
        frame++;
        if (time(NULL) > startTime)
            now = time(NULL);
        if (time(NULL) - 1 > lastTime)
            lastTime = time(NULL);
        if (lastTime == now)
        {
            if (drawFlag)
                framerate = frame / ((int) now - (int) startTime);
            drawFlag = false;
        }
        else
            drawFlag = true;
        drawText(intToString(framerate, whatever), 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF}, false);
        //printf("Framerate: %d\n", frame / ((int) now - (int) startTime));
        drawASprite(tilesTexture, playerSprite->spr, playerSprite->flip);
        SDL_RenderPresent(mainRenderer);
    }
    return exitCode;
}

void checkCollision(player* player, int* outputData, int moveX, int moveY)
{
    for(int i = 0; i < 7; i++)
    {
        outputData[i] = 0;
    }
    if (moveX || moveY)
    {
        int thisX = player->spr.x, thisY = player->spr.y;
        int topLeft = eventmap[thisY / TILE_SIZE][thisX / TILE_SIZE], topRight = eventmap[thisY / TILE_SIZE][thisX / TILE_SIZE + (thisX % TILE_SIZE != 0)], bottomLeft = eventmap[thisY / TILE_SIZE + (thisY % TILE_SIZE != 0)][thisX / TILE_SIZE], bottomRight = eventmap[thisY / TILE_SIZE + (thisY % TILE_SIZE != 0)][thisX / TILE_SIZE + (thisX % TILE_SIZE != 0)];
        if (-1 != checkArrayForIVal(1, (int[]) {topLeft, topRight, bottomLeft, bottomRight}, 4))
            outputData[0] = topLeft + 2 * topRight + 4 * bottomLeft + 8 * bottomRight;
        if (((outputData[0] == 1 || outputData[0] == 5) && moveX < 0 && moveY > 0) || ((outputData[0] == 2 || outputData[0] == 10) && moveX > 0 && moveY > 0) || ((outputData[0] == 4 || outputData[0] == 5) && moveX < 0 && moveY < 0) || ((outputData[0] == 8 || outputData[0] == 10) && moveX > 0 && moveY < 0))
        {  //manually adding y sliding
            outputData[0] = false;
            player->spr.x -= moveX * PIXELS_MOVED;
        }
        if (((outputData[0] == 1 || outputData[0] == 3) && moveX > 0 && moveY < 0) || ((outputData[0] == 2 || outputData[0] == 3) && moveX < 0 && moveY < 0) || ((outputData[0] == 4 || outputData[0] == 12) && moveX > 0 && moveY > 0) || ((outputData[0] == 8 || outputData[0] == 12) && moveX < 0 && moveY > 0))
        {  //manually adding x sliding
            outputData[0] = false;
            player->spr.y -= moveY * PIXELS_MOVED;
        }

        /*if (collideID && debug && doDebugDraw)
            printf("X - %d\n", collideID);*/
        /*if (debug && doDebugDraw)
        {
            drawTile(7, (thisX / TILE_SIZE) * TILE_SIZE, (thisY / TILE_SIZE) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
            drawTile(15, (thisX / TILE_SIZE + (thisX % TILE_SIZE != 0)) * TILE_SIZE, (thisY / TILE_SIZE) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
            drawTile(2, (thisX / TILE_SIZE) * TILE_SIZE, (thisY / TILE_SIZE + (thisY % TILE_SIZE != 0)) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
            drawTile(9, (thisX / TILE_SIZE + (thisX % TILE_SIZE != 0)) * TILE_SIZE, (thisY / TILE_SIZE + (thisY % TILE_SIZE != 0)) * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
            SDL_RenderPresent(mainRenderer);
        }*/
        for(int i = 1; i < 7; i++)
        {
            if (-1 != checkArrayForIVal(i + 1, (int[]) {topLeft, topRight, bottomLeft, bottomRight}, 4))
            outputData[i] = true;
        }
    }
}

void drawOverTilemap(SDL_Texture* texture, int startX, int startY, int endX, int endY, bool rerender)
{
    for(int y = startY; y < endY; y++)
        for(int x = startX; x < endX; x++)
        {
            drawATile(texture, tileIDArray[eventmap[y][x] + 2 - (eventmap[y][x] >= 1)], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        }
    if (rerender)
        SDL_RenderPresent(mainRenderer);
}
