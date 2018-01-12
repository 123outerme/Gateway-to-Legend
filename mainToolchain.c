#include "outermeSDL.h"  //uses outermeSDL v1.2 as of right now. Future versions of the header may not be compatible
#include "dirent.h"  //for directory searching

#define PIXELS_MOVED TILE_SIZE

typedef struct {
    sprite spr;  //?
    char name[8 + 1];  //9 bytes
    int level;  //
    int experience;  //
    int money;  //
    int HP;  //
    int maxHP;  //
    int worldNum;  //
    int mapScreen;  //8 bytes
    int lastScreen;  //8 bytes
    int overworldX;  //
    int overworldY;  //
    bool movementLocked;  // 1 byte
} player;

#define checkSKUp keyStates[26]
#define checkSKDown keyStates[22]
#define checkSKLeft keyStates[4]
#define checkSKRight keyStates[7]
#define checkSKInteract keyStates[44]
#define checkSKMenu keyStates[41]
#define SC_UP SDL_SCANCODE_W
#define SC_DOWN SDL_SCANCODE_S
#define SC_LEFT SDL_SCANCODE_A
#define SC_RIGHT SDL_SCANCODE_D
#define SC_INTERACT SDL_SCANCODE_SPACE
#define SC_MENU SDL_SCANCODE_ESCAPE
//SDL_SCANCODE_W
//SDL_SCANCODE_S
//SDL_SCANCODE_A
//SDL_SCANCODE_D
//SDL_SCANCODE_SPACE
//SDL_SCANCODE_ESCAPE

#define CACHE_NAME "GtLToolchainCache.cfg"

#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)
#define WINDOW_NAME "Gateway to Legend Map Creator"
#define MAIN_TILESET "tileset/mainTileset8x6.png"

//^map creator defines. v map-pack wizard defines

#define PICK_MESSAGES_ARRAY {"Pick the main character tile.", "Pick the cursor.", "Pick the HP icon.", "Pick the fully-transparent tile.", "Pick button 1.", "Pick button 2.", "Pick button 3.", "Pick door 1.", "Pick door 2.", "Pick door 3.", "Pick the teleporter.", "Pick the damaging hazard.", "Pick the warp gate.", "Pick the player sword.", "Pick enemy 1.", "Pick enemy 2.", "Pick enemy 3."}
#define MAX_SPRITE_MAPPINGS 17  //sprite defines and other map-pack data? I'm really not sure where this data comes from
#define MAX_MAP_PACK_DATA 6  //does not include sprite defines


typedef struct {
    SDL_Texture* mapPackTexture;
    char mainFilePath[MAX_PATH];
    char name[MAX_PATH];
    char mapFilePath[MAX_PATH];
    char tilesetFilePath[MAX_PATH];
    char saveFilePath[MAX_PATH];
    char scriptFilePath[MAX_PATH];
    int initX;
    int initY;
    int initMap;
    int tilesetMaps[MAX_SPRITE_MAPPINGS];
} mapPack;
int aMenu(SDL_Texture* texture, int cursorID, char* title, char* opt1, char* opt2, char* opt3, char* opt4, char* opt5, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain);
// ^ whatever

#define MAX_LIST_OF_MAPS 30
#define MAX_CHAR_IN_FILEPATH MAX_PATH
#define MAP_PACKS_SUBFOLDER "map-packs/"
#define MAX_MAPPACKS_PER_PAGE 11
int subMain(mapPack* workingPack);

void editFilePaths(mapPack* workingPack);
void editTileEquates(mapPack* workingPack);

void createMapPack(mapPack* newPack);
void loadMapPackData(mapPack* loadPack, char* location);
char** getListOfFiles(const size_t maxStrings, const size_t maxLength, const char* directory, int* strNum);
void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag);
//^ working with loading a map-pack to work on

int mainMapCreator();
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);
void mainMapCreatorLoop(player* playerSprite);
void viewMap(char* filePath, int thisLineNum, bool drawLineNum);
int chooseMap(char* mapFilePath);
SDL_Keycode getKey();
void drawEventmap(int startX, int startY, int endX, int endY, bool drawHiddenTiles, bool updateScreen);
void drawEventTile(int id, int xCoord, int yCoord, int width, SDL_RendererFlip flip);
void initPlayer(player* player, int x, int y, int size, int angle, SDL_RendererFlip flip, int tileIndex);
void writeTileData();
//^map creator functions.

//V map-pack wizard functions
int mainMapPackWizard();
void mainMapPackWizardLoop(sprite* playerSprite, int* numArray);
void strPrepend(char* input, const char* prepend);

