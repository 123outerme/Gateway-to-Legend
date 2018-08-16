#include "SDLGateway.h"

int initSounds()
{
    musicIndex = -1;
    MUSIC(1) = Mix_LoadMUS(MAIN_MUSIC_FILE);  //Gateway to Legend: Main Theme by Ian Groat
    if (!MUSIC(1))
    {
        strncpy(customError, "Error: Main theme missing.", 256);
        return -5;
    }

    MUSIC(2) = Mix_LoadMUS(OVERWORLD1_MUSIC_FILE);  //Gateway to Legend: GtL3 by _iPhoenix_
    if (!MUSIC(2))
    {
        strncpy(customError, "Error: Overworld 1 Theme missing.", 256);
        return -5;
    }

    MUSIC(3) = Mix_LoadMUS(OVERWORLD2_MUSIC_FILE);  //Gateway to Legend: GtL5 by _iPhoenix_
    if (!MUSIC(3))
    {
        strncpy(customError, "Error: Overworld 2 Theme missing.", 256);
        return -5;
    }

    /*MUSIC(4) = Mix_LoadMUS(OVERWORLD3_MUSIC_FILE);
    if (!MUSIC(4))
    {
        strncpy(customError, "Error: Overworld 3 Theme missing.", 256);
        return -5;
    }*/

    MUSIC(5) = Mix_LoadMUS(BOSS_MUSIC_FILE);  //Gateway to Legend: Boss Theme 1 by Ian Groat
    if (!MUSIC(5))
    {
        strncpy(customError, "Error: Boss theme missing.", 256);
        return -5;
    }

    MUSIC(6) = Mix_LoadMUS(FANFARE_MUSIC_FILE);  //Gateway to Legend: GtL4 by _iPhoenix_
    if (!MUSIC(6))
    {
        strncpy(customError, "Error: Fanfare missing.", 256);
        return -5;
    }

    MUSIC(7) = Mix_LoadMUS(GAMEOVER_MUSIC_FILE);  //Gateway to Legend: Gateway to Retry by Ian Groat
    if (!MUSIC(7))
    {
        strncpy(customError, "Error: Game Over theme missing.", 256);
        return -5;
    }

    PAUSE_SOUND = Mix_LoadWAV(PAUSE_FILE);
    if (!PAUSE_SOUND)
    {
        strncpy(customError, "Error: No such sound for PAUSE_SOUND", 256);
        return -5;
    }

    UNSHEATH_SOUND = Mix_LoadWAV(UNSHEATH_FILE);
    if (!UNSHEATH_SOUND)
    {
        strncpy(customError, "Error: No such sound for UNSHEATH_SOUND", 256);
        return -5;
    }

    OPTION_SOUND = Mix_LoadWAV(OPTION_FILE);
    if (!OPTION_SOUND)
    {
        strncpy(customError, "Error: No such sound for OPTION_SOUND", 256);
        return -5;
    }

    PING_SOUND = Mix_LoadWAV(PING_FILE);
    if (!PING_SOUND)
    if (!OPTION_SOUND)
    {
        strncpy(customError, "Error: No such sound for PING_SOUND", 256);
        return -5;
    }

    for(int i = 1; i < 4; i++)
    {
        char filepath[23] = STEP_FILE;
        snprintf(filepath, 23, "%s%d.ogg", STEP_FILE, i);
        STEP_SOUND(i) = Mix_LoadWAV(filepath);
        if (!STEP_SOUND(i))
        {
            strncpy(customError, "Error: No such sound for a STEP_SOUND", 256);
            return -5;
        }
    }

    SWING_SOUND = Mix_LoadWAV(SWING_FILE);
    if (!SWING_SOUND)
    {
        strncpy(customError, "Error: No such sound for SWING_SOUND", 256);
        return -5;
    }

    GATEWAYSTART_SOUND = Mix_LoadWAV(GATEWAYSTART_FILE);
    if (!GATEWAYSTART_SOUND)
    {
        strncpy(customError, "Error: No such sound for GATEWAYSTART_SOUND", 256);
        return -5;
    }

    GATEWAYEND_SOUND = Mix_LoadWAV(GATEWAYEND_FILE);
    if (!GATEWAYEND_SOUND)
    {
        strncpy(customError, "Error: No such sound for GATEWAYEND_SOUND", 256);
        return -5;
    }

    DOOROPEN_SOUND = Mix_LoadWAV(DOOROPEN_FILE);
    if (!DOOROPEN_SOUND)
    {
        strncpy(customError, "Error: No such sound for DOOROPEN_SOUND", 256);
        return -5;
    }

    CASH_SOUND = Mix_LoadWAV(CASH_FILE);
    if (!CASH_SOUND)
    {
        strncpy(customError, "Error: No such sound for CASH_SOUND", 256);
        return -5;
    }

    PLAYERHURT_SOUND = Mix_LoadWAV(PLAYERHURT_FILE);
    if (!PLAYERHURT_SOUND)
    {
        strncpy(customError, "Error: No such sound for PLAYERHURT_SOUND", 256);
        return -5;
    }

    ENEMYHURT_SOUND = Mix_LoadWAV(ENEMYHURT_FILE);
    if (!ENEMYHURT_SOUND)
    {
        strncpy(customError, "Error: No such sound for ENEMYHURT_SOUND", 256);
        return -5;
    }

    TELEPORT_SOUND = Mix_LoadWAV(TELEPORT_FILE);
    if (!TELEPORT_SOUND)
    {
        strncpy(customError, "Error: No such sound for TELEPORT_SOUND", 256);
        return -5;
    }

    HEAL_SOUND = Mix_LoadWAV(HEAL_FILE);
    if (!HEAL_SOUND)
    {
        strncpy(customError, "Error: No such sound for HEAL_SOUND", 256);
        return -5;
    }

    for(int i = 1; i < 4; i++)
    {
        char filepath[23] = STEP_FILE;
        snprintf(filepath, 23, "%s%d.ogg", DASH_FILE, i);
        DASH_SOUND(i) = Mix_LoadWAV(filepath);
        if (!DASH_SOUND(i))
        {
            strncpy(customError, "Error: No such sound for a DASH_SOUND", 256);
            return -5;
        }
    }

    return 0;
}

void loadMapPackData(mapPack* loadPack, char* location)
{
    char buffer[MAX_FILE_PATH];
    strcpy(loadPack->mainFilePath, location);
    uniqueReadLine((char**) &buffer, MAX_FILE_PATH, location, 0);
    strcpy(loadPack->name, buffer);
    uniqueReadLine((char**) &buffer, MAX_FILE_PATH, location, 1);
    strcpy(loadPack->mapFilePath, buffer);
    uniqueReadLine((char**) &buffer, MAX_FILE_PATH, location, 2);
    strcpy(loadPack->tilesetFilePath, buffer);
    uniqueReadLine((char**) &buffer, MAX_FILE_PATH, location, 3);
    strcpy(loadPack->saveFilePath, buffer);
    uniqueReadLine((char**) &buffer, MAX_FILE_PATH, location, 4);
    strcpy(loadPack->scriptFilePath, buffer);
    loadPack->initX = strtol(readLine(loadPack->mainFilePath, 5, (char**) &buffer), NULL, 10);
    loadPack->initY = strtol(readLine(loadPack->mainFilePath, 6, (char**) &buffer), NULL, 10);
    loadPack->initMap = strtol(readLine(loadPack->mainFilePath, 7, (char**) &buffer), NULL, 10);
    for(int i = 0; i < MAX_SPRITE_MAPPINGS; i++)
        loadPack->tilesetMaps[i] = strtol(readLine(loadPack->mainFilePath, i + 8, (char**) &buffer), NULL, 10);

    loadPack->numBosses = strtol(readLine(loadPack->mainFilePath, MAX_SPRITE_MAPPINGS, (char**) &buffer), NULL, 10);

    int countBosses = 0;
    for(int i = 0; i < checkFile(loadPack->scriptFilePath, -1); i++)
    {
        char* temp = "";
        script aScript;
        readScript(&aScript, readLine(loadPack->scriptFilePath, i, &temp), i);
        if (aScript.action == script_boss_actions)
            countBosses++;
    }
    if (countBosses != loadPack->numBosses)
    {
        loadPack->numBosses = countBosses;
        saveMapPack(loadPack);
    }

    loadIMG(loadPack->tilesetFilePath, &(loadPack->mapPackTexture));
}

