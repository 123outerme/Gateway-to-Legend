#include "outermeSDL.h"  //uses outermeSDL v1.2 as of right now. Future versions of the header may not be compatible
#include "SDLGateway.h"  //for directory searching

#define PIXELS_MOVED TILE_SIZE

#define checkSKUp keyStates[SC_UP]
#define checkSKDown keyStates[SC_DOWN]
#define checkSKLeft keyStates[SC_LEFT]
#define checkSKRight keyStates[SC_RIGHT]
#define checkSKInteract keyStates[SC_INTERACT]
#define checkSKMenu keyStates[SC_MENU]
#define checkSKAttack keyStates[SC_SPECIAL]
//SDL_SCANCODE_W
//SDL_SCANCODE_S
//SDL_SCANCODE_A
//SDL_SCANCODE_D
//SDL_SCANCODE_SPACE
//SDL_SCANCODE_ESCAPE

#define CACHE_NAME "assets/GtLToolchainCache.cfg"

#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)
#define WINDOW_NAME "Gateway to Legend Map Creator"
#define MAIN_TILESET "tileset/mainTileset48.png"

#define MAIN_ARROW_ID 34
//^map creator defines. v map-pack wizard defines

#define MAX_SPRITE_MAPPINGS 20
#define PICK_MESSAGES_ARRAY {"Pick the main character idle.", "Pick the main character walking.", "Pick the cursor.", "Pick the HP icon.", "Pick the player sword.", "Pick the fully-transparent tile.", "Pick button 1.", "Pick button 2.", "Pick button 3.", "Pick door 1.", "Pick door 2.", "Pick door 3.", "Pick the teleporter.", "Pick the damaging hazard.", "Pick the warp gate.", "Pick the weak enemy.", "Pick the ghost enemy.", "Pick the strong enemy.", "Pick the gold.", "Pick the NPC."}

#define MAIN_HELP_TEXT "Make map-packs using this toolchain! Create maps, scripts, and setup your files and tileset using this. To navigate, use the keys you set up in the main program."
#define SCRIPT_HELP_TEXT "Use your movement keys to maneuver between maps and to the tile you want. Press Confirm to \"drop the anchor\" there. Set the width and height next. Toggle interval between 1/8 tile and a full tile using Attack."
#define MAPPACK_SETUP_HELP_TEXT "Change which files you use, assign tiles in your tileset to display certain objects, and change your New Game spawn here. Maneuver using movement keys and follow the instructions."

void initConfig();
void loadConfig(char* filePath);

#define MAX_LIST_OF_MAPS 30
#define MAX_CHAR_IN_FILEPATH MAX_FILE_PATH
#define MAP_PACKS_SUBFOLDER "map-packs/"
#define MAX_MAPPACKS_PER_PAGE 11
#define MAX_ENEMIES 6
int toolchain_main();
int subMain(mapPack* workingPack);

void editFilePaths(mapPack* workingPack);
void editInitSpawn(mapPack* workingPack);
void editTileEquates(mapPack* workingPack);

void createMapPack(mapPack* newPack);
void loadMapPackData(mapPack* loadPack, char* location);
void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag);
void locationSelectLoop(mapPack workingPack, int* map, int* x, int* y);
//^ working with loading a map-pack to work on

int mainMapCreator();
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);
script* mainMapCreatorLoop(player* playerSprite, int* scriptCount, mapPack workingPack);
void viewMap(mapPack workingPack, int thisLineNum, bool drawLineNum, bool update);
int chooseMap(mapPack workingPack);
void drawMaps(mapPack workingPack, int thisTilemap[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, bool hideCollision, bool isEvent, bool updateScreen);
void writeTileData(mapPack workingPack, int line);
//^map creator functions.

//V script editor functions
void mainScriptEdtior(mapPack* workingPack);
int scriptSelectLoop(mapPack workingPack);
script mainScriptLoop(mapPack workingPack, script* editScript);
script visualLoadScript(mapPack* workingPack);
void initScript(script* scriptPtr, scriptBehavior action, int mapNum, int x, int y, int w, int h, char* data);
void writeScriptData(mapPack workingPack, script* mapScripts, int count);

//V map-pack wizard functions
int mainMapPackWizard();
void mainMapPackWizardLoop(mapPack workingPack, sprite* playerSprite, int* numArray);

void strPrepend(char* input, const char* prepend);

//this is to match the tilemap array in outermeSDL.h
int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* mainTilesetTexture;

int toolchain_main()
{
    SDL_SetWindowTitle(mainWindow, "Gateway to Legend Map Tools");
    mapPack workingPack;
    strcpy(workingPack.mainFilePath, "/0");
    loadIMG(MAIN_TILESET, &mainTilesetTexture);

    if (checkFile(CONFIG_FILEPATH, 0))
        loadConfig(CONFIG_FILEPATH);
    else
        initConfig(CONFIG_FILEPATH);

    bool quit = false, proceed = false;
    char* resumeStr = "\0";
    while(!quit)
    {
        readLine(CACHE_NAME, 0, &resumeStr);
        resumeStr = removeChar(resumeStr, '\n', MAX_FILE_PATH, false);
        if (checkFile(resumeStr, 0))
            resumeStr += 10;  //pointer arithmetic to get rid of the "map-packs/" part of the string (use 9 instead to include the /)
        else
            resumeStr = "(No Resume)\0";
        int code = aMenu(tilesetTexture, MAIN_ARROW_ID, "Gateway to Legend Toolchain", (char*[5]) {"New Map-Pack", "Load Map-Pack", resumeStr, "Info/Help", "Back"}, 5, 1, AMENU_MAIN_THEME, true, false, NULL);
        if (code == 1)
        {
            createMapPack(&workingPack);
            proceed = true;
        }

        if (code == 2)
        {
            char mainFilePath[MAX_FILE_PATH];
            char** listOfFilenames;
            int maxStrNum = 0;
            bool back = false;
            listOfFilenames = getListOfFiles(MAX_LIST_OF_MAPS, MAX_CHAR_IN_FILEPATH - 9, MAP_PACKS_SUBFOLDER, &maxStrNum);
            mapSelectLoop(listOfFilenames, (char*) mainFilePath, maxStrNum, &back);
            if (!back)
            {
                loadMapPackData(&workingPack, mainFilePath);
                createFile(CACHE_NAME);
                appendLine(CACHE_NAME, (char*) mainFilePath);

            }
			proceed = !back;
		}

        if (code == 3 && strcmp(resumeStr, "(No Resume)\0") != 0)
        {
            char mainFilePath[MAX_FILE_PATH];
            uniqueReadLine((char**) &mainFilePath, MAX_FILE_PATH, CACHE_NAME, 0);
            loadMapPackData(&workingPack, (char*) mainFilePath);
            proceed = true;
        }

        if (code == 4)
        {
            int key = 0;
            while(!key)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                SDL_RenderFillRect(mainRenderer, NULL);
                drawText(MAIN_HELP_TEXT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                key = getKey();
            }
        }

        if (code == 5 || code == -1)
            quit = true;

        if (proceed && code < 4 && workingPack.mainFilePath[0] != '/')
            subMain(&workingPack);
    }
    SDL_DestroyTexture(mainTilesetTexture);
	SDL_SetWindowTitle(mainWindow, "Gateway to Legend");
    return 0;
}

void createMapPack(mapPack* newPack)
{
    char* getString = calloc(sizeof(char), MAX_FILE_PATH);
    char* message;
    char mapPackData[MAX_MAP_PACK_DATA][MAX_FILE_PATH];
    int wizardState = 0;
    bool quit = false;
	while (!quit)
    {
        switch(wizardState)
        {
        case 0:
            message = "File name? map-packs/";
            break;
        case 1:
            message = "Title of map pack? ";
            break;
        case 2:
            message = "Path for maps file? maps/";
            break;
        case 3:
            message = "Path for tileset file? tileset/";
            break;
        case 4:
            message = "Path for savefile? saves/";
            break;
        case 5:
            message = "Path for scripts? scripts/";
            break;
        case 6:
            message = "Initial X spawn-coordinate? ";
            break;
        case 7:
            message = "Initial Y spawn-coordinate? ";
            break;
        case 8:
            message = "Initial map number? ";
            break;
        }
        stringInput(&getString, message, MAX_FILE_PATH, "default.txt", false);
        switch(wizardState)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            strncpy(mapPackData[wizardState], getString, MAX_FILE_PATH);

            if (wizardState == 0)
                strPrepend((char*) mapPackData[0], "map-packs/");

            if (wizardState == 2)
                strPrepend((char*) mapPackData[2], "maps/");

            if (wizardState == 3)
                strPrepend((char*) mapPackData[3], "tileset/");

            //printf("%s\n", mapPackData[wizardState - 1]);

            if (wizardState == 4)
                strPrepend((char*) mapPackData[4], "saves/");

            if (wizardState == 5)
                strPrepend((char*) mapPackData[5], "scripts/");

            wizardState++;
            break;
        case 6:
            sscanf(getString, "%d", &(newPack->initX));
            wizardState++;
            break;
        case 7:
            sscanf(getString, "%d", &(newPack->initY));
            wizardState++;
            break;
        case 8:
            sscanf(getString, "%d", &(newPack->initMap));
            wizardState++;
            quit = true;
            break;
        }
    }
    strcpy(newPack->mainFilePath, mapPackData[0]);
    strcpy(newPack->name, mapPackData[1]);
    strcpy(newPack->mapFilePath, mapPackData[2]);
    strcpy(newPack->tilesetFilePath, mapPackData[3]);
    strcpy(newPack->saveFilePath, mapPackData[4]);
    strcpy(newPack->scriptFilePath, mapPackData[5]);
    createFile(newPack->mainFilePath);

    for(int i = 1; i < 6; i++)
        appendLine(newPack->mainFilePath, mapPackData[i]);

    appendLine(newPack->mainFilePath, intToString(newPack->initX, getString));
    appendLine(newPack->mainFilePath, intToString(newPack->initY, getString));
    appendLine(newPack->mainFilePath, intToString(newPack->initMap, getString));
    sprite chooser;
    initSprite(&chooser, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_player);
    char* temp = "";
    mainMapPackWizardLoop(*newPack, &chooser, newPack->tilesetMaps);

    for(int i = 0; i < MAX_SPRITE_MAPPINGS; i++)
    {
        if (newPack->tilesetMaps[0] == -1)
            newPack->tilesetMaps[i] = 0;
        appendLine(newPack->mainFilePath, intToString(newPack->tilesetMaps[i], temp));
    }

    getString = freeThisMem((void*) getString);
    loadMapPackData(newPack, newPack->mainFilePath);

    createFile(CACHE_NAME);
    appendLine(CACHE_NAME, newPack->mainFilePath);
}

