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

#define WINDOW_NAME "Gateway to Legend"
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
#define RELOAD_GAMECODE 5

#define MAX_TILE_ID_ARRAY 12
#define MAX_COLLISIONDATA_ARRAY 10

#define drawASprite(tileset, spr, flip) drawATile(tileset, spr.tileIndex, spr.x, spr.y, spr.w, flip)

int mainLoop(player* playerSprite);
void checkCollision(player* player, int* outputData, int moveX, int moveY);
void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag);
void drawOverTilemap(SDL_Texture* texture, int startX, int startY, int endX, int endY, bool drawDoors[], bool rerender);
bool executeScriptAction(script* scriptData, player* player);

/*bool debug;
bool doDebugDraw;
SDL_Texture* eventTexture;  //eventmap layer is needed, this is just for debug, so when you're all done you can prob remove these*/

int tileIDArray[MAX_TILE_ID_ARRAY];
#define PLAYER_ID tileIDArray[0]
#define CURSOR_ID tileIDArray[1]
bool doorFlags[3] = {true, true, true};  //this works; however it persists through map packs as well
script* allScripts;
int sizeOfAllScripts;

int main(int argc, char* argv[])
{
    //debug = true;
    {
        int initCode = initSDL(WINDOW_NAME, GLOBALTILES_FILEPATH, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
        if (initCode != 0)
            return initCode;
    }
    //loading in map pack header files
    char mainFilePath[MAX_CHAR_IN_FILEPATH], mapFilePath[MAX_CHAR_IN_FILEPATH - 9], tileFilePath[MAX_CHAR_IN_FILEPATH - 9],
            saveFilePath[MAX_CHAR_IN_FILEPATH - 9], scriptFilePath[MAX_CHAR_IN_FILEPATH - 9];
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
            person.mapScreen = 0;
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
            mapSelectLoop(listOfFilenames, (char*) mainFilePath, maxStrNum, &quitGame);
            if (quitGame)
            {
                gameState = START_GAMECODE;
                quitGame = false;
                break;
            }
            //loading map pack stuff
            uniqueReadLine((char**) &mapFilePath, MAX_CHAR_IN_FILEPATH - 9, mainFilePath, 1);
            //printf("%s\n", mapFilePath);
            uniqueReadLine((char**) &tileFilePath, MAX_CHAR_IN_FILEPATH - 9, mainFilePath, 2);
            //printf("%s\n", tileFilePath);
            uniqueReadLine((char**) &saveFilePath, MAX_CHAR_IN_FILEPATH - 9, mainFilePath, 3);
            //printf("%s\n", saveFilePath);
            uniqueReadLine((char**) &scriptFilePath, MAX_CHAR_IN_FILEPATH - 9, mainFilePath, 4);
            //printf("%s\n", scriptFilePath);
            allScripts = calloc(checkFile(scriptFilePath, -1) + 1, sizeof(script));
            for(int i = 0; i < checkFile(scriptFilePath, -1) + 1; i++)
            {
                script thisScript;
                readScript(&thisScript, readLine(scriptFilePath, i, &buffer));
                allScripts[i] = thisScript;
                sizeOfAllScripts = i + 1;
            }
            quitGame = false;
            if (checkFile(saveFilePath, 0))
                /*load local save file*/;
            else
                createFile(saveFilePath);

            for(int i = 0; i < MAX_TILE_ID_ARRAY; i++)
            {
                tileIDArray[i] = strtol(readLine(mainFilePath, 7 + i, &buffer), NULL, 10);
            }
            loadIMG(tileFilePath, &tilesTexture);
            initPlayer(&person, strtol(readLine(mainFilePath, 5, &buffer), NULL, 10), strtol(readLine(mainFilePath, 6, &buffer), NULL, 10), TILE_SIZE, person.mapScreen, PLAYER_ID);
            //done loading map-pack specific stuff
            gameState = MAINLOOP_GAMECODE;
            break;
        case MAINLOOP_GAMECODE:  //main game loop
            loadMapFile(mapFilePath, tilemap, eventmap, person.mapScreen, HEIGHT_IN_TILES, WIDTH_IN_TILES);
            person.extraData = mapFilePath;
            choice = mainLoop(&person);
            if (choice == ANYWHERE_QUIT)
                quitGame = true;
            if (choice == 1)
                gameState = OVERWORLDMENU_GAMECODE;
            if (choice == 2)
                gameState = RELOAD_GAMECODE;
            break;
        case OVERWORLDMENU_GAMECODE:  //overworld menu
            choice = aMenu(tilesTexture, CURSOR_ID, "Overworld Menu", "Back", " ", "Quit", " ", " " , 3, 1, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xA5, 0xA5, 0xA5, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);
            if (choice == 1)
                gameState = MAINLOOP_GAMECODE;
            if (choice == 3)
            {
                gameState = START_GAMECODE;
                for(int i = 0; i < 3; i++)
                    doorFlags[i] = true;
            }
            break;
        case RELOAD_GAMECODE:
            for(int i = 0; i < 3; i++)
                doorFlags[i] = true;
            gameState = MAINLOOP_GAMECODE;
            break;
        }
    }
    printf("Quit successfully\n");
    //SDL_DestroyTexture(eventTexture);  //once we delete eventTexture, you can remove this.
    SDL_DestroyTexture(tilesTexture);
    free(allScripts);
    closeSDL();
}