void initPlayer(player* player, int x, int y, int w, int h, int mapScreen, int angle, SDL_RendererFlip flip, int tileIndex)
{
    initSprite(&(player->spr), x, y, w, h, tileIndex, angle, flip, type_player);
    strcpy(player->name, "");
    player->level = 1;
    player->money = 0;
    player->HP = DEFAULT_PLAYER_HEALTH;
    player->maxHP = DEFAULT_PLAYER_HEALTH;
    player->mapScreen = mapScreen;
    player->invincCounter = 0;
    player->animationCounter = 0;
    player->movementLocked = false;
    player->extraData = "";
    player->xVeloc = 0;
    player->yVeloc = 0;
    player->lastMap = -1;
    player->lastX = -1;
    player->lastY = -1;
    player->lastDirection = 8; //facing right
    player->nextBossPos = 0;
    for(int i = 0; i < MAX_PLAYER_TECHNIQUES; i++)
        player->techUnlocks[i] = false;
    //name, x, y, w, level, HP, maxHP, attack, speed, statPts, move1 - move4, steps, worldNum, mapScreen, lastScreen, overworldX, overworldY
}

void createLocalPlayer(player* playerSprite, char* filePath, int x, int y, int w, int h, int mapScreen, int angle, SDL_RendererFlip flip, int tileIndex, int numScripts)
{
    initPlayer(playerSprite, x, y, w, h, mapScreen, angle, flip, tileIndex);
    playerSprite->HP = playerSprite->maxHP;
    for(int i = 0; i < maxBosses; i++)
    {
        playerSprite->defeatedBosses[i] = -1;
    }
	playerSprite->disabledScripts = calloc(numScripts, sizeof(int));
	for(int i = 0; i < numScripts; i++)
    {
        playerSprite->disabledScripts[i] = 0;
    }
    maxScripts = numScripts;
    saveLocalPlayer(*playerSprite, filePath);
}

void createGlobalPlayer(player* playerSprite, char* filePath)
{
    char* newName = calloc(MAX_PLAYER_NAME + 1, sizeof(char));
    stringInput(&newName, "Your name?", MAX_PLAYER_NAME, "Player", true);
    strncpy(playerSprite->name, newName, strlen(newName));
    free(newName);
    playerSprite->maxHP = 12;
    playerSprite->level = 1;
    playerSprite->money = 0;
    for(int i = 0; i < MAX_PLAYER_TECHNIQUES; i++)
    {
        playerSprite->techUnlocks[i] = 0;
    }
	saveGlobalPlayer(*playerSprite, filePath);
    //saves: name, max HP, level, money, techniques
}

void initEnemy(enemy* enemyPtr, int x, int y, int w, int h, int tileIndex, int HP, entityType type)
{
    initSprite(&(enemyPtr->spr), x, y, w, h, tileIndex, 0, SDL_FLIP_NONE, type);
    enemyPtr->HP = HP;
    enemyPtr->invincTimer = 0;
}

void initConfig(char* filePath)
{
    SC_UP = SDL_SCANCODE_W;
    SC_DOWN = SDL_SCANCODE_S;
    SC_LEFT = SDL_SCANCODE_A;
    SC_RIGHT = SDL_SCANCODE_D;
    SC_INTERACT = SDL_SCANCODE_SPACE;
    SC_MENU = SDL_SCANCODE_ESCAPE;
    SC_SPECIAL = SDL_SCANCODE_LSHIFT;
    FPS = 60;
    targetTime = calcWaitTime(FPS);
    soundVolume = MIX_MAX_VOLUME;
    musicVolume = MIX_MAX_VOLUME;
    saveConfig(filePath);
}

void initScript(script* scriptPtr, scriptBehavior action, int mapNum, int x, int y, int w, int h, char* data, int lineNum)
{
	scriptPtr->action = action;
	scriptPtr->mapNum = mapNum;
	scriptPtr->x = x;
	scriptPtr->y = y;
	scriptPtr->w = w;
	scriptPtr->h = h;
	strcpy(scriptPtr->data, data);
	scriptPtr->active = true;
	scriptPtr->disabled = false;
	scriptPtr->lineNum = lineNum;
}

void initNode(node* nodePtr, int x, int y, node* lastNode, bool visited, int distance)
{
    nodePtr->x = x;
    nodePtr->y = y;
    nodePtr->lastNode = lastNode;
    nodePtr->visited = visited;
    nodePtr->distance = distance;
}

void initSpark(spark* sparkPtr, SDL_Rect boundsRect, SDL_Color color, int num, int maxW, int maxH, int maxTimer, int update)
{
    for(int i = 0; i < num; i++)
    {
        sparkPtr->sparkRects[i] = (SDL_Rect) {0, 0, 0, 0};
    }
    sparkPtr->boundsRect = boundsRect;
    sparkPtr->color = color;
    sparkPtr->num = num;
    sparkPtr->maxW = maxW;
    sparkPtr->maxH = maxH;
    sparkPtr->timer = maxTimer;
    sparkPtr->maxTimer = maxTimer;
    sparkPtr->update = update;
}

void loadConfig(char* filePath)
{
    char* buffer = "";
    for(int i = 0; i < SIZE_OF_SCANCODE_ARRAY; i++)
    {
        readLine(filePath, i, &buffer);
        CUSTOM_SCANCODES[i] = strtol(buffer, NULL, 10);
    }
    readLine(filePath, SIZE_OF_SCANCODE_ARRAY, &buffer);
    FPS = strtol(strtok(buffer, "FPS="), NULL, 10);
    targetTime = calcWaitTime(FPS);
    readLine(filePath, SIZE_OF_SCANCODE_ARRAY + 1, &buffer);
    soundVolume = strtol(buffer, NULL, 10);
    Mix_Volume(-1, soundVolume);
    readLine(filePath, SIZE_OF_SCANCODE_ARRAY + 2, &buffer);
    musicVolume = strtol(buffer, NULL, 10);
    Mix_VolumeMusic(musicVolume);
}

void loadLocalPlayer(player* playerSprite, char* filePath, int tileIndex)
{
    char* buffer = "";
    playerSprite->mapScreen = strtol(readLine(filePath, 0, &buffer), NULL, 10);
    playerSprite->spr.x = strtol(readLine(filePath, 1, &buffer), NULL, 10);
    playerSprite->spr.y = strtol(readLine(filePath, 2, &buffer), NULL, 10);
    playerSprite->HP = strtol(readLine(filePath, 3, &buffer), NULL, 10);
    playerSprite->lastMap = strtol(readLine(filePath, 4, &buffer), NULL, 10);
    playerSprite->lastX = strtol(readLine(filePath, 5, &buffer), NULL, 10);
    playerSprite->lastY = strtol(readLine(filePath, 6, &buffer), NULL, 10);
    char* data = readLine(filePath, 7, &data);
    char* dataCpy = calloc(3 * maxBosses, sizeof(data));
    strcpy(dataCpy, data);
	for(int i = 0; i < maxBosses; i++)
	{
		playerSprite->defeatedBosses[i] = -1;
	}
    playerSprite->defeatedBosses[0] = strtol(strtok(dataCpy, "{,}"), NULL, 10);
    if (playerSprite->defeatedBosses[0] == -1)
        playerSprite->nextBossPos = 0;
	else
		playerSprite->nextBossPos = 1;
    for(int i = 1; i < maxBosses; i++)
    {
        playerSprite->defeatedBosses[i] = strtol(strtok(NULL, "{,}"), NULL, 10);
        if (playerSprite->defeatedBosses[i] == -1 && playerSprite->nextBossPos == -1)
            playerSprite->nextBossPos = i;
    }
    char* disabledScriptData = readLine(filePath, 8, &buffer);
    int maxScriptSize = 100;
    playerSprite->disabledScripts = calloc(maxScriptSize, sizeof(int));
    playerSprite->disabledScripts[0] = strtol(strtok(disabledScriptData, "{,}"), NULL, 10);
    int i = 1;
    if (playerSprite->disabledScripts[0] == 0 || playerSprite->disabledScripts[0] == 1)
    {
        bool quit = false;
        while(!quit)
        {
            char* dataPt = strtok(NULL, "{,}");
            if (dataPt != NULL)
                playerSprite->disabledScripts[i++] = strtol(dataPt, NULL, 10);
            else
                quit = true;

            if (i > maxScriptSize)  //hopefully this never goes off because I can't get it to work
            {
                int* temp = realloc(playerSprite->disabledScripts, maxScriptSize + 10);
                if (temp != NULL)
                {
                    maxScriptSize += 10;
                    playerSprite->disabledScripts = temp;
                }
                else
                    quit = true;
            }
        }
    }
    maxScripts = i;
    //done with scripts
    playerSprite->spr.tileIndex = tileIndex;
    playerSprite->spr.w = TILE_SIZE;
    playerSprite->spr.h = TILE_SIZE;
    playerSprite->spr.angle = 0;
    playerSprite->spr.flip = SDL_FLIP_NONE;
    playerSprite->movementLocked = false;
    playerSprite->xVeloc = 0;
    playerSprite->yVeloc = 0;
    playerSprite->lastDirection = 8;
    playerSprite->invincCounter = 0;
    playerSprite->animationCounter = 0;
    //loads: map, x, y, current HP, beaten bosses, disabled scripts
}