void locationSelectLoop(mapPack workingPack, int* map, int* x, int* y)
{
    *map = 0, *x = 0, *y = 0;
    SDL_Keycode key = 0;
    *map = chooseMap(workingPack);
    bool inQuit = false;
    while(!inQuit)
    {
        SDL_RenderClear(mainRenderer);
        viewMap(workingPack, *map, false, false);
        drawText("Choose x/y coord.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, false);
        key = getKey();
        if (SC_UP == SDL_GetScancodeFromKey(key) && *y > 0)
            *y -= TILE_SIZE;
        if (SC_DOWN == SDL_GetScancodeFromKey(key) && *y < SCREEN_HEIGHT)
            *y += TILE_SIZE;
        if (SC_LEFT == SDL_GetScancodeFromKey(key) && *x > 0)
            *x -= TILE_SIZE;
        if (SC_RIGHT == SDL_GetScancodeFromKey(key) && *x < SCREEN_WIDTH)
            *x += TILE_SIZE;
        if (SC_INTERACT == SDL_GetScancodeFromKey(key) || key == ANYWHERE_QUIT)
            inQuit = true;
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = *x, .y = *y, .w = TILE_SIZE, .h = TILE_SIZE}));
        SDL_RenderPresent(mainRenderer);
    }
}

int subMain(mapPack* workingPack)
{
    bool quit = false;
    loadIMG(workingPack->tilesetFilePath, &(workingPack->mapPackTexture));  //for some reason we need to load twice??
    while(!quit)
    {
        int code = aMenu(workingPack->mapPackTexture, workingPack->tilesetMaps[2], "Map-Pack Tools", (char*[4]) {"Map Creator", "Script Editor", "Map-Pack Wizard", "Back"}, 4, 1, AMENU_MAIN_THEME, true, false, NULL);
        if (code == 1)
            mainMapCreator(workingPack);
        if (code == 2)
            mainScriptEdtior(workingPack);
        if (code == 3)
            mainMapPackWizard(workingPack);
        if (code == 4 || code == -1)
            quit = true;
    }
    SDL_DestroyTexture(workingPack->mapPackTexture);
    return 0;
}

