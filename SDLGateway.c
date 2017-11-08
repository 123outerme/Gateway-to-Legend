#include "SDLGateway.h"

#define TILE_ID_CURSOR 17

#define calcWaitTime(x) x == 0 ? 0 : 1000 / x

void initPlayer(player* player, int x, int y, int size, int mapScreen, int tileIndex)
{
    initSprite(&(player->spr), x, y, size, tileIndex, (entityType) type_player);
	player->flip = SDL_FLIP_NONE;
	player->movementLocked = false;
	player->extraData = "";
    //name, x, y, w, level, HP, maxHP, attack, speed, statPts, move1 - move4, steps, worldNum, mapScreen, lastScreen, overworldX, overworldY
}

void createGlobalPlayer(player* playerSprite, char* filePath)
{
    strcpy(playerSprite->name, "Player");
    //inputName(playerSprite);  //custom text input routine to get player->name
    initSprite(&(playerSprite->spr), 0, 0, TILE_SIZE, 0, (entityType) type_player);
	playerSprite->level = 1;
	playerSprite->experience = 0;
	playerSprite->money = 0;
	playerSprite->HP = 12;
	playerSprite->maxHP = 12;
	playerSprite->mapScreen = 0;
	playerSprite->flip = SDL_FLIP_NONE;
	playerSprite->movementLocked = false;
	playerSprite->extraData = "";
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
    saveConfig(filePath);
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
}

void loadLocalPlayer(player* playerSprite, char* filePath)
{
    char* buffer = "";
    playerSprite->mapScreen = strtol(readLine(filePath, 0, &buffer), NULL, 10);
    playerSprite->spr.x = strtol(readLine(filePath, 1, &buffer), NULL, 10);
    playerSprite->spr.y = strtol(readLine(filePath, 2, &buffer), NULL, 10);
    playerSprite->HP = strtol(readLine(filePath, 3, &buffer), NULL, 10);
    //loads: map, x, y, current HP
}

void loadGlobalPlayer(player* playerSprite, char* filePath)
{
    char* buffer = "";
    strcpy(playerSprite->name, removeChar(readLine(filePath, 0, &buffer), '\n', 8, true));
    playerSprite->maxHP = strtol(readLine(filePath, 1, &buffer), NULL, 10);
    playerSprite->level = strtol(readLine(filePath, 2, &buffer), NULL, 10);
    playerSprite->experience = strtol(readLine(filePath, 3, &buffer), NULL, 10);
    playerSprite->money = strtol(readLine(filePath, 4, &buffer), NULL, 10);
    //loads: name, max HP, level, exp, money
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

int aMenu(SDL_Texture* texture, int cursorID, char* title, char* opt1, char* opt2, char* opt3, char* opt4, char* opt5, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain)
{
    if (curSelect < 1)
        curSelect = 1;
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, (curSelect + 4) * TILE_SIZE, TILE_SIZE, cursorID, (entityType) type_na);
    SDL_Event e;
    bool quit = false;
    int selection = -1;
    //While application is running
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        if (border)
            SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
        else
            SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, NULL);
        SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        //background text (drawn first)
        drawText(title, 1 * TILE_SIZE + (5 - 2 * !isMain) * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, titleColorUnder, false);
        //foreground text
        drawText(title, 1 * TILE_SIZE + TILE_SIZE / (2 + 2 * !isMain) , 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, titleColorOver, false);

        drawText(opt1, 2 * TILE_SIZE + TILE_SIZE / 4, 5 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 5) * TILE_SIZE, textColor, false);
        drawText(opt2, 2 * TILE_SIZE + TILE_SIZE / 4, 6 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 6) * TILE_SIZE, textColor, false);
        drawText(opt3, 2 * TILE_SIZE + TILE_SIZE / 4, 7 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 7) * TILE_SIZE, textColor, false);
        drawText(opt4, 2 * TILE_SIZE + TILE_SIZE / 4, 8 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 8) * TILE_SIZE, textColor, false);
        drawText(opt5, 2 * TILE_SIZE + TILE_SIZE / 4, 9 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 9) * TILE_SIZE, textColor, false);

        /*if (isMain)
        {
            char version[12];
            strcpy(version, FULLVERSION_STRING);
            strcat(version, STATUS_SHORT);
            strcat(version, "\0");
            drawTile(TILE_ID_TILDA, 0, 0, TILE_SIZE, SDL_FLIP_NONE);
            drawTile(TILE_ID_CUBED, 1 * TILE_SIZE, 0, TILE_SIZE, SDL_FLIP_NONE);
            drawTile(TILE_ID_TILDA, 2 * TILE_SIZE, 0, TILE_SIZE, SDL_FLIP_NONE);
            drawText(version, 2 * TILE_SIZE + TILE_SIZE / 4, 11 * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - 11) * TILE_SIZE, (SDL_Color){24, 195, 247}, true);
        }*/

        //SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.w}));
        //Handle events on queue
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
                //const Uint8* keyStates = SDL_GetKeyboardState(NULL);
                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_UP))
                {
                    if (cursor.y > 5 * TILE_SIZE)
                        cursor.y -= TILE_SIZE;
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_DOWN))
                {
                    if (cursor.y < (options + 4) * TILE_SIZE)
                        cursor.y += TILE_SIZE;
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_INTERACT))
                {
                    selection = cursor.y / TILE_SIZE - 4;
                    quit = true;
                }
                /*if (isMain && (keyStates[SDL_SCANCODE_LCTRL] || keyStates[SDL_SCANCODE_RCTRL]) && keyStates[SDL_SCANCODE_R])
                {
                    SC_UP = SDL_SCANCODE_W;
                    SC_DOWN = SDL_SCANCODE_S;
                    SC_LEFT = SDL_SCANCODE_A;
                    SC_RIGHT = SDL_SCANCODE_D;
                    SC_INTERACT = SDL_SCANCODE_SPACE;
                    SC_MENU = SDL_SCANCODE_ESCAPE;
                    saveConfig(CONFIG_FILE_NAME);
                }*/
            }
        }
        drawATile(texture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
    return selection;
}

