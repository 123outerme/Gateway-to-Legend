#include "outermeSDL.h"  //uses outermeSDL v1.2 as of right now. Future versions of the header may not be compatible
#include "SDLGateway.h"  //for directory searching

#define PIXELS_MOVED TILE_SIZE

#define checkSKUp keyStates[SC_UP]
#define checkSKDown keyStates[SC_DOWN]
#define checkSKLeft keyStates[SC_LEFT]
#define checkSKRight keyStates[SC_RIGHT]
#define checkSKInteract keyStates[SC_INTERACT]
#define checkSKMenu keyStates[SC_MENU]
#define checkSKSpecial keyStates[SC_SPECIAL]
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

//#define MAX_SPRITE_MAPPINGS 21  //already defined
#define PICK_MESSAGES_ARRAY {"Pick the main character idle.", "Pick the main character walking.", "Pick the cursor.", "Pick the HP icon.", "Pick the player sword.", "Pick the fully-transparent tile.", "Pick button 1.", "Pick button 2.", "Pick button 3.", "Pick door 1.", "Pick door 2.", "Pick door 3.", "Pick the boss door.", "Pick the teleporter.", "Pick the damaging hazard.", "Pick the warp gate.", "Pick the weak enemy.", "Pick the ghost enemy.", "Pick the strong enemy.", "Pick the gold.", "Pick the NPC."}

#define MAIN_HELP_TEXT1 "Make map-packs using this toolchain!\n\nCreate maps, scripts, and setup your files and tileset using this."
#define MAIN_HELP_TEXT2 "To navigate, use the keys you set up in the main program.\n\nEdit maps, scripts, and the map-pack itself."
#define MAIN_HELP_TEXT3 "See the Help option in the other sections for more info."

#define SCRIPT_HELP_TEXT1 "Use your movement keys to maneuver between maps and to the tile you want.\n\nPress Confirm to set the top-left corner position. Set the width and height next."
#define SCRIPT_HELP_TEXT2 "Toggle movement interval between 1/8 tile and a full tile using Special.\n\nThen, you will go into an editing screen, which is different for every script."
#define SCRIPT_HELP_TEXT_LOAD "Choose which script you want to load. Confirm the location, then re-enter the data necessary. See other options for more help."
#define SCRIPT_HELP_TEXT_DIALOGUE "After placing the script, just type in the text you want shown."
#define SCRIPT_HELP_TEXT_TRIGGERBOSS "After placing the script, select the boss script that corresponds.\n\nBoss must be on the same map as the trigger script to work."
#define SCRIPT_HELP_TEXT_MAPTELEPORT "After placing the script, select which map the player goes to. Then, place your cursor where you want the player to land."
#define SCRIPT_HELP_TEXT_TELEPORTER "After placing the script, place your cursor where you want the player to land."
#define SCRIPT_HELP_TEXT_DOORS "After placing the script, select how you want your doors to change."
#define SCRIPT_HELP_TEXT_ANIMATION "After placing the script, select the locations where you want the sprite to move. Then, input the amount of frames they must arrive in. When done, press Menu."
#define SCRIPT_HELP_TEXT_BOSSACTIONS "After placing the script, input the boss data such as health and where the sprite is on the spritesheet."
#define SCRIPT_HELP_TEXT_BOSSACTIONS2 "Then, select the location where you want the sprite to move. Then, input the amount of frames they must arrive in."
#define SCRIPT_HELP_TEXT_NUMINPUT "After placing the script, input the number you want."

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

int editFilePaths(mapPack* workingPack);
int editInitSpawn(mapPack* workingPack);
int editTileEquates(mapPack* workingPack);

void createMapPack(mapPack* newPack);
void loadMapPackData(mapPack* loadPack, char* location);
void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag);
void locationSelectLoop(mapPack workingPack, int* map, int* x, int* y);
//^ working with loading a map-pack to work on