int mainMapCreator(mapPack* workingPack)
{
    for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
        for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
            eventmap[dy][dx] = 0;
    char* mainFilePath = calloc(MAX_FILE_PATH, sizeof(char));
    char mapFilePath[MAX_FILE_PATH];
    char tileFilePath[MAX_FILE_PATH];

    int choice = aMenu(tilesetTexture, MAIN_ARROW_ID, "New or Load Map?", (char*[3]) {"New", "Load", "Back"}, 3, 0, AMENU_MAIN_THEME, true, false, NULL);
    if (choice != 3)
    {
        if (choice == 2)
        {
            strncpy(mainFilePath, workingPack->mainFilePath, MAX_FILE_PATH);
            if (!checkFile(mainFilePath, 0))
            {
                printf("Invalid main file.\n");
                return 1;
            }
            uniqueReadLine((char**) &mapFilePath, MAX_FILE_PATH, mainFilePath, 1);
            uniqueReadLine((char**) &tileFilePath, MAX_FILE_PATH, mainFilePath, 2);
        }
        if (choice == 1)
        {
            strcpy(mainFilePath, "map-packs/a.txt");
            uniqueReadLine((char**) &mapFilePath, MAX_FILE_PATH, mainFilePath, 1);
            uniqueReadLine((char**) &tileFilePath, MAX_FILE_PATH, mainFilePath, 2);
            for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
            {
                for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
                {
                    tilemap[dy][dx] = 0;
                    eventmap[dy][dx] = 0;
                }
            }
        }
        player creator;
        initPlayer(&creator, 0, 0, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, 0);
        if (choice == 1)
        {
            creator.mapScreen = chooseMap(*workingPack);
        }
        if (choice == 2)
        {
            creator.mapScreen = chooseMap(*workingPack);
            loadMapFile(workingPack->mapFilePath, tilemap, eventmap, creator.mapScreen, HEIGHT_IN_TILES, WIDTH_IN_TILES);
        }
        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        int scriptCount = 0;
        script* mapScripts = mainMapCreatorLoop(&creator, &scriptCount, *workingPack);

        int newChoice = aMenu(tilesetTexture, MAIN_ARROW_ID, "Save Map?", (char*[2]) {"Save", "Discard"}, 2, 0, AMENU_MAIN_THEME, true, false, NULL);



        if (newChoice == 1)
        {
            writeTileData(*workingPack, -1);
            writeScriptData(*workingPack, mapScripts, scriptCount);
            SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
            SDL_RenderClear(mainRenderer);
            char* exitNote = calloc(138, sizeof(char));
            strcpy(exitNote, "Outputted to output/map.txt");
            if (scriptCount > 0)
                strcat(exitNote, " and to your script file.\n\nNOTE: If the second argument of a script is -1, change to (line number of new map) - 1");
            drawText(exitNote, TILE_SIZE, TILE_SIZE, SCREEN_WIDTH - TILE_SIZE, SCREEN_HEIGHT - TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
            waitForKey();
            free(exitNote);
        }
        free(mapScripts);
    }
    return 0;
}
//C:/Stephen/C/CodeBlocks/Gateway-to-Legend/Map-Creator/map-packs/a.txt

void viewMap(mapPack workingPack, int thisLineNum, bool drawLineNum, bool update)
{
    SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(mainRenderer);
    int newTilemap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
    int newEventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
    char* buffer = "";
    loadMapFile(workingPack.mapFilePath, newTilemap, newEventmap, thisLineNum, HEIGHT_IN_TILES, WIDTH_IN_TILES);
    drawMaps(workingPack, newTilemap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, true, false, false);
    drawMaps(workingPack, newEventmap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, true, true, update ? !drawLineNum : false);
    if (drawLineNum)
        drawText(intToString(thisLineNum, buffer), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, update);
}

int chooseMap(mapPack workingPack)
{
    bool quit = false;
    int mapNum = 0, maxMapNum = checkFile(workingPack.mapFilePath, -1);
    SDL_Keycode keycode;
    SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
    while(!quit)
    {
        viewMap(workingPack, mapNum, true, true);
        keycode = getKey();
        mapNum += (keycode == SDLK_d && mapNum < maxMapNum) - (keycode == SDLK_a && mapNum > 0) + 10 * (keycode == SDLK_s && mapNum + 9 < maxMapNum) - 10 * (keycode == SDLK_w && mapNum > 9);
        if (keycode == SDLK_RETURN || keycode == SDLK_ESCAPE || keycode == SDLK_SPACE || keycode == -1)
            quit = true;
    }
    return mapNum;
}

void chooseCoords(mapPack workingPack, int mapNum, int* xPtr, int* yPtr)
{
    bool quit = false;
    SDL_Keycode keycode;
    int x = 0, y = 0;
    while(!quit)
    {
        viewMap(workingPack, mapNum, true, false);
        keycode = getKey();
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = x, .y = y, .w = TILE_SIZE, .h = TILE_SIZE}));
        x += TILE_SIZE * ((SC_RIGHT == SDL_GetScancodeFromKey(keycode) && x < SCREEN_WIDTH - TILE_SIZE) - (SC_LEFT == SDL_GetScancodeFromKey(keycode) && x > 0));
        y += TILE_SIZE * ((SC_DOWN == SDL_GetScancodeFromKey(keycode) && y < SCREEN_HEIGHT - TILE_SIZE) - (SC_UP == SDL_GetScancodeFromKey(keycode) && y > 0));
        if (SC_INTERACT == SDL_GetScancodeFromKey(keycode) || SDL_SCANCODE_RETURN == SDL_GetScancodeFromKey(keycode))
            quit = true;
        SDL_RenderPresent(mainRenderer);
    }
    *xPtr = x;
    *yPtr = y;
}