void loadGlobalPlayer(player* playerSprite, char* filePath)
{
    char* buffer = "";
    strcpy(playerSprite->name, readLine(filePath, 0, &buffer));
    playerSprite->name[strnlen(playerSprite->name, 10) - 1] = 0;  //removes \n
    playerSprite->maxHP = strtol(readLine(filePath, 1, &buffer), NULL, 10);
    playerSprite->level = strtol(readLine(filePath, 2, &buffer), NULL, 10);
    playerSprite->money = strtol(readLine(filePath, 3, &buffer), NULL, 10);
    playerSprite->movementLocked = false;
    playerSprite->xVeloc = 0;
    playerSprite->yVeloc = 0;
    playerSprite->lastDirection = 8;
    playerSprite->invincCounter = 0;
    playerSprite->animationCounter = 0;
    char* thing;
    readLine(filePath, 4, &thing);
    playerSprite->techUnlocks[0] = strtol(strtok(thing, "{,}"), NULL, 10);
    for(int i = 1; i < MAX_PLAYER_TECHNIQUES; i++)
        playerSprite->techUnlocks[i] = strtol(strtok(NULL, "{,}"), NULL, 10);
    //loads: name, max HP, level, exp, money
}

void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x)
{
    int numsC = 0, numsR = 0,  i = y, num;  //no reason to set i to y, just warding off warnings
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

//this one is more general purpose drawing
void drawAMap(SDL_Texture* tileTexture, int thisTilemap[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, bool hideCollision, bool isEvent, bool updateScreen)
{
        int tile = 0;
        for(int dy = startY; dy < endY; dy++)
            for(int dx = startX; dx < endX; dx++)
            {
                if (isEvent)
                {
                    tile = tileIDArray[thisTilemap[dy][dx] + 4];  //add 4 to start at buttons
                    if (thisTilemap[dy][dx] < 2)
                        tile = 127 - (thisTilemap[dy][dx] == 1 && !hideCollision);
                    drawATile(thisTilemap[dy][dx] < 2 ? tilesetTexture : tileTexture, tile, dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
                }
                else
                    drawATile(tileTexture, thisTilemap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
            }

    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}

//This one is used for ingame drawing (when enemies are loaded)
void drawOverTilemap(SDL_Texture* texture, int anEventmap[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, int xOffset, int yOffset, bool drawDoors[], bool drawEnemies, bool rerender)
{
    int searchIndex = 0;
    for(int y = startY; y < endY; y++)
        for(int x = startX; x < endX; x++)
        {
            searchIndex = anEventmap[y][x] + 5 - (anEventmap[y][x] > 0);  //search index for these tiles is beyond HUD/player slots -> Minus 1 because there's only 1 index for invis tile but two cases right next to each other that need it
            if (((searchIndex == 9 || searchIndex == 10 || searchIndex == 11 || searchIndex == 12) && drawDoors[searchIndex < 13 ? searchIndex - 9 : 0] == false) || (!drawEnemies && (searchIndex == 16 || searchIndex == 17 || searchIndex == 18)))  //8,9,10,11 are the door indexes <- this may be old
                searchIndex = 5;  //5 is index for invis tile
            drawATile(texture, tileIDArray[searchIndex], x * TILE_SIZE + xOffset, y * TILE_SIZE + yOffset, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        }
    if (rerender)
        SDL_RenderPresent(mainRenderer);
}

int aMenu(SDL_Texture* texture, int cursorID, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain, void (*extraDrawing)(void))
{
    const int MAX_ITEMS = 9;
    if (curSelect < 1)
        curSelect = 1;
    if (options < 0)
        return ANYWHERE_QUIT;
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, (curSelect + 4) * TILE_SIZE, TILE_SIZE, TILE_SIZE, cursorID, 0, SDL_FLIP_NONE, (entityType) type_na);
    SDL_Event e;
    bool quit = false, settingsReset = false;
    int selection = -1;
    //While application is running
    while(!quit)
    {
        if (border)
            SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
        else
            SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);

        SDL_RenderClear(mainRenderer);
        SDL_RenderFillRect(mainRenderer, NULL);
        SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        //background text (drawn first)
        drawText(title, TILE_SIZE + TILE_SIZE / (2 + 2 * !isMain), 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, titleColorUnder, false);
        //foreground text
        drawText(title, TILE_SIZE + TILE_SIZE / (2 + 2 * !isMain), 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, titleColorOver, false);

        for(int i = 0; ((options <= MAX_ITEMS) ? i < options : i < MAX_ITEMS); i++)
	        drawText(optionsArray[i], 2.25 * TILE_SIZE, (5 + i) * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - (5 + i)) * TILE_SIZE, textColor, false);
        if (extraDrawing)
            (*extraDrawing)();

        //SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.w}));
        //Handle events on queue
        while(SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if(e.type == SDL_QUIT)
            {
                quit = true;
                selection = ANYWHERE_QUIT;
                Mix_PlayChannel(-1, OPTION_SOUND, 0);
            }
            //User presses a key
            else
            {
                if(e.type == SDL_KEYDOWN)
                {
                    const Uint8* keyStates = SDL_GetKeyboardState(NULL);
                    if (keyStates[SC_UP] && cursor.y > 5 * TILE_SIZE)
                    {
                        cursor.y -= TILE_SIZE;
                        Mix_PlayChannel(-1, PING_SOUND, 0);
                    }

                    if (keyStates[SC_DOWN] && cursor.y < (options + 4) * TILE_SIZE)
                    {
                        cursor.y += TILE_SIZE;
                        Mix_PlayChannel(-1, PING_SOUND, 0);
                    }

                    if (keyStates[SC_INTERACT])
                    {
                        selection = cursor.y / TILE_SIZE - 4;
                        quit = true;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                    }
                    if (isMain && (keyStates[SDL_SCANCODE_LCTRL] || keyStates[SDL_SCANCODE_RCTRL]) && keyStates[SDL_SCANCODE_R] && !settingsReset)
                    {
                        SC_UP = SDL_SCANCODE_W;
                        SC_DOWN = SDL_SCANCODE_S;
                        SC_LEFT = SDL_SCANCODE_A;
                        SC_RIGHT = SDL_SCANCODE_D;
                        SC_SPECIAL = SDL_SCANCODE_LSHIFT;
                        SC_INTERACT = SDL_SCANCODE_SPACE;
                        SC_MENU = SDL_SCANCODE_ESCAPE;
                        saveConfig(CONFIG_FILEPATH);
                        Mix_PlayChannel(-1, PLAYERHURT_SOUND, 0);
                        settingsReset = true;
                    }
                    if (isMain && ((keyStates[SDL_SCANCODE_LCTRL] || keyStates[SDL_SCANCODE_RCTRL]) && keyStates[SDL_SCANCODE_M]))
                    {
                        static int prevMusicVolume = 0;
                        static int prevSoundVolume = 0;
                        if (musicVolume != 0 && soundVolume != 0)
                        {

                            prevMusicVolume = (musicVolume ? musicVolume : MIX_MAX_VOLUME);
                            prevSoundVolume = (soundVolume ? soundVolume : MIX_MAX_VOLUME);
                            musicVolume = 0;
                            soundVolume = 0;
                        }
                        else
                        {
                            musicVolume = prevMusicVolume;
                            soundVolume = prevSoundVolume;
                        }
                        Mix_Volume(-1, soundVolume);
                        Mix_VolumeMusic(musicVolume);
                    }
                }
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                {

                    int choice = (e.button.y / TILE_SIZE) - 4;
                    if (choice > 0 && choice <= options)
                    {
                        selection = choice;
                        Mix_PlayChannel(-1, OPTION_SOUND, 0);
                        quit = true;
                    }
                }
                if (e.type == SDL_MOUSEMOTION)
                {
                    if (e.motion.y / TILE_SIZE > 4 && e.motion.y / TILE_SIZE < options + 5)
                        cursor.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                }
            }
        }
        drawATile(texture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
    return selection;
}

void stringInput(char** data, char* prompt, int maxChar, char* defaultStr, bool startCaps)
{
    char* stringData = calloc(maxChar + 1, sizeof(char)), * dispString = calloc(maxChar + 2, sizeof(char));
    stringData[0] = ' ';
    dispString[0] = '_';
    bool quit = false, hasTyped = false, capital = startCaps;
    int numChar = 0, frame = 0;
    SDL_Event e;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_TEXTCOLOR);
            SDL_RenderClear(mainRenderer);
            SDL_RenderFillRect(mainRenderer, NULL);
            SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
            SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
            drawText(prompt, SCREEN_WIDTH / 64, SCREEN_WIDTH / 64, 63 * SCREEN_WIDTH / 64, 63 * SCREEN_HEIGHT / 64, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
            if(e.type == SDL_QUIT)
            {
                strcpy(*data, "\"\0");
                quit = 2;
                hasTyped = false;
            }

            if (e.type != SDL_KEYDOWN)
                frame++;
            else
            {
                if ((e.key.keysym.sym >= SDLK_SPACE && e.key.keysym.sym <= SDLK_z) && numChar < maxChar)
                {
                    char* temp = calloc(1, sizeof(char));
                    strncpy(temp, SDL_GetKeyName(e.key.keysym.sym), 1);
                    if (e.key.keysym.sym == SDLK_SPACE)  //space doesn't work until here
                        temp[0] = ' ';
                    else if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z && !capital)
                        temp[0] = (char) tolower(temp[0]);

                    if (capital)  //top row, hardcoded
                    {
                        if (e.key.keysym.sym == SDLK_BACKQUOTE)
                            temp[0] = '~';
                        if (e.key.keysym.sym == SDLK_1)
                            temp[0] = '!';
                        if (e.key.keysym.sym == SDLK_2)
                            temp[0] = '@';
                        if (e.key.keysym.sym == SDLK_3)
                            temp[0] = '#';
                        if (e.key.keysym.sym == SDLK_4)
                            temp[0] = '$';
                        if (e.key.keysym.sym == SDLK_5)
                            temp[0] = '%';
                        if (e.key.keysym.sym == SDLK_6)
                            temp[0] = '^';
                        if (e.key.keysym.sym == SDLK_7)
                            temp[0] = '&';
                        if (e.key.keysym.sym == SDLK_8)
                            temp[0] = '*';
                        if (e.key.keysym.sym == SDLK_9)
                            temp[0] = '(';
                        if (e.key.keysym.sym == SDLK_0)
                            temp[0] = ')';
                        if (e.key.keysym.sym == SDLK_MINUS)
                            temp[0] = '_';
                        if (e.key.keysym.sym == SDLK_EQUALS)
                            temp[0] = '+';

                        if (e.key.keysym.sym == SDLK_BACKSLASH)
                            temp[0] = '|';
                        if (e.key.keysym.sym == SDLK_SEMICOLON)
                            temp[0] = ':';
                        if (e.key.keysym.sym == SDLK_QUOTE)
                            temp[0] = '\"';
                        if (e.key.keysym.sym == SDLK_SLASH)
                            temp[0] = '?';
                        if (e.key.keysym.sym == SDLK_COMMA)
                            temp[0] = '<';
                        if (e.key.keysym.sym == SDLK_PERIOD)
                            temp[0] = '>';
                    }


                    stringData[numChar++] = temp[0];
                    hasTyped = true;
                    strncpy(dispString, stringData, numChar);
                    if (numChar < maxChar)
                        strncat(dispString, "_\0", numChar + 1);
                }

                if (e.key.keysym.sym == SDLK_BACKSPACE && numChar > 0)
                {
                    stringData[--numChar] = '\0';
                    hasTyped = (numChar > 0);
                    strncpy(dispString, stringData, numChar + 1);
                    strncat(dispString, "_\0", numChar + 1);
                }

                if (e.key.keysym.sym == SDLK_LSHIFT || e.key.keysym.sym == SDLK_RSHIFT)
                    capital = !capital;

                if (e.key.keysym.scancode == SC_MENU || e.key.keysym.scancode == SDL_SCANCODE_RETURN)
                    quit = true;
            }

            if (capital)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_TEXTCOLOR);
                drawText("/", .5 * TILE_SIZE, 13.3 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
                drawText("\\", TILE_SIZE, 13.3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
                SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = TILE_SIZE, .y = 13.6 * TILE_SIZE, .w = .25 * TILE_SIZE, .h = .9 * TILE_SIZE}));
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
            }
            drawText(dispString, 2 * TILE_SIZE, 3.5 * TILE_SIZE, SCREEN_WIDTH - 3.5 * TILE_SIZE, SCREEN_HEIGHT - 4 * TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
            SDL_RenderPresent(mainRenderer);
        }
    }
    Mix_PlayChannel(-1, OPTION_SOUND, 0);

    if (quit != 2)
    {
        if (!hasTyped || !strlen(stringData))
            strncpy(*data, defaultStr, strlen(defaultStr));
        else
            strncpy(*data, stringData, numChar);
    }
    free(stringData);
    free(dispString);
}