int mainMapCreator(mapPack* workingPack);
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);
script* mainMapCreatorLoop(player* playerSprite, int* scriptCount, mapPack workingPack);
void viewMap(mapPack workingPack, int thisLineNum, bool drawLineNum, bool update);
int chooseMap(mapPack workingPack);
void drawMaps(mapPack workingPack, int thisTilemap[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, bool hideCollision, bool isEvent, bool updateScreen);
void writeTileData(mapPack workingPack, int line);
//^map creator functions.

//V script editor functions
int mainScriptEdtior(mapPack* workingPack);
int scriptSelectLoop(mapPack workingPack);
script mainScriptLoop(mapPack workingPack, script* editScript);
script visualLoadScript(mapPack* workingPack);
void initScript(script* scriptPtr, scriptBehavior action, int mapNum, int x, int y, int w, int h, char* data, int lineNum);
void writeScriptData(mapPack workingPack, script* mapScripts, int count);

//V map-pack wizard functions
int mainMapPackWizard();
void mainMapPackWizardLoop(mapPack workingPack, int* numArray);
int chooseTile(mapPack workingPack, char* prompt);

void strPrepend(char* input, const char* prepend);

//this is to match the tilemap array in outermeSDL.h
int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* mainTilesetTexture;
int _TglobalInt1;  //

int toolchain_main()
{
    SDL_SetWindowTitle(mainWindow, "Gateway to Legend Map Tools");
    mapPack workingPack;
    strcpy(workingPack.mainFilePath, "/0");
    loadIMG(MAIN_TILESET, &mainTilesetTexture);
    bool quit = false, proceed = false;
    int code = 0;
    char* resumeStr = "\0";
    while(!quit)
    {
        readLine(CACHE_NAME, 0, &resumeStr);
        resumeStr = removeChar(resumeStr, '\n', MAX_FILE_PATH, false);
        if (checkFile(resumeStr, 0))
            resumeStr += 10;  //pointer arithmetic to get rid of the "map-packs/" part of the string (use 9 instead to include the /)
        else
            resumeStr = "(No Resume)\0";
        code = aMenu(tilesetTexture, MAIN_ARROW_ID, "Gateway to Legend Toolchain", (char*[5]) {"New Map-Pack", "Load Map-Pack", resumeStr, "Info/Help", "Back"}, 5, 1, AMENU_MAIN_THEME, true, false, NULL);
        if (code == 1)
        {
            createMapPack(&workingPack);
            if (workingPack.initX == -1)
            {
                quit = true;
                code = -1;
            }
            else
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
            if (back == -1)
            {
                back = true;
                quit = true;
                code = -1;
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
            int pauseKey = 0;
            char* helpTexts[3] = {MAIN_HELP_TEXT1, MAIN_HELP_TEXT2, MAIN_HELP_TEXT3};
            int helpIndex = 0;
            while(helpIndex <= 2)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                SDL_RenderFillRect(mainRenderer, NULL);
                drawText(helpTexts[helpIndex], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                pauseKey = waitForKey(true);
                helpIndex++;
                Mix_PlayChannel(-1, OPTION_SOUND, 0);
                if (pauseKey == -1)
                {
                    helpIndex = 3;
                    code = -1;
                }
            }
        }

        if (proceed && code < 4 && workingPack.mainFilePath[0] != '/')
            code = subMain(&workingPack);

        if (code == 5 || code == -1)
            quit = true;
    }
    SDL_DestroyTexture(mainTilesetTexture);
	SDL_SetWindowTitle(mainWindow, "Gateway to Legend");
    return -1 * (code == -1);
}

void createMapPack(mapPack* newPack)
{
    char* getString = calloc(sizeof(char), MAX_FILE_PATH);
    char* message;
    int intData = 0;
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
        if (wizardState < 6)
        {
            stringInput(&getString, message, MAX_FILE_PATH, "default.txt", false);
            if (getString[0] == '\"')
            {
                quit = true;
                newPack->initX = -1;
            }
        }
        else
        {
            intData = intInput(message, 3, 0, 0, SCREEN_WIDTH, false);
            if (intData < 0)
            {
                quit = true;
                newPack->initX = -1;
            }
        }
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
            newPack->initX = intData;
            wizardState++;
            break;
        case 7:
            newPack->initY = intData;
            wizardState++;
            break;
        case 8:
            newPack->initMap = intData;
            wizardState++;
            quit = true;
            break;
        }
    }
    if (newPack->initX == -1)  //quit
        return;
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
    mainMapPackWizardLoop(*newPack, newPack->tilesetMaps);

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

    *x = 0, *y = 0;
    SDL_Keycode key = 0;
    printf("%d\n", *map);
    if (*map == -1)
        *map = chooseMap(workingPack);
    SDL_Event e;
    bool quit = false;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                *map = -1;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                *x = TILE_SIZE * (e.motion.x / TILE_SIZE);
                *y = TILE_SIZE *(e.motion.y / TILE_SIZE);
                quit = true;
            }
            if (e.type == SDL_MOUSEMOTION)
            {
                *x = TILE_SIZE * (e.motion.x / TILE_SIZE);
                *y = TILE_SIZE *(e.motion.y / TILE_SIZE);
            }
            if (e.type == SDL_KEYDOWN)
            {
                if (SC_UP == e.key.keysym.scancode && *y > 0)
                    *y -= TILE_SIZE;
                if (SC_DOWN == e.key.keysym.scancode && *y < SCREEN_HEIGHT)
                    *y += TILE_SIZE;
                if (SC_LEFT == e.key.keysym.scancode && *x > 0)
                    *x -= TILE_SIZE;
                if (SC_RIGHT == e.key.keysym.scancode && *x < SCREEN_WIDTH)
                    *x += TILE_SIZE;
                if (SC_INTERACT == e.key.keysym.scancode || key == ANYWHERE_QUIT)
                    quit = true;
            }
        }
        SDL_RenderClear(mainRenderer);
        viewMap(workingPack, *map, false, false);
        drawText("Choose x/y coord.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, false);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = *x, .y = *y, .w = TILE_SIZE, .h = TILE_SIZE}));
        SDL_RenderPresent(mainRenderer);
    }
}

int subMain(mapPack* workingPack)
{
    bool quit = false;
    int code = 0;
    loadIMG(workingPack->tilesetFilePath, &(workingPack->mapPackTexture));  //for some reason we need to load twice??
    while(!quit)
    {
        code = aMenu(workingPack->mapPackTexture, workingPack->tilesetMaps[2], "Map-Pack Tools", (char*[4]) {"Map Creator", "Script Editor", "Map-Pack Wizard", "Back"}, 4, 1, AMENU_MAIN_THEME, true, false, NULL);
        if (code == 1)
            code = mainMapCreator(workingPack);
        if (code == 2)
            code = mainScriptEdtior(workingPack);
        if (code == 3)
            code = mainMapPackWizard(workingPack);
        if (code == 4 || code == -1)
            quit = true;
    }
    SDL_DestroyTexture(workingPack->mapPackTexture);
    return -1 * (code == -1);
}