script* mainMapCreatorLoop(player* playerSprite, int* scriptCount, mapPack workingPack)
{
    /*for(int i = 0; i < 4; i++)
        viewMap("maps/MainMaps.txt", i);*/
    *scriptCount = 0;
    int scriptMaxCount = 5;
    script* mapScripts = calloc(scriptMaxCount, sizeof(script));
    bool quit = false, editingTiles = true;
    int frame = 0, sleepFor = 0, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = SDL_GetTicks(), lastTile = -1;
    int enemyCount = 0;
    for(int y = 0; y < HEIGHT_IN_TILES; y++)
    {
        for(int x = 0; x < WIDTH_IN_TILES; x++)
        {
            if (eventmap[y][x] > 10 && eventmap[y][x] < 14 && enemyCount < MAX_ENEMIES)
                enemyCount++;
            if (eventmap[y][x] > 10 && eventmap[y][x] < 14 && enemyCount > MAX_ENEMIES)
                eventmap[y][x] = 0;
        }
    }
    SDL_Delay(500);  //gives time for keypresses to unregister
    SDL_Event e;
    while(!quit)
    {
        SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(mainRenderer);
        drawMaps(workingPack, tilemap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, true, false, false);
        if (!editingTiles)
        {
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0x58);
            SDL_RenderFillRect(mainRenderer, NULL);
            drawMaps(workingPack, eventmap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, false, true, false);
            drawATile(playerSprite->spr.tileIndex < 2 ? mainTilesetTexture : workingPack.mapPackTexture, playerSprite->spr.tileIndex < 2 ? 127 - playerSprite->spr.tileIndex : workingPack.tilesetMaps[playerSprite->spr.tileIndex + 4], playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE, 0, playerSprite->spr.flip);
        }
        else
        {
            drawATile(workingPack.mapPackTexture, playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE, 0, playerSprite->spr.flip);
            drawMaps(workingPack, eventmap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, true, true, false);
        }
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}));
        SDL_RenderPresent(mainRenderer);
        while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            /*if (e.key.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_g && debug)
                doDebugDraw = !doDebugDraw;*/
        }
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        //getKey(false);  //editor freezes without this //not anymore sucka
        if ((int) (SDL_GetTicks() - lastKeypressTime) >= 80 + 48 * (keyStates[SDL_SCANCODE_LSHIFT] || keyStates[SDL_SCANCODE_Q] || keyStates[SDL_SCANCODE_E]))
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

            if (keyStates[SDL_SCANCODE_E] && playerSprite->spr.tileIndex < 127 - (127 - MAX_SPRITE_MAPPINGS + 3) * (!editingTiles))  //+3 to avoid the first few sprite mappings
                playerSprite->spr.tileIndex++;

            if (keyStates[SDL_SCANCODE_SPACE] && editingTiles)
                tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;

            if (keyStates[SDL_SCANCODE_SPACE] && !editingTiles)
            {

                if (playerSprite->spr.tileIndex > 10 && playerSprite->spr.tileIndex < 14)  //enemies
                {
                    int curTile = eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE];
                    if ((playerSprite->spr.tileIndex > 10 && playerSprite->spr.tileIndex < 14) && !(curTile > 10 && curTile < 14) && enemyCount < MAX_ENEMIES)
                    {
                        enemyCount++;
                        eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
                        //printf("%d (+)\n", enemyCount);
                    }

                    if (curTile > 10 && curTile < 14)
                    {
                        if (!(playerSprite->spr.tileIndex > 10 && playerSprite->spr.tileIndex < 14))
                            enemyCount--;
                        eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
                        //printf("%d (-)\n", enemyCount);
                    }
                }
                else
                    eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;

            if (playerSprite->spr.tileIndex == 10)  //warp gate
                {
                    script gateScript;
                    int map = 0, x = 0, y = 0;
                    locationSelectLoop(workingPack, &map, &x, &y);
                    char* data = calloc(99, sizeof(char));
                    snprintf(data, 99, "[%d/%d/%d]", map, x, y);
                    initScript(&gateScript, script_use_gateway, playerSprite->mapScreen, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE, data);
                    mapScripts[(*scriptCount)++] = gateScript;
                    if (*scriptCount >= scriptMaxCount)
                    {
                        scriptMaxCount += 5;
                        script* temp = realloc(mapScripts, scriptMaxCount);
                        if (temp)
                            mapScripts = temp;
                    }
                    free(data);
                    SDL_Delay(500);  //gives time for keypresses to unregister
                }
                if (playerSprite->spr.tileIndex == 8)  //teleporter
                {
                    script teleportScript;
                    int x = 0, y = 0;
                    SDL_Keycode key = 0;
                    bool inQuit = false;
                    while(!inQuit)
                    {
                        SDL_RenderClear(mainRenderer);
                        drawMaps(workingPack, tilemap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, true, false, false);
                        drawMaps(workingPack, eventmap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, true, true, false);
                        drawText("Choose x/y coord to place player in.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, false);
                        key = getKey();
                        if (SC_UP == SDL_GetScancodeFromKey(key) && y > 0)
                            y -= TILE_SIZE;
                        if (SC_DOWN == SDL_GetScancodeFromKey(key) && y < SCREEN_HEIGHT)
                            y += TILE_SIZE;
                        if (SC_LEFT == SDL_GetScancodeFromKey(key) && x > 0)
                            x -= TILE_SIZE;
                        if (SC_RIGHT == SDL_GetScancodeFromKey(key) && x < SCREEN_WIDTH)
                            x += TILE_SIZE;
                        if (SC_INTERACT == SDL_GetScancodeFromKey(key) || key == ANYWHERE_QUIT)
                            inQuit = true;
                        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = x, .y = y, .w = TILE_SIZE, .h = TILE_SIZE}));
                        SDL_RenderPresent(mainRenderer);
                    }
                    char* data = calloc(99, sizeof(char));
                    snprintf(data, 99, "[%d/%d]", x, y);
                    initScript(&teleportScript, script_use_teleporter, playerSprite->mapScreen, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE, data);
                    mapScripts[(*scriptCount)++] = teleportScript;
                    if (*scriptCount >= scriptMaxCount)
                    {
                        scriptMaxCount += 5;
                        script* temp = realloc(mapScripts, scriptMaxCount);
                        if (temp)
                            mapScripts = temp;
                    }
                    //printf("%s\n", data);
                    //printf("%s\n", mapScripts[*scriptCount - 1].data);
                    free(data);
                    SDL_Delay(500);  //gives time for keypresses to unregister
                }
            }

            if (keyStates[SDL_SCANCODE_LSHIFT])
            {
                editingTiles = !editingTiles;
                int temp = lastTile;
                lastTile = playerSprite->spr.tileIndex;

                if (!editingTiles && temp == -1)
                    playerSprite->spr.tileIndex = 1;
                else
                    playerSprite->spr.tileIndex = temp;
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
    return mapScripts;
}

