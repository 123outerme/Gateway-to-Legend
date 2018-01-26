//#include "outermeSDL.h"
#include "SDLGateway.h"

#define checkSKUp keyStates[SC_UP]
#define checkSKDown keyStates[SC_DOWN]
#define checkSKLeft keyStates[SC_LEFT]
#define checkSKRight keyStates[SC_RIGHT]
#define checkSKAttack keyStates[SC_ATTACK]
#define checkSKInteract keyStates[SC_INTERACT]
#define checkSKMenu keyStates[SC_MENU]
#define TILE_ID_PLAYER 16
#define PIXELS_MOVED 6

#define MAX_MAPPACKS_PER_PAGE 11
#define MAX_ENEMIES 6


#define START_GAMECODE 0
#define OPTIONS_GAMECODE 1
#define PLAY_GAMECODE 2
#define MAINLOOP_GAMECODE 3
#define OVERWORLDMENU_GAMECODE 4
#define RELOAD_GAMECODE 5
#define SAVE_GAMECODE 6
#define MAX_TILE_ID_ARRAY 18
#define MAX_COLLISIONDATA_ARRAY 13

#define drawASprite(tileset, spr) drawATile(tileset, spr.tileIndex, spr.x, spr.y, spr.w, spr.h, spr.angle, spr.flip)

#define checkRectCol(x1, y1, x2, y2) ((abs(abs(x1) - abs(x2)) < TILE_SIZE) && (abs(abs(y1) - abs(y2)) < TILE_SIZE))


void changeVolumes();
int changeControls();
void changeName();
void changeFPS();
void clearData(player* playerSprite);

int mainLoop(player* playerSprite);
void checkCollision(player* player, int* outputData, int moveX, int moveY, int lastX, int lastY);
void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag);
void drawOverTilemap(SDL_Texture* texture, int startX, int startY, int endX, int endY, bool drawDoors[], bool rerender);

#define AMENU_MAIN_TEXTCOLOR  0x00, 0xB0, 0xDA
#define AMENU_MAIN_BGCOLOR 0xE4, 0xE9, 0xF3
#define AMENU_MAIN_TITLECOLOR1 0x4D, 0xD2, 0xFF
#define AMENU_MAIN_TITLECOLOR2 0x00, 0xAC, 0xE6

#define AMENU_MAIN_THEME (SDL_Color) {AMENU_MAIN_BGCOLOR, 0xFF}, (SDL_Color) {AMENU_MAIN_TITLECOLOR2, 0xFF}, (SDL_Color) {AMENU_MAIN_TITLECOLOR1, 0xFF},  (SDL_Color) {AMENU_MAIN_TEXTCOLOR, 0xFF}


/*bool debug;
bool doDebugDraw;
SDL_Texture* eventTexture;  //eventmap layer is needed, this is just for debug, so when you're all done you can prob remove these*/

int tileIDArray[MAX_TILE_ID_ARRAY];
#define PLAYER_ID tileIDArray[0]
#define PLAYERWALK_ID tileIDArray[1]
#define CURSOR_ID tileIDArray[2]
#define HP_ID tileIDArray[3]
#define SWORD_ID tileIDArray[4]
#define ENEMY(x) tileIDArray[14 + x]

#define MAIN_ARROW_ID 34

bool doorFlags[3] = {true, true, true};  //this works; however it persists through map packs as well
bool enemyFlags[MAX_ENEMIES + 1] = {true, true, true, true, true, true, true};  //last bool is reloadEnemies
sprite enemies[MAX_ENEMIES];
script* allScripts;
int sizeOfAllScripts;

