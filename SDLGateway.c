#include "SDLGateway.h"

#define TILE_ID_CURSOR 17
#define TILE_ID_TILDA 125
#define TILE_ID_CUBED 124

int initSounds()
{
    /*MUSIC(0) = Mix_LoadWAV();
    if (!MUSIC(0))
        return -5;*/

    /*MUSIC(1) = Mix_LoadWAV();
    if (!MUSIC(1))
        return -5;*/

    UNSHEATH_SOUND = Mix_LoadWAV(UNSHEATH_FILE);
    if (!UNSHEATH_SOUND)
        return -5;

    OPTION_SOUND = Mix_LoadWAV(OPTION_FILE);
    if (!OPTION_SOUND)
        return -5;

    PING_SOUND = Mix_LoadWAV(PING_FILE);
    if (!PING_SOUND)
        return -5;
    {
        char* buffer = "";

        for(int i = 1; i < 4; i++)
        {
            char filepath[15] = STEP_FILE;
            strcat(filepath, intToString(i, buffer));
            STEP_SOUND(i) = Mix_LoadWAV(strcat(filepath, ".ogg\0"));
            if (!STEP_SOUND(i))
                return -5;
        }
    }
    SWING_SOUND = Mix_LoadWAV(SWING_FILE);
    if (!SWING_SOUND)
        return -5;

    GATEWAYSTART_SOUND = Mix_LoadWAV(GATEWAYSTART_FILE);
    if (!GATEWAYSTART_SOUND)
        return -5;

    GATEWAYEND_SOUND = Mix_LoadWAV(GATEWAYEND_FILE);
    if (!GATEWAYEND_SOUND)
        return -5;

    DOOROPEN_SOUND = Mix_LoadWAV(DOOROPEN_FILE);
    if (!DOOROPEN_SOUND)
        return -5;

    CASH_SOUND = Mix_LoadWAV(CASH_FILE);
    if (!CASH_SOUND)
        return -5;

    PLAYERHURT_SOUND = Mix_LoadWAV(PLAYERHURT_FILE);
    if (!PLAYERHURT_SOUND)
        return -5;

    ENEMYHURT_SOUND = Mix_LoadWAV(ENEMYHURT_FILE);
    if (!ENEMYHURT_SOUND)
        return -5;

    return 0;
}

void initPlayer(player* player, int x, int y, int w, int h, int mapScreen, int angle, SDL_RendererFlip flip, int tileIndex)
{
    initSprite(&(player->spr), x, y, w, h, tileIndex, angle, flip, type_player);
    strcpy(player->name, "");
    player->level = 1;
    player->experience = 0;
    player->money = 0;
    player->HP = 12;
    player->maxHP = 12;
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
    //name, x, y, w, level, HP, maxHP, attack, speed, statPts, move1 - move4, steps, worldNum, mapScreen, lastScreen, overworldX, overworldY
}

void createLocalPlayer(player* playerSprite, char* filePath, int x, int y, int w, int h, int mapScreen, int angle, SDL_RendererFlip flip, int tileIndex)
{
    initPlayer(playerSprite, x, y, w, h, mapScreen, angle, flip, tileIndex);
    playerSprite->HP = playerSprite->maxHP;
    saveLocalPlayer(*playerSprite, filePath);
}

void createGlobalPlayer(player* playerSprite, char* filePath)
{
    strcpy(playerSprite->name, "Player");
    //inputName(playerSprite);  //custom text input routine to get player->name
	saveGlobalPlayer(*playerSprite, filePath);
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
    SC_ATTACK = SDL_SCANCODE_LSHIFT;
    FPS = 60;
    targetTime = calcWaitTime(FPS);
    soundVolume = MIX_MAX_VOLUME;
    musicVolume = MIX_MAX_VOLUME;
    saveConfig(filePath);
}

void initScript(script* scriptPtr, scriptBehavior action, int mapNum, int x, int y, int w, int h, char* data)
{
	scriptPtr->action = action;
	scriptPtr->mapNum = mapNum;
	scriptPtr->x = x;
	scriptPtr->y = y;
	scriptPtr->w = w;
	scriptPtr->h = h;
	scriptPtr->data = data;
	scriptPtr->active = true;
}

void initNode(node* nodePtr, int x, int y, node* lastNode, bool visited, int distance)
{
    nodePtr->x = x;
    nodePtr->y = y;
    nodePtr->lastNode = lastNode;
    nodePtr->visited = visited;
    nodePtr->distance = distance;
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
    //loads: map, x, y, current HP
}