int intInput(char* prompt, int maxDigits, int defaultNum, int minVal, int maxVal, bool allowNeg)
{
    char* stringData = calloc(maxDigits + 1, sizeof(char)), * dispString = calloc(maxDigits + 2, sizeof(char));
    stringData[0] = ' ';
    dispString[0] = '_';
    bool quit = false, hasTyped = false;
    int numDigits = 0, frame = 0;
    SDL_Event e;
    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_TEXTCOLOR);
            SDL_RenderClear(mainRenderer);
            SDL_RenderFillRect(mainRenderer, NULL);
            SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR);
            SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
            drawText(prompt, SCREEN_WIDTH / 64, SCREEN_WIDTH / 64, 63 * SCREEN_WIDTH / 64, 63 * SCREEN_HEIGHT / 64, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
            if(e.type == SDL_QUIT)
            {
                quit = true;
                hasTyped = false;
            }

            if (e.type != SDL_KEYDOWN)
                frame++;
            else
            {
                if (((e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) || (e.key.keysym.sym == SDLK_MINUS && (allowNeg && minVal < 0))) && numDigits < maxDigits)
                {
                    char* temp = calloc(1, sizeof(char));
                    strncpy(temp, SDL_GetKeyName(e.key.keysym.sym), 1);
                    stringData[numDigits++] = temp[0];
                    hasTyped = true;
                    strncpy(dispString, stringData, numDigits);
                    if (numDigits < maxDigits)
                        strncat(dispString, "_\0", numDigits + 1);
                }

                if (e.key.keysym.sym == SDLK_BACKSPACE && numDigits > 0)
                {
                    stringData[--numDigits] = '\0';
                    hasTyped = (numDigits > 0);
                    strncpy(dispString, stringData, numDigits + 1);
                    strncat(dispString, "_\0", numDigits + 1);
                }

                if (e.key.keysym.scancode == SC_MENU || e.key.keysym.scancode == SDL_SCANCODE_RETURN)
                    quit = true;
            }

            drawText(dispString, 2 * TILE_SIZE, 3.5 * TILE_SIZE, SCREEN_WIDTH - 3.5 * TILE_SIZE, SCREEN_HEIGHT - 4 * TILE_SIZE, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, false);
            SDL_RenderPresent(mainRenderer);
        }
    }
    Mix_PlayChannel(-1, OPTION_SOUND, 0);
    int retVal;

    if (!hasTyped || !strlen(stringData))
        retVal = defaultNum;
    else
        retVal = strtol(stringData, NULL, 10);

    if (retVal < minVal)
        retVal = minVal;

    if (retVal > maxVal)
        retVal = maxVal;

    free(stringData);
    free(dispString);
    return retVal;
}