int main(int argc, char* argv[])
{
    //setting up default values
    //debug = true;
    {
        int initCode = initSDL(WINDOW_NAME, GLOBALTILES_FILEPATH, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
        if (initCode != 0)
            return initCode;
        initCode = initSounds();
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
    player person;
    if (checkFile(GLOBALSAVE_FILEPATH, 0))
        loadGlobalPlayer(&person, GLOBALSAVE_FILEPATH);
    else
        createGlobalPlayer(&person, GLOBALSAVE_FILEPATH);
    if (checkFile(CONFIG_FILEPATH, 6))  //load config
        loadConfig(CONFIG_FILEPATH);
    else
        initConfig(CONFIG_FILEPATH);
    /*if (debug)
        loadIMG("tileset/eventTile48.png", &eventTexture);*/
    SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mainRenderer);
    int gameState = 0;
    char* buffer = "";  //actually needed
    bool quitGame = false;
    allScripts = NULL;
    int choice = 0;
    while(!quitGame)
    {
        switch(gameState)
        {
        case START_GAMECODE:  //start menu
            person.mapScreen = 0;
            choice = aMenu(tilesetTexture, MAIN_ARROW_ID, "Gateway to Legend", (char*[3]) {"Play", "Settings", "Quit"}, 3, 1, AMENU_MAIN_THEME, true, true);
            if (choice == 1)
                gameState = PLAY_GAMECODE;
            if (choice == 2)
                gameState = OPTIONS_GAMECODE;
            if (choice == 3 || choice == -1)
                quitGame = true;
            break;
        case OPTIONS_GAMECODE:
            choice = aMenu(tilesetTexture, MAIN_ARROW_ID, "Options", (char*[6]) {"Sounds", "Controls", "Change Name", "Change FPS", "Reset Data", "Back"}, 6, 0, AMENU_MAIN_THEME, true, false);
            if (choice == 1)
                changeVolumes();
            if (choice == 2)
                choice = changeControls();
            if (choice == 3)
                changeName();
            if (choice == 4)
                changeFPS();
            if (choice == 5)
                clearData(&person);
            if (choice == 6)
                gameState = START_GAMECODE;
            if (choice == -1)
                quitGame = true;
            break;
        case PLAY_GAMECODE:  //main menu
            mapSelectLoop(listOfFilenames, (char*) mainFilePath, maxStrNum, &quitGame);
            if (quitGame)  //yes I do need this, this is gonna tell me if we quit
            {
                gameState = START_GAMECODE;
                quitGame = (quitGame == -1);  //will quit if we signal it
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
            loadIMG(tileFilePath, &tilesTexture);
            free(allScripts);
            allScripts = calloc(checkFile(scriptFilePath, -1) + 1, sizeof(script));
            for(int i = 0; i < checkFile(scriptFilePath, -1) + 1; i++)
            {
                script thisScript;
                readScript(&thisScript, readLine(scriptFilePath, i, &buffer));
                allScripts[i] = thisScript;
                sizeOfAllScripts = i + 1;
            }
            for(int i = 0; i < MAX_TILE_ID_ARRAY; i++)
            {
                tileIDArray[i] = strtol(readLine(mainFilePath, 8 + i, &buffer), NULL, 10);
            }
            quitGame = aMenu(tilesTexture, CURSOR_ID, readLine(mainFilePath, 0, &buffer), (char*[3]) {"New Game", "Load Game", "Back"}, 3, 2, AMENU_MAIN_THEME, true, false);
            if (quitGame == 3 || quitGame == -1)
            {
                quitGame = (quitGame == -1);
                break;
            }
            if (checkFile(saveFilePath, 0) && quitGame == 2)
                loadLocalPlayer(&person, saveFilePath, PLAYER_ID);
            else
                createLocalPlayer(&person, saveFilePath, strtol(readLine(mainFilePath, 5, &buffer), NULL, 10), strtol(readLine(mainFilePath, 6, &buffer), NULL, 10), TILE_SIZE, strtol(readLine(mainFilePath, 7, &buffer), NULL, 10), 0, SDL_FLIP_NONE, PLAYER_ID);
            quitGame = false;
            //done loading map-pack specific stuff
            if (checkFile(GLOBALSAVE_FILEPATH, 0))
                loadGlobalPlayer(&person, GLOBALSAVE_FILEPATH);  //loaded twice just to ensure nothing is overwritten?
            else
                createGlobalPlayer(&person, GLOBALSAVE_FILEPATH);
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
            Mix_HaltChannel(-1);
            choice = aMenu(tilesTexture, CURSOR_ID, "Overworld Menu", (char*[3]) {"Back", "Save", "Exit"}, 3, 1, AMENU_MAIN_THEME, true, false);
            if (choice == 1)
                gameState = MAINLOOP_GAMECODE;
            if (choice == 2 || choice == 3 || choice == -1)
                gameState = SAVE_GAMECODE;
            break;
        case RELOAD_GAMECODE:
            gameState = MAINLOOP_GAMECODE;
            for(int i = 0; i < 3; i++)
                doorFlags[i] = true;
            for(int i = 0; i < MAX_ENEMIES + 1; i++)
                enemyFlags[i] = true;
            person.invincCounter = 0;
            break;
        case SAVE_GAMECODE:
            saveLocalPlayer(person, saveFilePath);
            if (choice == 2)
                gameState = MAINLOOP_GAMECODE;
            if (choice == 3)
            {
                for(int i = 0; i < 3; i++)
                    doorFlags[i] = true;
                for(int i = 0; i < MAX_ENEMIES + 1; i++)
                    enemyFlags[i] = true;
                gameState = START_GAMECODE;
            }
            if (choice == -1)
                quitGame = true;
            break;
        }
    }
    Mix_HaltChannel(-1);
    saveGlobalPlayer(person, GLOBALSAVE_FILEPATH);
    SDL_DestroyTexture(tilesTexture);
    free(allScripts);
    closeSDL();
    return 0;
}

void changeVolumes()
{
    SDL_Color textColor = (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, bgColor = (SDL_Color) {AMENU_MAIN_BGCOLOR}, titleOverColor = (SDL_Color) {AMENU_MAIN_TITLECOLOR1};
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, MAIN_ARROW_ID, 0, SDL_FLIP_NONE, (entityType) type_na);
    SDL_Event e;
    bool quit = false;
    while(!quit)
    {
        SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
        SDL_RenderClear(mainRenderer);
        SDL_RenderFillRect(mainRenderer, NULL);
        SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        //background text (drawn first)
        drawText("Sound Volumes", 1 * TILE_SIZE + 3 * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, (SDL_Color) {AMENU_MAIN_TITLECOLOR2, 0xFF}, false);
        //foreground text
        drawText("Sound Volumes", 1 * TILE_SIZE + TILE_SIZE / 4 , 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, (SDL_Color) {AMENU_MAIN_TITLECOLOR1, 0xFF}, false);

        SDL_SetRenderDrawColor(mainRenderer, titleOverColor.r, titleOverColor.g, titleOverColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.25 * TILE_SIZE, .y = 4.75 * TILE_SIZE, .w = musicVolume * 4 + TILE_SIZE / 2, .h = .75 * TILE_SIZE}));
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.25 * TILE_SIZE, .y = 5.75 * TILE_SIZE, .w = soundVolume * 4 + TILE_SIZE / 2, .h = .75 * TILE_SIZE}));

        SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.5 * TILE_SIZE, .y = 5 * TILE_SIZE, .w = musicVolume * 4, .h = TILE_SIZE / 2}));
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.5 * TILE_SIZE, .y = 6 * TILE_SIZE, .w = soundVolume * 4, .h = TILE_SIZE / 2}));

        drawText("Music", 2.25 * TILE_SIZE, 5 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR, 0xFF}, false);
        drawText("Sounds", 2.25 * TILE_SIZE, 6 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR, 0xFF}, false);

        drawText("Back", 2 * TILE_SIZE + TILE_SIZE / 4, 7 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR, 0xFF}, false);

        while(SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if(e.type == SDL_QUIT)
                quit = true;
            //User presses a key
            else if(e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_UP))
                {
                    if (cursor.y > 5 * TILE_SIZE)
                        cursor.y -= TILE_SIZE;
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_DOWN))
                {
                    if (cursor.y < 7 * TILE_SIZE)
                        cursor.y += TILE_SIZE;
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_LEFT)  && cursor.y != TILE_SIZE * 7)
                {
                    if (cursor.y == TILE_SIZE * 5 && musicVolume > 0)
                    {
                        musicVolume -= 16;
                    }
                    else if (cursor.y == TILE_SIZE * 6 && soundVolume > 0)
                    {
                        soundVolume -= 16;
                    }
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_RIGHT)  && cursor.y != TILE_SIZE * 7)
                {
                    if (cursor.y == TILE_SIZE * 5 && musicVolume < MIX_MAX_VOLUME)
                    {
                        musicVolume += 16;
                    }
                    else if (cursor.y == TILE_SIZE * 6 && soundVolume < MIX_MAX_VOLUME)
                    {
                        soundVolume += 16;
                    }
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_INTERACT) && cursor.y == TILE_SIZE * 7)
                {
                    quit = true;
                    Mix_PlayChannel(-1, OPTION_SOUND, 0);
                }
            }
        }
        drawATile(tilesetTexture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
    saveConfig(CONFIG_FILEPATH);
    Mix_Volume(-1, soundVolume);
    Mix_VolumeMusic(musicVolume);
}

