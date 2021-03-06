//#include "outermeSDL.h"
#include "SDLGateway.h"

#define checkSKUp keyStates[SC_UP]
#define checkSKDown keyStates[SC_DOWN]
#define checkSKLeft keyStates[SC_LEFT]
#define checkSKRight keyStates[SC_RIGHT]
#define checkSKSpecial keyStates[SC_SPECIAL]
#define checkSKInteract keyStates[SC_INTERACT]
#define checkSKMenu keyStates[SC_MENU]
#define TILE_ID_PLAYER 16
#define PIXELS_MOVED 6

#define MAX_MAPPACKS_PER_PAGE 11

#define TITLESCREEN_GAMECODE -1
#define START_GAMECODE 0
#define OPTIONS_GAMECODE 1
#define PLAY_GAMECODE 2
#define MAINLOOP_GAMECODE 3
#define OVERWORLDMENU_GAMECODE 4
#define RELOAD_GAMECODE 5
#define SAVE_GAMECODE 6
#define MAX_COLLISIONDATA_ARRAY MAX_SPRITE_MAPPINGS

#define checkSquareCol(x1, y1, x2, y2, w) ((abs(abs(x1) - abs(x2)) < w) && (abs(abs(y1) - abs(y2)) < w))

#define checkRectCol(x1, y1, w1, h1, x2, y2, w2, h2) (x1 < x2 + w2   &&   x1 + w1 > x2   &&   y1 < y2 + h2   &&   h1 + y1 > y2)

int allOptions(player* player);
bool upgradeShop(player* playerSprite);
void changeVolumes();
void soundTestMenu();
int changeControls();
void changeName();
void changeFPS(int newFPS);
void clearData(player* playerSprite);

int mainLoop(player* playerSprite);
void smoothScrolling(player* playerSprite, int newMapLine, int moveX, int moveY);
void checkCollision(player* player, int* outputData, int moveX, int moveY, int lastX, int lastY);
void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag);
void drawSparks(spark* s);

void aMenu_drawMain();
void aMenu_drawMoney();
int gameOver();
void screenTransitions();

int toolchain_main();

#define PLAYER_ID tileIDArray[0]
#define PLAYERWALK_ID tileIDArray[1]
#define CURSOR_ID tileIDArray[2]
#define HP_ID tileIDArray[3]
#define SWORD_ID tileIDArray[4]
#define INVIS_ID tileIDArray[5]
#define ENEMY(x) tileIDArray[15 + x]
#define GOLD_ID tileIDArray[19]

#define MAIN_ARROW_ID 34

#define ALL_TECHNIQUES {"Dash", "Spin", "Illusion", "Laser", "Charge"}

#define HELP_MENU_TEXT1 "Gateway to Legend\nis an Action-Puzzle game. Use (default)\n[WASD]+[Space]\n+[L-Shift]+[Esc]\n to maneuver various worlds."
#define HELP_MENU_TEXT2 "Play and create different map-packs! You can create engaging content and play others' content as well!"
#define HELP_MENU_TEXT3 "Made by:\nStephen Policelli"

enemy bossSprite;
bool loadBoss;
script* allScripts;
int sizeOfAllScripts;

bool debugFlag;
Sint64 gameTicks;