void drawMaps(mapPack workingPack, int thisTilemap[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, bool hideCollision, bool isEvent, bool updateScreen)
{
        int tile = 0;
        for(int dy = startY; dy < endY; dy++)
            for(int dx = startX; dx < endX; dx++)
            {
                if (isEvent)
                {
                    tile = workingPack.tilesetMaps[thisTilemap[dy][dx] + 4];  //add 4 to start at buttons
                    if (thisTilemap[dy][dx] < 2)
                        tile = 127 - (thisTilemap[dy][dx] == 1 && !hideCollision);
                    drawATile(thisTilemap[dy][dx] < 2 ? mainTilesetTexture : workingPack.mapPackTexture, tile, dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
                }
                else
                    drawATile(workingPack.mapPackTexture, thisTilemap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
            }

    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}

void writeTileData(mapPack workingPack, int line)
{
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

    if (line > 0)  //note: not working  //adds into file
    {
        int lines = checkFile(workingPack.mapFilePath, -1);
        char** entireFile = calloc(lines, sizeof(char*));
        printf("%d\n", lines);
        for(int i = 0; i < lines; i++)  //alloc
        {
            entireFile[i] = calloc(1200, sizeof(char));
            uniqueReadLine((char**) &(entireFile[i]), 1200, workingPack.mapFilePath, i);
            printf("-%d - %s\n", i, entireFile[i]);
        }

        strncpy(entireFile[line], output, 1200);

        createFile(workingPack.mapFilePath);
        for(int i = 0; i < lines; i++)
            appendLine(workingPack.mapFilePath, entireFile[i]);

        for(int i = 0; i < lines; i++)  //dealloc
            entireFile[i] = freeThisMem((void*) entireFile[i]);
    }
    else
    {
        char* outputFile = "output/map.txt";
        createFile(outputFile);
        appendLine("output/map.txt", output);
    }
}


void writeScriptData(mapPack workingPack, script* mapScripts, int count)
{
    if (count < 1)
        return;
    char scriptText[600];
    for(int i = 0; i < count; i++)
    {
        snprintf(scriptText, 160, "{%d,%d,%d,%d,%d,%d,%s}", mapScripts[i].action, mapScripts[i].mapNum, mapScripts[i].x, mapScripts[i].y, mapScripts[i].w, mapScripts[i].h, mapScripts[i].data);
        appendLine(workingPack.scriptFilePath, scriptText);
    }
}
//end map creator code.

//start script editor code
void mainScriptEdtior(mapPack* workingPack)
{
    script editScript;
    int scriptNum = scriptSelectLoop(*workingPack);
    if (scriptNum == 0 && checkFile(workingPack->scriptFilePath, -1) > 0)
        editScript = visualLoadScript(workingPack);

    if (scriptNum > 0)
        initScript(&editScript, (scriptBehavior) scriptNum, chooseMap(*workingPack), 0, 0, TILE_SIZE, TILE_SIZE, "");

    if (scriptNum >= 0)
    {
        mainScriptLoop(*workingPack, &editScript);
        if (editScript.action != script_none)
        {
            if ((editScript.action == script_boss_actions && workingPack->numBosses < 10) || editScript.action != script_boss_actions)
            writeScriptData(*workingPack, &editScript, 1);

            if (editScript.action == script_boss_actions && workingPack->numBosses < 10)
            {
                workingPack->numBosses++;
                saveMapPack(workingPack);
            }

            SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
            SDL_RenderClear(mainRenderer);
            drawText("Appended to your script file.\n\nNOTE: If the second argument of a script is -1, change to (line number of new map) - 1", TILE_SIZE, TILE_SIZE, SCREEN_WIDTH - TILE_SIZE, SCREEN_HEIGHT - TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
            waitForKey();
        }
    }
}

int scriptSelectLoop(mapPack workingPack)
{
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, workingPack.tilesetMaps[2], 0, SDL_FLIP_NONE, (entityType) type_na);
    const int optionsSize = 13;
    char* optionsArray[] = {"Load", "TriggerDialogue", "TriggerDialOnce", "TriggerBoss", "SwitchMaps", "Gateway", "Teleporter", "ToggleDoor", "Animation", "BossActions", "GainMoney", "HurtPlayer", "placeholder"};
    int scriptType = 0, selection = -1;
    SDL_Color textColor = (SDL_Color) {AMENU_MAIN_TEXTCOLOR};
    SDL_Color bgColor = (SDL_Color) {AMENU_MAIN_BGCOLOR};
    SDL_Event e;
    bool quit = false;
    while(!quit)
    {
        SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);

        SDL_RenderClear(mainRenderer);
        SDL_RenderFillRect(mainRenderer, NULL);
        SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        drawText("Script Type?", 1 * TILE_SIZE + 3 * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, (SDL_Color) {AMENU_MAIN_TITLECOLOR2}, false);
        //foreground text
        drawText("Script Type?", 1.25 * TILE_SIZE , 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, (SDL_Color) {AMENU_MAIN_TITLECOLOR1}, false);

        drawText(optionsArray[scriptType], 2.25 * TILE_SIZE, 5 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 5) * TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);

        drawText("Start", 2.25 * TILE_SIZE, 6 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 6) * TILE_SIZE, textColor, false);
        drawText("Info/Help", 2.25 * TILE_SIZE, 7 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 7) * TILE_SIZE, textColor, false);
        drawText("Back", 2.25 * TILE_SIZE, 8 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 8) * TILE_SIZE, textColor, false);

        //SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.w}));
        //Handle events on queue
        while(SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if(e.type == SDL_QUIT)
            {
                quit = true;
                scriptType = ANYWHERE_QUIT;
            }
            //User presses a key
            else if(e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_UP))
                {
                    if (cursor.y > 5 * TILE_SIZE)
                        cursor.y -= TILE_SIZE;
                    Mix_PlayChannel(-1, PING_SOUND, 0);
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_DOWN))
                {
                    if (cursor.y < 8 * TILE_SIZE)
                        cursor.y += TILE_SIZE;
                    Mix_PlayChannel(-1, PING_SOUND, 0);
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_LEFT) && cursor.y == 5 * TILE_SIZE)
                {
                    if (scriptType > 0)
                    {
                        if (scriptType == 7)
                            scriptType -= 3;
                        else
                            scriptType--;
                    }
                    Mix_PlayChannel(-1, PING_SOUND, 0);
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_RIGHT) && cursor.y == 5 * TILE_SIZE)
                {
                    if (scriptType < optionsSize - 1)
                    {
                        if (scriptType == 4)
                            scriptType += 3;
                        else
                            scriptType++;
                    }
                    Mix_PlayChannel(-1, PING_SOUND, 0);
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_INTERACT))
                {
                    selection = cursor.y / TILE_SIZE - 4;
                    if (selection == 2 || selection == 4)
                        quit = true;
                    Mix_PlayChannel(-1, OPTION_SOUND, 0);
                    if (selection == 3)
                    {
                        while(!getKey())
                        {
                            SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                            SDL_RenderFillRect(mainRenderer, NULL);
                            drawText(SCRIPT_HELP_TEXT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                        }
                        selection = 0;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                    }
                }
            }
        }
        if (cursor.y / TILE_SIZE - 4 == 1)
            drawATile(workingPack.mapPackTexture, cursor.tileIndex, 18 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        drawATile(workingPack.mapPackTexture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, cursor.y / TILE_SIZE - 4 == 1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
    if (selection == 4)
        scriptType = -1;
    return scriptType;
}

int toolchain_min(int x, int y)
{
    return ((x > y) ? y : x);
}

script mainScriptLoop(mapPack workingPack, script* editScript)
{
    int map = editScript->mapNum, x1 = editScript->x, y1 = editScript->y, x2 = editScript->x + editScript->w, y2 = editScript->y + editScript->h;
    int intervalSize = TILE_SIZE;
    char* data = calloc(99, sizeof(char));
    sprite cursor;
    initSprite(&cursor, x1, y1, x2 - x1, y2 - y1, 0, 0, SDL_FLIP_NONE, type_na);
    bool quit = false, editXY = true, bigIntervalSize = true;
    SDL_Keycode key;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        viewMap(workingPack, map, false, false);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = !editXY ? x1 : cursor.x, .y = !editXY ? y1 : cursor.y, .w = !editXY ? cursor.x - x1 : cursor.w, .h = !editXY ? cursor.y - y1 : cursor.h}));
        key = getKey();
        if (SC_SPECIAL == SDL_GetScancodeFromKey(key) && bigIntervalSize == false)
        {
            intervalSize = 48;
            bigIntervalSize = true;
        }
        else
        if (bigIntervalSize == true && SC_SPECIAL == SDL_GetScancodeFromKey(key))
        {
            intervalSize = 6;
            bigIntervalSize = false;
        }

        if (SC_UP == SDL_GetScancodeFromKey(key) && cursor.y > 0)
            cursor.y -= intervalSize;
        if (SC_DOWN == SDL_GetScancodeFromKey(key) && cursor.y < SCREEN_HEIGHT)
            cursor.y += intervalSize;
        if (SC_LEFT == SDL_GetScancodeFromKey(key) && cursor.x > 0)
            cursor.x -= intervalSize;
        if (SC_RIGHT == SDL_GetScancodeFromKey(key) && cursor.x < SCREEN_WIDTH)
            cursor.x += intervalSize;
        if (SC_INTERACT == SDL_GetScancodeFromKey(key))
        {
            if (editXY)
            {
                x1 = cursor.x;
                y1 = cursor.y;
                cursor.x = x1 + cursor.w;
                cursor.y = y1 + cursor.h;
                bigIntervalSize = false;
                intervalSize = 6;
                editXY = false;
            }
            else
            {
                x2 = cursor.x;
                y2 = cursor.y;
                quit = true;
            }
        }
        if (key == ANYWHERE_QUIT || key == SDL_GetKeyFromScancode(SC_MENU))
            quit = true;
        SDL_RenderPresent(mainRenderer);
    }
    if (!(key == ANYWHERE_QUIT || key == SDL_GetKeyFromScancode(SC_MENU)))
    {
        if (editScript->action == script_trigger_dialogue || editScript->action == script_trigger_dialogue_once)
        {
            stringInput(&data, "What should be said?", 88, "Hello!", true);
        }

        if (editScript->action == script_trigger_boss)
        {
            //ask user to select a boss
            int maxScriptLines = checkFile(workingPack.scriptFilePath, -1), * bossLineArray = calloc(12, sizeof(int));
            int bossIndex = 0, bossArraySize = 12;
            char* temp = "";
            for(int i = 0; i < maxScriptLines; i++)
            {
                script aScript;
                readScript(&aScript, readLine(workingPack.scriptFilePath, i, &temp));
                if (aScript.action == script_boss_actions && bossIndex < bossArraySize - 1)
                    bossLineArray[bossIndex++] = i;
            }
            bossArraySize = bossIndex;
            bool quit = false;
            int foundIndex = 0;
            script loadedScript;
            SDL_Keycode key;
            while(!quit)
            {
                SDL_RenderClear(mainRenderer);
                viewMap(workingPack, loadedScript.mapNum, true, false);
                key = getKey();
                if (key == SDL_GetKeyFromScancode(SC_UP))
                {
                    if (foundIndex > 9)
                        foundIndex -= 10;
                    readScript(&loadedScript, readLine(workingPack.scriptFilePath, bossLineArray[foundIndex], &temp));
                }

                if (key == SDL_GetKeyFromScancode(SC_DOWN))
                {
                    if (foundIndex < bossArraySize && bossArraySize >= 10 + foundIndex)
                        foundIndex += 10;
                    readScript(&loadedScript, readLine(workingPack.scriptFilePath, bossLineArray[foundIndex], &temp));
                }

                if (key == SDL_GetKeyFromScancode(SC_LEFT))
                {
                    if (foundIndex > 0)
                        foundIndex--;
                    readScript(&loadedScript, readLine(workingPack.scriptFilePath, bossLineArray[foundIndex], &temp));
                }

                if (key == SDL_GetKeyFromScancode(SC_RIGHT))
                {
                    if (foundIndex < bossArraySize)
                        foundIndex++;
                    readScript(&loadedScript, readLine(workingPack.scriptFilePath, bossLineArray[foundIndex], &temp));
                }
                if (key == SDL_GetKeyFromScancode(SC_INTERACT) || key == SDLK_RETURN || key == ANYWHERE_QUIT)
                    quit = true;

                SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = loadedScript.x, .y = loadedScript.y, .w = loadedScript.w, .h = loadedScript.h}));
                SDL_RenderPresent(mainRenderer);
            }
            snprintf(data, 3, "%d", bossLineArray[foundIndex]);
        }

        if (editScript->action == script_switch_maps)
        {
            int map = 0, x = 0, y = 0;
            locationSelectLoop(workingPack, &map, &x, &y);
            snprintf(data, 12, "[%d/%d/%d]", map, x, y);
        }

        if (editScript->action == script_toggle_door)
        {
            int newDoors[3] = {0, 0, 0};
            char* doorGraphics[4] = {"Flip State","Leave Alone","Open","Closed"};
            sprite cursor;
            initSprite(&cursor, TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, workingPack.tilesetMaps[2], 0, SDL_FLIP_NONE, (entityType) type_na);
            SDL_Color textColor = {AMENU_MAIN_TEXTCOLOR};
            SDL_Color bgColor = {AMENU_MAIN_BGCOLOR};
            SDL_Color titleColorUnder = {AMENU_MAIN_TITLECOLOR2};
            SDL_Color titleColorOver = {AMENU_MAIN_TITLECOLOR1};
            SDL_Event e;
            bool quit = false;
            //While application is running
            while(!quit)
            {
                SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);

                SDL_RenderClear(mainRenderer);
                SDL_RenderFillRect(mainRenderer, NULL);
                SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
                SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
                //background text (drawn first)
                drawText("Set Doors", TILE_SIZE + 3 * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, titleColorUnder, false);
                //foreground text
                drawText("Set Doors", 1.25 * TILE_SIZE, 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, titleColorOver, false);

                drawText(doorGraphics[newDoors[0] + 2], 4.25 * TILE_SIZE, 5 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, textColor, false);
                drawText(doorGraphics[newDoors[1] + 2], 4.25 * TILE_SIZE, 6 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, textColor, false);
                drawText(doorGraphics[newDoors[2] + 2], 4.25 * TILE_SIZE, 7 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, textColor, false);

                drawATile(workingPack.mapPackTexture, workingPack.tilesetMaps[9], 2.25 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
                drawATile(workingPack.mapPackTexture, workingPack.tilesetMaps[10], 2.25 * TILE_SIZE, 6 * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
                drawATile(workingPack.mapPackTexture, workingPack.tilesetMaps[11], 2.25 * TILE_SIZE, 7 * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);

                if (cursor.y < TILE_SIZE * 8)
                    drawATile(workingPack.mapPackTexture, cursor.tileIndex, 15 * TILE_SIZE, cursor.y, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);

                drawText("Done", 2.25 * TILE_SIZE, 8 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, textColor, false);

                while(SDL_PollEvent(&e) != 0)
                {
                    //User requests quit
                    if(e.type == SDL_QUIT)
                    {
                        quit = true;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                    }
                    //User presses a key
                    else if(e.type == SDL_KEYDOWN)
                    {
                        if (e.key.keysym.scancode == SC_UP && cursor.y > 5 * TILE_SIZE)
                        {
                            cursor.y -= TILE_SIZE;
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }

                        if (e.key.keysym.scancode == SC_DOWN && cursor.y < 8 * TILE_SIZE)
                        {
                            cursor.y += TILE_SIZE;
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }

                        if (e.key.keysym.scancode == SC_LEFT && newDoors[cursor.y / TILE_SIZE - 5] > -2)
                        {
                            newDoors[cursor.y / TILE_SIZE - 5]--;
                        }

                        if (e.key.keysym.scancode == SC_RIGHT && newDoors[cursor.y / TILE_SIZE - 5] < 1)
                        {
                            newDoors[cursor.y / TILE_SIZE - 5]++;
                        }

                        if (e.key.keysym.scancode == SC_INTERACT && cursor.y == TILE_SIZE * 8)
                        {
                            quit = true;
                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        }
                    }
                }
                drawATile(workingPack.mapPackTexture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, (cursor.y < TILE_SIZE * 8 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
                SDL_RenderPresent(mainRenderer);
            }
            snprintf(data, 9, "%d/%d/%d", newDoors[0], newDoors[1], newDoors[2]);
        }

        if (editScript->action == script_animation)
        {
            //figure this out
        }

        if (editScript->action == script_boss_actions)
        {
			//[starting tile|health](actions...)
			//select starting tile
			int startingTile = 0;
			{
				#undef SCREEN_WIDTH
				#undef SCREEN_HEIGHT
				#define SCREEN_WIDTH TILE_SIZE * 16
				#define SCREEN_HEIGHT TILE_SIZE * 9
				loadTTFont(FONT_FILE_NAME, &mainFont, 24);
				sprite cursor;
				initSprite(&cursor, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_na);
				int frame = 0, sleepFor = 0, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = lastFrame + 1;
				bool quit = false, whiteBG = true;
				SDL_Event e;
				while(!quit)
				{
					if (whiteBG)
						SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					else
						SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0xFF);
					SDL_RenderClear(mainRenderer);
					SDL_RenderCopy(mainRenderer, workingPack.mapPackTexture, NULL, &((SDL_Rect) {.x = 0, .y = TILE_SIZE, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT - TILE_SIZE}));
					SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0x1C, 0xC6, 0xFF);
					SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.h}));
					SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					drawText("Choose the top left boss tile.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF * (!whiteBG), 0xFF * (!whiteBG), 0xFF * (!whiteBG), 0xFF}, true);
					const Uint8* keyStates = SDL_GetKeyboardState(NULL);
					while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
					{
						if (e.type == SDL_QUIT)
						{
							quit = true;
						}
						if (e.type == SDL_KEYDOWN && SDL_GetTicks() - lastKeypressTime >= 48)
						{
							if (cursor.y > TILE_SIZE && checkSKUp)
								cursor.y -= PIXELS_MOVED;
							if (cursor.y < SCREEN_HEIGHT - cursor.h && checkSKDown)
								cursor.y += PIXELS_MOVED;
							if (cursor.x > 0 && checkSKLeft)
								cursor.x -= PIXELS_MOVED;
							if (cursor.x < SCREEN_WIDTH - cursor.w && checkSKRight)
								cursor.x += PIXELS_MOVED;
							if (checkSKInteract)
							{
								startingTile = 8 * (cursor.x / TILE_SIZE) + cursor.y / TILE_SIZE - 1;  //-1 because we don't start at y=0
								quit = true;
							}
							if (keyStates[SDL_SCANCODE_LSHIFT])
								whiteBG = !whiteBG;
							lastKeypressTime = SDL_GetTicks();
						}
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
				#undef SCREEN_WIDTH
				#undef SCREEN_HEIGHT
				#define SCREEN_WIDTH TILE_SIZE * 20
				#define SCREEN_HEIGHT TILE_SIZE * 15
			}
			loadTTFont(FONT_FILE_NAME, &mainFont, 48);
			//get health

			char* dataStr = calloc(10, sizeof(char));
			snprintf(dataStr, 10, "[%d/%d]", startingTile, intInput("How many hits to kill?", 2, 5));
			char* moveStr;
			//visualize boss actions
			quit = false;
			int coords = 0;
			const int maxCoords = 10;
			int xCoords[maxCoords];
			int yCoords[maxCoords];
			int frameCoords[maxCoords];
			for(int i = 0; i < maxCoords; i++)
            {
                xCoords[i] = 0;
                yCoords[i] = 0;
                frameCoords[i] = 0;
            }
			initSprite(&cursor, toolchain_min(x1, x2), toolchain_min(y1, y2), TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_na);
			while(!quit && coords < maxCoords)
			{
				bool select = false;
                SDL_Event e;
                Uint32 lastKeypressTime = SDL_GetTicks(), lastFrame = lastKeypressTime;
                int sleepFor = 0, frame = 0;
				while(!select)
				{
					SDL_RenderClear(mainRenderer);
					viewMap(workingPack, map, false, false);
                    SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
                    drawATile(workingPack.mapPackTexture, startingTile, toolchain_min(x1, x2), toolchain_min(y1, y2), abs(x2 - x1), abs(y2 - y1), 0, SDL_FLIP_NONE);
                    SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = toolchain_min(x1, x2), .y = toolchain_min(y1, y2), .w = abs(x2 - x1), .h = abs(y2 - y1)}));
                    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.h}));
					drawText("Choose the top left coord.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF}, true);
					const Uint8* keyStates = SDL_GetKeyboardState(NULL);
					while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
					{
						if (e.type == SDL_QUIT)
						{
						    select = true;
							quit = true;
						}
						if (e.type == SDL_KEYDOWN && SDL_GetTicks() - lastKeypressTime >= 48)
						{
							if (cursor.y > TILE_SIZE && checkSKUp)
								cursor.y -= PIXELS_MOVED;
							if (cursor.y < SCREEN_HEIGHT - cursor.h && checkSKDown)
								cursor.y += PIXELS_MOVED;
							if (cursor.x > 0 && checkSKLeft)
								cursor.x -= PIXELS_MOVED;
							if (cursor.x < SCREEN_WIDTH - cursor.w && checkSKRight)
								cursor.x += PIXELS_MOVED;
							if (checkSKInteract)
								select = true;
							lastKeypressTime = SDL_GetTicks();

                            if (checkSKMenu || keyStates[SDL_SCANCODE_RETURN])
                            {
                                select = true;
                                quit = true;
                            }
						}
					}

					sleepFor = targetTime - (SDL_GetTicks() - lastFrame);  //FPS limiter; rests for (16 - time spent) ms per frame, effectively making each frame run for ~16 ms, or 60 FPS
					if (sleepFor > 0)
						SDL_Delay(sleepFor);
					lastFrame = SDL_GetTicks();
					frame++;
					//SDL_RenderPresent(mainRenderer);
				}
				if (!quit)
                {
                    xCoords[coords] = cursor.x;
                    yCoords[coords] = cursor.y;
                    frameCoords[coords++] = intInput("Gets there in how many frames?", 3, 15);
                }
				//printf("%s, %s, %d\n(%d, %d) for %d\n", printBool(quit), printBool(select), coords, xCoords[coords - 1], yCoords[coords - 1], frameCoords[coords - 1]);
				//get num of frames, add into array
			}
			//then make it all a string
			moveStr = calloc(coords * 4 + 2, sizeof(char));
			moveStr[0] = '(';
			char* temp = "";
			for(int i = 0; i < coords; i++)
            {
                strcat(moveStr, intToString(xCoords[i], temp));
                strcat(moveStr, "|");
                strcat(moveStr, intToString(yCoords[i], temp));
                strcat(moveStr, "|");
                strcat(moveStr, intToString(frameCoords[i], temp));
                if (i < coords - 1)
                    strcat(moveStr, "|");
                    //printf("%s\n", moveStr);
            }
            strcat(moveStr, ")");
			strcat(dataStr, moveStr);
			free(moveStr);
			strcpy(data, dataStr);
			free(dataStr);
			//printf("%s\n", data);
		}

        if (editScript->action == script_gain_money || editScript->action == script_player_hurt)
        {
            char* message = calloc(17, sizeof(char)), * temp = "";
            snprintf(message, 17, "How much %s?", editScript->action == script_gain_money ? "money" : "damage");
            int intData = intInput(message, 3 - (editScript->action == script_gain_money), 1);
            if (editScript->action == script_gain_money)
            {
                if (intData > 20)
                    intData = 20;

                if (intData < 1)
                    intData = 1;
            }
            else
            {
                if (intData > 4)
                    intData = 4;

                if (intData < -4)
                    intData = -4;
            }
            strcpy(data, intToString(intData, temp));
            free(message);
        }
	}
	if (key == ANYWHERE_QUIT || key == SDL_GetKeyFromScancode(SC_MENU))
		initScript(editScript, script_none, map, toolchain_min(x1, x2), toolchain_min(y1, y2), abs(x2 - x1), abs(y2 - y1), " ");
	else
		initScript(editScript, editScript->action, map, toolchain_min(x1, x2), toolchain_min(y1, y2), abs(x2 - x1), abs(y2 - y1), data);
	free(data);
	return *editScript;
}