void saveConfig(char* filePath)
{
    char* buffer = "";
    createFile(filePath);
    for(int i = 0; i < SIZE_OF_SCANCODE_ARRAY; i++)
    {
        appendLine(filePath, intToString(CUSTOM_SCANCODES[i], buffer));
    }
    char newBuffer[8];
    snprintf(newBuffer, 8, "FPS=%d", FPS);
    appendLine(filePath, newBuffer);
    appendLine(filePath, intToString(soundVolume, buffer));
    appendLine(filePath, intToString(musicVolume, buffer));
}

void saveMapPack(mapPack* writePack)
{
    char mapPackData[MAX_MAP_PACK_DATA][MAX_FILE_PATH];
    char* getString = "";
    strcpy(mapPackData[1], writePack->name);
    strcpy(mapPackData[2], writePack->mapFilePath);
    strcpy(mapPackData[3], writePack->tilesetFilePath);
    strcpy(mapPackData[4], writePack->saveFilePath);
    strcpy(mapPackData[5], writePack->scriptFilePath);
    createFile(writePack->mainFilePath);

    for(int i = 1; i < 6; i++)
        appendLine(writePack->mainFilePath, mapPackData[i]);

    appendLine(writePack->mainFilePath, intToString(writePack->initX, getString));
    appendLine(writePack->mainFilePath, intToString(writePack->initY, getString));
    appendLine(writePack->mainFilePath, intToString(writePack->initMap, getString));

    for(int i = 0; i < MAX_SPRITE_MAPPINGS; i++)
        appendLine(writePack->mainFilePath, intToString(writePack->tilesetMaps[i], getString));

    appendLine(writePack->mainFilePath, intToString(writePack->numBosses, getString));
}

void saveLocalPlayer(const player playerSprite, char* filePath)
{
    char* buffer = "";
    createFile(filePath);
    appendLine(filePath, intToString(playerSprite.mapScreen, buffer));
    appendLine(filePath, intToString(playerSprite.spr.x, buffer));
    appendLine(filePath, intToString(playerSprite.spr.y, buffer));
    appendLine(filePath, intToString(playerSprite.HP, buffer));
    appendLine(filePath, intToString(playerSprite.lastMap, buffer));
    appendLine(filePath, intToString(playerSprite.lastX, buffer));
    appendLine(filePath, intToString(playerSprite.lastY, buffer));
    char* beatBosses = calloc(maxBosses * 3, sizeof(char));
    beatBosses[0] = '{';
    for(int i = 0; i < maxBosses; i++)
    {
        strcat(beatBosses, intToString(playerSprite.defeatedBosses[i], buffer));
        strcat(beatBosses, (i < maxBosses - 1) ?  "," : "}");
    }
    appendLine(filePath, beatBosses);
    char* disabledScripts = calloc(maxScripts * 3, sizeof(char));
    disabledScripts[0] = '{';
    for(int i = 0; i < maxScripts; i++)
    {
        strcat(disabledScripts, intToString(playerSprite.disabledScripts[i], buffer));
        strcat(disabledScripts, (i < maxScripts - 1) ? "," : "}");
    }
    appendLine(filePath, disabledScripts);

    //free(beatBosses);

    beatBosses = NULL;
    free(disabledScripts);
    disabledScripts = NULL;
    //saves: map, x, y, current HP, beaten bosses, disabled scripts
}

void saveGlobalPlayer(const player playerSprite, char* filePath)
{
    char* buffer = "";
    createFile(filePath);
    appendLine(filePath, (char*) playerSprite.name);
    appendLine(filePath, intToString(playerSprite.maxHP, buffer));
    appendLine(filePath, intToString(playerSprite.level, buffer));
    appendLine(filePath, intToString(playerSprite.money, buffer));
    {  //saving skills
        const int maxStrSize = MAX_PLAYER_TECHNIQUES * 2 + 2;
        char* skillsArray = calloc(maxStrSize, sizeof(char));
        strncpy(skillsArray, "{", maxStrSize);
        for(int i = 0; i < MAX_PLAYER_TECHNIQUES; i++)
        {
            strncat(skillsArray, intToString(playerSprite.techUnlocks[i], buffer), maxStrSize);
            if (i < MAX_PLAYER_TECHNIQUES - 1)
                strncat(skillsArray, ",", maxStrSize);
        }
        strncat(skillsArray, "}", maxStrSize);
        appendLine(filePath, skillsArray);
    }
    //saves: name, max HP, level, exp, money
}

void getNewKey(char* titleText, SDL_Color bgColor, SDL_Color textColor, int selection)
{
    SDL_RenderClear(mainRenderer);
    SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
    SDL_RenderFillRect(mainRenderer, NULL);
    drawText(titleText, .5 * TILE_SIZE, 5 * SCREEN_HEIGHT / 64, (WIDTH_IN_TILES - .5) * TILE_SIZE, (HEIGHT_IN_TILES - 4) * TILE_SIZE, textColor, true);
    SDL_Keycode kc = waitForKey(false);
    bool conflict = false;
    for(int i = 0; i < SIZE_OF_SCANCODE_ARRAY; i++)
    {
        if (CUSTOM_SCANCODES[i] == SDL_GetScancodeFromKey(kc))
            conflict = true;
    }

    if (SDL_GetScancodeFromKey(kc) == SDL_SCANCODE_LCTRL || SDL_GetScancodeFromKey(kc) == SDL_SCANCODE_RCTRL || SDL_GetScancodeFromKey(kc) == SDL_SCANCODE_MINUS || SDL_GetScancodeFromKey(kc) == SDL_SCANCODE_EQUALS)
        conflict = true;
    if (!conflict)
        CUSTOM_SCANCODES[selection] = SDL_GetScancodeFromKey(kc);
}

char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum)
{
    char* dummy = "";
    readLine(filePath, lineNum, &dummy);
    strcpy((char*) output, dummy);
    dummy = removeChar((char*) output, '\n', outputLength, false);
    strcpy((char*) output, dummy);
    dummy = removeChar((char*) output, '\r', outputLength, false);
    strcpy((char*) output, dummy);
    return *output;
}

int readScript(script* scriptPtr, char* input, int lineNum)
{
	int intData[6];
	char* strData;
	//printf("Splitting string \"%s\" into tokens:\n", input);
	intData[0] = strtol(strtok(input, "{,}"), NULL, 10);
	for(int i = 1; i < 6; i++)
    {
        intData[i] = strtol(strtok(NULL, "{,}"), NULL, 10);
    }
	strData = strtok(NULL, "}");
	//printf("{%d,%d,%d,%d,%d,%d,%s}\n", mapNum, x, y, w, h, (int) action, data);
	initScript(scriptPtr, (scriptBehavior) intData[0], intData[1], intData[2], intData[3], intData[4], intData[5], strData, lineNum);
	//printf("done.\n");
	return 0;
}