int main(int argc, char* argv[])
{
    strcpy(customError, "Generic error");
    //setting up default values
    {
        int initCode = initSDL(GAME_WINDOW_NAME, GLOBALTILES_FILEPATH, FONT_FILE_NAME, "assets/icon.png", SCREEN_WIDTH, SCREEN_HEIGHT, 48);
        if (initCode != 0)
        {
            printf("%s\n", customError);
            return initCode;
        }
        initCode = initSounds();
        if (initCode != 0)
        {
            printf("%s\n", customError);
            return initCode;
        }
        debugFlag = false;
        if (argc > 1 && (!strncmp(argv[1], "--debug", 7) || !strncmp(argv[1], "-d", 2)))
            debugFlag = true;
    }
    //loading in map pack header files
    char** listOfFilenames;
    int maxStrNum = 0;
    listOfFilenames = getListOfFiles(MAX_LIST_OF_MAPS, MAX_FILE_PATH - 9, MAP_PACKS_SUBFOLDER, &maxStrNum);
    //done loading map pack header files
    player person;
    initPlayer(&person, 0, 0, 0, 0, 0, 0, SDL_FLIP_NONE, 0);
    #if defined(_WIN32)
        mkdir("saves/");
    #else
        mkdir("./saves", 0755);
    #endif //makes ./saves/ if it doesn't exist
    if (checkFile(GLOBALSAVE_FILEPATH, 0))
        loadGlobalPlayer(&person, GLOBALSAVE_FILEPATH);
    else
        createGlobalPlayer(&person, GLOBALSAVE_FILEPATH);
    if (checkFile(CONFIG_FILEPATH, 6))  //load config
        loadConfig(CONFIG_FILEPATH);
    else
        initConfig(CONFIG_FILEPATH);

    createFile(LOG_PATH);

    /*if (debug)
        loadIMG("tileset/eventTile48.png", &eventTexture);*/
    char* buffer = calloc(201, sizeof(char));  //actually needed
    allScripts = NULL;
    int choice = 0;
    SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(mainRenderer);
    int gameState = TITLESCREEN_GAMECODE;
    int lastMap = 0;
    bool quitGame = false;
    while(!quitGame)
    {
        switch(gameState)
        {
        case TITLESCREEN_GAMECODE:
            {
                SDL_Texture* titlescreen;
                loadIMG("assets/splashscreen.png", &titlescreen);
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_TEXTCOLOR);
                int key = 0;
                while(!key)
                //while(key != SDLK_SPACE)  //uncomment for videos
                {
                    SDL_RenderClear(mainRenderer);
                    SDL_RenderCopy(mainRenderer, titlescreen, NULL, NULL);
                    //drawText("Gateway to Legend", 1.5 * TILE_SIZE, 1.5 * TILE_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TITLECOLOR2}, false);
                    //drawText("Press Any Key", 3.5 * TILE_SIZE, 12 * TILE_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TITLECOLOR2}, false);
                    key = getKey(true);
                    SDL_RenderPresent(mainRenderer);
                }
                SDL_DestroyTexture(titlescreen);
                Mix_PlayChannel(-1, SWING_SOUND, 0);
                Mix_PlayChannel(-1, ENEMYHURT_SOUND, 0);
                if (key == ANYWHERE_QUIT)
                quitGame = true;
            }
            gameState = START_GAMECODE;
            break;
        case START_GAMECODE:  //start menu
            if (!Mix_PlayingMusic())
                playMainMusic();
            person.mapScreen = 0;  //todo: maybe add one more option to main menu?
            choice = aMenu(tilesetTexture, MAIN_ARROW_ID, "Gateway to Legend", (char*[5]) {"Play", "Upgrade Shop", "Toolchain", "Settings", "Quit"}, 5, 1, AMENU_MAIN_THEME, true, true, &(aMenu_drawMain));
            if (choice == 1)
                gameState = PLAY_GAMECODE;
            if (choice == 2)
            {
                if (upgradeShop(&person))
                    quitGame = true;
            }
            if (choice == 3)
                choice = toolchain_main();
            if (choice == 4)
                gameState = OPTIONS_GAMECODE;
            if (choice == 5 || choice == -1)
                quitGame = true;
            break;
        case OPTIONS_GAMECODE:
            {
                int choice = allOptions(&person);
                if (choice == 0)
                    gameState = START_GAMECODE;
                if (choice == -1)
                    quitGame = true;
            }
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
            uniqueReadLine((char**) &mapFilePath, MAX_FILE_PATH - 1, mainFilePath, 1);
            //printf("%s\n", mapFilePath);
            uniqueReadLine((char**) &tileFilePath, MAX_FILE_PATH - 1, mainFilePath, 2);
            //printf("%s\n", tileFilePath);
            uniqueReadLine((char**) &saveFilePath, MAX_FILE_PATH - 1, mainFilePath, 3);
            //printf("%s\n", saveFilePath);
            uniqueReadLine((char**) &scriptFilePath, MAX_FILE_PATH - 1, mainFilePath, 4);
            //printf("%s\n", scriptFilePath);
            loadIMG(tileFilePath, &tilesTexture);
            free(allScripts);
            for(int i = 0; i < MAX_SPRITE_MAPPINGS; i++)
            {
                tileIDArray[i] = strtol(readLine(mainFilePath, 8 + i, &buffer), NULL, 10);
            }
            maxBosses = strtol(readLine(mainFilePath, 8 + MAX_SPRITE_MAPPINGS, &buffer), NULL, 10);
            quitGame = aMenu(tilesTexture, CURSOR_ID, readLine(mainFilePath, 0, &buffer), (char*[3]) {"New Game", "Load Game", "Back"}, 3, 2, AMENU_MAIN_THEME, true, false, NULL);
            if (quitGame == 3 || quitGame == -1)
            {
                quitGame = (quitGame == -1);
                break;
            }
            logWrite(mainFilePath);
            allScripts = calloc(checkFile(scriptFilePath, -1) + 2, sizeof(script));
            if (checkFile(scriptFilePath, -1))
            {
                for(int i = 0; i < checkFile(scriptFilePath, -1) + 1; i++)
                {
                    script thisScript;
                    readScript(&thisScript, readLine(scriptFilePath, i, &buffer), i);
                    allScripts[i] = thisScript;
                    sizeOfAllScripts = i + 1;
                }
            }

            if (checkFile(saveFilePath, 0) && quitGame == 2)
                loadLocalPlayer(&person, saveFilePath, PLAYER_ID);
            else
                createLocalPlayer(&person, saveFilePath, strtol(readLine(mainFilePath, 5, &buffer), NULL, 10), strtol(readLine(mainFilePath, 6, &buffer), NULL, 10), TILE_SIZE, TILE_SIZE, strtol(readLine(mainFilePath, 7, &buffer), NULL, 10), 0, SDL_FLIP_NONE, PLAYER_ID, sizeOfAllScripts);
            quitGame = false;

            for(int i = 0; i < sizeOfAllScripts; i++)
            {
                if (person.disabledScripts[i])
                    allScripts[i].disabled = true;
            }

            //done loading map-pack specific stuff
            if (checkFile(GLOBALSAVE_FILEPATH, 0))
                loadGlobalPlayer(&person, GLOBALSAVE_FILEPATH);  //loaded twice just to ensure nothing is overwritten?
            else
                createGlobalPlayer(&person, GLOBALSAVE_FILEPATH);

            //initializing game
            person.lastMap = strtol(readLine(saveFilePath, 4, &buffer), NULL, 10);
            person.lastX = strtol(readLine(saveFilePath, 5, &buffer), NULL, 10);
            person.lastY = strtol(readLine(saveFilePath, 6, &buffer), NULL, 10);
            initEnemy(&bossSprite, -TILE_SIZE, -TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 1, type_boss);
            loadBoss = true;
            noclip = false;
            sparkFlag = false;
            for(int i = 0; i < MAX_SPARKS; i++)
            {
                theseSparkFlags[i] = false;
                initSpark(&theseSparks[i], (SDL_Rect) {0, 0, 0, 0}, (SDL_Color) {0, 0, 0, 0}, 1, 6, 6, 10, 1);
            }

            for(int i = 0; i < MAX_ENEMIES; i++)
                initEnemy(&enemies[i], 0, 0, 0, 0, INVIS_ID, 0, type_na);

            //done game init
            screenTransitions();
            musicIndex = 0;
            playOverworldMusic();
            choice = 3;
            gameState = RELOAD_GAMECODE;
            break;
        case MAINLOOP_GAMECODE:  //main game loop
            loadMapFile(mapFilePath, tilemap, eventmap, person.mapScreen, HEIGHT_IN_TILES, WIDTH_IN_TILES);
            snprintf(buffer, 300, "Loaded map ID %d from %s", person.mapScreen, mapFilePath);
            logWrite(buffer);
            person.extraData = mapFilePath;
            gameTicks = SDL_GetTicks();
            lastMap = person.mapScreen;
            choice = mainLoop(&person);
            if (choice == ANYWHERE_QUIT)
                quitGame = true;
            if (choice == 1)
                gameState = OVERWORLDMENU_GAMECODE;
            if (choice == 2 || choice == 3 || choice == 4)  //2 is normal transition quit, 3 is special quit, 4 is quit with loading screen
                gameState = RELOAD_GAMECODE;
            if (choice == 5)  //go back to main menu
                gameState = START_GAMECODE;
            break;
        case OVERWORLDMENU_GAMECODE:  //overworld menu
            if (choice)  //if we're coming back from another selection, don't stop music
            {
                Mix_VolumeMusic(musicVolume / 3);
                Mix_HaltChannel(-1);
                Mix_PlayChannel(-1, PAUSE_SOUND, 0);
            }
            choice = aMenu(tilesTexture, CURSOR_ID, "Overworld Menu", (char*[4]) {"Back", "Save", "Options", "Exit"}, 4, 1, AMENU_MAIN_THEME, true, false, NULL);
            if (choice == 1)
                gameState = MAINLOOP_GAMECODE;
            if (choice == 3)
                choice = allOptions(&person);
            if (choice == 2 || choice == 4 || choice == -1)
            {
                gameState = SAVE_GAMECODE;
                if (choice == 4)
                    choice = 3;
            }
            if (gameState != OVERWORLDMENU_GAMECODE)
                Mix_VolumeMusic(musicVolume);
            break;
        case RELOAD_GAMECODE:
            gameState = MAINLOOP_GAMECODE;
            for(int i = 0; i < 4; i++)
                doorFlags[i] = true;
            person.invincCounter = 0;
            loadBoss = true;
            bossLoaded = false;
			initSprite(&animationSpr, 0, 0, TILE_SIZE, TILE_SIZE, INVIS_ID, 0, SDL_FLIP_NONE, type_generic);
            if (choice == 2)
                smoothScrolling(&person, person.mapScreen, 2 * ((person.mapScreen - lastMap) % 10), 2 * ((person.mapScreen - lastMap) / 10));
            if (choice == 4)
                screenTransitions();

            for(int i = 0; i < MAX_ENEMIES + 1; i++)  //reset enemy flags
            {
                if (i < MAX_ENEMIES)
                    initEnemy(&enemies[i], 0, 0, 0, 0, INVIS_ID, 0, type_na);
                enemyFlags[i] = true;
            }
            person.lastMap = person.mapScreen;
            person.lastX = person.spr.x;
            person.lastY = person.spr.y;
            break;
        case SAVE_GAMECODE:
            saveLocalPlayer(person, saveFilePath);
            if (choice == 2)
                gameState = MAINLOOP_GAMECODE;
            if (choice == 3 || choice == -1)
            {
                for(int i = 0; i < 4; i++)  //reset door flags
                    doorFlags[i] = true;
                for(int i = 0; i < MAX_ENEMIES + 1; i++)  //reset enemy flags
                {
                    if (i < MAX_ENEMIES)
                        initEnemy(&enemies[i], 0, 0, 0, 0, INVIS_ID, 0, type_na);
                    enemyFlags[i] = true;
                }
                initEnemy(&bossSprite, -TILE_SIZE, -TILE_SIZE, 0, 0, 0, 1, type_boss);
                loadBoss = true;
                person.lastMap = -1;
                person.lastX = -1;
                person.lastY = -1;
                script resetScript;
                initScript(&resetScript, script_boss_actions, 0, 0, 0, 0, 0, "r", -1);
                executeScriptAction(&resetScript, &person);  //resets boss movement timer
                noclip = false;
                sparkFlag = false;
                for(int i = 0; i < MAX_SPARKS; i++)
                {
                    theseSparkFlags[i] = false;
                    initSpark(&theseSparks[i], (SDL_Rect) {0, 0, 0, 0}, (SDL_Color) {0, 0, 0, 0}, 1, 6, 6, 10, 1);
                }
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
    SDL_DestroyTexture(tilesetTexture);
    free(allScripts);
	free(listOfFilenames);
    closeSDL();
    return 0;
}

//returns -1 if force quit, returns -2 if back, else 0
int allOptions(player* player)
{
    int choice = 0;
    while(choice != -1 && choice != 6)
    {
        choice = aMenu(tilesetTexture, MAIN_ARROW_ID, "Options", (char*[6]) {"Sounds", "Controls", "Change FPS", "Reset Data", "Info/Help", "Back"}, 6, 0, AMENU_MAIN_THEME, true, false, NULL);
        if (choice == 1)
            changeVolumes();

        if (choice == 2)
            choice = changeControls();

        /*if (choice == 3)
            changeName(player);*/  //name isn't used so why have it?

        if (choice == 3)
        {
            int newFPS = intInput("New FPS? 0 -> No Cap", 3, 60, 0, 500, false);  //todo: show old FPS
            if (newFPS > 0 && newFPS < 30)
                newFPS = 30;
            changeFPS(newFPS);
        }

        if (choice == 4)
            clearData(player);

        if (choice == 5)
        {
            int pauseKey = 0;
            char* helpTexts[3] = {HELP_MENU_TEXT1, HELP_MENU_TEXT2, HELP_MENU_TEXT3};
            int helpIndex = 0;
            while(pauseKey != -1 && helpIndex <= 2)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
                SDL_RenderFillRect(mainRenderer, NULL);
                drawText(helpTexts[helpIndex], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                pauseKey = waitForKey(true);
                helpIndex++;
                Mix_PlayChannel(-1, OPTION_SOUND, 0);
            }
        }
    }

    if (choice == 6)
        choice = 0;

    return choice;
}

bool upgradeShop(player* playerSprite)
{
    bool totalQuit = false;
    bool quit = false;
    while(!quit)
    {
        char title[17];
        snprintf(title, 17, "Shop: %d Coins", playerSprite->money);
        int choice = aMenu(tilesetTexture, MAIN_ARROW_ID, title, (char*[3]) {"Extra Health", "Abilities", "Back"}, 3, 0, AMENU_MAIN_THEME, true, false, NULL);
        switch(choice)
        {
        case 1:  //extra health
            {
                {
                    const int coinsPerHalfHeart = 16;
                    char textString[25];
                    SDL_Color textColor = (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, bgColor = (SDL_Color) {AMENU_MAIN_BGCOLOR}, titleOverColor = (SDL_Color) {AMENU_MAIN_TITLECOLOR1};
                    sprite cursor;
                    initSprite(&cursor, TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, MAIN_ARROW_ID, 0, SDL_FLIP_NONE, (entityType) type_na);
                    SDL_Event e;
                    bool iQuit = false;
                    while(!iQuit)
                    {
                        SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
                        SDL_RenderClear(mainRenderer);
                        SDL_RenderFillRect(mainRenderer, NULL);
                        SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
                        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
                        //background text (drawn first)
                        drawText("Increase Max HP", 1 * TILE_SIZE + 3 * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, (SDL_Color) {AMENU_MAIN_TITLECOLOR2}, false);
                        //foreground text
                        drawText("Increase Max HP", 1 * TILE_SIZE + TILE_SIZE / 4 , 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, (SDL_Color) {AMENU_MAIN_TITLECOLOR1}, false);

                        SDL_SetRenderDrawColor(mainRenderer, titleOverColor.r, titleOverColor.g, titleOverColor.b, 0xFF);
                        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 11.25 * TILE_SIZE, .y = 5 * TILE_SIZE, .w = (playerSprite->maxHP - DEFAULT_PLAYER_HEALTH) * 16 + TILE_SIZE / 2, .h = .75 * TILE_SIZE}));

                        SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
                        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 11.5 * TILE_SIZE, .y = 5.25 * TILE_SIZE, .w = (playerSprite->maxHP - DEFAULT_PLAYER_HEALTH) * 16, .h = TILE_SIZE / 2}));

                        drawText("Increase:", 2.25 * TILE_SIZE, 5 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
                        snprintf(textString, 12, "%.1f Hearts", playerSprite->maxHP / 4.0);
                        drawText(textString, 2.25 * TILE_SIZE, 6 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);

                        drawText("Back", 2.25 * TILE_SIZE, 7 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);

                        snprintf(textString, 12, "%d Coins", playerSprite->money);
                        drawText(textString, 2.25 * TILE_SIZE, 9 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);

                        snprintf(textString, 24, "%d Coins per half-heart", coinsPerHalfHeart);
                        drawText(textString, 2.25 * TILE_SIZE, 10.5 * TILE_SIZE, SCREEN_WIDTH, 2 * TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);

                        while(SDL_PollEvent(&e) != 0)
                        {
                            //User requests quit
                            if(e.type == SDL_QUIT)
                            {
                                iQuit = true;
                                quit = true;
                                totalQuit = true;
                            }
                            //User presses a key
                            else
                            {
                                if(e.type == SDL_KEYDOWN)
                                {
                                    if (e.key.keysym.scancode == SC_UP && cursor.y > 5 * TILE_SIZE)
                                    {
                                        cursor.y -= TILE_SIZE;
                                        Mix_PlayChannel(-1, PING_SOUND, 0);
                                    }

                                    if (e.key.keysym.scancode == SC_DOWN && cursor.y < 7 * TILE_SIZE)
                                    {
                                        cursor.y += TILE_SIZE;
                                        Mix_PlayChannel(-1, PING_SOUND, 0);
                                    }

                                    if (e.key.keysym.scancode == SC_INTERACT)
                                    {
                                        if (cursor.y == TILE_SIZE * 7)
                                        {
                                            iQuit = true;
                                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                                        }
                                        if (cursor.y == TILE_SIZE * 5 && playerSprite->maxHP != MAX_PLAYER_HEALTH && playerSprite->money >= coinsPerHalfHeart)
                                        {
                                            playerSprite->maxHP += 2;
                                            playerSprite->money -= coinsPerHalfHeart;
                                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                                        }
                                    }
                                }
                                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                                {

                                    int choice = (e.button.y / TILE_SIZE) - 4;
                                    if (choice == 3)
                                    {
                                        iQuit = true;
                                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                                    }
                                    if (choice == 1 && playerSprite->maxHP != MAX_PLAYER_HEALTH && playerSprite->money >= coinsPerHalfHeart)
                                    {
                                        playerSprite->maxHP += 2;
                                        playerSprite->money -= coinsPerHalfHeart;
                                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                                    }
                                }
                                if (e.type == SDL_MOUSEMOTION)
                                {
                                    if (e.motion.y / TILE_SIZE == 5 || e.motion.y / TILE_SIZE == 7)
                                        cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                                }
                            }
                        }
                        drawATile(tilesetTexture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
                        SDL_RenderPresent(mainRenderer);
                    }
                        saveGlobalPlayer(*playerSprite, GLOBALSAVE_FILEPATH);
                }
            }
            break;
        case 2:  //techniques
            {
                const int coinsPerTech = 40;
                char* literalsArray[MAX_PLAYER_TECHNIQUES] = ALL_TECHNIQUES;
                bool eQuit = false;
                int selection = 0;
                while(!eQuit)
                {
                    char* techniqueArray[MAX_PLAYER_TECHNIQUES + 2];
                    int posArray[MAX_PLAYER_TECHNIQUES];
                    int nextPos = 0;
                    for(int i = 0; i < MAX_PLAYER_TECHNIQUES; i++)
                    {
                        if (playerSprite->techUnlocks[i])
                        {
                            techniqueArray[nextPos] = calloc(23, sizeof(char));
                            posArray[nextPos] = i;
                            strncpy(techniqueArray[nextPos++], literalsArray[i], 11);
                            if (playerSprite->techUnlocks[i] > 1)
                                strncat(techniqueArray[nextPos - 1], " -Un-Equip", 22); //11 + 11
                            else
                                strncat(techniqueArray[nextPos - 1], " -Equip", 19); //11 + 8
                        }
                    }
                    techniqueArray[nextPos++] = "Buy";
                    techniqueArray[nextPos++] = "Back";

                    selection = aMenu(tilesetTexture, MAIN_ARROW_ID, "Abilities", techniqueArray, nextPos, selection, AMENU_MAIN_THEME, true, false, NULL);

                    if (selection == ANYWHERE_QUIT || selection == nextPos)
                    {
                        eQuit = true;
                        if (selection == ANYWHERE_QUIT)
                        {
                            quit = true;
                            totalQuit = true;
                        }
                    }
                    if (selection == nextPos - 1)
                    {
                        bool bQuit = false;
                        while(!bQuit)
                        {
                            char* techniqueArray[MAX_PLAYER_TECHNIQUES + 1];
                            int nextPos = 0;
                            for(int i = 0; i < MAX_PLAYER_TECHNIQUES; i++)
                            {
                                if (!playerSprite->techUnlocks[i])
                                    techniqueArray[nextPos++] = literalsArray[i];
                            }
                            techniqueArray[nextPos++] = "Back";
                            _globalInt1 = playerSprite->money;
                            _globalInt2 = coinsPerTech;
                            int retCode = aMenu(tilesetTexture, MAIN_ARROW_ID, "Buy Abilities", (char**) techniqueArray, nextPos, 0, AMENU_MAIN_THEME, true, false, aMenu_drawMoney);
                            _globalInt1 = 0;
                            _globalInt2 = 0;
                            if (retCode == ANYWHERE_QUIT || retCode == nextPos)
                            {
                                bQuit = true;
                                if (retCode == ANYWHERE_QUIT)
                                {
                                    quit = true;
                                    totalQuit = true;
                                }
                            }

                            if (retCode != ANYWHERE_QUIT && retCode < nextPos && playerSprite->money >= coinsPerTech)
                            {
                                playerSprite->money -= coinsPerTech;
                                int position = 0;
                                for(int i = 0; i < MAX_PLAYER_TECHNIQUES; i++)
                                {
                                    if (strcmp(techniqueArray[retCode - 1], literalsArray[i]) == 0)
                                    {
                                        position = i;
                                        break;
                                    }
                                }
                                playerSprite->techUnlocks[position] = true;
                                saveGlobalPlayer(*playerSprite, GLOBALSAVE_FILEPATH);
                            }
                        }
                    }
                    else if (selection != nextPos)
                    {
                        playerSprite->techUnlocks[posArray[selection - 1]] = 1 + !(playerSprite->techUnlocks[posArray[selection - 1]] - 1);  //flips between 2 and 1
                        if (playerSprite->techUnlocks[posArray[selection - 1]] == 2 && posArray[selection - 1] > 0)
                        {
                            for(int i = 1; i < MAX_PLAYER_TECHNIQUES; i++)
                            {
                                if (i != posArray[selection - 1] && playerSprite->techUnlocks[i] == 2)
                                {
                                    playerSprite->techUnlocks[i] = 1;
                                }
                            }
                        }
                    }
                    for(int i = 0; i < nextPos - 2; i++)
                    {
                        free(techniqueArray[i]);
                    }
                }
                saveGlobalPlayer(*playerSprite, GLOBALSAVE_FILEPATH);
            }
            break;
        case 3:
        default:
            quit = true;
            break;
        case -1:
            quit = true;
            totalQuit = true;
            break;
        }
    }
    return totalQuit;
}