script visualLoadScript(mapPack* workingPack)
{
    script loadedScript;
    bool quit = false;
    char* temp = "";
    int scriptLineNum = 0, maxLines = checkFile(workingPack->mapFilePath, -1);
    readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp));
    SDL_Keycode key;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        viewMap(*workingPack, loadedScript.mapNum, true, false);
        key = getKey();
        if (key == SDL_GetKeyFromScancode(SC_UP))
        {
            if (scriptLineNum > 9)
                scriptLineNum -= 10;
            readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp));
        }

        if (key == SDL_GetKeyFromScancode(SC_DOWN))
        {
            if (scriptLineNum < maxLines)
                scriptLineNum += 10;
            readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp));
        }

        if (key == SDL_GetKeyFromScancode(SC_LEFT))
        {
            if (scriptLineNum > 0)
                scriptLineNum--;
            readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp));
        }

        if (key == SDL_GetKeyFromScancode(SC_RIGHT))
        {
            if (scriptLineNum < maxLines)
                scriptLineNum++;
            readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp));
        }
        if (key == SDL_GetKeyFromScancode(SC_INTERACT) || key == SDLK_RETURN || key == ANYWHERE_QUIT)
            quit = true;

        drawText(intToString(scriptLineNum, temp), SCREEN_WIDTH - TILE_SIZE * (!scriptLineNum ? 1 : digits(scriptLineNum)), 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, false);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = loadedScript.x, .y = loadedScript.y, .w = loadedScript.w, .h = loadedScript.h}));
        SDL_RenderPresent(mainRenderer);
    }
    return loadedScript;
}