//V take a look a these later. For now I'm leaving them as is
int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* eventTexture;

const int targetTime = 1000 / FRAMERATE;

int main(int argc, char* argv[])
{
    mapPack workingPack;
    strcpy(workingPack.mainFilePath, "/\0");
    initSDL("Gateway to Legend Map-Pack Tools", MAIN_TILESET, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
    bool quit = false;
    char* resumeStr = "\0";
    while(!quit)
    {
        readLine(CACHE_NAME, 0, &resumeStr);
        resumeStr = removeChar(resumeStr, '\n', MAX_PATH, false);
        if (checkFile(resumeStr, 0))
            resumeStr += 10;  //pointer arithmetic to get rid of the "map-packs/" part of the string (use 9 instead to include the /)
        else
            resumeStr = "(No Resume)\0";
        int code = aMenu(tilesetTexture, 17, "Gateway to Legend Toolchain", "New Map-Pack", "Load Map-Pack", resumeStr, "Quit", " ", 4, 1, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xA5, 0xA5, 0xA5, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);
        if (code == 1)
        {
            closeSDL();
            createMapPack(&workingPack);
            initSDL("Gateway to Legend Map-Pack Tools", MAIN_TILESET, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
        }

        if (code == 2)
        {
            char mainFilePath[MAX_PATH];
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
        }

        if (code == 3 && strcmp(resumeStr, "(No Resume)\0") != 0)
        {
            char mainFilePath[MAX_PATH];
            uniqueReadLine((char**) &mainFilePath, MAX_PATH, CACHE_NAME, 0);
            loadMapPackData(&workingPack, (char*) mainFilePath);
        }

        if (code == 4)
            quit = true;

        if (code < 4 && workingPack.mainFilePath[0] != '/')
        {
            closeSDL();
            subMain(&workingPack);
            initSDL("Gateway to Legend Map-Pack Tools", MAIN_TILESET, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
        }
    }
    closeSDL();
    return 0;
}

void createMapPack(mapPack* newPack)
{
    char* getString = calloc(sizeof(char), MAX_CHAR_IN_FILEPATH);
    char mapPackData[MAX_MAP_PACK_DATA][MAX_CHAR_IN_FILEPATH];
    int wizardState = 0;
    bool quit = false;
	while (!quit)
    {
        switch(wizardState)
        {
        case 0:
            printf("File name? map-packs/");
            break;
        case 1:
            printf("Title of map pack? ");
            break;
        case 2:
            printf("Path for maps file? maps/");
            break;
        case 3:
            printf("Path for tileset file? tileset/");
            break;
        case 4:
            printf("Path for savefile? saves/");
            break;
        case 5:
            printf("Path for scripts? scripts/");
            break;
        case 6:
            printf("Initial X spawn-coordinate? ");
            break;
        case 7:
            printf("Initial Y spawn-coordinate? ");
            break;
	case 8:
	    printf("Initial map number? ");
        }
        scanf(wizardState == 1 ? "%19[^\n]%*c" : "%259[^\n]%*c", getString);
        switch(wizardState)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            strncpy(mapPackData[wizardState], getString, 260);

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

    for(int i = 0; i < MAX_SPRITE_MAPPINGS; i++)
    {
        appendLine(newPack->mainFilePath, "0\0");
        newPack->tilesetMaps[i] = 0;
    }

    getString = freeThisMem((void*) getString);
    loadMapPackData(newPack, newPack->mainFilePath);

    createFile(CACHE_NAME);
    appendLine(CACHE_NAME, newPack->mainFilePath);
}

void loadMapPackData(mapPack* loadPack, char* location)
{
    char buffer[MAX_PATH];
    strcpy(loadPack->mainFilePath, location);
    uniqueReadLine((char**) &buffer, MAX_PATH, location, 0);
    strcpy(loadPack->name, buffer);
    uniqueReadLine((char**) &buffer, MAX_PATH, location, 1);
    strcpy(loadPack->mapFilePath, buffer);
    uniqueReadLine((char**) &buffer, MAX_PATH, location, 2);
    strcpy(loadPack->tilesetFilePath, buffer);
    uniqueReadLine((char**) &buffer, MAX_PATH, location, 3);
    strcpy(loadPack->saveFilePath, buffer);
    uniqueReadLine((char**) &buffer, MAX_PATH, location, 4);
    strcpy(loadPack->scriptFilePath, buffer);
    loadPack->initX = strtol(readLine(loadPack->mainFilePath, 5, (char**) &buffer), NULL, 10);
    loadPack->initY = strtol(readLine(loadPack->mainFilePath, 6, (char**) &buffer), NULL, 10);
    loadPack->initMap = strtol(readLine(loadPack->mainFilePath, 7, (char**) &buffer), NULL, 10);
    for(int i = 0; i < MAX_SPRITE_MAPPINGS; i++)
        loadPack->tilesetMaps[i] = strtol(readLine(loadPack->mainFilePath, i + 8, (char**) &buffer), NULL, 10);
}

void saveMapPack(mapPack* writePack)
{
    char mapPackData[MAX_MAP_PACK_DATA][MAX_PATH];
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

        drawTile(17, 10, (selectItem + 2) * TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
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

int subMain(mapPack* workingPack)
{
    initSDL("Gateway to Legend Map Tools", MAIN_TILESET, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
    bool quit = false;
    while(!quit)
    {
        int code = aMenu(tilesetTexture, 17, "Map-Pack Tools", "Map Creator", "Map-Pack Wizard", " ", " ", "Back", 5, 1, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xA5, 0xA5, 0xA5, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);
        closeSDL();
        if (code == 1)
            mainMapCreator(workingPack);
        if (code == 2)
            mainMapPackWizard(workingPack);
        if (code == 5)
            quit = true;
    }
    return 0;
}

int aMenu(SDL_Texture* texture, int cursorID, char* title, char* opt1, char* opt2, char* opt3, char* opt4, char* opt5, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain)
{
    if (curSelect < 1)
        curSelect = 1;
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, (curSelect + 4) * TILE_SIZE, TILE_SIZE, cursorID, 0, SDL_FLIP_NONE, (entityType) type_na);
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
            else if (e.type == SDL_KEYDOWN)
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
                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_MENU))
                {
                    selection = ANYWHERE_QUIT;
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
        drawATile(texture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
    return selection;
}

int mainMapCreator(mapPack* workingPack)
{
    for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
        for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
            eventmap[dy][dx] = 0;
    char* mainFilePath = calloc(MAX_PATH, sizeof(char));
    char mapFilePath[MAX_PATH];
    char tileFilePath[MAX_PATH];
    char loadCheck[2];
    printf("Load? (y/n) ");
	scanf("%s", loadCheck);
	if (loadCheck[0] == 'y')
    {
        printf("%s\n", workingPack->mainFilePath);
        strncpy(mainFilePath, workingPack->mainFilePath, MAX_PATH);
        if (!checkFile(mainFilePath, 0))
        {
            printf("Invalid main file.\n");
            return 1;
        }
        uniqueReadLine((char**) &mapFilePath, MAX_PATH, mainFilePath, 1);
        uniqueReadLine((char**) &tileFilePath, MAX_PATH, mainFilePath, 2);
    }
    else
    {
        strcpy(mainFilePath, "map-packs/a.txt");
        uniqueReadLine((char**) &mapFilePath, MAX_PATH, mainFilePath, 1);
        uniqueReadLine((char**) &tileFilePath, MAX_PATH, mainFilePath, 2);
        for(int dy = 0; dy < HEIGHT_IN_TILES; dy++)
        {
            for(int dx = 0; dx < WIDTH_IN_TILES; dx++)
            {
                tilemap[dy][dx] = 0;
                eventmap[dy][dx] = 0;
            }
        }
    }
    initSDL(WINDOW_NAME, tileFilePath, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
    loadIMG("tileset/eventTile48.png", &eventTexture);
    if (loadCheck[0] == 'y')
        loadMapFile(mapFilePath, tilemap, eventmap, chooseMap(mapFilePath), HEIGHT_IN_TILES, WIDTH_IN_TILES);
    player creator;
    initPlayer(&creator, 0, 0, TILE_SIZE, 0, SDL_FLIP_NONE, 0);
    SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    mainMapCreatorLoop(&creator);
    closeSDL();
    char saveCheck[2];
    printf("Save? (y/n) ");
	scanf("%s", saveCheck);
	if (saveCheck[0] == 'y')
        writeTileData();
    //waitForKey();
    SDL_DestroyTexture(eventTexture);
    //SDL_Delay(1000);
    return 0;
}
//C:/Stephen/C/CodeBlocks/Gateway-to-Legend/Map-Creator/map-packs/a.txt

void viewMap(char* filePath, int thisLineNum, bool drawLineNum)
{
    SDL_RenderClear(mainRenderer);
    int newTilemap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
    int newEventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
    char* buffer = "";
    loadMapFile(filePath, newTilemap, newEventmap, thisLineNum, HEIGHT_IN_TILES, WIDTH_IN_TILES);
    drawATilemap(tilesetTexture, newTilemap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, -1, false);
    drawATilemap(eventTexture, newEventmap, 0, 0, WIDTH_IN_TILES, HEIGHT_IN_TILES, 1, !drawLineNum);
    if (drawLineNum)
        drawText(intToString(thisLineNum, buffer), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {0xFF, 0xFF, 0xFF}, true);
}

int chooseMap(char* mapFilePath)
{
    bool quit = false;
    int mapNum = 0, maxMapNum = checkFile(mapFilePath, -1);
    SDL_Keycode keycode;
    while(!quit)
    {
        viewMap(mapFilePath, mapNum, true);
        keycode = getKey();
        mapNum += (keycode == SDLK_d && mapNum < maxMapNum) - (keycode == SDLK_a && mapNum > 0) + 10 * (keycode == SDLK_s && mapNum + 9 < maxMapNum) - 10 * (keycode == SDLK_w && mapNum > 9);
        if (keycode == SDLK_RETURN || keycode == SDLK_ESCAPE || keycode == SDLK_SPACE || keycode == -1)
            quit = true;
    }
    return mapNum;
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

void mainMapCreatorLoop(player* playerSprite)
{
    /*for(int i = 0; i < 4; i++)
        viewMap("maps/MainMaps.txt", i);*/
    bool quit = false, editingTiles = true;
    int frame = 0, sleepFor = 0, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = SDL_GetTicks(), lastTile = -1;
    SDL_Event e;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        drawTilemap(0, 0, 20, 15, false);
        if (!editingTiles)
        {
            SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0x00, 0x58);
            SDL_RenderFillRect(mainRenderer, NULL);
            SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            drawEventmap(0, 0, 20, 15, !editingTiles, false);
            drawATile(eventTexture, playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, TILE_SIZE, 0, playerSprite->spr.flip);
        }
        else
        {
            drawTile(playerSprite->spr.tileIndex, playerSprite->spr.x, playerSprite->spr.y, TILE_SIZE, 0, playerSprite->spr.flip);
            drawEventmap(0, 0, 20, 15, !editingTiles, false);
        }

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
        if (SDL_GetTicks() - lastKeypressTime >= 80 + 48 * (keyStates[SDL_SCANCODE_LSHIFT] || keyStates[SDL_SCANCODE_Q] || keyStates[SDL_SCANCODE_E]))
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
            if (keyStates[SDL_SCANCODE_E] && playerSprite->spr.tileIndex < 127)
                playerSprite->spr.tileIndex++;
            if (keyStates[SDL_SCANCODE_SPACE] && editingTiles)
                tilemap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
            if (keyStates[SDL_SCANCODE_SPACE] && !editingTiles)
                eventmap[playerSprite->spr.y / TILE_SIZE][playerSprite->spr.x / TILE_SIZE] = playerSprite->spr.tileIndex;
            if (keyStates[SDL_SCANCODE_LSHIFT])
            {
                editingTiles = !editingTiles;
                int temp = lastTile;
                lastTile = playerSprite->spr.tileIndex;
                if (!editingTiles && temp == -1)
                {
                    playerSprite->spr.tileIndex = 1;
                }
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
                keycode = e.key.keysym.sym;
    }
    return keycode;
}

void initPlayer(player* player, int x, int y, int size, int angle, SDL_RendererFlip flip, int tileIndex)
{
    //inputName(player);  //custom text input routine to get player->name
    initSprite(&(player->spr), x, y, size, tileIndex, angle, flip, (entityType) type_player);
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
	player->movementLocked = false;
    SDL_Delay(300);
    //name, x, y, w, level, HP, maxHP, attack, speed, statPts, move1 - move4, steps, worldNum, mapScreen, lastScreen, overworldX, overworldY
}

void drawEventmap(int startX, int startY, int endX, int endY, bool drawHiddenTiles, bool updateScreen)
{
    for(int dy = startY; dy < endY; dy++)
        for(int dx = startX; dx < endX; dx++)
            drawATile(eventTexture, eventmap[dy][dx] == 1 && !drawHiddenTiles ? 0 : eventmap[dy][dx], dx * TILE_SIZE, dy * TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
    if (updateScreen)
        SDL_RenderPresent(mainRenderer);
}

void writeTileData()
{
    char* outputFile = "output/map.txt";
    createFile(outputFile);
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
    appendLine("output/map.txt", output);
    printf("%s\n", output);
    printf("outputted to output/map.txt\n");
}
//end map creator code.

//start map-pack wizard code
#undef SCREEN_WIDTH
#undef SCREEN_HEIGHT
#define SCREEN_WIDTH TILE_SIZE * 16
#define SCREEN_HEIGHT TILE_SIZE * 9
#undef WIDTH_IN_TILES
#undef HEIGHT_IN_TILES
#define WIDTH_IN_TILES SCREEN_WIDTH / TILE_SIZE
#define HEIGHT_IN_TILES SCREEN_HEIGHT / TILE_SIZE

int mainMapPackWizard(mapPack* workingPack)
{
    initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, TILE_SIZE * 20, TILE_SIZE * 15, 48);
    bool quit = false;
    while (!quit)
    {
        int choice = aMenu(tilesetTexture, workingPack->tilesetMaps[1], workingPack->mainFilePath + 10, "Edit Filepaths", "Edit Init Spawn", "Edit Tile Equates", " ", "Back", 5, 0, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xA5, 0xA5, 0xA5, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);

        if (choice == 1)
            editFilePaths(workingPack);

	if (choice == 2)
            editInitSpawn(workingPack);

        if (choice == 3)
        {
            editTileEquates(workingPack);
            closeSDL();
            initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, TILE_SIZE * 20, TILE_SIZE * 15, 48);
        }

        if (choice == 4)
            ;

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
        int choice = aMenu(tilesetTexture, workingPack->tilesetMaps[1], workingPack->mainFilePath + 10, "Change Name", "Edit Map Path", "Edit Tileset Path", "Edit Save Path", "Edit Script Path", 5, 0, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, (SDL_Color) {0xA5, 0xA5, 0xA5, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, (SDL_Color) {0x00, 0x00, 0x00, 0xFF}, true, false);
        if (choice < 0)
        {
            quit = true;
        }
        else
        {
            closeSDL();
            char getString[MAX_PATH];
            getString[0] = '\0';
            switch(choice)
            {
            case 1:
                printf("Title of map pack? ");
                break;
            case 2:
                printf("Path for maps file? maps/");
                break;
            case 3:
                printf("Path for tileset file? tileset/");
                break;
            case 4:
                printf("Path for savefile? saves/");
                break;
            case 5:
                printf("Path for scripts? scripts/");
                break;
            case 6:
                printf("Initial X spawn-coordinate? ");
                break;
            case 7:
                printf("Initial Y spawn-coordinate? ");
                break;
            }
            scanf(choice == 1 ? "%19[^\n]%*c" : "%259[^\n]%*c", getString);
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
            case 6:
                sscanf(getString, "%d", &(workingPack->initX));
                break;
            case 7:
                sscanf(getString, "%d", &(workingPack->initY));
                break;
            }
            initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, TILE_SIZE * 20, TILE_SIZE * 15, 48);
        }
    }
    saveMapPack(workingPack);
}

void editInitSpawn(mapPack* workingPack)
{

}

void editTileEquates(mapPack* workingPack)
{
    closeSDL();
    initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 24);
    SDL_RenderClear(mainRenderer);
    int numbers[MAX_SPRITE_MAPPINGS];
    numbers[0] = -1;
    sprite chooser;
    initSprite(&chooser, 0, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_player);
    mainMapPackWizardLoop(&chooser, (int*) numbers);
    if (!(numbers[0] == -1))
    {
        for(int i = 0; i < MAX_SPRITE_MAPPINGS; i++)
        {
            workingPack->tilesetMaps[i] = numbers[i];
            //printf("%d\n", numbers[i]);
        }
        printf("Outputted to your file.\n");
        saveMapPack(workingPack);
    }
}

void mainMapPackWizardLoop(sprite* playerSprite, int* numArray)
{
    int numArrayTracker = 0, frame = 0, sleepFor = 0, lastFrame = SDL_GetTicks() - 1, lastKeypressTime = lastFrame + 1;
    char* text[] = PICK_MESSAGES_ARRAY;
    bool quit = false;
    SDL_Event e;
    while(numArrayTracker < MAX_SPRITE_MAPPINGS && !quit)
    {
        SDL_RenderClear(mainRenderer);
        SDL_RenderCopy(mainRenderer, tilesetTexture, NULL, &((SDL_Rect) {.x = 0, .y = TILE_SIZE, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT - TILE_SIZE}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0x1C, 0xC6, 0xFF);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->x, .y= playerSprite->y, .w = playerSprite->w, .h = playerSprite->h}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        drawText(text[numArrayTracker], 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0x00, 0x0, 0x00}, true);
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        while(SDL_PollEvent(&e) != 0)  //while there are events in the queue
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            /*if (e.key.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_g && debug)
                doDebugDraw = !doDebugDraw;*/
        }
        if (SDL_GetTicks() - lastKeypressTime >= 80 + 48 * checkSKInteract)
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
            lastKeypressTime = SDL_GetTicks();
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