void changeVolumes()
{
    SDL_Color textColor = (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, bgColor = (SDL_Color) {AMENU_MAIN_BGCOLOR}, titleOverColor = (SDL_Color) {AMENU_MAIN_TITLECOLOR1};
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, MAIN_ARROW_ID, 0, SDL_FLIP_NONE, (entityType) type_na);
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
        drawText("Sound Volumes", 1 * TILE_SIZE + 3 * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, (SDL_Color) {AMENU_MAIN_TITLECOLOR2}, false);
        //foreground text
        drawText("Sound Volumes", 1 * TILE_SIZE + TILE_SIZE / 4 , 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, (SDL_Color) {AMENU_MAIN_TITLECOLOR1}, false);

        SDL_SetRenderDrawColor(mainRenderer, titleOverColor.r, titleOverColor.g, titleOverColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.25 * TILE_SIZE, .y = 5 * TILE_SIZE, .w = musicVolume * 4 + TILE_SIZE / 2, .h = .75 * TILE_SIZE}));
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.25 * TILE_SIZE, .y = 6 * TILE_SIZE, .w = soundVolume * 4 + TILE_SIZE / 2, .h = .75 * TILE_SIZE}));

        SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.5 * TILE_SIZE, .y = 5.25 * TILE_SIZE, .w = musicVolume * 4, .h = TILE_SIZE / 2}));
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.5 * TILE_SIZE, .y = 6.25 * TILE_SIZE, .w = soundVolume * 4, .h = TILE_SIZE / 2}));

        drawText("Music", 2.25 * TILE_SIZE, 5 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
        drawText("Sounds", 2.25 * TILE_SIZE, 6 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);

        SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.5 * TILE_SIZE, .y = 5.25 * TILE_SIZE, .w = 3 * TILE_SIZE / 4, .h = TILE_SIZE / 4}));  // -

        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 220 * 4, .y = 5.25 * TILE_SIZE, .w = 3 * TILE_SIZE / 4, .h = TILE_SIZE / 4}));  // -
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 223 * 4, .y = 5 * TILE_SIZE, .w = TILE_SIZE / 4, .h = 3 * TILE_SIZE / 4}));  // |

        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 8.5 * TILE_SIZE, .y = 6.25 * TILE_SIZE, .w = 3 * TILE_SIZE / 4, .h = TILE_SIZE / 4}));  // -

        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 220 * 4, .y = 6.25 * TILE_SIZE, .w = 3 * TILE_SIZE / 4, .h = TILE_SIZE / 4}));  // -
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 223 * 4, .y = 6 * TILE_SIZE, .w = TILE_SIZE / 4, .h = 3 * TILE_SIZE / 4}));  // |

        drawText("Sound Test", 2.25 * TILE_SIZE, 7 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
        drawText("Back", 2.25 * TILE_SIZE, 8 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);

        while(SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if(e.type == SDL_QUIT)
                quit = true;
            //User presses a key
            else
            {
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

                    if (e.key.keysym.scancode == SC_LEFT  && cursor.y < TILE_SIZE * 7)
                    {
                        if (cursor.y == TILE_SIZE * 5 && musicVolume > 0)
                        {
                            musicVolume -= 16;
                            Mix_VolumeMusic(musicVolume);
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }
                        else if (cursor.y == TILE_SIZE * 6 && soundVolume > 0)
                        {
                            soundVolume -= 16;
                            Mix_Volume(-1, soundVolume);
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }
                    }

                    if (e.key.keysym.scancode == SC_RIGHT  && cursor.y < TILE_SIZE * 7)
                    {
                        if (cursor.y == TILE_SIZE * 5 && musicVolume < MIX_MAX_VOLUME)
                        {
                            musicVolume += 16;
                            Mix_VolumeMusic(musicVolume);
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }
                        else if (cursor.y == TILE_SIZE * 6 && soundVolume < MIX_MAX_VOLUME)
                        {
                            soundVolume += 16;
                            Mix_Volume(-1, soundVolume);
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }
                    }

                    if (e.key.keysym.scancode == SC_INTERACT)
                    {
                        Mix_Volume(-1, soundVolume);
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        if (cursor.y == TILE_SIZE * 7)
                            soundTestMenu();
                        if (cursor.y == TILE_SIZE * 8)
                            quit = true;
                    }
                }
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                {

                    int choice = (e.button.y / TILE_SIZE) - 4;
                    if (choice > 0 && choice <= 4)
                    {
                        if (choice < 3)
                        {
                            SDL_Rect minus1 = {8.25 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                            SDL_Rect plus1 = {18 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE};

                            SDL_Rect minus2 = {8.25 * TILE_SIZE, 6 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                            SDL_Rect plus2 = {223 * 4, 6 * TILE_SIZE, TILE_SIZE, TILE_SIZE};

                            if ((e.button.x - minus1.x <= minus1.w && e.button.x - minus1.x > 0) && (e.button.y - minus1.y <= minus1.h && e.button.y - minus1.y > 0) && musicVolume > 0)
                            {
                                musicVolume -= 16;
                                Mix_VolumeMusic(musicVolume);
                                Mix_PlayChannel(-1, PING_SOUND, 0);
                            }

                            if ((e.button.x - plus1.x <= plus1.w && e.button.x - plus1.x > 0) && (e.button.y - plus1.y <= plus1.h && e.button.y - plus1.y > 0) && musicVolume < MIX_MAX_VOLUME)
                            {
                                musicVolume += 16;
                                Mix_VolumeMusic(musicVolume);
                                Mix_PlayChannel(-1, PING_SOUND, 0);
                            }
                            //music

                            if ((e.button.x - minus2.x <= minus2.w && e.button.x - minus2.x > 0) && (e.button.y - minus2.y <= minus2.h && e.button.y - minus2.y > 0) && soundVolume > 0)
                            {
                                soundVolume -= 16;
                                Mix_Volume(-1, soundVolume);
                                Mix_PlayChannel(-1, PING_SOUND, 0);
                            }

                            if ((e.button.x - plus2.x <= plus2.w && e.button.x - plus2.x > 0) && (e.button.y - plus2.y <= plus2.h && e.button.y - plus2.y > 0) && soundVolume < MIX_MAX_VOLUME)
                            {
                                soundVolume += 16;
                                Mix_Volume(-1, soundVolume);
                                Mix_PlayChannel(-1, PING_SOUND, 0);
                            }
                            //sfx
                        }
                        else
                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        if (choice == 3)
                            soundTestMenu();
                        if (choice == 4)
                            quit = true;
                    }
                }
                if (e.type == SDL_MOUSEMOTION)
                {
                    if (e.motion.y / TILE_SIZE > 4 && e.motion.y / TILE_SIZE <= 8)
                        cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
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

void soundTestMenu()
{
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, MAIN_ARROW_ID, 0, SDL_FLIP_NONE, (entityType) type_na);
    const int optionsSize = 23;
    char* optionsArray[] = {"Cancel Playback", "Main Theme", "Overworld 1", "Overworld 2", "Overworld 3", "Boss Theme", "Fanfare", "Gateway to Retry", "Reserved", "Unsheath", "Option", "Cursor", "Step 1", "Step 2", "Step 3", "Sword Swing", "Gateway In", "Gateway Out", "Door", "Coins", "Player Hurt", "Enemy Hurt", "Teleport"};
    int soundIndex = 0, selection = -1;
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
        drawText("Sound Test", 1 * TILE_SIZE + 3 * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, (SDL_Color) {AMENU_MAIN_TITLECOLOR2}, false);
        //foreground text
        drawText("Sound Test", 1.25 * TILE_SIZE , 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, (SDL_Color) {AMENU_MAIN_TITLECOLOR1}, false);

        drawText(optionsArray[soundIndex], 2.25 * TILE_SIZE, 5 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 5) * TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);

        drawText("Back", 2.25 * TILE_SIZE, 6 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 8) * TILE_SIZE, textColor, false);

        //SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.w}));
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                quit = true;
                soundIndex = ANYWHERE_QUIT;
            }
            else
            {
                if (e.type == SDL_KEYDOWN)
                {
                    if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_UP))
                    {
                        if (cursor.y > 5 * TILE_SIZE)
                            cursor.y -= TILE_SIZE;
                        Mix_PlayChannel(-1, PING_SOUND, 0);
                    }

                    if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_DOWN))
                    {
                        if (cursor.y < 6 * TILE_SIZE)
                            cursor.y += TILE_SIZE;
                        Mix_PlayChannel(-1, PING_SOUND, 0);
                    }

                    if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_LEFT) && cursor.y == 5 * TILE_SIZE && soundIndex > 0)
                    {
                        soundIndex--;
                        Mix_PlayChannel(-1, PING_SOUND, 0);
                    }

                    if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_RIGHT) && cursor.y == 5 * TILE_SIZE && soundIndex < optionsSize - 1)
                    {
                        soundIndex++;
                        Mix_PlayChannel(-1, PING_SOUND, 0);
                    }

                    if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_INTERACT))
                    {
                        selection = cursor.y / TILE_SIZE - 4;
                        if (selection == 2)
                            quit = true;
                        if (selection == 1)
                        {
                            Mix_HaltMusic();
                            if (soundIndex != 0)
                            {
                                Mix_HaltChannel(-1);

                                if (soundIndex < 8)
                                    Mix_PlayMusic(MUSIC((musicIndex = soundIndex)), (soundIndex != 6 ? -1 : 0));
                                else
                                    Mix_PlayChannel(-1, audioArray[soundIndex - 8], 0);
                            }
                        }
                        else
                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                    }
                }
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                {

                    int choice = (e.button.y / TILE_SIZE) - 4;
                    if (choice > 0 && choice <= 2)
                    {
                        if (choice == 1)
                        {
                            Mix_HaltMusic();
                            bool arrows = false;
                            SDL_Rect minus = {TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                            SDL_Rect plus = {18 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE};

                            if ((e.button.x - minus.x <= minus.w && e.button.x - minus.x > 0) && (e.button.y - minus.y <= minus.h && e.button.y - minus.y > 0) && soundIndex > 0)
                            {
                                soundIndex--;
                                Mix_PlayChannel(-1, PING_SOUND, 0);
                                arrows = true;
                            }

                            if ((e.button.x - plus.x <= plus.w && e.button.x - plus.x > 0) && (e.button.y - plus.y <= plus.h && e.button.y - plus.y > 0) && soundIndex < optionsSize - 1)
                            {
                                soundIndex++;
                                Mix_PlayChannel(-1, PING_SOUND, 0);
                                arrows = true;
                            }

                            if (soundIndex != 0 && !arrows)
                            {
                                Mix_HaltChannel(-1);

                                if (soundIndex < 8)
                                    Mix_PlayMusic(MUSIC((musicIndex = soundIndex)), (soundIndex != 6 ? -1 : 0));
                                else
                                    Mix_PlayChannel(-1, audioArray[soundIndex - 8], 0);
                            }
                        }
                        else
                        {
                            quit = true;
                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        }
                    }
                }
                if (e.type == SDL_MOUSEMOTION)
                {
                    if (e.motion.y / TILE_SIZE > 4 && e.motion.y / TILE_SIZE <= 6)
                        cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                }
            }
        }
        if (cursor.y / TILE_SIZE - 4 == 1)
            drawATile(tilesetTexture, cursor.tileIndex, 18 * TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        drawATile(tilesetTexture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, cursor.y / TILE_SIZE - 4 == 1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
}

int changeControls()
{
    sprite cursor;
    SDL_Color textColor = (SDL_Color) {AMENU_MAIN_TEXTCOLOR};
    SDL_Color bgColor = (SDL_Color) {AMENU_MAIN_BGCOLOR};
    initSprite(&cursor, TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, MAIN_ARROW_ID, 0, SDL_FLIP_NONE, (entityType) type_na);
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

            strcpy(keyText, "Spc.: ");
            drawText(strcat(keyText, SDL_GetKeyName(SDL_GetKeyFromScancode(SC_SPECIAL))), 2 * TILE_SIZE + TILE_SIZE / 4, 10 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 10) * TILE_SIZE, textColor, false);

            drawText("Back", 2.25 * TILE_SIZE, 11 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 11) * TILE_SIZE, textColor, false);
            drawText("Default is W/S/A/D, Space, Esc, LShift", .75 * TILE_SIZE, 13 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 13) * TILE_SIZE, textColor, false);
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
                else
                {
                    if(e.type == SDL_KEYDOWN)
                    {
                        if (e.key.keysym.scancode == SC_UP && cursor.y > 4 * TILE_SIZE)
                        {
                            cursor.y -= TILE_SIZE;
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }

                        if (e.key.keysym.scancode == SC_DOWN && cursor.y < 11 * TILE_SIZE)
                        {
                            cursor.y += TILE_SIZE;
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }

                        if (e.key.keysym.scancode == SC_INTERACT)
                        {
                            selection = cursor.y / TILE_SIZE - 3;
                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                            quit = true;
                        }
                    }
                    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                    {

                        int choice = (e.button.y / TILE_SIZE) - 3;
                        if (choice > 0 && choice <= 8)
                        {
                            selection = choice;
                            quit = true;
                            Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        }
                    }
                    if (e.type == SDL_MOUSEMOTION)
                    {
                        if (e.motion.y / TILE_SIZE - 3 > 0 && e.motion.y / TILE_SIZE - 3 <= 8)
                            cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                    }
                }
            }
        }
        if (selection > 0 && selection < 8)
        {
            char* keyName[7] = {"Up", "Down", "Left", "Right", "Confirm", "Menu", "Special"};
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

void changeName(player* playerSprite)
{
    char* newName = calloc(MAX_PLAYER_NAME + 1, sizeof(char));
    stringInput(&newName, "Change name to:", MAX_PLAYER_NAME, "Player", true);
    for(int i = 0; i < MAX_PLAYER_NAME; i++)
        playerSprite->name[i] = 0;
    strncpy(playerSprite->name, newName, strlen(newName));
    saveGlobalPlayer(*playerSprite, GLOBALSAVE_FILEPATH);
    free(newName);
}

void changeFPS(int newFPS)
{
    FPS = newFPS;
    targetTime = calcWaitTime(FPS);
    saveConfig(CONFIG_FILEPATH);
}

void clearData(player* playerSprite)
{
    int choice = aMenu(tilesetTexture, MAIN_ARROW_ID, "Clear All Data:\nAre You Sure?", (char*[2]) {"Yes, Do It", "No, Back"}, 2, 0, AMENU_MAIN_THEME, true, false, NULL);
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
    char junkArray[MAX_FILE_PATH];
    static int menuPage = 0;
    int selectItem = 1;
    SDL_Event e;
    while(!quitMenu)
    {
        SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_TEXTCOLOR);
        SDL_RenderClear(mainRenderer);
        SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        for(int i = 0; i < (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE); i++)  //11 can comfortably be max
            drawText(readLine((char*) strcat(strcpy(junkArray, MAP_PACKS_SUBFOLDER), listOfFilenames[i + (menuPage * MAX_MAPPACKS_PER_PAGE)]),  /*concatting the path and one of the filenames together into one string*/
                          0, (char**) &junkArray), TILE_SIZE + 10, (i + 3) * TILE_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
        drawText("Back", TILE_SIZE + 10, 2 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
        while(SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                selectItem = 0;
                *backFlag = -1;
            }
            else
            {
                if (e.type == SDL_KEYDOWN)
                {

                    if ((e.key.keysym.scancode == SC_LEFT && menuPage > 0) || (e.key.keysym.scancode == SC_RIGHT && menuPage < maxStrNum / MAX_MAPPACKS_PER_PAGE))
                    {
                        menuPage += (e.key.keysym.scancode == SC_RIGHT) - (e.key.keysym.scancode == SC_LEFT);
                        selectItem = 0;
                    }

                    if ((e.key.keysym.scancode == SC_UP && selectItem > 0) || (e.key.keysym.scancode == SC_DOWN && selectItem < (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE)))
                    {
                        selectItem += (e.key.keysym.scancode == SC_DOWN) - (e.key.keysym.scancode == SC_UP);
                        Mix_PlayChannel(-1, PING_SOUND, 0);
                    }
                    if (e.key.keysym.scancode == SC_INTERACT)
                    {
                        if (selectItem != 0)
                            selectItem = menuPage * MAX_MAPPACKS_PER_PAGE + selectItem;
                        else
                            *backFlag = true;
                        quitMenu = true;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                    }
                }
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    int choice = (e.button.y / TILE_SIZE) - 3;
                    if (choice < 0)
                    {
                        SDL_Rect minus = {8.5 * TILE_SIZE, 2, TILE_SIZE, TILE_SIZE};
                        SDL_Rect plus = {9.75 * TILE_SIZE, 2, TILE_SIZE, TILE_SIZE};

                        if ((e.button.x - minus.x <= minus.w && e.button.x - minus.x > 0) && (e.button.y - minus.y <= minus.h && e.button.y - minus.y > 0) && menuPage > 0)
                        {
                            menuPage--;
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }

                        if ((e.button.x - plus.x <= plus.w && e.button.x - plus.x > 0) && (e.button.y - plus.y <= plus.h && e.button.y - plus.y > 0) && menuPage < maxStrNum / MAX_MAPPACKS_PER_PAGE)
                        {
                            menuPage++;
                            Mix_PlayChannel(-1, PING_SOUND, 0);
                        }
                    }
                    if (choice >= 0 && choice <= (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE))
                    {
                        selectItem = menuPage * MAX_MAPPACKS_PER_PAGE + choice + 1;
                        if (selectItem < (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE) + 1)
                            quitMenu = true;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                    }
                    if (choice == -1)
                    {
                        selectItem = 0;
                        *backFlag = 1;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                    }
                }
                if (e.type == SDL_MOUSEMOTION)
                {
                    if (e.motion.y / TILE_SIZE - 3 >= -1 && e.motion.y / TILE_SIZE - 3 < (maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE > MAX_MAPPACKS_PER_PAGE ? MAX_MAPPACKS_PER_PAGE : maxStrNum - menuPage * MAX_MAPPACKS_PER_PAGE))
                        selectItem = (e.motion.y / TILE_SIZE) - 2;
                }
            }
        }
        if (selectItem == 0 && *backFlag)
            quitMenu = true;

        if (maxStrNum / MAX_MAPPACKS_PER_PAGE > 0)
        {
            if (menuPage > 0)
                drawATile(tilesetTexture, MAIN_ARROW_ID, 8.5 * TILE_SIZE, 2, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_HORIZONTAL);
            if (menuPage < maxStrNum / MAX_MAPPACKS_PER_PAGE)
                drawATile(tilesetTexture, MAIN_ARROW_ID, 9.75 * TILE_SIZE, 2, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        }

        drawATile(tilesetTexture, MAIN_ARROW_ID, 10, (selectItem + 2) * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);

    }
    //loading map pack stuff
    if (selectItem > 0)
        strncat(strcpy(mapPackName, MAP_PACKS_SUBFOLDER), listOfFilenames[selectItem - 1], MAX_FILE_PATH - 9);
}

int mainLoop(player* playerSprite)
{
    SDL_Event e;
	bool quit = false, drawFPS = false;
	//static bool textBoxOn = false;
	char mapFilePath[MAX_FILE_PATH];
	strcpy(mapFilePath, playerSprite->extraData);
    int maxTheseScripts = 0, * collisionData = calloc(MAX_COLLISIONDATA_ARRAY, sizeof(int));
    script newScript, * thisScript = &newScript, ** theseScripts = calloc(sizeOfAllScripts, sizeof(script)), bossScript;
    initScript(&bossScript, script_boss_actions, -1, -48, -48, 0, 0, "[0/1]", -1);
    static int bossHP = 1, maxBossHP = 1;
    thisScript->active = false;
    for(int i = 0; i < sizeOfAllScripts; i++)
    {
        if (allScripts[i].mapNum == playerSprite->mapScreen)
            theseScripts[maxTheseScripts++] = &allScripts[i];
    }
    if (maxTheseScripts)
    {
        script** new_ptr = realloc(theseScripts, maxTheseScripts * sizeof(script*));
        if (new_ptr != NULL)
            theseScripts = new_ptr;
    }
    else
        theseScripts = NULL;
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
                    initEnemy(&enemies[enemyCount - 1], x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, ENEMY(eventmap[y][x] - 11), 1 + (eventmap[y][x] == 14), enemyFlags[enemyCount - 1] ? type_enemy : type_na);
                }
            }
        }
    }
    enemyFlags[MAX_ENEMIES] = false;

    bool bossUndefeated = true;
    for(int i = 0; i < playerSprite->nextBossPos + 1; i++)  //this is because nextBossPos is for arrays, + 1 to be an appropriate bound for all beaten bosses
    {
        if (playerSprite->defeatedBosses[i] == playerSprite->mapScreen)
            bossUndefeated = false;
    }
    if (bossUndefeated)
    {
        for(int i = 0; i < maxTheseScripts; i++)
        {
            if (theseScripts[i]->action == script_boss_actions)
            {
                bossScript = *theseScripts[i];
                char* data = calloc(99, sizeof(char));
                bossSprite.spr.tileIndex = strtol(strtok(strcpy(data, bossScript.data), "[/]"), NULL, 10);
                if (!loadBoss)
                {
                    bossScript.x = bossSprite.spr.x;
                    bossScript.y = bossSprite.spr.y;
                }
                else
                {
                    bossHP = strtol(strtok(NULL, "[/]"), NULL, 10);
                    maxBossHP = bossHP;
                }

                initEnemy(&bossSprite, bossScript.x, bossScript.y, bossScript.w, bossScript.h, bossSprite.spr.tileIndex, bossScript.h, type_boss);
                loadBoss = false;
				//bossScript.active = false;
                free(data);
                break;
            }
        }
    }
    else
    {
        /*script openDoors;
        initScript(&openDoors, script_toggle_door, 0, 0, 0, 0, 0, "[-1/-1/-1/0]", -1);
        executeScriptAction(&openDoors, playerSprite);*/
        doorFlags[3] = 0;
    }
    checkCollision(playerSprite, collisionData, 1, 1, playerSprite->spr.x, playerSprite->spr.y);
    if (collisionData[4] || collisionData[5] || collisionData[6] || collisionData[7])
    {  //if player spawns on top of door
        script openDoors;
        if (collisionData[4])
            initScript(&openDoors, script_toggle_door, 0, 0, 0, 0, 0, "[0/-1/-1/-1]", -1);

        if (collisionData[5])
            initScript(&openDoors, script_toggle_door, 0, 0, 0, 0, 0, "[-1/0/-1/-1]", -1);

        if (collisionData[6])
            initScript(&openDoors, script_toggle_door, 0, 0, 0, 0, 0, "[-1/-1/0/-1]", -1);

        if (collisionData[7])
            initScript(&openDoors, script_toggle_door, 0, 0, 0, 0, 0, "[-1/-1/-1/0]", -1);

        executeScriptAction(&openDoors, playerSprite);
    }
    //printf("%d < %d\n", maxTheseScripts, sizeOfAllScripts);
    //doDebugDraw = false;
    int exitCode = 2;
    char whatever[5] = "    \0";
    startTime = SDL_GetTicks();
    frame = 0;
    int lastFrame = startTime, framerate = 0, sleepFor = 0, spinCycle = 0;
    bool initSword = false;
    Uint32 lastXPress = 0, lastYPress = 0, lastSpcPress = 0, illusionTimer = 0, swordTimer = startTime + 250, laserTimer = 0,
        lastUpdateTime = startTime, lastBoostTime = startTime, lastSpinTime = 0, lastIllusionTime = 0, lastLaserTime = 0,
        lastChargeTime = 0;
    sprite sword, illusionSpr;
    initSprite(&sword, 0, 0, TILE_SIZE, TILE_SIZE, SWORD_ID, 0, SDL_FLIP_NONE, type_na);
    initSprite(&illusionSpr, 0, 0, TILE_SIZE, TILE_SIZE, PLAYER_ID, 0, SDL_FLIP_NONE, type_generic);
    while(!quit && playerSprite->HP > 0)
    {
        SDL_RenderClear(mainRenderer);
        drawATilemap(tilesTexture, tilemap, 0, 0, 20, 15, 0, 0, -1, false);
        drawOverTilemap(tilesTexture, eventmap, 0, 0, 20, 15, 0, 0, doorFlags, false, false);
        {  //drawing HUD
            SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0x7F);
            SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = 0, .y = 0, .w = playerSprite->maxHP / 4.0 * TILE_SIZE, .h = TILE_SIZE}));
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

            if (!playerSprite->movementLocked && (checkSKUp || checkSKDown || checkSKLeft || checkSKRight || checkSKSpecial || checkSKInteract || playerSprite->xVeloc || playerSprite->yVeloc))
            {
                int lastY = playerSprite->spr.y;
                int lastX = playerSprite->spr.x;

                Uint32 curTime = SDL_GetTicks();

                if (lastXPress * (checkSKRight - checkSKLeft) < lastUpdateTime - 32 && lastXPress * (checkSKRight - checkSKLeft) + 128 > curTime && (checkSKRight - checkSKLeft) && playerSprite->techUnlocks[0] == 2 && lastBoostTime + 500 < curTime)
                {
                    //printf("boost: %d, now %d\n", lastXPress, curTime);
                    playerSprite->xVeloc += (checkSKRight - checkSKLeft) * 36;
					lastBoostTime = curTime;
					Mix_PlayChannel(-1, DASH_SOUND(1 + (rand() % 3)), 0);
                }
                if (lastYPress * (checkSKDown - checkSKUp) < (Uint32) lastUpdateTime - 32 && lastYPress * (checkSKDown - checkSKUp) + 128 > curTime && (checkSKDown - checkSKUp) && playerSprite->techUnlocks[0] == 2 && lastBoostTime + 500 < curTime)
                {
                    //printf("boost: %d, now %d\n", lastYPress, curTime);
                    playerSprite->yVeloc += (checkSKDown - checkSKUp) * 36;
					lastBoostTime = curTime;
					Mix_PlayChannel(-1, DASH_SOUND(1 + (rand() % 3)), 0);
                }

                if (playerSprite->spr.y > 0 && checkSKUp && !playerSprite->yVeloc)
                    playerSprite->yVeloc -= PIXELS_MOVED;

                if (playerSprite->spr.y < SCREEN_HEIGHT - playerSprite->spr.h && checkSKDown && !playerSprite->yVeloc)
                    playerSprite->yVeloc += PIXELS_MOVED;

                if (playerSprite->spr.x > 0 && checkSKLeft && !playerSprite->xVeloc)
                    playerSprite->xVeloc -= PIXELS_MOVED;

                if (playerSprite->spr.x < SCREEN_WIDTH - playerSprite->spr.w && checkSKRight && !playerSprite->xVeloc)
                    playerSprite->xVeloc += PIXELS_MOVED;

                if (checkSKRight - checkSKLeft)
                {
                    lastXPress = curTime * (checkSKRight - checkSKLeft);
                    lastYPress = (curTime - 121) * (checkSKDown - checkSKUp);  //disqualify y-dash
                }

                if (checkSKDown - checkSKUp)
                {
                    lastYPress = curTime * (checkSKDown - checkSKUp);
                    lastXPress = (curTime - 121) * (checkSKRight - checkSKLeft);  //disqualify x-dash
                }

                if (checkSKSpecial)
                {
                    if (lastSpcPress < lastUpdateTime - 32 && lastSpcPress + 120 > curTime)
                    {
                        if (lastSpinTime + 500 < curTime && playerSprite->techUnlocks[1] == 2)  //spin attack
                        {
                            lastSpinTime = curTime;
                            spinCycle = 1;
                            initSword = true;
                        }
                        if (lastIllusionTime + 3000 < curTime && playerSprite->techUnlocks[2] == 2)  //illusion
                        {
                            lastIllusionTime = curTime;
                            illusionTimer = curTime + 2000;
                            playerSprite->invincCounter = 60;  //1 second invincibility
                            initSprite(&illusionSpr, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE, PLAYER_ID, 0, SDL_FLIP_VERTICAL, type_generic);
                            Mix_PlayChannel(-1, UNSHEATH_SOUND, 0);
                            initSpark(&theseSparks[0], (SDL_Rect) {playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_COLOR_GRAY, 4, 8, 8, framerate / 2, framerate / 4);
                            theseSparkFlags[0] = true;
                            sparkFlag = true;

                        }
                        if (lastLaserTime + 1250 < curTime && playerSprite->techUnlocks[3] == 2 && !swordTimer)
                        {
                            lastLaserTime = curTime;
                            laserTimer = curTime + 650;
                            initSword = true;
                        }
                        if (lastChargeTime + 2500 < curTime && playerSprite->techUnlocks[4] == 2 && !swordTimer)
                        {
                            lastChargeTime = curTime;
                            if (playerSprite->lastDirection < 4)
                            {
                                playerSprite->yVeloc = 48 - 96 * (playerSprite->lastDirection % 2 == 1);
                            }
                            if (playerSprite->lastDirection > 3)
                            {
                                playerSprite->xVeloc = 48 - 96 * (playerSprite->lastDirection / 4 == 1);
                            }
                            playerSprite->invincCounter = 20;
                            lastBoostTime = curTime + 400;
                            initSword = true;
                        }
                    }
                    lastSpcPress = curTime;
                }

                if (((checkSKUp && !checkSKDown) || (!checkSKUp && checkSKDown) || (checkSKLeft && !checkSKRight) || (!checkSKLeft && checkSKRight)) && !checkSKSpecial)
                    playerSprite->lastDirection = checkSKUp + 2 * checkSKDown + 4 * checkSKLeft + 8 * checkSKRight;

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
                    initSword = true;

                if (!noclip &&(collisionData[0] || ((collisionData[4] && doorFlags[0] == true) || (collisionData[5] && doorFlags[1] == true) || (collisionData[6] && doorFlags[2] == true) || (collisionData[7] && doorFlags[3] == true))  || collisionData[9] || collisionData[15]))
                {  //unwalkable tile or closed door or spikes
                    playerSprite->spr.y = lastY;
                    playerSprite->spr.x = lastX;
                    playerSprite->xVeloc = 0;
                    playerSprite->yVeloc = 0;
                    //printf("%d\n", exitCode);
                    if (collisionData[9] && playerSprite->invincCounter < 1 && !noclip)  //spikes
                    {
                        playerSprite->xVeloc -= 24 * (checkSKRight - checkSKLeft);
                        playerSprite->yVeloc -= 24 * (checkSKDown - checkSKUp);
                        script hurtPlayer;
                        initScript(&hurtPlayer, script_player_hurt, 0, 0, 0, 0, 0, "1", -1);
                        executeScriptAction(&hurtPlayer, playerSprite);
                        playerSprite->invincCounter = 10;  //10 frames of invincibility at 60fps, or approx. .167 of a second
                    }
                }

                if (!playerSprite->spr.x || !playerSprite->spr.y || playerSprite->spr.x == SCREEN_WIDTH - TILE_SIZE || playerSprite->spr.y == SCREEN_HEIGHT - TILE_SIZE)
                {  //if we're moving to a different map
                    bool quitThis = false;
                    if (!playerSprite->spr.x && playerSprite->mapScreen % 10 > 0)
                    {
                        playerSprite->mapScreen--;
                        quitThis = true;
                    }

                    if (!playerSprite->spr.y && playerSprite->mapScreen / 10 > 0)
                    {
                        playerSprite->mapScreen -= 10;
                        quitThis = true;
                    }

                    if (playerSprite->spr.x == SCREEN_WIDTH - TILE_SIZE && playerSprite->mapScreen % 10 < 9)
                    {
                        playerSprite->mapScreen++;
                        quitThis = true;
                    }

                    if (playerSprite->spr.y == SCREEN_HEIGHT - TILE_SIZE && playerSprite->mapScreen / 10 < 9)
                    {
                        playerSprite->mapScreen += 10;
                        quitThis = true;
                    }

                    if (quitThis)
                    {
                        quit = true;
                        exitCode = 2;
                        playerSprite->xVeloc = 0;
                        playerSprite->yVeloc = 0;
                        playerSprite->lastX = playerSprite->spr.x;
                        playerSprite->lastY = playerSprite->spr.y;
                        playerSprite->lastMap = playerSprite->mapScreen;
                    }
                }

                if (collisionData[1] || collisionData[2] || collisionData[3])   //door buttons
                {
                    bool exec = false;
                    bool newDoorFlags[4] = {-1, -1, -1, doorFlags[3]};
                    for(int i = 0; i < 3; i++)
                    {
                        if (collisionData[i + 1])
                        {
                            if (doorFlags[i])
                                exec = true;
                            newDoorFlags[i] = false;
                        }
                    }
                    if (exec)
                    {
                        script openDoorScript;
                        char* data = calloc(99, sizeof(char));
                        snprintf(data, 99, "[%d/%d/%d/-1]", newDoorFlags[0], newDoorFlags[1], newDoorFlags[2]);
                        initScript(&openDoorScript, script_toggle_door, 0, 0, 0, 0, 0, data, -1);
                        executeScriptAction(&openDoorScript, playerSprite);
                        free(data);
                    }
                }
                if (collisionData[8])  //teleporter
                {
                    bool found = false;
                    for(int i = 0; i < maxTheseScripts; i++)
                    {
                        if (theseScripts[i]->action == script_use_teleporter && SDL_HasIntersection(&((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}), &((SDL_Rect){.x = theseScripts[i]->x, .y = theseScripts[i]->y, .w = theseScripts[i]->w, .h = theseScripts[i]->h})))  //not using faster collision bc some scripts might be width != 48
                        {
                            thisScript = theseScripts[i];
                            found = true;
                            break;
                        }
                    }
                    thisScript->active = found;
                    initSpark(&theseSparks[4], (SDL_Rect) {playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_COLOR_BLUE, 4, 8, 8, framerate / 3, framerate / 6);
                    sparkFlag = true;
                    theseSparkFlags[4] = true;
                }
                if (collisionData[10])  //gateway
                {
                    bool found = false;
                    for(int i = 0; i < maxTheseScripts; i++)
                    {
                        if (theseScripts[i]->action == script_use_gateway && SDL_HasIntersection(&((SDL_Rect){.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}), &((SDL_Rect){.x = theseScripts[i]->x, .y = theseScripts[i]->y, .w = theseScripts[i]->w, .h = theseScripts[i]->h})))  //not using faster collision bc some scripts might be width != 48
                        {
                            thisScript = theseScripts[i];
                            found = true;
                            break;
                        }
                    }
                    thisScript->active = found;
                    //initScript(&thisScript, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, script_use_portal, "[0/456/336]\0");
                    playerSprite->extraData = mapFilePath;
                    exitCode = 2;
                }
            }

            if (keyStates[SDL_SCANCODE_GRAVE] && debugFlag) //console / ~
            {
                bool done = false;
                script exec;
                initScript(&exec, script_none, 0, 0, 0, 0, 0, "", -1);
                char* command = calloc(50, sizeof(char));
                char* commandCpy = calloc(50, sizeof(char));

                stringInput(&command, "console ~", 50, "x", false);

                strncpy(commandCpy, command, 50);

                 if (!strncmp(command, "noclip", 6))
                    noclip = !noclip;

                if (!strncmp(command, "invincible", 10))
                    playerSprite->invincCounter = 600;

                if (!strncmp(command, "teleport", 8))
                {
                    playerSprite->spr.x = TILE_SIZE * strtol(strtok(commandCpy, "teleport ,"), NULL, 10);
                    playerSprite->spr.y = TILE_SIZE * strtol(strtok(NULL, ","), NULL, 10);
                }

                if (!strncmp(command, "opendoors", 50))
                    initScript(&exec, script_toggle_door, 0, 0, 0, 0, 0, "[0/0/0/0]", -1);  //opens all doors

                if (!strncmp(command, "closedoors", 50))
                    initScript(&exec, script_toggle_door, 0, 0, 0, 0, 0, "[1/1/1/1]", -1);  //closes all doors

                if (!strncmp(command, "hurt", 4))
                    initScript(&exec, script_player_hurt, 0, 0, 0, 0, 0, strtok(commandCpy, "hurt "), -1);

                if (!strncmp(command, "diemenu", 7))
                    gameOver();

                if (!strncmp(command, "fps", 3))
                    changeFPS((int) strtol(strtok(commandCpy, "fps "), NULL, 10));

                if (!strncmp(command, "execscript", 10))
                {
                    char* temp = "";
                    int lineNum = strtol(strtok(commandCpy, "execscript "), NULL, 10);
                    readScript(&exec, readLine(scriptFilePath, lineNum, &temp), lineNum);
                }

                if (!strncmp(command, "particles", 9))
                {
                    initSpark(&theseSparks[6], (SDL_Rect) {playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_COLOR_ORANGE, 4, 8, 8, framerate / 2, framerate / 4);
                    sparkFlag = true;
                    theseSparkFlags[6] = true;
                }

                done = executeScriptAction(&exec, playerSprite);
                free(command);
                free(commandCpy);
                if (done)
                {
                    quit = true;
                    exitCode = 2;
                    playerSprite->xVeloc = 0;
                    playerSprite->yVeloc = 0;
                }
            }

            {
                bool playHitSound = false;
                for(int i = 0; i < enemyCount; i++)
                {
                    bool collidedOnce = false;

                    if (checkSquareCol(sword.x, sword.y, enemies[i].spr.x, enemies[i].spr.y, TILE_SIZE)
                        && swordTimer > SDL_GetTicks() + 250)  //sword collision
                    {
                        if (enemies[i].spr.type == type_enemy && (enemies[i].invincTimer == 0 || enemies[i].invincTimer < (int) SDL_GetTicks() + 250))
                        {
                            enemies[i].HP--;
                            playHitSound = true;
                            if (enemies[i].HP < 1 /*&& (enemies[i].angle == false || enemies[i].angle < SDL_GetTicks() + 250*/)
                            {
                                enemies[i].spr.type = type_generic;
                                enemyFlags[i] = false;
                                enemies[i].spr.tileIndex = GOLD_ID;
                            }
                            enemies[i].invincTimer = swordTimer;  //angle == hit detection cooldown timer
                            if (!theseSparkFlags[1])
                            {
                                initSpark(&theseSparks[1], (SDL_Rect) {sword.x, sword.y, sword.w, sword.h}, SPARK_COLOR_SILVER, 4, 6, 6, framerate / 4, framerate / 8);
                                sparkFlag = true;
                                theseSparkFlags[1] = true;
                            }
                        }
                    }

                    if (!collidedOnce && checkSquareCol(playerSprite->spr.x, playerSprite->spr.y, enemies[i].spr.x, enemies[i].spr.y, TILE_SIZE) && enemies[i].spr.type != type_na && !(playerSprite->invincCounter))  //player collision
                    {
                        if (enemies[i].spr.type == type_enemy && !noclip)
                        {
                            script hurtPlayer;
                            initScript(&hurtPlayer, script_player_hurt, 0, 0, 0, 0, 0, enemies[i].spr.tileIndex != ENEMY(3) ? "1" : "2", -1);
                            playerSprite->xVeloc += 24 * (abs(playerSprite->spr.x - enemies[i].spr.x) > abs(playerSprite->spr.y - enemies[i].spr.y))
                             - 48 * (enemies[i].spr.x > playerSprite->spr.x && (abs(playerSprite->spr.x - enemies[i].spr.x) > abs(playerSprite->spr.y - enemies[i].spr.y)));

                            playerSprite->yVeloc += 24 * (abs(playerSprite->spr.y - enemies[i].spr.y) > abs(playerSprite->spr.x - enemies[i].spr.x))
                             - 48 * (enemies[i].spr.y > playerSprite->spr.y && (abs(playerSprite->spr.y - enemies[i].spr.y) > abs(playerSprite->spr.x - enemies[i].spr.x)));
                            executeScriptAction(&hurtPlayer, playerSprite);
                            playerSprite->invincCounter = 11;  //22 frames of invincibility at 60fps, or approx. .367 of a second
                            collidedOnce = true;
                        }
                        else if (enemies[i].spr.type == type_generic)
                        {
                            script rewardScript;
                            initScript(&rewardScript, script_gain_money, 0, 0, 0, 0, 0, "2", -1); //todo: re-evaluate coins given per enemy
                            executeScriptAction(&rewardScript, playerSprite);
                            enemies[i].spr.tileIndex = INVIS_ID;
                            enemies[i].spr.type = type_na;
                        }
                    }
                    if (!playerSprite->movementLocked)  //enemy movement
                    {
                        if (enemies[i].spr.tileIndex == ENEMY(1) && enemies[i].spr.type == type_enemy)
                        {
                            //behavior: move quickly at player, with little HP
                            int length = 0;
                            int targetX = 0, targetY = 0;
                            if (illusionTimer)
                            {
                                targetX = illusionSpr.x;
                                targetY = illusionSpr.y;
                            }
                            else
                            {
                                targetX = playerSprite->spr.x;
                                targetY = playerSprite->spr.y;
                            }
                            node* nodeArray = BreadthFirst(enemies[i].spr.x, enemies[i].spr.y, targetX, targetY, &length, false);
                            /*if (enemies[i].spr.x != playerSprite->spr.x)
                                enemies[i].spr.x += 3 - 6 * (playerSprite->spr.x < enemies[i].spr.x);
                            if (enemies[i].spr.y != playerSprite->spr.y)
                                enemies[i].spr.y += 3 - 6 * (playerSprite->spr.y < enemies[i].spr.y);*/
                            if (length > 0 && (enemies[i].invincTimer == false || enemies[i].invincTimer < (int) SDL_GetTicks() + 250))
                            {
                                if (nodeArray[1].x < enemies[i].spr.x)
                                    enemies[i].spr.flip = SDL_FLIP_HORIZONTAL;
                                else
                                    enemies[i].spr.flip = SDL_FLIP_NONE;

                                if (enemies[i].spr.x != nodeArray[1].x)  //nodeArray[1] -> next tile
                                    enemies[i].spr.x += 3 - 6 * (nodeArray[1].x < enemies[i].spr.x);
                                if (enemies[i].spr.y != nodeArray[1].y)
                                    enemies[i].spr.y += 3 - 6 * (nodeArray[1].y < enemies[i].spr.y);

                            }
                            free(nodeArray);
                        }

                        if (enemies[i].spr.tileIndex == ENEMY(2) && enemies[i].spr.type == type_enemy)
                        {
                            //behavior: burst movement towards player?
                            if (enemies[i].invincTimer == false || enemies[i].invincTimer < (int) SDL_GetTicks() + 250)
                            {
                                int targetX = 0, targetY = 0;
                                if (illusionTimer)
                                {
                                    targetX = illusionSpr.x;
                                    targetY = illusionSpr.y;
                                }
                                else
                                {
                                    targetX = playerSprite->spr.x;
                                    targetY = playerSprite->spr.y;
                                }
                                if (targetX < enemies[i].spr.x)
                                    enemies[i].spr.flip = SDL_FLIP_HORIZONTAL;
                                else
                                    enemies[i].spr.flip = SDL_FLIP_NONE;

                                {
                                    double a = atan2(enemies[i].spr.y - targetY, enemies[i].spr.x - targetX);
                                    double rate = 2.5;
                                    int dx = (rate * cos(a));
                                    int dy = (rate * sin(a));
                                    enemies[i].spr.x -= dx;
                                    enemies[i].spr.y -= dy;
                                }

                                /*if (enemies[i].spr.x != targetX)
                                    enemies[i].spr.x += 2 - 4 * (targetX < enemies[i].spr.x);
                                if (enemies[i].spr.y != targetY)
                                    enemies[i].spr.y += 2 - 4 * (targetY < enemies[i].spr.y);*/
                            }
                        }

                        if (enemies[i].spr.tileIndex == ENEMY(3) && enemies[i].spr.type == type_enemy)
                        {
                            //behavior: move slowly at player, matching up x coord first then y, w/ lot of HP
                            int length = 0;
                            static node curNode;
                            if (!length || (curNode.x == enemies[i].spr.x && curNode.y == enemies[i].spr.y))
                            {
                                int targetX = 0, targetY = 0;
                                if (illusionTimer)
                                {
                                    targetX = illusionSpr.x;
                                    targetY = illusionSpr.y;
                                }
                                else
                                {
                                    targetX = playerSprite->spr.x;
                                    targetY = playerSprite->spr.y;
                                }
                                node* nodeArray = BreadthFirst(enemies[i].spr.x, enemies[i].spr.y, targetX, targetY, &length, false);
                                if (length > 0)
                                {
                                    curNode = nodeArray[1];
                                    free(nodeArray);
                                }
                            }
                            if (length > 0 && (enemies[i].invincTimer == false || enemies[i].invincTimer < (int) SDL_GetTicks() + 250))
                            {
                                if (curNode.x < enemies[i].spr.x)
                                    enemies[i].spr.flip = SDL_FLIP_HORIZONTAL;
                                else
                                    enemies[i].spr.flip = SDL_FLIP_NONE;

                                if (enemies[i].spr.x != curNode.x)
                                    enemies[i].spr.x += 3 - 6 * (curNode.x < enemies[i].spr.x);
                                if (enemies[i].spr.y != curNode.y)
                                    enemies[i].spr.y += 3 - 6 * (curNode.y < enemies[i].spr.y);
                            }
                        }
                    }
                }
                if (playHitSound)
                    Mix_PlayChannel(-1, ENEMYHURT_SOUND, 0);
            }
            if (bossSprite.spr.x >= 0 && bossSprite.spr.type == type_boss && bossLoaded && !playerSprite->movementLocked)
            {
                if (!noclip && checkRectCol(playerSprite->spr.x, playerSprite->spr.y, playerSprite->spr.w, playerSprite->spr.h, bossSprite.spr.x, bossSprite.spr.y, bossSprite.spr.w, bossSprite.spr.h))
                {
                    script hurtPlayer;
                    initScript(&hurtPlayer, script_player_hurt, 0, 0, 0, 0, 0, "2", -1);
                    playerSprite->xVeloc += 24 * (abs(playerSprite->spr.x - bossSprite.spr.x) > abs(playerSprite->spr.y - bossSprite.spr.y))
                        - 48 * (bossSprite.spr.x > playerSprite->spr.x && (abs(playerSprite->spr.x - bossSprite.spr.x) > abs(playerSprite->spr.y - bossSprite.spr.y)));

                    playerSprite->yVeloc += 24 * (abs(playerSprite->spr.y - bossSprite.spr.y) > abs(playerSprite->spr.x - bossSprite.spr.x))
                        - 48 * (bossSprite.spr.y > playerSprite->spr.y && (abs(playerSprite->spr.y - bossSprite.spr.y) > abs(playerSprite->spr.x - bossSprite.spr.x)));
                    executeScriptAction(&hurtPlayer, playerSprite);
                    playerSprite->invincCounter = 11;  //22 frames of invincibility at 60fps, or approx. .367 of a second
                }
                if (checkRectCol(sword.x, sword.y, sword.w, sword.h, bossSprite.spr.x, bossSprite.spr.y, bossSprite.spr.w, bossSprite.spr.h) && swordTimer > SDL_GetTicks() + 250)
                {
                    if (bossSprite.invincTimer == false || bossSprite.invincTimer < (int) SDL_GetTicks() + 250)
                    {
                        bossHP--;
                        if (bossHP < 1 /*&& (bossSprite.angle == false || bossSprite.angle < SDL_GetTicks() + 250)*/)
                        {
                            bossSprite.spr.type = type_na;
                            script bossDeadScript;
                            initScript(&bossDeadScript, script_toggle_door, 0, 0, 0, 0, 0, "[0/0/0/0]", -1);  //opens all doors
                            executeScriptAction(&bossDeadScript, playerSprite);
                            initScript(&bossDeadScript, script_gain_money, 0, 0, 0, 0, 0, "15", -1);  //boss should probably become money, "dropping" it
                            executeScriptAction(&bossDeadScript, playerSprite);
                            initSpark(&theseSparks[7], (SDL_Rect) {playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_BOSS, 6, 8, 8, framerate / 2, framerate / 4);
                            sparkFlag = true;
                            theseSparkFlags[7] = true;
                            playerSprite->defeatedBosses[playerSprite->nextBossPos++] = playerSprite->mapScreen;

                            Mix_PlayMusic(MUSIC((musicIndex = 6)), 0);  //fanfare
                            Mix_HookMusicFinished(playOverworldMusic);
                        }
                        else
                        {
                            initSpark(&theseSparks[1], (SDL_Rect) {sword.x, sword.y, sword.w, sword.h}, SPARK_COLOR_SILVER, 4, 6, 6, framerate / 4, framerate / 8);
                            sparkFlag = true;
                            theseSparkFlags[1] = true;
                        }
                        bossSprite.invincTimer = swordTimer;  //angle == hit detection cooldown timer
                        Mix_PlayChannel(-1, ENEMYHURT_SOUND, 0);
                    }
                }
                executeScriptAction(&bossScript, playerSprite);
                bossSprite.spr.x = bossScript.x;
                bossSprite.spr.y = bossScript.y;
            }
            if (!thisScript->active)
            {  //script search loop
                for(int i = 0; i < maxTheseScripts; i++)
                {
                    if (SDL_HasIntersection(&((SDL_Rect) {.x = theseScripts[i]->x, .y = theseScripts[i]->y, .w = theseScripts[i]->w, .h = theseScripts[i]->h}), &((SDL_Rect) {.x = playerSprite->spr.x, .y = playerSprite->spr.y, .w = playerSprite->spr.w, .h = playerSprite->spr.h}))
                        && theseScripts[i]->action != script_use_gateway && theseScripts[i]->action != script_use_teleporter
                        && theseScripts[i]->action != script_boss_actions)
                    {

                        thisScript = theseScripts[i];
                        thisScript->active = true;

                        if ((thisScript->action == script_none || thisScript->disabled) || ((thisScript->action == script_trigger_dialogue && (!checkSKInteract || _globalInt1 != 0)) || (thisScript->action == script_trigger_boss && (bossLoaded || !bossUndefeated))))
                            thisScript->active = false;

                        if (thisScript->active && (thisScript->action == script_force_dialogue_once || thisScript->action == script_force_dialogue ||
                            thisScript->action == script_trigger_boss || thisScript->action == script_switch_maps))
                            break;  //these are priority
                    }

                    if (_globalInt1 > 0)
                        _globalInt1--;
                }
            }

            if (initSword || spinCycle || laserTimer)
            {
                if ((initSword && !swordTimer) || spinCycle == 1)
                    SWING_CHANNEL = Mix_PlayChannel(-1, SWING_SOUND, 0);
                int xDir = 0;
                int yDir = 0;
                if (spinCycle)
                {
                    xDir = (((spinCycle - 1) / 4) + 1 == 2) - (((spinCycle - 1) / 4) + 1 == 4);
                    yDir = (((spinCycle - 1) / 4) + 1 == 3) - ((((spinCycle - 1) / 4) + 1) % 4 == 1);
                }
                else
                {
                    xDir = (playerSprite->lastDirection / 4) % 3;  //mod 3 to get rid of a value of 3 -- 3 == both directions pressed, or 0 movement
                    yDir = (playerSprite->lastDirection - xDir * 4) % 3 - 1;  //subtract 1 to turn either 0, 1, or 2 into either -1, 0, or 1
                    if ((xDir -= 1) != -1)
                        xDir -= !xDir;  //turns 0 and 1 into -1 and 1
                    else
                        xDir = 0;

                    if (yDir != -1)
                        yDir -= !yDir;
                    else
                        yDir = 0;
                    yDir *= !xDir;  //x direction takes precedence over the y direction
                }
                initSprite(&sword, playerSprite->spr.x + TILE_SIZE * xDir, playerSprite->spr.y + (TILE_SIZE + 2) * yDir, TILE_SIZE, TILE_SIZE, SWORD_ID, 90 * yDir, SDL_FLIP_HORIZONTAL * (xDir == -1), type_na);
                if (laserTimer)
                {
                    static int firstXDir = -2;
                    static int firstYDir = -2;
                    if (firstXDir == -2 || firstYDir == -2)
                    {
                        firstXDir = xDir;
                        firstYDir = yDir;
                    }
                    int distance = 21 - ((laserTimer - SDL_GetTicks()) / 32);
                    initSprite(&sword, playerSprite->spr.x + (TILE_SIZE * distance * firstXDir) + ((firstYDir != 0) * 1.5 * TILE_SIZE / 4), playerSprite->spr.y + ((TILE_SIZE + 2) * distance * firstYDir) + ((firstXDir != 0) * 1.5 * TILE_SIZE / 4), TILE_SIZE - ((firstYDir != 0) * 3 * TILE_SIZE / 4), TILE_SIZE - ((firstXDir != 0) * 3 * TILE_SIZE / 4), INVIS_ID, 0, SDL_FLIP_NONE, type_na);
                    if (SDL_GetTicks() > laserTimer ||
                        playerSprite->spr.x + (TILE_SIZE * distance) * firstXDir < -TILE_SIZE ||
                        playerSprite->spr.x + (TILE_SIZE * distance) * firstXDir > SCREEN_WIDTH + TILE_SIZE ||
                        playerSprite->spr.y + (TILE_SIZE + 2) * distance * firstYDir < - TILE_SIZE ||
                        playerSprite->spr.y + (TILE_SIZE + 2) * distance * firstYDir > SCREEN_HEIGHT + TILE_SIZE)
                    {
                        laserTimer = 0;
                        swordTimer = SDL_GetTicks() + 1;  //ends swordTimer without allowing for another swing
                        firstXDir = -2;
                        firstYDir = -2;
                    }
                }
                if (spinCycle)
                    spinCycle = (spinCycle + 1) % 24;
                //e

                if (!((thisScript->action == script_trigger_dialogue || thisScript->action == script_force_dialogue_once) && (thisScript->active == 1 || _globalInt1 > 0)) && initSword)
                {
                    if (swordTimer <= 0)
                        swordTimer = SDL_GetTicks() + 750 + 150 * (spinCycle > 0) - 150 * (laserTimer > 0);
                }
                else
                {
                    if (!(spinCycle || laserTimer || swordTimer))
                        Mix_HaltChannel(SWING_CHANNEL);
                }
                initSword = false;
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
            //printf("Framerate: %d\n", frame / ((int) now - (int) startTime));
        }
        if (swordTimer && SDL_GetTicks() >= swordTimer)
            swordTimer = 0;

        frame++;
        //if ((SDL_GetTicks() - startTime) % 250 == 0)
        framerate = (int) (frame * 1000.0 / (SDL_GetTicks() - startTime));  //multiplied by 1000 on both sides since 1000f / ms == 1f / s

        if (drawFPS)
            drawText(intToString(framerate, whatever), 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF, 0xFF, 0xFF, 0xFF}, false);

		if (animationSpr.x >= 0)
        {
            drawATile(tilesTexture, animationSpr.tileIndex, animationSpr.x, animationSpr.y, animationSpr.w, animationSpr.h, animationSpr.angle, animationSpr.flip);
        }

        if (illusionTimer)
        {
            drawASprite(tilesTexture, illusionSpr);
            if (SDL_GetTicks() > illusionTimer)
                illusionTimer = 0;
        }


		drawATile(tilesTexture, tileIDArray[(playerSprite->animationCounter > 0)], playerSprite->spr.x, playerSprite->spr.y, playerSprite->spr.w, playerSprite->spr.h, playerSprite->spr.angle, playerSprite->spr.flip);

        for(int i = 0; i < enemyCount; i++)
        {
            if (enemies[i].spr.type != type_na)
                drawATile(tilesTexture, enemies[i].spr.tileIndex, enemies[i].spr.x, enemies[i].spr.y, enemies[i].spr.w, enemies[i].spr.h, 0, enemies[i].spr.flip);
        }
        if (bossSprite.spr.x >= 0 && bossSprite.spr.type == type_boss && bossLoaded)
        {
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = bossSprite.spr.x, .y = bossSprite.spr.y - TILE_SIZE / 4, .w = bossHP * bossSprite.spr.w / maxBossHP, TILE_SIZE / 4}));
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
            drawATile(tilesTexture, bossSprite.spr.tileIndex, bossSprite.spr.x, bossSprite.spr.y, bossSprite.spr.w, bossSprite.spr.h, bossSprite.spr.angle, bossSprite.spr.flip);
            /*for(int i = 0; i < bossTiles; i++)
                drawATile(tilesTexture, bossSprite.spr.tileIndex + (i / (bossSprite.spr.w / TILE_SIZE)) + 8 * (i % (bossSprite.spr.h / TILE_SIZE)), bossSprite.spr.x + TILE_SIZE * (i % (bossSprite.spr.w / TILE_SIZE)), bossSprite.spr.y + TILE_SIZE * (i / (bossSprite.spr.w / TILE_SIZE)), TILE_SIZE, TILE_SIZE, 0, bossSprite.spr.flip);*/
        }

        if (swordTimer > SDL_GetTicks() + 250)
            drawASprite(tilesTexture, sword);

        if (laserTimer)
        {
            SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0x3F, 0x3F, 0xF0);
            SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {sword.x, sword.y, sword.w, sword.h}));
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
        }

        if (sparkFlag)
        {
            int notActive = 0;
            for(int i = 0; i < MAX_SPARKS; i++)
            {
                //printf("theseSparks[%d].timer == %d, theseSparkFlags[i] == %d\n", i, theseSparks[i].timer, theseSparkFlags[i]);
                if (theseSparks[i].timer && theseSparkFlags[i])
                {
                    drawSparks(&theseSparks[i]);
                    if (!theseSparks[i].timer)
                    {
                        initSpark(&theseSparks[i], (SDL_Rect) {0, 0, 0, 0}, (SDL_Color) {0, 0, 0, 0}, 1, 6, 6, 10, 1);
                        theseSparkFlags[i] = false;
                        notActive++;
                    }
                }
                else
                    notActive++;
            }
            if (notActive == MAX_SPARKS)
                sparkFlag = false;
        }

        SDL_RenderPresent(mainRenderer);
        if ((sleepFor = targetTime - (SDL_GetTicks() - lastFrame)) > 0)
            SDL_Delay(sleepFor);  //FPS limiter; rests for (16 - time spent) ms per frame, effectively making each frame run for ~16 ms, or 60 FPS
        lastFrame = SDL_GetTicks();
        if (thisScript->active)
        {
            bool quitScript = executeScriptAction(thisScript, playerSprite);
            quit = quit | quitScript;
            if (quitScript)
                exitCode = 3;
        }
    }

    _globalInt1 = 0;

    if (playerSprite->HP < 1)
    {
        exitCode = 5 - gameOver();
        playerSprite->HP = playerSprite->maxHP;
        theseSparkFlags[1] = false;
        playerSprite->xVeloc = 0;
        playerSprite->yVeloc = 0;
        playerSprite->spr.x = playerSprite->lastX;
        playerSprite->spr.y = playerSprite->lastY;
        playerSprite->mapScreen = playerSprite->lastMap;
    }
    if (theseScripts)
        free(theseScripts);
    free(collisionData);

    if (exitCode == 2 || exitCode == 3)
    {
        initEnemy(&bossSprite, -TILE_SIZE, -TILE_SIZE, 0, 0, 0, 1, type_boss);
        loadBoss = true;
        script resetScript;
        initScript(&resetScript, script_boss_actions, 0, 0, 0, 0, 0, "r", -1);
        executeScriptAction(&resetScript, playerSprite);  //resets boss movement timer
    }

    return exitCode;
}