int changeControls()
{
    sprite cursor;
    SDL_Color textColor = (SDL_Color) {AMENU_MAIN_TEXTCOLOR};
    SDL_Color bgColor = (SDL_Color) {AMENU_MAIN_BGCOLOR};
    initSprite(&cursor, TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, MAIN_ARROW_ID, 0, SDL_FLIP_NONE, (entityType) type_na);
    int selection = -1;
    bool superQuit = false;
    while(!superQuit)
    {
        SDL_Event e;
        bool quit = false;
        //While application is running
        while(!quit)
        {
            SDL_RenderClear(mainRenderer);
            SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
            SDL_RenderFillRect(mainRenderer, NULL);
            //foreground text
            drawText("Configure Keys", 2 * TILE_SIZE + TILE_SIZE / 4, 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, textColor, false);

            char keyText[99];
            strcpy(keyText, "Up: ");
            drawText(strcat(keyText, SDL_GetKeyName(SDL_GetKeyFromScancode(SC_UP))), 2 * TILE_SIZE + TILE_SIZE / 4, 4 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 5) * TILE_SIZE, textColor, false);
            //we don't use SDL_GetScancodeName() because the SDL documentation warns that it isn't a stable way to get key names

            strcpy(keyText, "Down: ");
            drawText(strcat(keyText, SDL_GetKeyName(SDL_GetKeyFromScancode(SC_DOWN))), 2 * TILE_SIZE + TILE_SIZE / 4, 5 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 6) * TILE_SIZE, textColor, false);

            strcpy(keyText, "Left: ");
            drawText(strcat(keyText, SDL_GetKeyName(SDL_GetKeyFromScancode(SC_LEFT))), 2 * TILE_SIZE + TILE_SIZE / 4, 6 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 7) * TILE_SIZE, textColor, false);

            strcpy(keyText, "Right: ");
            drawText(strcat(keyText, SDL_GetKeyName(SDL_GetKeyFromScancode(SC_RIGHT))), 2 * TILE_SIZE + TILE_SIZE / 4, 7 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 8) * TILE_SIZE, textColor, false);

            strcpy(keyText, "Confirm: ");
            drawText(strcat(keyText, SDL_GetKeyName(SDL_GetKeyFromScancode(SC_INTERACT))), 2 * TILE_SIZE + TILE_SIZE / 4, 8 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 9) * TILE_SIZE, textColor, false);

            strcpy(keyText, "Menu: ");
            drawText(strcat(keyText, SDL_GetKeyName(SDL_GetKeyFromScancode(SC_MENU))), 2 * TILE_SIZE + TILE_SIZE / 4, 9 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 10) * TILE_SIZE, textColor, false);

            strcpy(keyText, "Attack: ");  //note to self: change the key name later
            drawText(strcat(keyText, SDL_GetKeyName(SDL_GetKeyFromScancode(SC_ATTACK))), 2 * TILE_SIZE + TILE_SIZE / 4, 10 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 10) * TILE_SIZE, textColor, false);

            drawText("Back", 2.25 * TILE_SIZE, 11 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 11) * TILE_SIZE, textColor, false);
            drawText("Default is W/S/A/D, Space, LShift, Esc", .75 * TILE_SIZE, 13 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 13) * TILE_SIZE, textColor, false);
            drawATile(tilesetTexture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
            SDL_RenderPresent(mainRenderer);
            while(SDL_PollEvent(&e) != 0)
            {
                //User requests quit
                if(e.type == SDL_QUIT)
                {
                    quit = true;
                    selection = ANYWHERE_QUIT;
                }
                //User presses a key
                else if(e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_UP))
                    {
                        if (cursor.y > 4 * TILE_SIZE)
                            cursor.y -= TILE_SIZE;
                    }

                    if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_DOWN))
                    {
                        if (cursor.y < 11 * TILE_SIZE)
                            cursor.y += TILE_SIZE;
                    }

                    if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_INTERACT))
                    {
                        selection = cursor.y / TILE_SIZE - 3;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        quit = true;
                    }
                }
            }
        }
        if (selection > 0 && selection != 8)
        {
            char* keyName[7] = {"Up", "Down", "Left", "Right", "Confirm", "Menu", "Attack"};
            char titleText[] = "Press a key for\n";
            strcat(titleText, keyName[selection - 1]);
            getNewKey(titleText, bgColor, textColor, selection - 1);
        }
        else
        {
            saveConfig(CONFIG_FILEPATH);
            superQuit = true;
        }
    }
    return 0 - (selection == -1);  //returns -1 if selection == -1 else 0.
}