//end script editor code.


//start map-pack wizard code
int mainMapPackWizard(mapPack* workingPack)
{
    bool quit = false;
    while (!quit)
    {
        int choice = aMenu(workingPack->mapPackTexture, workingPack->tilesetMaps[2], workingPack->mainFilePath + 10, (char*[5]) {"Edit Filepaths", "Edit Init Spawn", "Edit Tile Equates", "Info/Help", "Back"}, 5, 0, AMENU_MAIN_THEME, true, false, NULL);

        if (choice == 1)
            editFilePaths(workingPack);

	if (choice == 2)
            editInitSpawn(workingPack);

        if (choice == 3)
        {
            editTileEquates(workingPack);
        }

        if (choice == 4)
        {
            int key = 0;
            while(!key)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                SDL_RenderFillRect(mainRenderer, NULL);
                drawText(MAPPACK_SETUP_HELP_TEXT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                key = getKey();
            }
        }

        if (choice == 5)
            quit = true;

    }
    return 0;
}

void editFilePaths(mapPack* workingPack)
{
    SDL_RenderClear(mainRenderer);
    bool quit = false;
    while(!quit)
    {
        int choice = aMenu(tilesetTexture, workingPack->tilesetMaps[2], workingPack->mainFilePath + 10, (char*[6]) {"Change Name", "Edit Map Path", "Edit Tileset Path", "Edit Save Path", "Edit Script Path", "Back"}, 6, 0, AMENU_MAIN_THEME, true, false, NULL);
        if (choice < 0 || choice == 6)
            quit = true;
        else
        {
            char* getString = calloc(MAX_FILE_PATH, sizeof(char));
            char* message = calloc(99, sizeof(char));
            getString[0] = '\0';
            switch(choice)
            {
            case 1:
                strcpy(message, "Title of map pack?");
                break;
            case 2:
                strcpy(message, "Path for maps file? maps/");  //we add numbers here to get rid of the "path/" part of the filepath
                break;
            case 3:
                strcpy(message, "Path for tileset file? tileset/");
                break;
            case 4:
                strcpy(message, "Path for savefile? saves/");
                break;
            case 5:
                strcpy(message, "Path for scripts? scripts/");
                break;
            }
            stringInput(&getString, message, 99, "default.txt", false);
            switch(choice)
            {

            case 1:
                strcpy(workingPack->name, getString);
                break;
            case 2:
                strPrepend((char*) getString, "maps/");
                strcpy(workingPack->mapFilePath, getString);
                break;
            case 3:
                strPrepend((char*) getString, "tileset/");
                strcpy(workingPack->tilesetFilePath, getString);
                break;
            case 4:
                strPrepend((char*) getString, "saves/");
                strcpy(workingPack->saveFilePath, getString);
                break;
            case 5:
                strPrepend((char*) getString, "scripts/");
                strcpy(workingPack->scriptFilePath, getString);
                break;
            }
            free(getString);
            free(message);
        }
    }
    saveMapPack(workingPack);
}