int mainMapCreator(mapPack* workingPack)
{
    for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
        for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
            eventmap[dy][dx] = 0;
    char* mainFilePath = calloc(MAX_FILE_PATH, sizeof(char));
    char mapFilePath[MAX_FILE_PATH];
    char tileFilePath[MAX_FILE_PATH];
    bool quit = false;
    while(!quit)
    {
        int choice = aMenu(tilesetTexture, MAIN_ARROW_ID, "New or Load Map?", (char*[4]) {"New", "Load", "Info/Help", "Back"}, 4, 0, AMENU_MAIN_THEME, true, false, NULL);
        if (choice == -1)
            return choice;
        if (choice < 3)
        {
            strncpy(mainFilePath, workingPack->mainFilePath, MAX_FILE_PATH);
            if (choice == 1)
            {
                for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
                {
                    for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
                    {
                        tilemap[dy][dx] = 0;
                        eventmap[dy][dx] = 0;
                    }
                }
            }
            uniqueReadLine((char**) &mapFilePath, MAX_FILE_PATH, mainFilePath, 1);
            uniqueReadLine((char**) &tileFilePath, MAX_FILE_PATH, mainFilePath, 2);
            player creator;
            initPlayer(&creator, 0, 0, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, 0);
            if (choice == 1)
            {
                creator.mapScreen = -1;
            }
            if (choice == 2)
            {
                creator.mapScreen = chooseMap(*workingPack);
                if (creator.mapScreen == -1)
                {
                    return -1;
                }
                loadMapFile(workingPack->mapFilePath, tilemap, eventmap, creator.mapScreen, HEIGHT_IN_TILES, WIDTH_IN_TILES);
            }
            SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            int scriptCount = 0;
            _TglobalInt1 = 0;  //used for save/discard prompt
            script* mapScripts = mainMapCreatorLoop(&creator, &scriptCount, *workingPack);
            if (creator.mapScreen == -1)
            {
                free(mapScripts);
                return -1;
            }

            if (_TglobalInt1 == 1)
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
                waitForKey(false);
                free(exitNote);
            }
            free(mapScripts);
        }
        if (choice == 3)
        {
            SDL_Texture* imgs[2];
            loadIMG("assets/help/MapCreatorEvent.png", &imgs[0]);
            loadIMG("assets/help/MapCreatorScript.png", &imgs[1]);
            SDL_Keycode pauseKey = 0;
            int helpIndex = 0;
            char* helpTexts[4] = {"Here's where you make maps!\nUse either your movement keys or the mouse to move your cursor.", "Use [Q] and [E] to change the type of tile.\n\nIf [Q] or [E] are in use, use [-] and [=] instead.", "Press Special (default [LShift]) to switch between editing the visual tiles and events.\nThe red backslash is collision.", "Some events require you make a script, outputted directly to the script file."};
            while(pauseKey != -1 && helpIndex < 4)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                SDL_RenderFillRect(mainRenderer, NULL);
                if (helpIndex > 1)
                    SDL_RenderCopy(mainRenderer, imgs[helpIndex - 2], NULL, &((SDL_Rect) {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}));
                drawText(helpTexts[helpIndex], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                pauseKey = waitForKey(true);
                helpIndex++;
                Mix_PlayChannel(-1, OPTION_SOUND, 0);
            }
            //display help menu
        }
        else
            quit = true;
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
        keycode = getKey(false);
        mapNum += (keycode == SDLK_d && mapNum < maxMapNum) - (keycode == SDLK_a && mapNum > 0) + 10 * (keycode == SDLK_s && mapNum + 9 < maxMapNum) - 10 * (keycode == SDLK_w && mapNum > 9);
        if (keycode == SDLK_RETURN || keycode == SDLK_ESCAPE || keycode == SDLK_SPACE || keycode == -1)
            quit = true;
        if (keycode == -1)
            mapNum = -1;
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
        keycode = getKey(false);
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
    bool conflict = false;
    for(int i = 0; i < SIZE_OF_SCANCODE_ARRAY; i++)
    {
        if (CUSTOM_SCANCODES[i] == SDL_SCANCODE_Q)
            conflict = true;
    }
    for(int i = 0; i < SIZE_OF_SCANCODE_ARRAY; i++)
    {
        if (CUSTOM_SCANCODES[i] == SDL_SCANCODE_E)
            conflict = true;
    }
    *scriptCount = 0;
    int scriptMaxCount = 5;
    script* mapScripts = calloc(scriptMaxCount, sizeof(script));
    bool quit = false, editingTiles = true, drawTile = false, toggleMouse = false;
    int frame = 0, sleepFor = 0, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = SDL_GetTicks(), lastTile = -1;
    int enemyCount = 0;
    for(int y = 0; y < HEIGHT_IN_TILES; y++)
    {
        for(int x = 0; x < WIDTH_IN_TILES; x++)
        {
            if (eventmap[y][x] > 11 && eventmap[y][x] < 15 && enemyCount < MAX_ENEMIES)
                enemyCount++;
            if (eventmap[y][x] > 11 && eventmap[y][x] < 15 && enemyCount > MAX_ENEMIES)
                eventmap[y][x] = 0;
        }
    }
	//printf("There are %d enemies at the start.\n", enemyCount);
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
                playerSprite->mapScreen = -1;
            }
            if (e.type == SDL_MOUSEMOTION)
            {
                playerSprite->spr.x = TILE_SIZE * (e.motion.x / TILE_SIZE);
                playerSprite->spr.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                if (toggleMouse)
                    drawTile = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                drawTile = true;
                toggleMouse = true;
            }
            if (e.type == SDL_MOUSEBUTTONUP)
            {
                toggleMouse = false;
            }
        }
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
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

            if ((!conflict ? (keyStates[SDL_SCANCODE_Q]) : (keyStates[SDL_SCANCODE_MINUS])) && playerSprite->spr.tileIndex > 0)
                playerSprite->spr.tileIndex--;

            if ((!conflict ? (keyStates[SDL_SCANCODE_E]) : (keyStates[SDL_SCANCODE_EQUALS])) && playerSprite->spr.tileIndex < 127 - (127 - MAX_SPRITE_MAPPINGS + 3) * (!editingTiles))  //+3 to avoid the first few sprite mappings
                playerSprite->spr.tileIndex++;

            if (keyStates[SDL_SCANCODE_SPACE])
                drawTile = true;

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

        if (drawTile && editingTiles)
        {
            tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
            drawTile = false;
        }

        if (drawTile && !editingTiles)
        {
            int prevTile = eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE];
            eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
            if (playerSprite->spr.tileIndex > 11 && playerSprite->spr.tileIndex < 15)  //enemies
            {

                if ((playerSprite->spr.tileIndex > 11 && playerSprite->spr.tileIndex < 15) && !(prevTile > 11 && prevTile < 15) && enemyCount < MAX_ENEMIES)
                    enemyCount++;
                //printf("There are now %d enemies.\n", enemyCount);
            }

            if (prevTile > 11 && prevTile < 15)
            {
                if (!(playerSprite->spr.tileIndex > 11 && playerSprite->spr.tileIndex < 15))
                    enemyCount--;
                //printf("There are now %d enemies.\n", enemyCount);
            }

            if (playerSprite->spr.tileIndex == 11)  //warp gate
            {
                script gateScript;
                int map = -1, x = 0, y = 0;
                locationSelectLoop(workingPack, &map, &x, &y);
                char* data = calloc(99, sizeof(char));
                snprintf(data, 99, "[%d/%d/%d]", map, x, y);
                initScript(&gateScript, script_use_gateway, playerSprite->mapScreen, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE, data, -1);
                mapScripts[(*scriptCount)++] = gateScript;
                free(data);
                toggleMouse = false; //this catches in infinite loop otherwise
                SDL_Delay(500);  //gives time for keypresses to unregister
            }
            if (playerSprite->spr.tileIndex == 9)  //teleporter
            {
                script teleportScript;
                int map = playerSprite->mapScreen, x = 0, y = 0;
                locationSelectLoop(workingPack, &map, &x, &y);
                char* data = calloc(99, sizeof(char));
                snprintf(data, 99, "[%d/%d]", x, y);
                initScript(&teleportScript, script_use_teleporter, playerSprite->mapScreen, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE, data, -1);
                mapScripts[(*scriptCount)++] = teleportScript;
                free(data);
                toggleMouse = false; //this catches in infinite loop otherwise
                SDL_Delay(500);  //gives time for keypresses to unregister
            }
            if (*scriptCount >= scriptMaxCount - 1)
            {
                scriptMaxCount += 5;
                script* temp = realloc(mapScripts, scriptMaxCount);
                if (temp)
                    mapScripts = temp;
                else
                    printf("can't realloc\n");
            }
            drawTile = false;
        }

        if (keyStates[SDL_SCANCODE_ESCAPE] || keyStates[SDL_SCANCODE_RETURN])
        {
            _TglobalInt1 = aMenu(tilesetTexture, MAIN_ARROW_ID, "Save Map?", (char*[3]) {"Save", "Discard", "Back"}, 3, 0, AMENU_MAIN_THEME, true, false, NULL);
            if (_TglobalInt1 != 3)
                quit = true;
        }

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
    char* actionDescriptions[14] = ALL_ACTION_DESCRIPTIONS;
    if (count < 1)
        return;
    char scriptText[240];
    for(int i = 0; i < count; i++)
    {
        snprintf(scriptText, 240, "{%d,%d,%d,%d,%d,%d,%s} ;%s on map %d", mapScripts[i].action, mapScripts[i].mapNum, mapScripts[i].x, mapScripts[i].y, mapScripts[i].w, mapScripts[i].h, mapScripts[i].data, actionDescriptions[(int) mapScripts[i].action], mapScripts[i].mapNum);
        appendLine(workingPack.scriptFilePath, scriptText);
    }
}
//end map creator code.