void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag)
{
    bool quitMenu = false;
    char junkArray[MAX_CHAR_IN_FILEPATH];
    SDL_Keycode menuKeycode;
    int menuPage = 0, selectItem = 0;
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
}

int mainLoop(player* playerSprite)
{
    SDL_Event e;
    bool quit = false;
    char* mapFilePath = playerSprite->extraData;
    int maxTheseScripts = 0, * collisionData = calloc(MAX_COLLISIONDATA_ARRAY, sizeof(int));
    script thisScript, * theseScripts = calloc(sizeOfAllScripts, sizeof(script));
    for(int i = 0; i < sizeOfAllScripts; i++)
    {
        if (allScripts[i].mapNum == playerSprite->mapScreen)
            theseScripts[maxTheseScripts++] = allScripts[i];
    }
    theseScripts = realloc(theseScripts, maxTheseScripts * sizeof(script));
    //printf("%d < %d\n", maxTheseScripts, sizeOfAllScripts);
    //doDebugDraw = false;
    int frame = 0, framerate = 0;
    int exitCode = 0;
    char whatever[5] = "    \0";
    int startTime = SDL_GetTicks() - 1;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        drawATilemap(tilesTexture, false, 0, 0, 20, 15, false);
        drawOverTilemap(tilesTexture, 0, 0, 20, 15, doorFlags, false);
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
        if (!playerSprite->movementLocked && (checkSKUp || checkSKDown || checkSKLeft || checkSKRight || checkSKInteract) && frame % 18 == 0)
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
                if (checkSKInteract)
                    initScript(&thisScript, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, script_trigger_dialogue, "Hello world!");
                /*if (checkCollision(playerSprite, tilemap[playerSprite->spr.y / TILE_SIZE + checkSKDown][playerSprite->spr.x / TILE_SIZE + checkSKRight], 0, 0))
                {
                    if (SDL_HasIntersection(&((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}), &((SDL_Rect) {.x = (playerSprite->spr.x / TILE_SIZE) * TILE_SIZE + checkSKRight, .y = (playerSprite->spr.y / TILE_SIZE) * TILE_SIZE + checkSKDown, .w = TILE_SIZE, .h = TILE_SIZE})))
                    {
                        playerSprite->spr.y = lastY;
                        playerSprite->spr.x = lastX;
                    }
                }*/
                if (!playerSprite->spr.x || !playerSprite->spr.y || playerSprite->spr.x == SCREEN_WIDTH - TILE_SIZE || playerSprite->spr.y == SCREEN_HEIGHT - TILE_SIZE)
                {
                    bool quitThis = false;
                    if (!playerSprite->spr.x && playerSprite->mapScreen % 10 > 0)
                    {
                        playerSprite->spr.x = SCREEN_WIDTH - (2 * TILE_SIZE);
                        playerSprite->mapScreen--;
                        quitThis = true;
                    }
                    if (!playerSprite->spr.y && playerSprite->mapScreen / 10 > 0)
                    {
                        playerSprite->spr.y = SCREEN_HEIGHT - (2 * TILE_SIZE);
                        playerSprite->mapScreen -= 10;
                        quitThis = true;
                    }
                    if (playerSprite->spr.x == SCREEN_WIDTH - TILE_SIZE && playerSprite->mapScreen % 10 < 9)
                    {
                        playerSprite->spr.x = TILE_SIZE;
                        playerSprite->mapScreen++;
                        quitThis = true;
                    }
                    if (playerSprite->spr.y == SCREEN_HEIGHT - TILE_SIZE && playerSprite->mapScreen / 10 < 9)
                    {
                        playerSprite->spr.y = TILE_SIZE;
                        playerSprite->mapScreen += 10;
                        quitThis = true;
                    }
                    if (quitThis)
                        return 2;
                }
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
                if (collisionData[9])
                {
                    bool found = false;
                    for(int i = 0; i < maxTheseScripts; i++)
                    {
                        if (theseScripts[i].action == script_use_portal && SDL_HasIntersection(&((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}), &((SDL_Rect){.x = theseScripts[i].x, .y = theseScripts[i].y, .w = theseScripts[i].w, .h = theseScripts[i].h})))
                            {
                                thisScript = theseScripts[i];
                                found = true;
                            }
                    }
                    thisScript.active = found;
                    //printf("%s\n", thisScript.data);
                    //initScript(&thisScript, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, script_use_portal, "[0,456,336]\0");
                    playerSprite->extraData = mapFilePath;
                    exitCode = 2;
                }
        }
        if (checkSKMenu)
        {
            quit = true;
            exitCode = 1;
        }
        frame++;
        if ((SDL_GetTicks() - startTime) % 500 == 0)
                framerate = (int) (frame / ((SDL_GetTicks() - startTime) / 1000.0));
        //printf("%d / %f == %d\n", frame, (SDL_GetTicks() - startTime) / 1000.0, framerate);

        drawText(intToString(framerate, whatever), 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF}, false);
        //printf("Framerate: %d\n", frame / ((int) now - (int) startTime));
        drawASprite(tilesTexture, playerSprite->spr, playerSprite->flip);
        SDL_RenderPresent(mainRenderer);
        if (thisScript.active)
            quit = executeScriptAction(&thisScript, playerSprite);
    }
    free(theseScripts);
    free(collisionData);
    return exitCode;
}