void editInitSpawn(mapPack* workingPack)
{
    SDL_RenderClear(mainRenderer);
    workingPack->initMap = chooseMap(*workingPack);
    chooseCoords(*workingPack, workingPack->initMap, &(workingPack->initX), &(workingPack->initY));
    saveMapPack(workingPack);
}

#undef SCREEN_WIDTH
#undef SCREEN_HEIGHT
#define SCREEN_WIDTH TILE_SIZE * 16
#define SCREEN_HEIGHT TILE_SIZE * 9
void editTileEquates(mapPack* workingPack)
{
    loadTTFont(FONT_FILE_NAME, &mainFont, 24);
    SDL_RenderClear(mainRenderer);
    int numbers[MAX_SPRITE_MAPPINGS];
    numbers[0] = -1;
    sprite chooser;
    initSprite(&chooser, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_player);
    mainMapPackWizardLoop(*workingPack, &chooser, (int*) numbers);
    if (!(numbers[0] == -1))
    {
        for(int i = 0; i < MAX_SPRITE_MAPPINGS; i++)
        {
            workingPack->tilesetMaps[i] = numbers[i];
            //printf("%d\n", numbers[i]);
        }
        SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
        SDL_RenderClear(mainRenderer);
        drawText("Outputted to your file.", TILE_SIZE, TILE_SIZE, SCREEN_WIDTH - TILE_SIZE, SCREEN_HEIGHT - TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
        saveMapPack(workingPack);
    }
    loadTTFont(FONT_FILE_NAME, &mainFont, 48);
}

void mainMapPackWizardLoop(mapPack workingPack, sprite* playerSprite, int* numArray)
{
    int numArrayTracker = 0, frame = 0, sleepFor = 0, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = lastFrame + 1;
    char* text[] = PICK_MESSAGES_ARRAY;
    bool quit = false, whiteBG = true;
    SDL_Event e;
    while(numArrayTracker < MAX_SPRITE_MAPPINGS && !quit)
    {
        if (whiteBG)
            SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        else
            SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(mainRenderer);
        SDL_RenderCopy(mainRenderer, workingPack.mapPackTexture, NULL, &((SDL_Rect) {.x = 0, .y = TILE_SIZE, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT - TILE_SIZE}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0x1C, 0xC6, 0xFF);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->x, .y= playerSprite->y, .w = playerSprite->w, .h = playerSprite->h}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        drawText(text[numArrayTracker], 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF * (!whiteBG), 0xFF * (!whiteBG), 0xFF * (!whiteBG), 0xFF}, true);
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN && SDL_GetTicks() - lastKeypressTime >= 48)
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
                if (keyStates[SDL_SCANCODE_LSHIFT])
                    whiteBG = !whiteBG;
                lastKeypressTime = SDL_GetTicks();
            }
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
    if (numArrayTracker < MAX_SPRITE_MAPPINGS)
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