void loadGlobalPlayer(player* playerSprite, char* filePath)
{
    char* buffer = "";
    strcpy(playerSprite->name, readLine(filePath, 0, &buffer));
    playerSprite->name[strnlen(playerSprite->name, 10) - 1] = 0;  //removes \n
    playerSprite->maxHP = strtol(readLine(filePath, 1, &buffer), NULL, 10);
    playerSprite->level = strtol(readLine(filePath, 2, &buffer), NULL, 10);
    playerSprite->experience = strtol(readLine(filePath, 3, &buffer), NULL, 10);
    playerSprite->money = strtol(readLine(filePath, 4, &buffer), NULL, 10);
    playerSprite->movementLocked = false;
    playerSprite->xVeloc = 0;
    playerSprite->yVeloc = 0;
    playerSprite->lastDirection = 8;
    playerSprite->invincCounter = 0;
    playerSprite->animationCounter = 0;
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

int aMenu(SDL_Texture* texture, int cursorID, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain)
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
        drawText(title, 1 * TILE_SIZE + (5 - 2 * !isMain) * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, titleColorUnder, false);
        //foreground text
        drawText(title, 1 * TILE_SIZE + TILE_SIZE / (2 + 2 * !isMain) , 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, titleColorOver, false);

        for(int i = 0; ((options <= MAX_ITEMS) ? i < options : i < MAX_ITEMS); i++)
	        drawText(optionsArray[i], 2 * TILE_SIZE + TILE_SIZE / 4, (5 + i) * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - (5 + i)) * TILE_SIZE, textColor, false);
        if (isMain)
        {
            drawTile(TILE_ID_TILDA, 2, 0, TILE_SIZE, 0, SDL_FLIP_NONE);
            drawTile(TILE_ID_CUBED, TILE_SIZE, 0, TILE_SIZE, 0, SDL_FLIP_NONE);
            drawTile(TILE_ID_TILDA, 2 * TILE_SIZE - 2, 0, TILE_SIZE, 0, SDL_FLIP_NONE);
            drawText(VERSION_NUMBER, 2.25 * TILE_SIZE, 11 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 11) * TILE_SIZE, (SDL_Color){AMENU_MAIN_TEXTCOLOR, 0xFF}, false);
        }

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
            else if(e.type == SDL_KEYDOWN)
            {
                const Uint8* keyStates = SDL_GetKeyboardState(NULL);
                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_UP) && cursor.y > 5 * TILE_SIZE)
                {
                    cursor.y -= TILE_SIZE;
                    Mix_PlayChannel(-1, PING_SOUND, 0);
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_DOWN) && cursor.y < (options + 4) * TILE_SIZE)
                {
                    cursor.y += TILE_SIZE;
                    Mix_PlayChannel(-1, PING_SOUND, 0);
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_INTERACT))
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
                    SC_ATTACK = SDL_SCANCODE_LSHIFT;
                    SC_INTERACT = SDL_SCANCODE_SPACE;
                    SC_MENU = SDL_SCANCODE_ESCAPE;
                    saveConfig(CONFIG_FILEPATH);
                    Mix_PlayChannel(-1, PLAYERHURT_SOUND, 0);
                    settingsReset = true;
                }
            }
        }
        drawATile(texture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
    return selection;
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
            {
                keycode = e.key.keysym.sym;
            }
    }
    return keycode;
}

void saveConfig(char* filePath)
{
    char* buffer = "";
    createFile(filePath);
    appendLine(filePath, intToString(SC_UP, buffer));
    appendLine(filePath, intToString(SC_DOWN, buffer));
    appendLine(filePath, intToString(SC_LEFT, buffer));
    appendLine(filePath, intToString(SC_RIGHT, buffer));
    appendLine(filePath, intToString(SC_INTERACT, buffer));
    appendLine(filePath, intToString(SC_MENU, buffer));
    appendLine(filePath, intToString(SC_ATTACK, buffer));
    char newBuffer[8];
    strcpy(newBuffer, "FPS=");
    appendLine(filePath, strcat(newBuffer, intToString(FPS, buffer)));
    appendLine(filePath, intToString(soundVolume, buffer));
    appendLine(filePath, intToString(musicVolume, buffer));
    //alternatively, we could iterate through all of CUSTOM_SCANCODES[].
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
    //saves: map, x, y, current HP
}

void saveGlobalPlayer(const player playerSprite, char* filePath)
{
    char* buffer = "";
    createFile(filePath);
    appendLine(filePath, (char*) playerSprite.name);
    appendLine(filePath, intToString(playerSprite.maxHP, buffer));
    appendLine(filePath, intToString(playerSprite.level, buffer));
    appendLine(filePath, intToString(playerSprite.experience, buffer));
    appendLine(filePath, intToString(playerSprite.money, buffer));
    //saves: name, max HP, level, exp, money
}

void getNewKey(char* titleText, SDL_Color bgColor, SDL_Color textColor, int selection)
{
    SDL_RenderClear(mainRenderer);
    SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
    SDL_RenderFillRect(mainRenderer, NULL);
    drawText(titleText, .5 * TILE_SIZE, 5 * SCREEN_HEIGHT / 64, (WIDTH_IN_TILES - .5) * TILE_SIZE, (HEIGHT_IN_TILES - 4) * TILE_SIZE, textColor, true);
    SDL_Keycode kc = waitForKey();
    bool conflict = false;
    for(int i = 0; i < SIZE_OF_SCANCODE_ARRAY; i++)
    {
        if (CUSTOM_SCANCODES[i] == (int) SDL_GetScancodeFromKey(kc))
            conflict = true;
    }
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
    return *output;
}