SDL_Keycode getKey()
{
    SDL_Event e;
    SDL_Keycode keycode = SDLK_ESCAPE;
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

char** getListOfFiles(const size_t maxStrings, const size_t maxLength, const char* directory, int* strNum)
{
	DIR* dir = opendir(directory);
	if (dir == NULL)
	{
		perror("Can't open this directory.");
		exit(1);
	}
	struct dirent* ent;
	char** strArray = malloc(maxStrings * sizeof(char*));
	for (int i =0 ; i < maxStrings; ++i)
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

void drawATilemap(SDL_Texture* texture, bool eventLayerFlag, int startX, int startY, int endX, int endY, bool updateScreen)
{
    for(int dy = startY; dy < endY; dy++)
        for(int dx = startX; dx < endX; dx++)
            drawATile(texture, eventLayerFlag ? eventmap[dy][dx] : tilemap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, SDL_FLIP_NONE);
    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}

void drawATile(SDL_Texture* texture, int id, int xCoord, int yCoord, int width, SDL_RendererFlip flip)
{
    SDL_RenderCopyEx(mainRenderer, texture, &((SDL_Rect) {.x = (id / 8) * width, .y = (id % 8) * width, .w = width, .h = width}), &((SDL_Rect) {.x = xCoord, .y = yCoord, .w = width, .h = width}), 0, &((SDL_Point) {.x = width / 2, .y = width / 2}), flip);
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
    drawText(input, textBoxRect.x + 2 * TILE_SIZE / 8, textBoxRect.y + 2 * TILE_SIZE / 8, textBoxRect.w -  3 * TILE_SIZE / 8, textBoxRect.h -  3 * TILE_SIZE / 8, (SDL_Color){0, 0, 0}, redraw);
    SDL_SetRenderDrawColor(mainRenderer, oldR, oldG, oldB, oldA);
}

bool executeScriptAction(script* scriptData, player* player)
{
    bool returnThis = false;
    if (scriptData->action == script_trigger_dialogue)
    {
        drawTextBox(scriptData->data, (SDL_Color){0, 0, 0}, (SDL_Rect){.y = 9 * TILE_SIZE, .w = SCREEN_WIDTH, .h = (HEIGHT_IN_TILES - 9) * TILE_SIZE}, true);  //change coords & color? Possibly use a drawTextBox funct instead?
        waitForKey();
    }
    if (scriptData->action == script_use_warp_gate)
    {
        for(int i = 120; i > -1; i--)
        {
            SDL_SetRenderDrawColor(mainRenderer, (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), (Uint8) (255 * (i / 120.0)), 0xFF);
            SDL_RenderClear(mainRenderer);
            SDL_RenderPresent(mainRenderer);
            SDL_Delay(9);
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
        SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0xFF);  //If you remove this, program loses ~12% of its FPS (-80 from 600 FPS)
        free(data);
        returnThis = true;
    }
    if (scriptData->action == script_gain_exp)
    {
        player->experience += strtol(scriptData->data, NULL, 10);
        //play animation?
    }
    scriptData->active = false;
    return returnThis;  //returns whether or not it wants to exit the loop
}