char** getListOfFiles(int maxStrings, int maxLength, const char* directory, int* strNum)
{
	DIR* dir;
	struct dirent* ent;
	dir = opendir(directory);
	if (dir == NULL)
	{
		perror("Can't open this directory.");
		exit(1);
	}
	char** strArray = calloc(maxStrings, sizeof(char*));
	for (int i = 0; i < maxStrings; ++i)
		strArray[i] = calloc(maxLength, sizeof(char));
	int i = 0;
	while ((ent = readdir (dir)) != NULL)
	{
		if (strlen(ent->d_name) > 2)
		{
			//printf("%s -> %d\n", ent->d_name, strArray[i]);
			sprintf(strArray[i++], "%s", ent->d_name);
		}
	}
	closedir(dir);
	if (maxStrings >= i)
		*strNum = i;
	else
		*strNum = maxStrings;
	//printf("Done\n\n");
	return strArray;
}

void drawTextBox(char* input, SDL_Color outlineColor, SDL_Rect textBoxRect, bool redraw)
{
    Uint8 oldR, oldG, oldB, oldA;
    SDL_GetRenderDrawColor(mainRenderer, &oldR, &oldG, &oldB, &oldA);
    //19 letters per line/5 lines at 48pt font
    SDL_SetRenderDrawColor(mainRenderer, outlineColor.r, outlineColor.g, outlineColor.b, 0xFF);
    SDL_RenderFillRect(mainRenderer, &(textBoxRect));
    SDL_SetRenderDrawColor(mainRenderer, 0xB5, 0xB6, 0xAD, 0xFF);
    SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = textBoxRect.x + TILE_SIZE / 8, .y = textBoxRect.y + TILE_SIZE / 8,
                                                  .w = textBoxRect.w - TILE_SIZE / 4, .h = textBoxRect.h - TILE_SIZE / 4}));
    drawText(input, textBoxRect.x + 2 * TILE_SIZE / 8, textBoxRect.y + TILE_SIZE / 4, textBoxRect.w -  3 * TILE_SIZE / 8, textBoxRect.h -  3 * TILE_SIZE / 8, (SDL_Color){0, 0, 0, 0xFF}, redraw);
    SDL_SetRenderDrawColor(mainRenderer, oldR, oldG, oldB, oldA);
}

node* BreadthFirst(const int startX, const int startY, const int endX, const int endY, int* lengthOfPath, const bool drawDebug)
{
    node* path = calloc(300, sizeof(node));
    node** queue = calloc(40, sizeof(node));
    if (!queue || (startX / TILE_SIZE == endX / TILE_SIZE && startY / TILE_SIZE == endY / TILE_SIZE))
    {
        *lengthOfPath = -1;
        return NULL;
    }
    node* curNode;
    node searchList[HEIGHT_IN_TILES][WIDTH_IN_TILES];
    int queueCount = 0;
    for(int y = 0; y < HEIGHT_IN_TILES; y++)
    {
        for(int x = 0; x < WIDTH_IN_TILES; x++)
            initNode(&(searchList[y][x]), x * TILE_SIZE, y * TILE_SIZE, NULL, false, 0);
    }
    curNode = &(searchList[endY / TILE_SIZE][endX / TILE_SIZE]);
    curNode->lastNode = (void*) 1;
    curNode->visited = true;
    bool quit = false;
    while(!quit)
    {
        curNode->visited = true;
        for(int i = 0; i < 4; i++)
        {
            int x = (curNode->x / TILE_SIZE) + (i == 0) - (i == 1);
            int y = (curNode->y / TILE_SIZE) + (i == 2) - (i == 3);
            if ((x >= 0 && y >= 0 && x < WIDTH_IN_TILES && y < HEIGHT_IN_TILES) && (eventmap[y][x] != 1 && !(eventmap[y][x] == 5 && doorFlags[0]) && !(eventmap[y][x] == 6 && doorFlags[1]) && !(eventmap[y][x] == 7 && doorFlags[2]) && !(eventmap[y][x] == 8 && doorFlags[3]) && eventmap[y][x] != 10 && eventmap[y][x] != 11 && eventmap[y][x] != 16) && searchList[y][x].visited == false)
            {
                queue[queueCount++] = &(searchList[y][x]);
                searchList[y][x].visited = true;
                searchList[y][x].lastNode = (void*) curNode;
                if ((x == startX / TILE_SIZE) && (y == startY / TILE_SIZE))
                {  //check if node is at startX, startY. Stop if is, continue if not
                    quit = true;
                    break;
                }
                if (drawDebug)
                {
                    SDL_RenderFillRect(mainRenderer, &((SDL_Rect) {.x = x * TILE_SIZE, .y = y * TILE_SIZE, .w = TILE_SIZE, .h = TILE_SIZE}));
                    SDL_RenderPresent(mainRenderer);
                    //printf("%p\n", searchList[y][x].lastNode);
                    waitForKey(true);
                }
            }
        }
        //enqueue valid adjacent nodes to selected node
        if (queueCount == 0)
        {
            *lengthOfPath = 0;
            return NULL;
        }
        //check if no items are enqueued i.e. no path, quits with a NULL path if this is true

        curNode = queue[0];
        //printf("t>>(%d, %d) ... %d\n", curNode->x / TILE_SIZE, curNode->y / TILE_SIZE, queueCount);
        //printf("n>>(%d, %d)\n", queue[1].x / TILE_SIZE, queue[1].y / TILE_SIZE);

        for(int i = 0; i < queueCount - 1; i++)
            queue[i] = queue[i + 1];

        queueCount--;
        //select a new adjacent node and delete the last enqueued item
    }
    free(queue);
    quit = false;
    int pathCount = 1;
    path[0] = searchList[startY / TILE_SIZE][startX / TILE_SIZE];
    path[1] = path[0];
    while(!quit)
    {
        if (path[pathCount - 1].lastNode == (void*) 1)
            quit = true;
        else
        {
            //printf("%d = pathCount\n", pathCount);
            path[pathCount] = *((node*) (path[pathCount - 1].lastNode));
            //printf("%p\n", (void*) path[pathCount].lastNode);
            pathCount++;
        }
    }
    //backtrack through the path found, starting at the start node and following lastNode to the end
    *lengthOfPath = pathCount;
    return path;
}