void changeName()
{
    //
}

void changeFPS()
{
    //
}

void clearData(player* playerSprite)
{
    int choice = aMenu(tilesetTexture, MAIN_ARROW_ID, "Clear All Data:\nAre You Sure?", (char*[2]) {"Yes", "No"}, 2, 0, AMENU_MAIN_THEME, true, false);
    if (choice == 1)
    {
        createFile(CONFIG_FILEPATH);
        initConfig(CONFIG_FILEPATH);
        createFile(GLOBALSAVE_FILEPATH);
        createGlobalPlayer(playerSprite, GLOBALSAVE_FILEPATH);
        Mix_PlayChannel(-1, PLAYERHURT_SOUND, 0);
    }
}

void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag)
{
    bool quitMenu = false;
    char junkArray[MAX_CHAR_IN_FILEPATH];
    SDL_Keycode menuKeycode;
    int menuPage = 0, selectItem = 0;
    while(!quitMenu)
    {
        SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_TEXTCOLOR, 0xFF);
        SDL_RenderClear(mainRenderer);
        SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        for(int i = 0; i < (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE); i++)  //11 can comfortably be max
            drawText(readLine((char*) strcat(strcpy(junkArray, MAP_PACKS_SUBFOLDER), listOfFilenames[i + (menuPage * 5)]),  /*concatting the path and one of the filenames together into one string*/
                          0, (char**) &junkArray), TILE_SIZE + 10, (i + 3) * TILE_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
        drawText("Back", TILE_SIZE + 10, 2 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
        menuKeycode = getKey();
        if ((menuKeycode == SDL_GetKeyFromScancode(SC_LEFT) && menuPage > 0) || (menuKeycode == SDL_GetKeyFromScancode(SC_RIGHT) && menuPage < maxStrNum / MAX_MAPPACKS_PER_PAGE))
        {
            menuPage += (menuKeycode == SDL_GetKeyFromScancode(SC_RIGHT)) - 1 * (menuKeycode == SDL_GetKeyFromScancode(SC_LEFT));
            selectItem = 0;
        }

        if ((menuKeycode == SDL_GetKeyFromScancode(SC_UP) && selectItem > 0) || (menuKeycode == SDL_GetKeyFromScancode(SC_DOWN) && selectItem < (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE)))
            selectItem += (menuKeycode == SDL_GetKeyFromScancode(SC_DOWN)) - 1 * (menuKeycode == SDL_GetKeyFromScancode(SC_UP));

        if (menuKeycode == -1)
        {
            *backFlag = -1;
            quitMenu = true;
        }
        drawTile(MAIN_ARROW_ID, 10, (selectItem + 2) * TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);

        if (menuKeycode == SDL_GetKeyFromScancode(SC_INTERACT))
        {
            if (selectItem != 0)
                selectItem = menuPage * MAX_MAPPACKS_PER_PAGE + selectItem - 1;
            else
                *backFlag = true;
                quitMenu = true;
            Mix_PlayChannel(-1, OPTION_SOUND, 0);
        }
    }
    //loading map pack stuff
    strncat(strcpy(mapPackName, MAP_PACKS_SUBFOLDER), listOfFilenames[selectItem], MAX_CHAR_IN_FILEPATH - 9);
}

int mainLoop(player* playerSprite)
{
    SDL_Event e;
	bool quit = false, debugDrawPath = false, drawFPS = false;
	//static bool textBoxOn = false;
	char mapFilePath[MAX_CHAR_IN_FILEPATH];
	strcpy(mapFilePath, playerSprite->extraData);
    int maxTheseScripts = 0, * collisionData = calloc(MAX_COLLISIONDATA_ARRAY, sizeof(int));
    script thisScript, * theseScripts = calloc(sizeOfAllScripts, sizeof(script));
    thisScript.active = false;
    for(int i = 0; i < sizeOfAllScripts; i++)
    {
        if (allScripts[i].mapNum == playerSprite->mapScreen)
            theseScripts[maxTheseScripts++] = allScripts[i];
    }
    {
        script* new_ptr = realloc(theseScripts, maxTheseScripts * sizeof(script));
        if (new_ptr != NULL)
            theseScripts = new_ptr;
    }
    int enemyCount = 0;
    for(int y = 0; y < HEIGHT_IN_TILES; y++)
    {
        for(int x = 0; x < WIDTH_IN_TILES; x++)
        {
            if(eventmap[y][x] > 11 && eventmap[y][x] < 15 && enemyCount < MAX_ENEMIES)
            {
                enemyCount++;
                if (enemyFlags[MAX_ENEMIES])
                {
                    initSprite(&enemies[enemyCount - 1], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, ENEMY(eventmap[y][x] - 11), 0, SDL_FLIP_NONE, enemyFlags[enemyCount - 1] ? type_enemy : type_na);
                    enemies[enemyCount - 1].h = 1 + (eventmap[y][x] == 14);
                }
            }
        }
    }
    enemyFlags[MAX_ENEMIES] = false;
    //printf("%d < %d\n", maxTheseScripts, sizeOfAllScripts);
    //doDebugDraw = false;
    int exitCode = 2;
    char whatever[5] = "    \0";
    int startTime = SDL_GetTicks(), lastFrame = startTime,
        frame = 0, framerate = 0, sleepFor = 0, lastUpdateTime = SDL_GetTicks(),
        swordTimer = SDL_GetTicks() + 250;
    sprite sword;
    initSprite(&sword, 0, 0, TILE_SIZE, SWORD_ID, 0, SDL_FLIP_NONE, type_na);
    while(!quit && playerSprite->HP > 0)
    {
        SDL_RenderClear(mainRenderer);
        drawATilemap(tilesTexture, tilemap, 0, 0, 20, 15, -1, false);
        drawOverTilemap(tilesTexture, 0, 0, 20, 15, doorFlags, false);
        {  //drawing HUD
            SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0x7F);
            SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 0, .y = 0, .w = playerSprite->maxHP / 4 * TILE_SIZE, .h = TILE_SIZE}));
            for(int i = 0; i < playerSprite->HP; i += 4)  //draw HP
                drawATile(tilesTexture, HP_ID, TILE_SIZE * (i / 4), 0, (playerSprite->HP - i - 4 > 0 ? 4 : playerSprite->HP - i - 4 % 4) * (TILE_SIZE / 4), TILE_SIZE, 0, SDL_FLIP_NONE);
        }
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

        if (SDL_GetTicks() - lastUpdateTime >= 32)
        {
            const Uint8* keyStates = SDL_GetKeyboardState(NULL);

            playerSprite->animationCounter--;

            if (!playerSprite->movementLocked && (checkSKUp || checkSKDown || checkSKLeft || checkSKRight || checkSKAttack || checkSKInteract || playerSprite->xVeloc || playerSprite->yVeloc))
            {
                int lastY = playerSprite->spr.y;
                int lastX = playerSprite->spr.x;

                if (playerSprite->spr.y > 0 && checkSKUp)
                    playerSprite->yVeloc -= PIXELS_MOVED;

                if (playerSprite->spr.y < SCREEN_HEIGHT - playerSprite->spr.h && checkSKDown)
                    playerSprite->yVeloc += PIXELS_MOVED;

                if (playerSprite->spr.x > 0 && checkSKLeft)
                    playerSprite->xVeloc -= PIXELS_MOVED;

                if (playerSprite->spr.x < SCREEN_WIDTH - playerSprite->spr.w && checkSKRight)
                    playerSprite->xVeloc += PIXELS_MOVED;

                /*if (checkSKAttack && !textBoxOn && frame > targetTime / 2)
                {
                    initScript(&thisScript, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, script_trigger_dialogue, "Hello world!");
                    textBoxOn = true;
                }*/

                if (((checkSKUp && !checkSKDown) || (!checkSKUp && checkSKDown) || (checkSKLeft && !checkSKRight) || (!checkSKLeft && checkSKRight)) && !checkSKAttack)
                    playerSprite->lastDirection = checkSKUp + 2 * checkSKDown + 4 * checkSKLeft+ 8 * checkSKRight;

                if (playerSprite->lastDirection / 4 == 1)
                    playerSprite->spr.flip = SDL_FLIP_HORIZONTAL;
                else if (playerSprite->lastDirection > 7)
                    playerSprite->spr.flip = SDL_FLIP_NONE;

                if (playerSprite->xVeloc)
                {
                    if (abs(playerSprite->xVeloc) > 48)
                        playerSprite->xVeloc = 48 - 96 * (playerSprite->xVeloc < 0);
                    playerSprite->spr.x += playerSprite->xVeloc;
                    if (playerSprite->spr.x < 0)
                        playerSprite->spr.x = 0;
                    if (playerSprite->spr.x > SCREEN_WIDTH - TILE_SIZE)
                        playerSprite->spr.x = SCREEN_WIDTH - TILE_SIZE;
                }

                if (playerSprite->yVeloc)
                {
                    if (abs(playerSprite->yVeloc) > 48)
                        playerSprite->yVeloc = 48 - 96 * (playerSprite->yVeloc < 0);
                    playerSprite->spr.y += playerSprite->yVeloc;
                    if (playerSprite->spr.y < 0)
                        playerSprite->spr.y = 0;
                    if (playerSprite->spr.y > SCREEN_HEIGHT - TILE_SIZE)
                        playerSprite->spr.y = SCREEN_HEIGHT - TILE_SIZE;
                }

                if ((playerSprite->spr.x != lastX || playerSprite->spr.y != lastY))
                {
                    if (playerSprite->animationCounter < -12)
                    {
                        playerSprite->animationCounter = 12;
                        Mix_PlayChannel(-1, STEP_SOUND(1 + (rand() % 3)), 0);
                    }
                    if (playerSprite->animationCounter == 0)
                        Mix_PlayChannel(-1, STEP_SOUND(1 + (rand() % 3)), 0);
                }

                checkCollision(playerSprite, collisionData, (playerSprite->xVeloc > 0) - (playerSprite->xVeloc < 0), (playerSprite->yVeloc > 0) - (playerSprite->yVeloc < 0), lastX, lastY);

                if (playerSprite->xVeloc)  //this is done so that the last frame of velocity input is still collision-checked
                    playerSprite->xVeloc -= 6 - 12 * (playerSprite->xVeloc < 0);

                if (playerSprite->yVeloc)
                    playerSprite->yVeloc -= 6 - 12 * (playerSprite->yVeloc < 0);

                if (checkSKInteract || swordTimer)
                {
                    if (checkSKInteract && !swordTimer)
                        SWING_CHANNEL = Mix_PlayChannel(-1, SWING_SOUND, 0);
                    int xDir = (playerSprite->lastDirection / 4) % 3;  //mod 3 to get rid of a value of 3 -- 3 == both directions pressed, or 0 movement
                    int yDir = (playerSprite->lastDirection - xDir * 4) % 3 - 1;  //subtract 1 to turn either 0, 1, or 2 into either -1, 0, or 1
                    if ((xDir -= 1) != -1)
                        xDir -= !xDir;  //turns 0 and 1 into -1 and 1
                    else
                        xDir = 0;

                    if (yDir != -1)
                        yDir -= !yDir;
                    else
                        yDir = 0;
                    yDir *= !xDir;  //x direction takes precedence over the y direction
                    initSprite(&sword, playerSprite->spr.x + TILE_SIZE * xDir, playerSprite->spr.y + (TILE_SIZE + 2) * yDir, TILE_SIZE, SWORD_ID, 90 * yDir, SDL_FLIP_HORIZONTAL * (xDir == -1), type_na);

                    if (!swordTimer)
                        swordTimer = SDL_GetTicks() + 750;
                }

                if (collisionData[0] || ((collisionData[4] && doorFlags[0] == true) || (collisionData[5] && doorFlags[1] == true) || (collisionData[6] && doorFlags[2] == true)))
                {  //unwalkable tile or closed door
                    playerSprite->spr.y = lastY;
                    playerSprite->spr.x = lastX;
                    playerSprite->xVeloc = 0;
                    playerSprite->yVeloc = 0;
                    //printf("%d\n", exitCode);
                }

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
                    {
                        quit = true;
                        exitCode = 2;
                        playerSprite->xVeloc = 0;
                        playerSprite->yVeloc = 0;
                    }
                }

                if (collisionData[1] || collisionData[2] || collisionData[3])   //door button
                {
                    bool playSound = false;
                    for(int i = 0; i < 3; i++)
                    {
                        if (collisionData[i + 1])
                        {
                            if (doorFlags[i])
                                playSound = true;
                            doorFlags[i] = false;
                        }
                    }
                    if (playSound)
                        Mix_PlayChannel(-1, DOOROPEN_SOUND, 0);
                }
                if (collisionData[7])  //teleporter
                {
                    bool found = false;
                    for(int i = 0; i < maxTheseScripts; i++)
                    {
                        if (theseScripts[i].action == script_use_teleporter && SDL_HasIntersection(&((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}), &((SDL_Rect){.x = theseScripts[i].x, .y = theseScripts[i].y, .w = theseScripts[i].w, .h = theseScripts[i].h})))  //not using faster collision bc some scripts might be width != 48
                            {
                                thisScript = theseScripts[i];
                                found = true;
                                break;
                            }
                    }
                    thisScript.active = found;
                }
                if (collisionData[8] && !playerSprite->invincCounter)
                {
                    playerSprite->xVeloc -= 24 * (checkSKRight - checkSKLeft);
                    playerSprite->yVeloc -= 24 * (checkSKDown - checkSKUp);
                    script hurtPlayer;
                    initScript(&hurtPlayer, script_player_hurt, 0, 0, 0, 0, 0, "1");
                    executeScriptAction(&hurtPlayer, playerSprite);
                    playerSprite->invincCounter = 5;  //10 frames of invincibility at 60fps, or approx. .167 of a second
                }
                if (collisionData[9])  //gateway
                {
                    bool found = false;
                    for(int i = 0; i < maxTheseScripts; i++)
                    {
                        if (theseScripts[i].action == script_use_gateway && SDL_HasIntersection(&((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}), &((SDL_Rect){.x = theseScripts[i].x, .y = theseScripts[i].y, .w = theseScripts[i].w, .h = theseScripts[i].h})))  //not using faster collision bc some scripts might be width != 48
                            {
                                thisScript = theseScripts[i];
                                found = true;
                                break;
                            }
                    }
                    thisScript.active = found;
                    //initScript(&thisScript, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, script_use_portal, "[0/456/336]\0");
                    playerSprite->extraData = mapFilePath;
                    exitCode = 2;
                }
            }
            {
                bool playHitSound = false;
                for(int i = 0; i < enemyCount; i++)
                {
                    bool collidedOnce = false;

                    if (checkRectCol(sword.x, sword.y, enemies[i].x, enemies[i].y) && swordTimer > SDL_GetTicks() + 250 && enemies[i].type == type_enemy)  //sword collision
                    {
                        if (enemies[i].angle == false || enemies[i].angle < SDL_GetTicks() + 250)
                        {
                            enemies[i].h--;
                            playHitSound = true;
                            if (enemies[i].h < 1 && (enemies[i].angle == false || enemies[i].angle < SDL_GetTicks() + 250))
                            {
                                enemies[i].type = type_na;
                                enemyFlags[i] = false;
                            }
                            enemies[i].angle = swordTimer;
                        }
                    }

                    if (!collidedOnce && checkRectCol(playerSprite->spr.x, playerSprite->spr.y, enemies[i].x, enemies[i].y) && enemies[i].type == type_enemy && !(playerSprite->invincCounter))  //player collision
                    {
                         script hurtPlayer;
                         initScript(&hurtPlayer, script_player_hurt, 0, 0, 0, 0, 0, enemies[i].tileIndex != ENEMY(3) ? "1" : "2");
                         playerSprite->xVeloc += 24 * (abs(playerSprite->spr.x - enemies[i].x) > abs(playerSprite->spr.y - enemies[i].y))
                         - 48 * (enemies[i].x > playerSprite->spr.x && (abs(playerSprite->spr.x - enemies[i].x) > abs(playerSprite->spr.y - enemies[i].y)));

                         playerSprite->yVeloc += 24 * (abs(playerSprite->spr.y - enemies[i].y) > abs(playerSprite->spr.x - enemies[i].x))
                         - 48 * (enemies[i].y > playerSprite->spr.y && (abs(playerSprite->spr.y - enemies[i].y) > abs(playerSprite->spr.x - enemies[i].x)));
                         executeScriptAction(&hurtPlayer, playerSprite);
                         playerSprite->invincCounter = 11;  //22 frames of invincibility at 60fps, or approx. .367 of a second
                         collidedOnce = true;
                    }

                    if (enemies[i].tileIndex == ENEMY(1) && enemies[i].type == type_enemy)
                    {
                        //behavior: move quickly at player, with little HP
                        int length = 0;
                        node* nodeArray = BreadthFirst(enemies[i].x, enemies[i].y, playerSprite->spr.x, playerSprite->spr.y, &length, debugDrawPath);
                        /*if (enemies[i].x != playerSprite->spr.x)
                            enemies[i].x += 3 - 6 * (playerSprite->spr.x < enemies[i].x);
                        if (enemies[i].y != playerSprite->spr.y)
                            enemies[i].y += 3 - 6 * (playerSprite->spr.y < enemies[i].y);*/
                        if (length > 0 && (enemies[i].angle == false || enemies[i].angle < SDL_GetTicks() + 250))
                        {
                            if (nodeArray[1].x < enemies[i].x)
                                enemies[i].flip = SDL_FLIP_HORIZONTAL;
                            else
                                enemies[i].flip = SDL_FLIP_NONE;

                            if (enemies[i].x != nodeArray[1].x)  //nodeArray[1] -> next tile
                                enemies[i].x += 3 - 6 * (nodeArray[1].x < enemies[i].x);
                            if (enemies[i].y != nodeArray[1].y)
                                enemies[i].y += 3 - 6 * (nodeArray[1].y < enemies[i].y);

                        }
                        free(nodeArray);
                    }

                    if (enemies[i].tileIndex == ENEMY(2) && enemies[i].type == type_enemy)
                    {
                        //behavior: burst movement towards player?
                        if (enemies[i].angle == false || enemies[i].angle < SDL_GetTicks() + 250)
                        {
                            if (playerSprite->spr.x < enemies[i].x)
                                enemies[i].flip = SDL_FLIP_HORIZONTAL;
                            else
                                enemies[i].flip = SDL_FLIP_NONE;

                            if (enemies[i].x != playerSprite->spr.x)
                                enemies[i].x += 2 - 4 * (playerSprite->spr.x < enemies[i].x);
                            if (enemies[i].y != playerSprite->spr.y)
                                enemies[i].y += 2 - 4 * (playerSprite->spr.y < enemies[i].y);
                        }
                    }

                    if (enemies[i].tileIndex == ENEMY(3) && enemies[i].type == type_enemy)
                    {
                        //behavior: move slowly at player, matching up x coord first then y, w/ lot of HP
                        int length = 0;
                        static node curNode;
                        if (!length || (curNode.x == enemies[i].x && curNode.y == enemies[i].y))
                        {
                            node* nodeArray = BreadthFirst(enemies[i].x, enemies[i].y, playerSprite->spr.x, playerSprite->spr.y, &length, debugDrawPath);
                            if (length > 0)
                            {
                                curNode = nodeArray[1];
                                free(nodeArray);
                            }
                            else
                            {
                                if (!length)
                                {
                                    initNode(&curNode, enemies[i].x, enemies[i].y, NULL, true, 0);
                                    enemies[i].angle = 1;  //bit of trickery to force enemies without a path to stop moving
                                }
                            }
                        }
                        if (length > 0 && (enemies[i].angle == false || enemies[i].angle < SDL_GetTicks() + 250))
                        {
                            if (curNode.x < enemies[i].x)
                                enemies[i].flip = SDL_FLIP_HORIZONTAL;
                            else
                                enemies[i].flip = SDL_FLIP_NONE;

                            if (enemies[i].x != curNode.x)
                                enemies[i].x += 3 - 6 * (curNode.x < enemies[i].x);
                            if (enemies[i].y != curNode.y)
                                enemies[i].y += 3 - 6 * (curNode.y < enemies[i].y);
                        }
                    }
                }
                if (playHitSound)
                    Mix_PlayChannel(-1, ENEMYHURT_SOUND, 0);
            }
            {
                for(int i = 0; i < maxTheseScripts; i++)
                {
                    if (SDL_HasIntersection(&((SDL_Rect) {.x = theseScripts[i].x, .y = theseScripts[i].y, .w = theseScripts[i].w, .h = theseScripts[i].h}), &((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h})) && (theseScripts[i].action != script_use_gateway || theseScripts[i].action != script_use_teleporter))
                    {
                        thisScript = theseScripts[i];
                        thisScript.active = true;
                        break;
                    }
                }
            }
            if (playerSprite->invincCounter)
                playerSprite->invincCounter--;
            lastUpdateTime = SDL_GetTicks();

            if (checkSKMenu)
            {
                quit = true;
                exitCode = 1;
            }
            //printf("%d / %f == %d\n", frame, (SDL_GetTicks() - startTime) / 1000.0, framerate);
            drawFPS = keyStates[SDL_SCANCODE_F12];
            debugDrawPath = keyStates[SDL_SCANCODE_RSHIFT];

            if (keyStates[SDL_SCANCODE_Z])
            {
                initScript(&thisScript, script_use_teleporter, 0, 0, 0, 0, 0, "[96/96]\0");
            }
            //printf("Framerate: %d\n", frame / ((int) now - (int) startTime));
        }

        if (swordTimer && SDL_GetTicks() >= swordTimer)
            swordTimer = 0;

        frame++;
        //if ((SDL_GetTicks() - startTime) % 250 == 0)
        framerate = (int) (frame / ((SDL_GetTicks() - startTime) / 1000.0));

	if (drawFPS)
	    drawText(intToString(framerate, whatever), 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF}, false);

        drawATile(tilesTexture, tileIDArray[(playerSprite->animationCounter > 0)], playerSprite->spr.x, playerSprite->spr.y, playerSprite->spr.w, playerSprite->spr.h, playerSprite->spr.angle, playerSprite->spr.flip);

        for(int i = 0; i < enemyCount; i++)
        {
            if (enemies[i].type == type_enemy)
                drawATile(tilesTexture, enemies[i].tileIndex, enemies[i].x, enemies[i].y, enemies[i].w, enemies[i].w, 0, enemies[i].flip);
        }

        if (swordTimer > SDL_GetTicks() + 250)
            drawASprite(tilesTexture, sword);
        SDL_RenderPresent(mainRenderer);
        if ((sleepFor = targetTime - (SDL_GetTicks() - lastFrame)) > 0)
            SDL_Delay(sleepFor);  //FPS limiter; rests for (16 - time spent) ms per frame, effectively making each frame run for ~16 ms, or 60 FPS
        lastFrame = SDL_GetTicks();
        if (thisScript.active)
            quit = executeScriptAction(&thisScript, playerSprite);
    }

    if (playerSprite->HP < 1)
    {
        exitCode = 1;
        playerSprite->HP = playerSprite->maxHP;
    }

    free(theseScripts);
    free(collisionData);
    return exitCode;
}