void smoothScrolling(player* playerSprite, int newMapLine, int moveX, int moveY)
{
    int newTilemap[HEIGHT_IN_TILES][WIDTH_IN_TILES], newEventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
    loadMapFile(mapFilePath, newTilemap, newEventmap, newMapLine, HEIGHT_IN_TILES, WIDTH_IN_TILES);
    int i = 0, j = 0;
    bool quit = moveX == moveY;
    while (!quit)
    {
        SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
        drawATilemap(tilesTexture, tilemap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, -i, -j, -1, false);
        drawOverTilemap(tilesTexture, eventmap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, -i, -j, doorFlags, false, false);
        drawATilemap(tilesTexture, newTilemap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, (SCREEN_WIDTH * ((moveX > 0) - (moveX < 0))) - i, (SCREEN_HEIGHT * ((moveY > 0) - (moveY < 0))) - j, -1, false);
        drawOverTilemap(tilesTexture, newEventmap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, (SCREEN_WIDTH * ((moveX > 0) - (moveX < 0))) - i, (SCREEN_HEIGHT * ((moveY > 0) - (moveY < 0))) - j, doorFlags, true, false);
        for(int x = 0; x < MAX_ENEMIES; x++)
            drawATile(tilesTexture, enemies[x].spr.tileIndex, enemies[x].spr.x - i, enemies[x].spr.y - j, TILE_SIZE, TILE_SIZE, 0, enemies[x].spr.flip);

        drawATile(tilesTexture, playerSprite->spr.tileIndex, playerSprite->spr.x - i, playerSprite->spr.y - j, TILE_SIZE, TILE_SIZE, 0, playerSprite->spr.flip);
        SDL_RenderPresent(mainRenderer);
        //SDL_Delay(50);
        i += moveX;
        j += moveY;
        if (moveX > 0)
            quit = i > SCREEN_WIDTH;
        else if (moveX < 0)
            quit = i < -SCREEN_WIDTH;

        if (moveY > 0)
            quit = quit | (j > SCREEN_HEIGHT);
        else if (moveY < 0)
            quit = quit | (j < -SCREEN_HEIGHT);
        //quit = ((moveX > 0) ? (i < moveX * SCREEN_WIDTH) : (moveX < 0 ? (i > moveX * SCREEN_WIDTH) : true)) && ((moveY > 0) ? (j < moveY * SCREEN_HEIGHT) : (moveX < 0 ? (j > moveY * SCREEN_HEIGHT) : true));
    }

    if (moveX < 0)
        playerSprite->spr.x = SCREEN_WIDTH - TILE_SIZE - 6;

    if (moveY < 0)
        playerSprite->spr.y = SCREEN_HEIGHT - TILE_SIZE - 6;

    if (moveX > 0)
        playerSprite->spr.x = 6;

    if (moveY > 0)
        playerSprite->spr.y = 6;

    loadMapFile(mapFilePath, tilemap, eventmap, newMapLine, HEIGHT_IN_TILES, WIDTH_IN_TILES);
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
        if (-1 != intSeqSearch(1, (int[]) {topLeft, topRight, bottomLeft, bottomRight}, 4) && !noclip)
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
            if (-1 != intSeqSearch(i + 1, (int[]) {topLeft, topRight, bottomLeft, bottomRight}, 4))
                outputData[i] = true;
        }
    }
}