//start script editor code
int mainScriptEdtior(mapPack* workingPack)
{
    bool quit = false;
    while(!quit)
    {
        script editScript;
        int scriptNum = scriptSelectLoop(*workingPack);
        if (scriptNum == -1)
            return 0;
        if (scriptNum == 0 && checkFile(workingPack->scriptFilePath, -1) > 0)
        {
            editScript = visualLoadScript(workingPack);
            if (!editScript.active)
                quit = 2;  //ret
        }

        if (scriptNum > 0)
        {
            initScript(&editScript, (scriptBehavior) scriptNum, chooseMap(*workingPack), 0, 0, TILE_SIZE, TILE_SIZE, "", -1);
            if (editScript.mapNum == -1)
                quit = 2;  //ret
        }

        if (scriptNum >= 0)
        {
            mainScriptLoop(*workingPack, &editScript);
            if (editScript.action > 0 && editScript.action != (scriptBehavior) -1)
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
                waitForKey(true);
                quit = true;
            }
            else
            {
                if (editScript.action == (scriptBehavior) -1)
                    quit = 2;  //ret
            }
        }
    }
    return 1 - quit;  //0 if quit == 1, -1 if quit == 2
}

int scriptSelectLoop(mapPack workingPack)
{
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, workingPack.tilesetMaps[2], 0, SDL_FLIP_NONE, (entityType) type_na);
    const int optionsSize = 14;
    char* optionsArray[] = {"Load", "TriggerDialogue", "TriggerDialOnce", "ForceDialogue", "TriggerBoss", "SwitchMaps", "Gateway", "Teleporter", "ToggleDoor", "Animation", "BossActions", "GainMoney", "HurtPlayer", "placeholder"};
    int scriptType = 0, selection = -1;
    SDL_Color textColor = (SDL_Color) {AMENU_MAIN_TEXTCOLOR};
    SDL_Color bgColor = (SDL_Color) {AMENU_MAIN_BGCOLOR};
    SDL_Event e;
    bool quit = false, showHelp = false;
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
            else
            {
                if(e.type == SDL_KEYDOWN)
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
                            if (scriptType == 8)
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
                            if (scriptType == 5)
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
                            showHelp = true;
                            selection = 0;
                        }
                    }
                }
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                {
                    int choice = (e.button.y / TILE_SIZE) - 4;
                    if (choice > 1 && choice <= 4)
                    {
                        selection = choice;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        if (selection != 3)
                        {
                            quit = true;
                        }
                        else
                        {
                            showHelp = true;
                            selection = 0;
                        }
                    }
                    else if (choice == 1)
                    {
                        SDL_Rect minus = {TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                        SDL_Rect plus = {18 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE};

                        if ((e.button.x - minus.x <= minus.w && e.button.x - minus.x > 0) && (e.button.y - minus.y <= minus.h && e.button.y - minus.y > 0) && scriptType > 0)
                        {
                            if (scriptType == 8)
                                scriptType -= 3;
                            else
                                scriptType--;
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }

                        if ((e.button.x - plus.x <= plus.w && e.button.x - plus.x > 0) && (e.button.y - plus.y <= plus.h && e.button.y - plus.y > 0) && scriptType < optionsSize - 1)
                        {
                            if (scriptType == 5)
                                scriptType += 3;
                            else
                                scriptType++;
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }
                    }
                }
                if (e.type == SDL_MOUSEMOTION)
                {
                    if (e.motion.y / TILE_SIZE > 4 && e.motion.y / TILE_SIZE <= 8)
                        cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                }
            }
        }
        if (showHelp)
        {
            int pauseKey = 0;
            char* helpTexts[4] = {SCRIPT_HELP_TEXT1, SCRIPT_HELP_TEXT2, "", ""};
            SDL_Texture* img;
            if (scriptType == 0)
            {
                helpTexts[2] = SCRIPT_HELP_TEXT_LOAD;
                loadIMG("assets/help/loadScript.png", &img);
            }
            if (scriptType > 0 && scriptType < 4)
                helpTexts[2] = SCRIPT_HELP_TEXT_DIALOGUE;
            if (scriptType == 4)
                helpTexts[2] = SCRIPT_HELP_TEXT_TRIGGERBOSS;
            if (scriptType == 5 || scriptType == 6)
                helpTexts[2] = SCRIPT_HELP_TEXT_MAPTELEPORT;
            if (scriptType == 7)
                helpTexts[2] = SCRIPT_HELP_TEXT_TELEPORTER;
            if (scriptType == 8)
                helpTexts[2] = SCRIPT_HELP_TEXT_DOORS;
            if (scriptType == 9)
                helpTexts[2] = SCRIPT_HELP_TEXT_ANIMATION;
            if (scriptType == 10)
            {
                helpTexts[2] = SCRIPT_HELP_TEXT_BOSSACTIONS;
                helpTexts[3] = SCRIPT_HELP_TEXT_BOSSACTIONS2;
            }
            if (scriptType == 11 || scriptType == 12)
                helpTexts[2] = SCRIPT_HELP_TEXT_NUMINPUT;
            if (scriptType == 13)
                helpTexts[2] = "Coming soon (?)";

            int helpIndex = 0;
            while(pauseKey != -1 && helpIndex <= 2 + (scriptType == 10))
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                SDL_RenderFillRect(mainRenderer, NULL);
                if (scriptType == 0 && helpIndex == 2)
                    SDL_RenderCopy(mainRenderer, img, NULL, &((SDL_Rect) {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}));
                drawText(helpTexts[helpIndex], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                pauseKey = waitForKey(true);
                helpIndex++;
                Mix_PlayChannel(-1, OPTION_SOUND, 0);
            }
            showHelp = false;
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
        key = getKey(false);
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
        if (editScript->action == script_trigger_dialogue || editScript->action == script_trigger_dialogue_once || editScript->action == script_force_dialogue)
        {
            stringInput(&data, "What should be said?", 88, "Hello!", true);
        }

        if (editScript->action == script_trigger_boss)
        {
            int maxScriptLines = checkFile(workingPack.scriptFilePath, -1);
            int foundIndex = -1;
            char* temp = "";
            for(int i = 0; i < maxScriptLines; i++)
            {
                script aScript;
                readScript(&aScript, readLine(workingPack.scriptFilePath, i, &temp), i);
                if (aScript.mapNum == editScript->mapNum)
                    foundIndex = i;
            }
            if (foundIndex > -1)
                snprintf(data, 3, "%d", foundIndex);
            else
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                SDL_RenderFillRect(mainRenderer, NULL);
                drawText("Error: No boss on this map! First, make a BossActions script.", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                waitForKey(true);
                SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
                key = SDL_GetKeyFromScancode(SC_MENU);
            }
        }

        if (editScript->action == script_switch_maps || editScript->action == script_use_gateway)
        {
            int map = -1, x = 0, y = 0;
            locationSelectLoop(workingPack, &map, &x, &y);
            snprintf(data, 12, "[%d/%d/%d]", map, x, y);
        }
        if (editScript->action == script_use_teleporter)
        {
            int map = editScript->mapNum, x = 0, y = 0;
            locationSelectLoop(workingPack, &map, &x, &y);
            snprintf(data, 12, "[%d/%d]", x, y);
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

                    if(e.type == SDL_KEYDOWN)
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

                    if (e.type == SDL_MOUSEBUTTONDOWN)
                    {
                        if (e.button.x / TILE_SIZE == 15 && e.button.y / TILE_SIZE < 8 && e.button.y / TILE_SIZE > 4)
                        {
                            if (newDoors[e.button.y / TILE_SIZE - 5] < 1)
                                newDoors[e.button.y / TILE_SIZE - 5]++;
                        }
                        if (e.button.x / TILE_SIZE == 1 && e.button.y / TILE_SIZE < 8 && e.button.y / TILE_SIZE > 4)
                        {
                            if (newDoors[e.button.y / TILE_SIZE - 5] > -2)
                                newDoors[e.button.y / TILE_SIZE - 5]--;
                        }
                        if (e.button.y / TILE_SIZE == 8)
                        {
                            quit = true;
                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        }
                    }

                    if (e.type == SDL_MOUSEMOTION)
                    {
                        if (e.motion.y / TILE_SIZE > 4 && e.motion.y / TILE_SIZE < 9)
                            cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                    }
                }
                drawATile(workingPack.mapPackTexture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, (cursor.y < TILE_SIZE * 8 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
                SDL_RenderPresent(mainRenderer);
            }
            snprintf(data, 14, "[%d/%d/%d/-1]", newDoors[0], newDoors[1], newDoors[2]);
        }

        if (editScript->action == script_animation)
        {
			SDL_Rect bounding = {.x = 0, .y = 0, .w = TILE_SIZE, .h = TILE_SIZE};
			//get bounding
			{
			    int xx1 = x1, yy1 = y1, xx2 = x2, yy2 = y2;
			    sprite ccursor;
                initSprite(&ccursor, xx1, yy1, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_na);
                bool quit = false, editXY = true;
			    while(!quit)
                {
                    SDL_RenderClear(mainRenderer);
                    viewMap(workingPack, map, false, false);
                    SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
                    SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = !editXY ? xx1 : ccursor.x, .y = !editXY ? yy1 : ccursor.y, .w = !editXY ? ccursor.x - xx1 : ccursor.w, .h = !editXY ? ccursor.y - yy1 : ccursor.h}));
                    key = getKey(false);
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

                    if (SC_UP == SDL_GetScancodeFromKey(key) && ccursor.y > 0)
                        ccursor.y -= intervalSize;
                    if (SC_DOWN == SDL_GetScancodeFromKey(key) && ccursor.y < SCREEN_HEIGHT)
                        ccursor.y += intervalSize;
                    if (SC_LEFT == SDL_GetScancodeFromKey(key) && ccursor.x > 0)
                        ccursor.x -= intervalSize;
                    if (SC_RIGHT == SDL_GetScancodeFromKey(key) && ccursor.x < SCREEN_WIDTH)
                        ccursor.x += intervalSize;
                    if (SC_INTERACT == SDL_GetScancodeFromKey(key))
                    {
                        if (editXY)
                        {
                            xx1 = ccursor.x;
                            yy1 = ccursor.y;
                            ccursor.x = xx1 + ccursor.w;
                            ccursor.y = yy1 + ccursor.h;
                            bigIntervalSize = false;
                            intervalSize = 6;
                            editXY = false;
                        }
                        else
                        {
                            xx2 = ccursor.x;
                            yy2 = ccursor.y;
                            quit = true;
                        }
                    }
                    if (key == ANYWHERE_QUIT || key == SDL_GetKeyFromScancode(SC_MENU))
                        quit = true;
                    SDL_RenderPresent(mainRenderer);
                }
                bounding.x = toolchain_min(xx1, xx2);
                bounding.y = toolchain_min(yy1, yy2);
                bounding.w = abs(xx2 - xx1);
                bounding.h = abs(yy2 - yy1);
			}
			//end get bounding
			int startingTile = chooseTile(workingPack, "Choose the (top left) tile.");
			loadTTFont(FONT_FILE_NAME, &mainFont, 48);
            //figure this out
			quit = false;
			targetTime = calcWaitTime(60);
			int coords = 0;
			const int maxCoords = 10;  //because we use 1 not really in use
			int xCoords[maxCoords];
			int yCoords[maxCoords];
			int frameCoords[maxCoords];
			for(int i = 0; i < maxCoords; i++)
            {
                xCoords[i] = 0;
                yCoords[i] = 0;
                frameCoords[i] = 0;
            }
			initSprite(&cursor, toolchain_min(x1, x2), toolchain_min(y1, y2), bounding.w, bounding.h, 0, 0, SDL_FLIP_NONE, type_na);
			while(!quit && coords < maxCoords)
			{
				bool select = false;
                SDL_Event e;
                Uint32 lastKeypressTime = SDL_GetTicks(), lastFrame = lastKeypressTime;
                int sleepFor = 0, frame = 0, animationMoveFrames = 0, animationMoveSegment = 0, startX = bounding.x, startY = bounding.y;
                sprite animationSpr;
                initSprite(&animationSpr, bounding.x, bounding.y, bounding.w, bounding.h, startingTile, 0, SDL_FLIP_NONE, type_boss);
				while(!select)
				{
					SDL_RenderClear(mainRenderer);
					viewMap(workingPack, map, false, false);
                    SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
                    SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = toolchain_min(x1, x2), .y = toolchain_min(y1, y2), .w = abs(x2 - x1), .h = abs(y2 - y1)}));
                    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.h}));
					drawATile(workingPack.mapPackTexture, animationSpr.tileIndex, cursor.x, cursor.y, bounding.w, bounding.h, cursor.angle, cursor.flip);
					if (coords > 0)
                    {
                        int totalFrames = 0;
                        for(int i = 0; i <= animationMoveSegment; i++)
                        {
                            totalFrames += frameCoords[i];
                        }
                        animationSpr.x += (xCoords[animationMoveSegment] - startX) / frameCoords[animationMoveSegment];
                        animationSpr.y += (yCoords[animationMoveSegment] - startY) / frameCoords[animationMoveSegment];
                        animationMoveFrames++;
                        if (animationMoveFrames == totalFrames)
                        {
                            animationSpr.x = xCoords[animationMoveSegment];
                            animationSpr.y = yCoords[animationMoveSegment];
                            startX = xCoords[animationMoveSegment];
                            startY = yCoords[animationMoveSegment];
                            if (coords > animationMoveSegment + 1)
                                animationMoveSegment++;
                            else
                            {
                                animationMoveSegment = 0;
                                animationMoveFrames = 0;
                                animationSpr.x = bounding.x;
                                animationSpr.y = bounding.y;
                                startX = bounding.x;
                                startY = bounding.y;
                            }
                        }
                    }
                    drawASprite(workingPack.mapPackTexture, animationSpr);
                    drawText("Choose the coords.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF}, true);
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
							if (checkSKUp)
								cursor.y -= PIXELS_MOVED;
							if (checkSKDown)
								cursor.y += PIXELS_MOVED;
							if (checkSKLeft)
								cursor.x -= PIXELS_MOVED;
							if (checkSKRight)
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
						if (e.type == SDL_MOUSEBUTTONDOWN)
                            select = true;

                        if (e.type == SDL_MOUSEMOTION)
                        {
                            cursor.x = TILE_SIZE * (e.motion.x / TILE_SIZE);
                            cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                        }
					}

					sleepFor = targetTime - (SDL_GetTicks() - lastFrame);  //FPS limiter; rests for (16 - time spent) ms per frame, effectively making each frame run for ~16 ms, or 60 FPS
					if (sleepFor > 0)
						SDL_Delay(sleepFor);
                    //printf("tT - %d, lF - %d, sF - %d, GT - %d\n", targetTime, lastFrame, sleepFor, SDL_GetTicks());
					lastFrame = SDL_GetTicks();
					frame++;
					//SDL_RenderPresent(mainRenderer);
				}
				if (!quit)
                {
                    xCoords[coords] = cursor.x;
                    yCoords[coords] = cursor.y;
                    frameCoords[coords++] = intInput("Get there in how many frames?", 3, 15, 1, 999, false);
                }
				//printf("%s, %s, %d\n(%d, %d) for %d\n", printBool(quit), printBool(select), coords, xCoords[coords - 1], yCoords[coords - 1], frameCoords[coords - 1]);
				//get num of frames, add into array
			}
			//then make it all a string
			char* moveStr = calloc(coords * 4 + 2, sizeof(char));
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
			//end figure this out
			targetTime = calcWaitTime(FPS);
			char* dialogueText = calloc(88, sizeof(char));
			bool keepOnscreen = (aMenu(tilesetTexture, MAIN_ARROW_ID, "Keep tile onscreen after done?", (char*[2]) {"Yes", "No"}, 2, 0, AMENU_MAIN_THEME, true, false, NULL)) == 1;
			stringInput(&dialogueText, "Dialogue after completion? (Optional)", 88, "0", true);
			snprintf(data, 116 + strlen(moveStr), "[%d/%d/%d/%d/%d/%d](%s)<%s>", bounding.x, bounding.y, bounding.w, bounding.h, startingTile, keepOnscreen, moveStr, dialogueText);
			free(dialogueText);
        }

        if (editScript->action == script_boss_actions)
        {
			//[starting tile|health](actions...)
			//select starting tile
			int startingTile = chooseTile(workingPack, "Choose the (top left) tile.");
			loadTTFont(FONT_FILE_NAME, &mainFont, 48);
			//get health

			char* dataStr = calloc(10, sizeof(char));
			snprintf(dataStr, 10, "[%d/%d]", startingTile, intInput("How many hits to kill?", 2, 5, 1, 60, false));
			char* moveStr;
			//visualize boss actions
			quit = false;
			targetTime = calcWaitTime(60);
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
                int sleepFor = 0, frame = 0, bossMoveFrames = 0, bossMoveSegment = 0, startX = toolchain_min(x1, x2), startY = toolchain_min(y1, y2);
                sprite bossSpr;
                initSprite(&bossSpr, toolchain_min(x1, x2), toolchain_min(y1, y2), abs(x2 - x1), abs(y2 - y1), startingTile, 0, SDL_FLIP_NONE, type_boss);
				while(!select)
				{
					SDL_RenderClear(mainRenderer);
					viewMap(workingPack, map, false, false);
                    SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
                    SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = toolchain_min(x1, x2), .y = toolchain_min(y1, y2), .w = abs(x2 - x1), .h = abs(y2 - y1)}));
                    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.h}));
					if (coords > 0)
                    {
                        int totalFrames = 0;
                        for(int i = 0; i <= bossMoveSegment; i++)
                        {
                            totalFrames += frameCoords[i];
                        }
                        bossSpr.x += (xCoords[bossMoveSegment] - startX) / frameCoords[bossMoveSegment];
                        bossSpr.y += (yCoords[bossMoveSegment] - startY) / frameCoords[bossMoveSegment];
                        bossMoveFrames++;
                        if (bossMoveFrames == totalFrames)
                        {
                            bossSpr.x = xCoords[bossMoveSegment];
                            bossSpr.y = yCoords[bossMoveSegment];
                            startX = xCoords[bossMoveSegment];
                            startY = yCoords[bossMoveSegment];
                            if (coords > bossMoveSegment + 1)
                                bossMoveSegment++;
                            else
                            {
                                bossMoveSegment = 0;
                                bossMoveFrames = 0;
                                bossSpr.x = toolchain_min(x1, x2);
                                bossSpr.y = toolchain_min(y1, y2);
                                startX = toolchain_min(x1, x2);
                                startY = toolchain_min(y1, y2);
                            }
                        }
                    }
                    drawASprite(workingPack.mapPackTexture, bossSpr);
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
						if (e.type == SDL_MOUSEBUTTONDOWN)
                            select = true;

                        if (e.type == SDL_MOUSEMOTION)
                        {
                            cursor.x = TILE_SIZE * (e.motion.x / TILE_SIZE);
                            cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
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
                    frameCoords[coords++] = intInput("Get there in how many frames?", 3, 15, 1, 999, false);
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
			targetTime = calcWaitTime(FPS);
			//printf("%s\n", data);
		}

        if (editScript->action == script_gain_money || editScript->action == script_player_hurt)
        {
            char* message = calloc(17, sizeof(char)), * temp = "";
            snprintf(message, 17, "How much %s?", editScript->action == script_gain_money ? "money" : "damage");
            int intData = intInput(message, 3 - (editScript->action == script_gain_money), 1, 1 - 21 * (editScript->action == script_player_hurt), 20 - 16 * (editScript->action == script_player_hurt), (editScript->action == script_player_hurt));
            strcpy(data, intToString(intData, temp));
            free(message);
        }
	}
	if (key == ANYWHERE_QUIT || key == SDL_GetKeyFromScancode(SC_MENU))
		initScript(editScript, 0 - (key == ANYWHERE_QUIT), map, toolchain_min(x1, x2), toolchain_min(y1, y2), abs(x2 - x1), abs(y2 - y1), " ", -1);
	else
		initScript(editScript, editScript->action, map, toolchain_min(x1, x2), toolchain_min(y1, y2), abs(x2 - x1), abs(y2 - y1), data, -1);
	free(data);
	return *editScript;
}