void checkCollision(player* player, int* outputData, int moveX, int moveY, int lastX, int lastY)
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
        if ((((outputData[0] == 1 || outputData[0] == 5) && moveX < 0 && moveY > 0) || ((outputData[0] == 2 || outputData[0] == 10) && moveX > 0 && moveY > 0) || ((outputData[0] == 4 || outputData[0] == 5) && moveX < 0 && moveY < 0) || ((outputData[0] == 8 || outputData[0] == 10) && moveX > 0 && moveY < 0)))
        {  //manually adding y sliding
            outputData[0] = false;
            player->spr.x = lastX;
            player->xVeloc = 0;
        }
        if ((((outputData[0] == 1 || outputData[0] == 3) && moveX > 0 && moveY < 0) || ((outputData[0] == 2 || outputData[0] == 3) && moveX < 0 && moveY < 0) || ((outputData[0] == 4 || outputData[0] == 12) && moveX > 0 && moveY > 0) || ((outputData[0] == 8 || outputData[0] == 12) && moveX < 0 && moveY > 0)))
        {  //manually adding x sliding
            outputData[0] = false;
            player->spr.y = lastY;
            player->yVeloc = 0;
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
            searchIndex = eventmap[y][x] + 5 - (eventmap[y][x] > 0);  //search index for these tiles is beyond HUD/player slots. Minus 1 because there's only 1 index for invis tile but two cases right next to each other that need it
            if (((searchIndex == 9 || searchIndex == 10 || searchIndex == 11) && drawDoors[searchIndex < 12 ? searchIndex - 9 : 0] == false) || (searchIndex == 16 || searchIndex == 17 || searchIndex == 18))  //8,9,10 are the door indexes
                searchIndex = 5;  //5 is index for invis tile
            drawATile(texture, tileIDArray[searchIndex], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        }
    if (rerender)
        SDL_RenderPresent(mainRenderer);
}