void drawSparks(spark* s)
{
    Uint8 oldR, oldG, oldB, oldA;
    SDL_GetRenderDrawColor(mainRenderer, &oldR, &oldG, &oldB, &oldA);
    for(int i = 0; i < (s->num < 99 ? s->num : 99); i++)
    {
        if (s->color.a == 0)
            s->color = (SDL_Color) {.r = rand() % 256, .g = rand() % 256, .b = rand() % 256, .a = 0xFF};
        SDL_SetRenderDrawColor(mainRenderer, s->color.r, s->color.g, s->color.b, s->color.a);

        if (s->timer % s->update == 0)
            s->sparkRects[i] = (SDL_Rect) {.x = s->boundsRect.x + (rand() % s->boundsRect.w), .y = s->boundsRect.y + (rand() % s->boundsRect.h), .w = 1 + rand() % s->maxW, .h = 1 + rand() % s->maxH};
        //printf("%d spark: (%d, %d) [%d, %d]\n", i, s->sparkRects[i].x, s->sparkRects[i].y, s->sparkRects[i].w, s->sparkRects[i].h);
        SDL_RenderFillRect(mainRenderer, &(s->sparkRects[i]));
    }
    s->timer--;
    SDL_SetRenderDrawColor(mainRenderer, oldR, oldG, oldB, oldA);
}