script visualLoadScript(mapPack* workingPack)
{
    script loadedScript;
    bool quit = false;
    char* temp = "";
    int scriptLineNum = 0, maxLines = checkFile(workingPack->scriptFilePath, -1);
    printf("%d\n", maxLines);  //this is wrong I think!
    readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp), 0);
    SDL_Keycode key;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        viewMap(*workingPack, loadedScript.mapNum, true, false);
        key = getKey(false);
        if (key == SDL_GetKeyFromScancode(SC_UP))
        {
            if (scriptLineNum > 9)
                scriptLineNum -= 10;
            readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp), scriptLineNum);
        }

        if (key == SDL_GetKeyFromScancode(SC_DOWN))
        {
            if (scriptLineNum < maxLines - 10)
                scriptLineNum += 10;
            readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp), scriptLineNum);
        }

        if (key == SDL_GetKeyFromScancode(SC_LEFT))
        {
            if (scriptLineNum > 0)
                scriptLineNum--;
            readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp), scriptLineNum);
        }

        if (key == SDL_GetKeyFromScancode(SC_RIGHT))
        {
            if (scriptLineNum < maxLines)
                scriptLineNum++;
            readScript(&loadedScript, readLine(workingPack->scriptFilePath, scriptLineNum, &temp), scriptLineNum);
        }
        if (key == SDL_GetKeyFromScancode(SC_INTERACT) || key == SDLK_RETURN || key == ANYWHERE_QUIT)
            quit = true;
        if (key == ANYWHERE_QUIT)
            loadedScript.active = false;

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
    int choice = 0;
    while (!quit)
    {
        choice = aMenu(workingPack->mapPackTexture, workingPack->tilesetMaps[2], workingPack->mainFilePath + 10, (char*[5]) {"Edit Filepaths", "Edit Init Spawn", "Edit Tile Equates", "Info/Help", "Back"}, 5, 0, AMENU_MAIN_THEME, true, false, NULL);

        if (choice == 1)
            choice = editFilePaths(workingPack);

        if (choice == 2)
            choice = editInitSpawn(workingPack);

        if (choice == 3)
            choice = editTileEquates(workingPack);

        if (choice == 4)
        {
            int key = 0;
            while(!key)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                SDL_RenderFillRect(mainRenderer, NULL);
                drawText(MAPPACK_SETUP_HELP_TEXT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                key = getKey(true);
            }
            if (key == -1)
                choice = key;
        }

        if (choice == 5 || choice == -1)
            quit = true;

    }
    return -1 * (choice == -1);
}