bool executeScriptAction(script* scriptData, player* player)
{
    bool exitGameLoop = false;
    if (!scriptData->disabled)
    {
        if (scriptData->action == script_trigger_dialogue || scriptData->action == script_trigger_dialogue_once || scriptData->action == script_force_dialogue)
        {
            drawTextBox(scriptData->data, (SDL_Color){0, 0, 0, 0xFF}, (SDL_Rect){.x = 0, .y = 9 * TILE_SIZE * (player->spr.y < 9 * TILE_SIZE), .w = SCREEN_WIDTH, .h = (HEIGHT_IN_TILES - 9) * TILE_SIZE}, true);
            waitForKey(true);
            if (scriptData->action == script_trigger_dialogue_once)
            {
                scriptData->disabled = true;
                player->disabledScripts[scriptData->lineNum] = true;  //disable permanently
            }
        }
        if (scriptData->action == script_trigger_boss)
        {
            Mix_FadeOutMusic(920);
            SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
            for(int i = 0; i < 120; i++)
            {
                SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderClear(mainRenderer);
                drawAMap(tilesTexture, tilemap, 0, 0, 20, 15, true, false, false);
                drawAMap(tilesTexture, eventmap, 0, 0, 20, 15, true, true, false);
                SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, (Uint8) (255 * (i / 120.0)));
                SDL_RenderFillRect(mainRenderer, NULL);
                SDL_RenderPresent(mainRenderer);
                SDL_Delay(7);
            }
            Mix_HaltMusic();
            Mix_PlayMusic(MUSIC((musicIndex = 5)), -1);
            char* temp = "", * data = calloc(99, sizeof(char));
            script theBoss;
            readScript(&theBoss, readLine((char*) scriptFilePath, strtol(scriptData->data, NULL, 10), &temp), strtol(scriptData->data, NULL, 10));
            int bossIndex = strtol(strtok(strcpy(data, theBoss.data), "[/]"), NULL, 10);
            SDL_Delay(400);
            for(int i = 0; i < 120; i++)
            {
                SDL_SetRenderDrawColor(mainRenderer, (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), 0xFF);
                SDL_RenderClear(mainRenderer);
                drawATile(tilesTexture, bossIndex, theBoss.x, theBoss.y, theBoss.w, theBoss.h, 0, SDL_FLIP_NONE);
                SDL_RenderPresent(mainRenderer);
                SDL_Delay(5);
            }
            //fade to white while displaying boss
            bossLoaded = true;
            free(data);
        }
        if (scriptData->action == script_switch_maps && scriptData->data[0] != '\0')  //why is this here?
        {
            char* firstChar = "\0";
            int tempMap = player->mapScreen, tempX = player->spr.x, tempY = player->spr.y;
            char* data = calloc(99, sizeof(char));
            firstChar = strtok(strncpy(data, scriptData->data, 99), "[/]");  //MUST use a seperate strcpy'd string of the original because C is never that simple
            if (firstChar[0] == 'l')
            {
                if(player->lastMap != -1)
                    player->mapScreen = player->lastMap;
            }
            else
                player->mapScreen = strtol(firstChar, NULL, 10);
                //printf("%d/", mapNum);
            firstChar = strtok(NULL, "[/]");
            if (firstChar[0] == 'l')
            {
                if(player->lastX != -1)
                    player->spr.x = player->lastX;
            }
            else
                player->spr.x = strtol(firstChar, NULL, 10);
                //printf("%d/", player->spr.x);
            firstChar = strtok(NULL, "[/]");
            if (firstChar[0] == 'l')
            {
                if(player->lastY != -1)
                    player->spr.y = player->lastY;
            }
            else
                player->spr.y = strtol(firstChar, NULL, 10);
            free(data);
            player->lastMap = tempMap;
            player->lastX = tempX;
            player->lastY = tempY;
            exitGameLoop = true;
        }
        if (scriptData->action == script_use_gateway)
        {
            int tempMap = player->mapScreen, tempX = player->spr.x, tempY = player->spr.y;
            GATEWAY_CHANNEL = Mix_PlayChannel(-1, GATEWAYSTART_SOUND, 0);
            for(int i = 0; i < 120; i++)
            {
                SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderClear(mainRenderer);
                drawAMap(tilesTexture, tilemap, 0, 0, 20, 15, true, false, false);
                drawOverTilemap(tilesTexture, eventmap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, 0, 0, doorFlags, false, false);
                for(int i = 0; i < MAX_ENEMIES; i++)
                    drawASprite(tilesTexture, enemies[i].spr);
                //drawAMap(tilesTexture, eventmap, 0, 0, 20, 15, true, true, false);
                SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, (Uint8) (255 * (i / 120.0)));
                SDL_RenderFillRect(mainRenderer, NULL);
                SDL_RenderPresent(mainRenderer);
                SDL_Delay(9);
            }
            SDL_Delay(90);
            Mix_HaltChannel(GATEWAY_CHANNEL);
            char* data = calloc(99, sizeof(char));
            //printf("%s\n", data);
            player->mapScreen = strtol(strtok(strncpy(data, scriptData->data, 99), "[/]"), NULL, 10);  //MUST use a seperate strcpy'd string of the original because C is never that simple
            //printf("%d/", mapNum);
            player->spr.x = strtol(strtok(NULL, "[/]"), NULL, 10);
            //printf("%d/", player->spr.x);
            player->spr.y = strtol(strtok(NULL, "[/]"), NULL, 10);
            //printf("%d\n", player->spr.y);
            //switch maps
            //loadMapFile(player->extraData, tilemap, eventmap, player->mapScreen, 15, 20);
            GATEWAY_CHANNEL = Mix_PlayChannel(-1, GATEWAYEND_SOUND, 0);
            for(int i = 0; i < 120; i++)
            {
                SDL_SetRenderDrawColor(mainRenderer, (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), 0xFF);
                SDL_RenderClear(mainRenderer);
                SDL_RenderPresent(mainRenderer);
                SDL_Delay(4);
            }
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);  //If you remove this, program loses ~12% of its FPS (-80 from 600 FPS)
            player->lastMap = tempMap;
            player->lastX = tempX;
            player->lastY = tempY;
            player->xVeloc = 0;
            player->yVeloc = 0;
            initSpark(&theseSparks[4], (SDL_Rect) {player->spr.x, player->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_GATEWAY, 5, 12, 12, (frame * 1000 / (SDL_GetTicks() - startTime) / 2), (frame * 1000 / (SDL_GetTicks() - startTime) / 4));
            sparkFlag = true;
            theseSparkFlags[4] = true;
            free(data);
            exitGameLoop = true;
            Mix_HaltChannel(GATEWAY_CHANNEL);
        }
        if (scriptData->action == script_use_teleporter)
        {
            char* data = calloc(99, sizeof(char));
            //printf("%s\n", data);
            player->spr.x = strtol(strtok(strncpy(data, scriptData->data, 99), "[/]"), NULL, 10);  //MUST use a seperate strcpy'd string of the original because C is never that simple
            //printf("%d/", player->spr.x);
            player->spr.y = strtol(strtok(NULL, "[/]"), NULL, 10);
            //printf("%d\n", player->spr.y);
            Mix_PlayChannel(-1, TELEPORT_SOUND, 0);
            //play animation at old & new coords?
            free(data);
            initSpark(&theseSparks[5], (SDL_Rect) {player->spr.x, player->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_COLOR_BLUE, 4, 6, 6, (frame * 1000 / (SDL_GetTicks() - startTime)) / 4, (frame * 1000 / (SDL_GetTicks() - startTime) / 8));
            sparkFlag = true;
            theseSparkFlags[5] = true;
        }
        if (scriptData->action == script_toggle_door)
        {  //-2 = flip state, -1 = unchanged, 0 = open, 1 = closed
            char* data = calloc(99, sizeof(char));
            bool oldDoorFlags[4] = {doorFlags[0], doorFlags[1], doorFlags[2], doorFlags[3]};
            bool newDoorFlags[4] = {-1, -1, -1, -1};
            newDoorFlags[0] = strtol(strtok(strncpy(data, scriptData->data, 99), "[/]"), NULL, 10);
            for(int i = 0; i < 4; i++)
            {
                if (i > 0)
                    newDoorFlags[i] = strtol(strtok(NULL, "[/]"), NULL, 10);
                if (newDoorFlags[i] > -1)  //set
                    doorFlags[i] = newDoorFlags[i];
                if (newDoorFlags[i] == -2)  //flip
                    doorFlags[i] = !doorFlags[i];

            }
            if (oldDoorFlags[0] != doorFlags[0] || oldDoorFlags[1] != doorFlags[1] || oldDoorFlags[2] != doorFlags[2] || oldDoorFlags[3] != doorFlags[3])
                Mix_PlayChannel(-1, DOOROPEN_SOUND, 0);
            free(data);
        }

        if (scriptData->action == script_animation)
        {
            player->movementLocked = true;
            char* data = calloc(strlen(scriptData->data), sizeof(char));
            strcpy(data, scriptData->data);
            static int moveFrame = 1;
            static int startX = 0;
            static int startY = 0;
            static char* animationData = "";
            animationData = calloc(256 ,sizeof(char));
            strncpy(animationData, strtok(data, "[]"), 256);
            //printf("animation - %s\n", animationData);
            int animationDataArr[6];
            animationDataArr[0] = strtol(strtok(animationData, "/"), NULL, 10);
            for(int i = 1; i < 6; i++)
            {
                animationDataArr[i] = strtol(strtok(NULL, "/"), NULL, 10);
            }
            animationSpr.w = animationDataArr[2];
            animationSpr.h = animationDataArr[3];
            animationSpr.tileIndex = animationDataArr[4];
            //printf("[%d,%d,%d,%d,%d,%d]\n", animationDataArr[0], animationDataArr[1], animationDataArr[2], animationDataArr[3], animationDataArr[4], animationDataArr[5]);
            int totalFrames = 0, x = 0, y = 0, frames = 0;
            char* dataCopy = calloc(strlen(scriptData->data), sizeof(char));
            strcpy(dataCopy, scriptData->data);
            bool found = false;
            char* dataCopy2 = calloc(strlen(scriptData->data), sizeof(char));
            strcpy(dataCopy2, scriptData->data);
            strtok(dataCopy2, "(|)");
            while(!found)
            {
                char* xStr = strtok(NULL, "(|)");
                if (xStr[0] == '<')  //no more data
                {
                    moveFrame = 0;  //repeat; has to be 1 to reset to the way it is init'ed
                    scriptData->action = script_none;  //disable further execution in this launch
                    strtok(dataCopy, "(|)");  //reset read location & does the fix mentioned above
                }
                else
                {
                    x = strtol(xStr, NULL, 10);
                    y = strtol(strtok(NULL, "(|)"), NULL, 10);
                    frames = strtol(strtok(NULL, "(|)"), NULL, 10);
                    totalFrames += frames;
                    //printf("tf - %d, f - %d, mf - %d\n", totalFrames, frames, moveFrame);
                    if (moveFrame <= totalFrames)
                        found = true;
                }
            }
            if (moveFrame == 1)
            {
                animationSpr.x = animationDataArr[0];
                animationSpr.y = animationDataArr[1];
                startX = animationDataArr[0];
                startY = animationDataArr[1];
                moveFrame++;
            }
            else if (moveFrame > 1)
            {
                animationSpr.x += (x - startX) / frames;
                animationSpr.y += (y - startY) / frames;
                moveFrame++;
                if (moveFrame == totalFrames)
                {
                    animationSpr.x = x;
                    animationSpr.y = y;
                    startX = x;
                    startY = y;
                }
            }

            if (moveFrame == 0)
            {
                if (!animationDataArr[5])  //if not stay onscreen
                    animationSpr.tileIndex = tileIDArray[5];
                player->movementLocked = false;
                script textBox;
                char* textStuff = calloc(88, sizeof(char));
                char* dataPtr = scriptData->data;
                strtok(scriptData->data, "<>");  //gets rid of extra data
                strncpy(textStuff, strtok(NULL, "<>"), 88);
                strcpy(scriptData->data, dataPtr);
                //printf("%s\n", textStuff);
                if (strcmp(textStuff, "0")) //if the two have differences aka aren't equal
                    initScript(&textBox, script_force_dialogue, scriptData->mapNum, scriptData->x, scriptData->y, scriptData->w, scriptData->h, textStuff, -1);
                executeScriptAction(&textBox, player);
                moveFrame = 1;
                scriptData->disabled = true;
                player->disabledScripts[scriptData->lineNum] = true;
            }
        }

        if (scriptData->action == script_boss_actions)
        {
            static int moveFrame = 1;  //starts on frame 1 to iterate for the desired # of frames
            static int startX = 0;
            static int startY = 0;
            if (startX == 0)
            {
                startX = scriptData->x;
                startY =  scriptData->y;
            }
            int x = 0, y = 0, frames = 0, totalFrames = 0;
            char* data = calloc(99, sizeof(char));
            char* dataCopy = calloc(99, sizeof(char));  //strtok messes with the data, so this is necessary to use a clean copy when end of string is reached (quick fix)
            strncpy(data, scriptData->data, 99);
            strncpy(dataCopy, data, 99);
            if (data[0] == 'r')
                moveFrame = 0;
            else
            {
                bool found = false;
                strtok(data, "(|)");  //this fixes an issue (takes out the first chunk which is just all the [/] junk
                while(!found)
                {
                    char* xStr = strtok(NULL, "(|)");
                    if (!xStr || xStr[0] == ' ')  //no more data
                    {
                        moveFrame = 1;  //repeat; has to be 1 to reset to the way it is init'ed
                        strtok(dataCopy, "(|)");  //reset read location & does the fix mentioned above
                    }
                    else
                    {
                        x = strtol(xStr, NULL, 10);
                        y = strtol(strtok(NULL, "(|)"), NULL, 10);
                        frames = strtol(strtok(NULL, "(|)"), NULL, 10);
                        totalFrames += frames;
                        if (moveFrame <= totalFrames)
                            found = true;
                    }
                }
                //printf("%d, %d for %d f. On frame %d. s(%d, %d) d(%d, %d)\n", (x - startX), (y - startY), frames, moveFrame, startX, startY, x, y);
                scriptData->x += (x - startX) / frames;
                scriptData->y += (y - startY) / frames;

                if (moveFrame == totalFrames)
                {
                    scriptData->x = x;
                    scriptData->y = y;
                    startX = x;
                    startY = y;
                }

                if (scriptData->x < 0)
                    scriptData->x = 0;
                if (scriptData->x > SCREEN_WIDTH - scriptData->w)
                    scriptData->x = SCREEN_WIDTH - scriptData->w;

                if (scriptData->y < 0)
                    scriptData->y = 0;
                if (scriptData->y > SCREEN_HEIGHT - scriptData->h)
                    scriptData->y = SCREEN_HEIGHT - scriptData->h;
                //format: (x|y|frames|x2|y2|frames2...)
                //fill with code to interpret actions and change coords via the script coord system
                moveFrame++;
            }
            free(data);
        }
        if (scriptData->action == script_gain_money)
        {
            scriptData->disabled = true;
            player->disabledScripts[scriptData->lineNum] = true;  //disabled as to not be abusable
            player->money += strtol(scriptData->data, NULL, 10);
            if (player->money > 9999)
                player->money = 9999;
                Mix_PlayChannel(-1, CASH_SOUND, 0);
            initSpark(&theseSparks[3], (SDL_Rect) {player->spr.x, player->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_COLOR_ORANGE, 4, 6, 6, (frame * 1000 / (SDL_GetTicks() - startTime) / 4), (frame * 1000 / (SDL_GetTicks() - startTime) / 8));
            sparkFlag = true;
            theseSparkFlags[3] = true;
            //play animation and sound
        }
        if (scriptData->action == script_player_hurt && player->invincCounter <= 0)
        {
            int dmg = strtol(scriptData->data, NULL, 10);
            player->HP -= dmg;
            if (player->HP < 0)
                player->HP = 0;
            if (player->HP > player->maxHP)
                player->HP = player->maxHP;
                player->invincCounter = 15;  //30 frames of invincibility at 60fps, or approx. 1/2 second
            if (dmg > 0)
            {
                Mix_PlayChannel(-1, PLAYERHURT_SOUND, 0);
                //printf("%d / (%d - %d) == %d\n", frame, SDL_GetTicks(), startTime, frame * 1000 / (SDL_GetTicks() - startTime));
                initSpark(&theseSparks[1], (SDL_Rect) {player->spr.x, player->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_COLOR_RED, 4, 6, 6, (frame * 1000 / (SDL_GetTicks() - startTime)) / 4, (frame * 1000 / (SDL_GetTicks() - startTime)) / 8);
                sparkFlag = true;
                theseSparkFlags[1] = true;
            }
            else if (player->HP < player->maxHP)
            {
                Mix_PlayChannel(-1, HEAL_SOUND, 0); //play heal sound
                initSpark(&theseSparks[1], (SDL_Rect) {player->spr.x, player->spr.y, TILE_SIZE, TILE_SIZE}, SPARK_COLOR_GREEN, 4, 6, 6, (frame * 1000 / (SDL_GetTicks() - startTime)) / 4, (frame * 1000 / (SDL_GetTicks() - startTime)) / 8);
                sparkFlag = true;
                theseSparkFlags[1] = true;
            }
            //play animation (?) and sound
        }
    }
    scriptData->active = false;
    return exitGameLoop;  //returns whether or not it wants to exit the game loop
}

void SDLCALL playMainMusic()
{
	Mix_PlayMusic(MUSIC((musicIndex = 1)), -1);
}

void SDLCALL playOverworldMusic()
{
    if (musicIndex < 2 || musicIndex > 4)
        Mix_PlayMusic(MUSIC((musicIndex = 2 + rand() % 3)), -1);
}