int readScript(script* scriptPtr, char* input)
{
	int intData[6];
	char* strData;
	//printf("Splitting string \"%s\" into tokens:\n", input);
	intData[0] = strtol(strtok(input, "{,}"), NULL, 10);
	for(int i = 1; i < 6; i++)
    {
        intData[i] = strtol(strtok(NULL, "{,}"), NULL, 10);
    }
	strData = strtok(NULL, "{,}");
	//printf("{%d,%d,%d,%d,%d,%d,%s}\n", mapNum, x, y, w, h, (int) action, data);
	initScript(scriptPtr, (scriptBehavior) intData[0], intData[1], intData[2], intData[3], intData[4], intData[5], strData);
	//printf("done.\n");
	return 0;
}

char** getListOfFiles(int maxStrings, int maxLength, const char* directory, int* strNum)
{
	DIR* dir = opendir(directory);
	if (dir == NULL)
	{
		perror("Can't open this directory.");
		exit(1);
	}
	struct dirent* ent;
	char** strArray = malloc(maxStrings * sizeof(char*));
	for (int i = 0 ; i < maxStrings; ++i)
		strArray[i] = malloc(maxLength * sizeof(char));
	int i = 0;
	while ((ent = readdir(dir)) != NULL)
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
                                                  .w = textBoxRect.w -  2 * TILE_SIZE / 8, .h = textBoxRect.h - 2 * TILE_SIZE / 8}));
    drawText(input, textBoxRect.x + 2 * TILE_SIZE / 8, textBoxRect.y + 2 * TILE_SIZE / 8, textBoxRect.w -  3 * TILE_SIZE / 8, textBoxRect.h -  3 * TILE_SIZE / 8, (SDL_Color){0, 0, 0, 0xFF}, redraw);
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
            if ((x >= 0 && y >= 0 && x < WIDTH_IN_TILES && y < HEIGHT_IN_TILES) && eventmap[y][x] != 1 && searchList[y][x].visited == false)
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
                    waitForKey();
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
    if ((scriptData->action == script_trigger_dialogue || scriptData->action == script_trigger_dialogue_once) && scriptData->data[0] != '\0')
    {
        drawTextBox(scriptData->data, (SDL_Color){0, 0, 0, 0xFF}, (SDL_Rect){.y = 9 * TILE_SIZE, .w = SCREEN_WIDTH, .h = (HEIGHT_IN_TILES - 9) * TILE_SIZE}, true);  //change coords & color? Possibly use a drawTextBox funct instead?
        waitForKey();
        if (scriptData->action == script_trigger_dialogue_once)
            scriptData->data[0] = '\0';
    }
    if (scriptData->action == script_switch_maps)
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
        for(int i = 120; i > -1; i--)
        {
            SDL_SetRenderDrawColor(mainRenderer, (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), 0xFF);
            SDL_RenderClear(mainRenderer);
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
        //play animation at old & new coords?
        free(data);
    }
    if (scriptData->action == script_toggle_door)
    {  //-1 = unchanged, 0 = open, 1 = closed
        char* data = calloc(99, sizeof(char));
        bool newDoorFlags[3] = {-1, -1, -1};
        newDoorFlags[0] = strtol(strtok(strncpy(data, scriptData->data, 99), "[/]"), NULL, 10);
        for(int i = 0; i < 3; i++)
        {
            if (i > 0)
                newDoorFlags[i] = strtol(strtok(NULL, "[/]"), NULL, 10);
            if (newDoorFlags[i] > -1)
                doorFlags[i] = newDoorFlags[i];
        }
        Mix_PlayChannel(-1, DOOROPEN_SOUND, 0);
        free(data);
    }
    if (scriptData->action == script_boss_actions)
    {
        static int moveFrame = 1;  //starts on frame 1 to iterate for the desired # of frames
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
                if (!xStr)  //no more data
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
            //printf("%d, %d for %d f. On frame %d\n", x, y, frames, moveFrame);
            scriptData->x += x;
            scriptData->y += y;

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
    if (scriptData->action == script_gain_exp)
    {
        player->experience += strtol(scriptData->data, NULL, 10);
        if (player->experience > 9999)
            player->experience = 9999;
        if (CASH_SOUND != Mix_GetChunk(CASH_CHANNEL))
            CASH_CHANNEL = Mix_PlayChannel(-1, CASH_SOUND, 0);
        //play animation (?) and sound
    }
    if (scriptData->action == script_gain_money)
    {
        player->money += strtol(scriptData->data, NULL, 10);
        if (player->money > 9999)
            player->money = 9999;
        if (CASH_SOUND != Mix_GetChunk(CASH_CHANNEL))
            CASH_CHANNEL = Mix_PlayChannel(-1, CASH_SOUND, 0);
        //play animation (?) and sound
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
            Mix_PlayChannel(-1, PLAYERHURT_SOUND, 0);
        else
        {
            ; //play heal sound
        }
        //play animation (?) and sound
    }
    scriptData->active = false;
    return exitGameLoop;  //returns whether or not it wants to exit the game loop
}
