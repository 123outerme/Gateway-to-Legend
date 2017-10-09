#include "SDLSeekers.h"

#define TILE_ID_CURSOR 17

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

void initConfig(char* filePath)
{
    SC_UP = SDL_SCANCODE_W;
    SC_DOWN = SDL_SCANCODE_S;
    SC_LEFT = SDL_SCANCODE_A;
    SC_RIGHT = SDL_SCANCODE_D;
    SC_INTERACT = SDL_SCANCODE_SPACE;
    SC_MENU = SDL_SCANCODE_ESCAPE;
    saveConfig(filePath);
}

void loadPlayerData(player* player, char* filePath, bool forceNew)
{
    if (!checkFile(filePath, SAVE_FILE_LINES) || forceNew)
	{
	    //initPlayer(player, 4 * TILE_SIZE, 6 * TILE_SIZE, TILE_SIZE, TILE_ID_PLAYER);
	}
	else
	{
	    char* buffer;
        strcpy(player->name, readLine(filePath, 0, &buffer));
        strcpy(player->name, removeChar(player->name, '\n', PLAYER_NAME_LIMIT + 1, false));
        player->spr.x = strtol(readLine(filePath, 1, &buffer), NULL, 10);
        player->spr.y = strtol(readLine(filePath, 2, &buffer), NULL, 10);
        player->spr.w = TILE_SIZE;
        player->spr.h = player->spr.w;
        player->spr.tileIndex = /*TILE_ID_PLAYER*/0;
        player->level = strtol(readLine(filePath, 3, &buffer), NULL, 10);
        player->experience = strtol(readLine(filePath, 4, &buffer), NULL, 10);
        player->money = strtol(readLine(filePath, 5, &buffer), NULL, 10);
        player->HP = strtol(readLine(filePath, 6, &buffer), NULL, 10);
        player->maxHP = strtol(readLine(filePath, 7, &buffer), NULL, 10);
        player->spr.clipRect = &((SDL_Rect){.x = (player->spr.tileIndex / 8) * player->spr.w, .y = (player->spr.tileIndex % 8) * player->spr.w, .w = player->spr.w, .h = player->spr.w});
        player->worldNum = strtol(readLine(filePath, 15, &buffer), NULL, 10);
        player->mapScreen = strtol(readLine(filePath, 16, &buffer), NULL, 10);
        player->lastScreen = strtol(readLine(filePath, 17, &buffer), NULL, 10);
        player->overworldX = strtol(readLine(filePath, 18, &buffer), NULL, 10);
        player->overworldY = strtol(readLine(filePath, 19, &buffer), NULL, 10);
        player->flip = SDL_FLIP_NONE;
        player->movementLocked = false;
	}
}

void loadConfig(char* filePath)
{
    char* buffer = "";
    for(int i = 0; i < SIZE_OF_SCANCODE_ARRAY; i++)
    {
        readLine(filePath, i, &buffer);
        CUSTOM_SCANCODES[i] = strtol(buffer, NULL, 10);
    }
}

void loadMapFile(char* filePath, int* tilemapData[], int* eventmapData[], const int lineNum, const int y, const int x)
{
    int numsC = 0, numsR = 0,  i, num;
    int sameArray[y][x], eventArray[y][x];
    bool writeToTilemap = false;
    char thisLine[1200], substring[2];
    strcpy(thisLine, readLine(filePath, lineNum, thisLine));
    //printf("%s\n", thisLine);
    for(i = 0; i < 1200; i += 2)
    {
        sprintf(substring, "%.2s", thisLine + i);
        //*(array + numsR++ + numsC * x)
        num = (int)strtol(substring, NULL, 16);
        if (writeToTilemap)
            sameArray[numsC][numsR++] = num;
        else
            eventArray[numsC][numsR] = num;
        //printf(writeToTilemap ? "i = %d @ nums[%d][%d] = (%s)\n" : "i = %d @ eventArray[%d][%d] = (%s)\n", i, numsC, numsR - writeToTilemap, substring);
        writeToTilemap = !writeToTilemap;
        if (numsR > x - 1)
        {
            numsC++;
            numsR = 0;
        }
        //printf("%d\n", num);
    }
    for(int dy = 0; dy < y; dy++)
    {
        for(int dx = 0; dx < x; dx++)
        {
            *(tilemapData + dx + dy * x) = sameArray[dy][dx];
            *(eventmapData + dx + dy * x) = eventArray[dy][dx];
        }
    }
}

int aMenu(char* title, char* opt1, char* opt2, char* opt3, char* opt4, char* opt5, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain)
{
    if (curSelect < 1)
        curSelect = 1;
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, (curSelect + 4) * TILE_SIZE, TILE_SIZE, TILE_ID_CURSOR, (entityType) type_na);
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
        drawText(title, 2 * TILE_SIZE + 3 * TILE_SIZE / 8, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, titleColorUnder, false);
        //foreground text
        drawText(title, 2 * TILE_SIZE + TILE_SIZE / 4, 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, titleColorOver, false);

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
        drawTile(cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
    return selection;
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
    //alternatively, we could iterate through all of CUSTOM_SCANCODES[].
}

char* uniqueReadLine(char* output[], int outputLength, const char* filePath, int lineNum)
{
    char* dummy = "";
    readLine(filePath, lineNum, &dummy);
    strcpy(output, dummy);
    dummy = removeChar(output, '\n', outputLength, false);
    strcpy(output, dummy);
    return *output;
}

char** getListOfFiles(const size_t maxStrings, const size_t maxLength, const char* directory, int* strNum)
{
	DIR* dir;
	struct dirent* ent;
	dir = opendir(directory);
	if (dir == NULL)
	{
		perror("Can't open this directory.");
		exit(1);
	}
	char** strArray = malloc(maxStrings * sizeof(char*));
	for (int i =0 ; i < maxStrings; ++i)
		strArray[i] = malloc(maxLength * sizeof(char));
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