void checkCollision(player* player, int* outputData, int moveX, int moveY)
{
    for(int i = 0; i < MAX_COLLISIONDATA_ARRAY; i++)
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
        for(int i = 1; i < MAX_COLLISIONDATA_ARRAY; i++)
        {
            if (-1 != checkArrayForIVal(i + 1, (int[]) {topLeft, topRight, bottomLeft, bottomRight}, 4))
            outputData[i] = true;
        }
    }
}

void drawOverTilemap(SDL_Texture* texture, int startX, int startY, int endX, int endY, bool drawDoors[], bool rerender)
{
    int searchIndex = 0;
    for(int y = startY; y < endY; y++)
        for(int x = startX; x < endX; x++)
        {
            searchIndex = eventmap[y][x] + 2 - (eventmap[y][x] >= 1);
            if ((searchIndex == 6 || searchIndex == 7 || searchIndex == 8) && drawDoors[searchIndex - 6] == false)
                searchIndex = 2;  //2 is index for invis tile
            drawATile(texture, tileIDArray[searchIndex], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
        }
    if (rerender)
        SDL_RenderPresent(mainRenderer);
}

bool executeScriptAction(script* scriptData, player* player)
{
    if (scriptData->action == script_trigger_dialogue)
    {
        drawTextBox(scriptData->data, (SDL_Color){0, 0, 0}, (SDL_Rect){.y = 9 * TILE_SIZE, .w = SCREEN_WIDTH, .h = (HEIGHT_IN_TILES - 9) * TILE_SIZE}, true);  //change coords & color? Possibly use a drawTextBox funct instead?
        waitForKey();
    }
    if (scriptData->action == script_use_portal)
    {
        for(int i = 0; i < 3; i++)
            doorFlags[i] = true;
        for(int i = 120; i > -1; i--)
        {
            SDL_SetRenderDrawColor(mainRenderer, (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), 0xFF);
            SDL_RenderClear(mainRenderer);
            SDL_RenderPresent(mainRenderer);
            SDL_Delay(10);
        }
        SDL_Delay(90);
        char* data = calloc(99, sizeof(char));
        //printf("%s\n", data);
        player->mapScreen = strtol(strtok(strcpy(data, scriptData->data), "[/]"), NULL, 10);  //MUST use a seperate strcpy'd string of the original because C is never that simple
        //printf("%d/", mapNum);
        player->spr.x = strtol(strtok(NULL, "[/]"), NULL, 10);
        //printf("%d/", player->spr.x);
        player->spr.y = strtol(strtok(NULL, "[/]"), NULL, 10);
        //printf("%d\n", player->spr.y);
        //switch maps
        //loadMapFile(player->extraData, tilemap, eventmap, player->mapScreen, 15, 20);
        for(int i = 0; i < 120; i++)
        {
            SDL_SetRenderDrawColor(mainRenderer, (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), 0xFF);
            SDL_RenderClear(mainRenderer);
            SDL_RenderPresent(mainRenderer);
            SDL_Delay(4);
        }
        free(data);
        return true;
    }
    if (scriptData->action == script_gain_exp)
        player->experience += strtol(scriptData->data, NULL, 10);
    scriptData->active = false;
    return false;  //returns whether or not it wants to exit the loop
}