int editFilePaths(mapPack* workingPack)
{
    SDL_RenderClear(mainRenderer);
    bool quit = false;
    int choice = 0;
    while(!quit)
    {
        choice = aMenu(tilesetTexture, workingPack->tilesetMaps[2], workingPack->mainFilePath + 10, (char*[6]) {"Change Name", "Edit Map Path", "Edit Tileset Path", "Edit Save Path", "Edit Script Path", "Back"}, 6, 0, AMENU_MAIN_THEME, true, false, NULL);
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
            if (getString[0] == '\"')
            {
                quit = true;
                choice = -1;
            }
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
    if (choice >= 0)
        saveMapPack(workingPack);
    return -1 * (choice == -1);
}

int editInitSpawn(mapPack* workingPack)
{
    SDL_RenderClear(mainRenderer);
    workingPack->initMap = chooseMap(*workingPack);
    if (workingPack->initMap == -1)
        return workingPack->initMap;
    chooseCoords(*workingPack, workingPack->initMap, &(workingPack->initX), &(workingPack->initY));
    saveMapPack(workingPack);
    return 0;
}

int editTileEquates(mapPack* workingPack)
{
    loadTTFont(FONT_FILE_NAME, &mainFont, 24);
    SDL_RenderClear(mainRenderer);
    int numbers[MAX_SPRITE_MAPPINGS];
    numbers[0] = -1;
    sprite chooser;
    initSprite(&chooser, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_player);
    mainMapPackWizardLoop(*workingPack, (int*) numbers);
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
    else
        return -1;
    loadTTFont(FONT_FILE_NAME, &mainFont, 48);
    return 0;
}

void mainMapPackWizardLoop(mapPack workingPack, int* numArray)
{
    int numArrayTracker = 0;
    char* text[] = PICK_MESSAGES_ARRAY;
    bool quit = false;
    while(numArrayTracker < MAX_SPRITE_MAPPINGS && !quit)
    {
        numArray[numArrayTracker] = chooseTile(workingPack, text[numArrayTracker]);
        if (numArray[numArrayTracker] == -1)
            quit = true;
        else
            numArrayTracker++;
    }
    /*for(int i = 0; i < maxArraySize; i++)
        printf("%d\n", numArray[i]);*/
    //waitForKey();
    if (numArrayTracker < MAX_SPRITE_MAPPINGS)
        numArray[0] = -1;
}

int chooseTile(mapPack workingPack, char* prompt)
{
    #undef SCREEN_WIDTH
    #undef SCREEN_HEIGHT
    #define SCREEN_WIDTH TILE_SIZE * 16
    #define SCREEN_HEIGHT TILE_SIZE * 9
	TTF_CloseFont(mainFont);
	loadTTFont(FONT_FILE_NAME, &mainFont, 24);
    int frame = 0, sleepFor = 0, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = lastFrame + 1, tile = -1;
    bool quit = false, whiteBG = true;
    sprite cursor;
    initSprite(&cursor, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_generic);
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
        drawText(prompt, 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF * (!whiteBG), 0xFF * (!whiteBG), 0xFF * (!whiteBG), 0xFF}, true);
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
                if (checkSKInteract || keyStates[SDL_SCANCODE_RETURN])
                {
                    tile = 8 * (cursor.x / TILE_SIZE) + cursor.y / TILE_SIZE - 1;  //-1 because we don't start at y=0
                    quit = true;
                }
                if (keyStates[SDL_SCANCODE_LSHIFT])
                    whiteBG = !whiteBG;
                lastKeypressTime = SDL_GetTicks();
            }
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                cursor.x = (e.button.x / TILE_SIZE) * TILE_SIZE;
                cursor.y = (e.button.y / TILE_SIZE) * TILE_SIZE;
                if (cursor.x > SCREEN_WIDTH)  //this needs to be bounded because the screen isn't sized appropriately
					cursor.x = SCREEN_WIDTH;
				if (cursor.y > SCREEN_HEIGHT)
					cursor.y = SCREEN_HEIGHT;
                tile = 8 * (cursor.x / TILE_SIZE) + cursor.y / TILE_SIZE - 1;  //-1 because we don't start at y=0
                quit = true;
            }
            if (e.type == SDL_MOUSEMOTION)
            {
                cursor.x = (e.button.x / TILE_SIZE) * TILE_SIZE;
                cursor.y = (e.button.y / TILE_SIZE) * TILE_SIZE;
                if (cursor.x > SCREEN_WIDTH)  //this needs to be bounded because the screen isn't sized appropriately
					cursor.x = SCREEN_WIDTH;
				if (cursor.y > SCREEN_HEIGHT)
					cursor.y = SCREEN_HEIGHT;
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
	TTF_CloseFont(mainFont);
	loadTTFont(FONT_FILE_NAME, &mainFont, 48);
    return tile;
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