void aMenu_drawMain()
{
    drawATile(tilesetTexture, TILE_ID_TILDA, 2, 0, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
    drawATile(tilesetTexture, TILE_ID_CUBED, TILE_SIZE, 0, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
    drawATile(tilesetTexture, TILE_ID_TILDA, 2 * TILE_SIZE - 2, 0, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
    drawText(VERSION_NUMBER, 2.25 * TILE_SIZE, 11 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 11) * TILE_SIZE, (SDL_Color){AMENU_MAIN_TEXTCOLOR}, false);
}

void aMenu_drawMoney()
{
    char* moneyStr = calloc(10, sizeof(char));
    strcpy(moneyStr, intToString(_globalInt1, moneyStr));
    strcat(moneyStr, " Coins");
    drawText(moneyStr, 2.25 * TILE_SIZE, 11.5 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
    char* moneyEachStr = calloc(15, sizeof(char));
    strcpy(moneyEachStr, intToString(_globalInt2, moneyEachStr));
    strcat(moneyEachStr, " Coins Each");
    drawText(moneyEachStr, 2.25 * TILE_SIZE, 13.5 * TILE_SIZE, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
    free(moneyStr);
    free(moneyEachStr);
}

int gameOver()
{
    Mix_PlayMusic(MUSIC(7), -1);
    if (1 == aMenu(tilesTexture, CURSOR_ID, "Game Over.\nContinue?", (char*[2]) {"Yes", "No"}, 2, 0, AMENU_GAMEOVER_THEME, true, false, NULL))
    {
        Mix_HookMusicFinished(playOverworldMusic);
        return 1;
    }
    else
    {
        Mix_HookMusicFinished(playMainMusic);
        return 0;
    }
}

void screenTransitions()
{

    SDL_Texture* transitionScreen;
    loadIMG("assets/loadingscreen.png", &transitionScreen);
    SDL_RenderCopy(mainRenderer, transitionScreen, NULL, NULL);
    SDL_RenderPresent(mainRenderer);
    SDL_Delay(400 + rand() % 500);
    Mix_FadeOutMusic(500);
    for(int i = 0; i < 255; i++)
    {
        SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, i);
        SDL_RenderCopy(mainRenderer, transitionScreen, NULL, NULL);
        SDL_RenderFillRect(mainRenderer, NULL);
        SDL_RenderPresent(mainRenderer);
        SDL_Delay(2);
    }
}
