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

typedef enum {
    script_none,                   //0 default, do nothing
    script_trigger_dialogue,       //1 if player steps in coords & presses interact, trigger a dialogue/text box
    script_trigger_dialogue_once,  //2 same as above, but just once.
    script_trigger_boss,           //3 if player steps in coords, spawn boss
    script_switch_maps,            //4 triggers a switching of rooms. Map borders do this by default so only use this when you are using some sort of other warp tile. Like a silent use_teleporter
    script_use_gateway,            //5 triggers a playing of an animation followed by a switching of rooms. Only to be used internally for warp gates.
    script_use_teleporter,         //6 teleports to a specified matching teleporter
    script_toggle_door,            //7 if player steps in coords or other action occurs, open a door
    script_animation,              //8 if player steps in coords, do animation
    script_boss_actions,           //9 if boss is still alive, execute boss actions
    script_gain_exp,               //19 gives player some EXP. Don't abuse please
    script_gain_money,             //11 gives player some money. Please don't abuse also
    script_player_hurt,            //12 hurts the player by <data> amount
    script_placeholder,            //13 ?
} scriptBehavior;

typedef struct {
    int mapNum;
    int x;
    int y;
    int w;
    int h;
    scriptBehavior action;
    char* data;
    bool active;
} script;

#define SIZE_OF_SCANCODE_ARRAY 7
int CUSTOM_SCANCODES[SIZE_OF_SCANCODE_ARRAY];
#define SC_UP CUSTOM_SCANCODES[0]
#define SC_DOWN CUSTOM_SCANCODES[1]
#define SC_LEFT CUSTOM_SCANCODES[2]
#define SC_RIGHT CUSTOM_SCANCODES[3]
#define SC_INTERACT CUSTOM_SCANCODES[4]
#define SC_MENU CUSTOM_SCANCODES[5]
#define SC_ATTACK CUSTOM_SCANCODES[6]

#define checkSKUp keyStates[SC_UP]
#define checkSKDown keyStates[SC_DOWN]
#define checkSKLeft keyStates[SC_LEFT]
#define checkSKRight keyStates[SC_RIGHT]
#define checkSKInteract keyStates[SC_INTERACT]
#define checkSKMenu keyStates[SC_MENU]
#define checkSKAttack keyStates[SC_ATTACK]
//SDL_SCANCODE_W
//SDL_SCANCODE_S
//SDL_SCANCODE_A
//SDL_SCANCODE_D
//SDL_SCANCODE_SPACE
//SDL_SCANCODE_ESCAPE

#define CACHE_NAME "GtLToolchainCache.cfg"
#define CONFIG_FILEPATH "GatewayToLegend.cfg"

#define drawSprite(spr, flip) drawTile(spr.tileIndex, spr.x, spr.y, spr.w, flip)
#define WINDOW_NAME "Gateway to Legend Map Creator"
#define MAIN_TILESET "tileset/mainTileset48.png"

#define MAINARROW_ID 34

#define AMENU_MAIN_TEXTCOLOR  0x00, 0xB0, 0xDA
#define AMENU_MAIN_BGCOLOR 0xE4, 0xE9, 0xF3
#define AMENU_MAIN_TITLECOLOR1 0x4D, 0xD2, 0xFF
#define AMENU_MAIN_TITLECOLOR2 0x00, 0xAC, 0xE6

#define AMENU_MAIN_THEME (SDL_Color) {AMENU_MAIN_BGCOLOR, 0xFF}, (SDL_Color) {AMENU_MAIN_TITLECOLOR2, 0xFF}, (SDL_Color) {AMENU_MAIN_TITLECOLOR1, 0xFF},  (SDL_Color) {AMENU_MAIN_TEXTCOLOR, 0xFF}
//^map creator defines. v map-pack wizard defines

#define PICK_MESSAGES_ARRAY {"Pick the main character idle.", "Pick the main character walking.", "Pick the cursor.", "Pick the HP icon.", "Pick the player sword.", "Pick the fully-transparent tile.", "Pick button 1.", "Pick button 2.", "Pick button 3.", "Pick door 1.", "Pick door 2.", "Pick door 3.", "Pick the teleporter.", "Pick the damaging hazard.", "Pick the warp gate.", "Pick enemy 1.", "Pick enemy 2.", "Pick enemy 3."}
#define MAX_SPRITE_MAPPINGS 18  //sprite defines and other map-pack data? I'm really not sure where this number comes from
#define MAX_MAP_PACK_DATA 6  //does not include sprite defines

#define MAIN_HELP_TEXT "Make map-packs using this toolchain! Create maps, scripts, and setup your files and tileset using this. To navigate, use the keys you set up in the main program."
#define SCRIPT_HELP_TEXT "Use your movement keys to maneuver between maps and to the tile you want. Press Confirm to \"drop the anchor\" there. Set the width and height next. Toggle interval between 1/8 tile and a full tile using Attack."
#define MAPPACK_SETUP_HELP_TEXT "Change which files you use, assign tiles in your tileset to display certain objects, and change your New Game spawn here. Maneuver using movement keys and follow the instructions."

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


int aMenu(SDL_Texture* texture, int cursorID, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, bool isMain);
// ^ whatever

void initConfig();
void loadConfig(char* filePath);

#define MAX_LIST_OF_MAPS 30
#define MAX_CHAR_IN_FILEPATH MAX_PATH
#define MAP_PACKS_SUBFOLDER "map-packs/"
#define MAX_MAPPACKS_PER_PAGE 11
#define MAX_ENEMIES 6
int subMain(mapPack* workingPack);

void editFilePaths(mapPack* workingPack);
void editInitSpawn(mapPack* workingPack);
void editTileEquates(mapPack* workingPack);

void createMapPack(mapPack* newPack);
void loadMapPackData(mapPack* loadPack, char* location);
char** getListOfFiles(const size_t maxStrings, const size_t maxLength, const char* directory, int* strNum);
void mapSelectLoop(char** listOfFilenames, char* mapPackName, int maxStrNum, bool* backFlag);
//^ working with loading a map-pack to work on

int mainMapCreator();
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);
void loadMapFile(char* filePath, int tilemapData[][WIDTH_IN_TILES], int eventmapData[][WIDTH_IN_TILES], const int lineNum, const int y, const int x);
script* mainMapCreatorLoop(player* playerSprite, int* scriptCount, mapPack workingPack);
void viewMap(mapPack workingPack, int thisLineNum, bool drawLineNum, bool update);
int chooseMap(mapPack workingPack);
SDL_Keycode getKey();
void drawMaps(mapPack workingPack, int thisTilemap[][WIDTH_IN_TILES], int startX, int startY, int endX, int endY, bool hideCollision, bool isEvent, bool updateScreen);
void initPlayer(player* player, int x, int y, int w, int h, int angle, SDL_RendererFlip flip, int tileIndex);
void writeTileData();
//^map creator functions.

//V script editor functions
void mainScriptEdtior(mapPack* workingPack);
int scriptSelectLoop(mapPack workingPack);
script mainScriptLoop(mapPack workingPack, scriptBehavior action);
void initScript(script* scriptPtr, scriptBehavior action, int mapNum, int x, int y, int w, int h, char* data);
void writeScriptData(script* mapScripts, int count);

//V map-pack wizard functions
int mainMapPackWizard();
void mainMapPackWizardLoop(sprite* playerSprite, int* numArray);

void strPrepend(char* input, const char* prepend);

//this is to match the tilemap array in outermeSDL.h
int eventmap[HEIGHT_IN_TILES][WIDTH_IN_TILES];
SDL_Texture* mainTilesetTexture;
const int targetTime = 1000 / FRAMERATE;

int main(int argc, char* argv[])
{
    mapPack workingPack;
    strcpy(workingPack.mainFilePath, "/\0");
    initSDL("Gateway to Legend Map-Pack Tools", MAIN_TILESET, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
    loadIMG(MAIN_TILESET, &mainTilesetTexture);

    if (checkFile(CONFIG_FILEPATH, 0))
        loadConfig(CONFIG_FILEPATH);
    else
        initConfig();

    bool quit = false, proceed = false;
    char* resumeStr = "\0";
    while(!quit)
    {
        readLine(CACHE_NAME, 0, &resumeStr);
        resumeStr = removeChar(resumeStr, '\n', MAX_PATH, false);
        if (checkFile(resumeStr, 0))
            resumeStr += 10;  //pointer arithmetic to get rid of the "map-packs/" part of the string (use 9 instead to include the /)
        else
            resumeStr = "(No Resume)\0";
        int code = aMenu(tilesetTexture, MAINARROW_ID, "Gateway to Legend Toolchain", (char*[5]) {"New Map-Pack", "Load Map-Pack", resumeStr, "Info/Help", "Quit"}, 5, 1, AMENU_MAIN_THEME, true, false);
        if (code == 1)
        {
            closeSDL();
            createMapPack(&workingPack);
            initSDL("Gateway to Legend Map-Pack Tools", MAIN_TILESET, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
            proceed = true;
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
			proceed = !back;
		}

        if (code == 3 && strcmp(resumeStr, "(No Resume)\0") != 0)
        {
            char mainFilePath[MAX_PATH];
            uniqueReadLine((char**) &mainFilePath, MAX_PATH, CACHE_NAME, 0);
            loadMapPackData(&workingPack, (char*) mainFilePath);
            proceed = true;
        }

        if (code == 4)
        {
            int key = 0;
            while(!key)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR, 0xFF);
                SDL_RenderFillRect(mainRenderer, NULL);
                drawText(MAIN_HELP_TEXT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                key = getKey();
            }
        }

        if (code == 5 || code == -1)
            quit = true;

        if (proceed && code < 4 && workingPack.mainFilePath[0] != '/')
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

    initSDL("Gateway to Legend Map-Pack Wizard", newPack->tilesetFilePath, FONT_FILE_NAME, TILE_SIZE * 16, TILE_SIZE * 9, 24);
    sprite chooser;
    initSprite(&chooser, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_player);
    char* temp = "";
    mainMapPackWizardLoop(&chooser, newPack->tilesetMaps);

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

    loadIMG(loadPack->tilesetFilePath, &(loadPack->mapPackTexture));
}

void initConfig()
{
    SC_UP = SDL_SCANCODE_W;
    SC_DOWN = SDL_SCANCODE_S;
    SC_LEFT = SDL_SCANCODE_A;
    SC_RIGHT = SDL_SCANCODE_D;
    SC_INTERACT = SDL_SCANCODE_SPACE;
    SC_MENU = SDL_SCANCODE_ESCAPE;
    SC_ATTACK = SDL_SCANCODE_LSHIFT;
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

        drawTile(MAINARROW_ID, 10, (selectItem + 2) * TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
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
        loadIMG(workingPack->tilesetFilePath, &(workingPack->mapPackTexture));  //for some reason we need to load twice??
        int code = aMenu(workingPack->mapPackTexture, workingPack->tilesetMaps[2], "Map-Pack Tools", (char*[4]) {"Map Creator", "Script Editor", "Map-Pack Wizard", "Back"}, 4, 1, AMENU_MAIN_THEME, true, false);
        closeSDL();
        if (code == 1)
            mainMapCreator(workingPack);
        if (code == 2)
            mainScriptEdtior(workingPack);
        if (code == 3)
            mainMapPackWizard(workingPack);
        if (code == 4 || code == -1)
            quit = true;
    }
    return 0;
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
        /*if (isMain)
        {
            char version[12];
            snprintf(version, 12, "%s%s", FULLVERSION_STRING, STATUS_SHORT);
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
                const Uint8* keyStates = SDL_GetKeyboardState(NULL);
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
                if (isMain && (keyStates[SDL_SCANCODE_LCTRL] || keyStates[SDL_SCANCODE_RCTRL]) && keyStates[SDL_SCANCODE_R] && !settingsReset)
                {
                    SC_UP = SDL_SCANCODE_W;
                    SC_DOWN = SDL_SCANCODE_S;
                    SC_LEFT = SDL_SCANCODE_A;
                    SC_RIGHT = SDL_SCANCODE_D;
                    SC_ATTACK = SDL_SCANCODE_LSHIFT;
                    SC_INTERACT = SDL_SCANCODE_SPACE;
                    SC_MENU = SDL_SCANCODE_ESCAPE;
                    settingsReset = true;
                }
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
    loadIMG(workingPack->tilesetFilePath, &(workingPack->mapPackTexture));  //We have to load again because we closed the renderer
    loadIMG(MAIN_TILESET, &mainTilesetTexture);
    player creator;
    initPlayer(&creator, 0, 0, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE, 0);
    creator.mapScreen = -1;
    if (loadCheck[0] == 'y')
    {
        creator.mapScreen = chooseMap(*workingPack);
        loadMapFile(workingPack->mapFilePath, tilemap, eventmap, creator.mapScreen, HEIGHT_IN_TILES, WIDTH_IN_TILES);
    }
    SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    int scriptCount = 0;
    script* mapScripts = mainMapCreatorLoop(&creator, &scriptCount, *workingPack);
    closeSDL();
    char saveCheck[2];
    printf("Save? (y/n) ");
	scanf("%s", saveCheck);
	if (saveCheck[0] == 'y')
    {
        writeTileData();
        writeScriptData(mapScripts, scriptCount);
    }
    free(mapScripts);
    //waitForKey();
    //SDL_Delay(1000);
    initSDL("Gateway to Legend Map Tools", MAIN_TILESET, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
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
        drawText(intToString(thisLineNum, buffer), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {0xFF, 0xFF, 0xFF}, update);
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
                    SDL_Keycode key = 0;
                    map = chooseMap(workingPack);
                    bool inQuit = false;
                    while(!inQuit)
                    {
                        SDL_RenderClear(mainRenderer);
                        viewMap(workingPack, map, false, false);
                        drawText("Choose x/y coord to place player in.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {0xFF, 0xFF, 0xFF}, false);
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
                        drawText("Choose x/y coord to place player in.", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color) {0xFF, 0xFF, 0xFF}, false);
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

SDL_Keycode getKey()
{
    SDL_Event e;
    SDL_Keycode keycode = 0;
    while(SDL_PollEvent(&e) != 0)
    {
        if(e.type == SDL_QUIT)
            keycode = ANYWHERE_QUIT;
        else
            if(e.type == SDL_KEYDOWN)
                keycode = e.key.keysym.sym;
    }
    return keycode;
}

void initPlayer(player* player, int x, int y, int w, int h, int angle, SDL_RendererFlip flip, int tileIndex)
{
    //inputName(player);  //custom text input routine to get player->name
    initSprite(&(player->spr), x, y, w, h, tileIndex, angle, flip, (entityType) type_player);
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


void writeScriptData(script* mapScripts, int count)
{
    if (count < 1)
        return;
    char* outputFile = "output/script.txt";
    char scriptText[600];
    createFile(outputFile);
    for(int i = 0; i < count; i++)
    {
        snprintf(scriptText, 160, "{%d,%d,%d,%d,%d,%d,%s}", mapScripts[i].action, mapScripts[i].mapNum, mapScripts[i].x, mapScripts[i].y, mapScripts[i].w, mapScripts[i].h, mapScripts[i].data);
        printf("%s\n", scriptText);
        appendLine(outputFile, scriptText);
    }
    printf("outputted to output/script.txt. NOTE: If the second argument is -1, change to (line number of new map) - 1\n");
}
//end map creator code.

//start script editor code
void mainScriptEdtior(mapPack* workingPack)
{
    initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, TILE_SIZE * 20, TILE_SIZE * 15, 48);
    int scriptNum = 0;
    loadIMG(workingPack->tilesetFilePath, &(workingPack->mapPackTexture));
    scriptNum = scriptSelectLoop(*workingPack);
    if (scriptNum > 0)
    {
        script newScript = mainScriptLoop(*workingPack, (scriptBehavior) scriptNum);
        if (newScript.action != script_none)
            writeScriptData(&newScript, 1);
    }
    else
    {
        sprite scriptSpr;
        initSprite(&scriptSpr, 0, 0, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, (entityType) type_na);
        bool quit = false;
        int scriptLineNum = 0;
        while(!quit)
        {
            //load a script
        }
    }
}

int scriptSelectLoop(mapPack workingPack)
{
    sprite cursor;
    initSprite(&cursor, TILE_SIZE, 5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, workingPack.tilesetMaps[2], 0, SDL_FLIP_NONE, (entityType) type_na);
    const int optionsSize = 14;
    char* optionsArray[] = {"Load", "TriggerDialogue", "TriggerDialOnce", "TriggerBoss", "SwitchMaps", "Gateway", "Teleporter", "ToggleDoor", "Animation", "BossActions", "GainExp", "GainMoney", "HurtPlayer", "placeholder"};
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
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_DOWN))
                {
                    if (cursor.y < 8 * TILE_SIZE)
                        cursor.y += TILE_SIZE;
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
                }

                if (e.key.keysym.sym == SDL_GetKeyFromScancode(SC_INTERACT))
                {
                    selection = cursor.y / TILE_SIZE - 4;
                    if (selection != 1 && selection != 3)
                        quit = true;
                }
                if (selection == 3)
                {
                    int key = 0;
                    while(!key)
                    {
                        SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR, 0xFF);
                        SDL_RenderFillRect(mainRenderer, NULL);
                        drawText(SCRIPT_HELP_TEXT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (SDL_Color) {AMENU_MAIN_TEXTCOLOR}, true);
                        key = getKey();
                    }
                    selection = 0;
                }
            }
        }
        drawATile(workingPack.mapPackTexture, cursor.tileIndex, cursor.x, cursor.y, TILE_SIZE, TILE_SIZE, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer);
    }
    if (selection > 2)
        scriptType = -1;
    return scriptType;
}

int toolchain_min(int x, int y)
{
    return ((x > y) ? y : x);
}

script mainScriptLoop(mapPack workingPack, scriptBehavior action)
{
    script outputScript;
    int map = chooseMap(workingPack), x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    int intervalSize = TILE_SIZE;
    char* data = calloc(99, sizeof(char));
    sprite cursor;
    initSprite(&cursor, 0, 0, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_na);
    bool quit = false, editXY = true, bigIntervalSize = false;
    SDL_Keycode key;
    while(!quit)
    {
        SDL_RenderClear(mainRenderer);
        viewMap(workingPack, map, false, false);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect) {.x = x1 ? x1 : cursor.x, .y = y1 ? y1 : cursor.y, .w = x1 ? cursor.x - x1 : cursor.w, .h = y1 ? cursor.y - y1 : cursor.h}));
        key = getKey();
        if (SC_ATTACK == SDL_GetScancodeFromKey(key) && bigIntervalSize == false)
        {
            intervalSize = 48;
            bigIntervalSize = true;
        }
        else
        if (!editXY && bigIntervalSize == true && SC_ATTACK == SDL_GetScancodeFromKey(key))
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
        if (key == ANYWHERE_QUIT)
            quit = true;
        SDL_RenderPresent(mainRenderer);
    }
    if (action == script_trigger_dialogue || script_trigger_dialogue_once)
    {
        //get dialogue text
    }
    if (action == script_gain_money || action == script_gain_exp || action == script_player_hurt)
    {
        //get amt
    }
    if (action == script_switch_maps)
    {
        //get location
    }
    if (key == ANYWHERE_QUIT)
        initScript(&outputScript, script_none, map, toolchain_min(x1, x2), toolchain_min(y1, y2), abs(x2 - x1), abs(y2 - y1), " ");
    else
        initScript(&outputScript, action, map, toolchain_min(x1, x2), toolchain_min(y1, y2), abs(x2 - x1), abs(y2 - y1), data);
    free(data);
    return outputScript;
}

void initScript(script* scriptPtr, scriptBehavior action, int mapNum, int x, int y, int w, int h, char* data)
{
	scriptPtr->action = action;
	scriptPtr->mapNum = mapNum;
	scriptPtr->x = x;
	scriptPtr->y = y;
	scriptPtr->w = w;
	scriptPtr->h = h;
	scriptPtr->data = calloc(99, sizeof(char));
	strncpy(scriptPtr->data, data, 99);
	scriptPtr->active = true;
}
//end script editor code.


//start map-pack wizard code
int mainMapPackWizard(mapPack* workingPack)
{
    initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, TILE_SIZE * 20, TILE_SIZE * 15, 48);
    bool quit = false;
    while (!quit)
    {
        int choice = aMenu(tilesetTexture, workingPack->tilesetMaps[2], workingPack->mainFilePath + 10, (char*[5]) {"Edit Filepaths", "Edit Init Spawn", "Edit Tile Equates", "Info/Help", "Back"}, 5, 0, AMENU_MAIN_THEME, true, false);

        if (choice == 1)
            editFilePaths(workingPack);

	if (choice == 2)
            editInitSpawn(workingPack);

        if (choice == 3)
        {
            editTileEquates(workingPack);
            closeSDL();
            initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
        }

        if (choice == 4)
        {
            int key = 0;
            while(!key)
            {
                SDL_SetRenderDrawColor(mainRenderer, AMENU_MAIN_BGCOLOR, 0xFF);
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
        int choice = aMenu(tilesetTexture, workingPack->tilesetMaps[2], workingPack->mainFilePath + 10, (char*[6]) {"Change Name", "Edit Map Path", "Edit Tileset Path", "Edit Save Path", "Edit Script Path", "Back"}, 6, 0, AMENU_MAIN_THEME, true, false);
        if (choice < 0 || choice == 6)
            quit = true;
        else
        {
            closeSDL();
            char getString[MAX_PATH];
            getString[0] = '\0';
            switch(choice)
            {
            case 1:
                printf("Title of map pack? (Was %s)\n", workingPack->name);
                break;
            case 2:
                printf("Path for maps file? (Was %s)\nmaps/", workingPack->mapFilePath + 5);  //we add numbers here to get rid of the "path/" part of the filepath
                break;
            case 3:
                printf("Path for tileset file? (Was %s)\ntileset/", workingPack->tilesetFilePath + 8);
                break;
            case 4:
                printf("Path for savefile? (Was %s)\nsaves/", workingPack->saveFilePath + 6);
                break;
            case 5:
                printf("Path for scripts? (Was %s)\nscripts/", workingPack->scriptFilePath + 8);
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
            }
            initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
        }
    }
    saveMapPack(workingPack);
}

void editInitSpawn(mapPack* workingPack)
{
    loadIMG(workingPack->tilesetFilePath, &(workingPack->mapPackTexture));
    SDL_RenderClear(mainRenderer);
    workingPack->initMap = chooseMap(*workingPack);
    chooseCoords(*workingPack, workingPack->initMap, &(workingPack->initX), &(workingPack->initY));
    initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 48);
    saveMapPack(workingPack);
}

#undef SCREEN_WIDTH
#undef SCREEN_HEIGHT
#define SCREEN_WIDTH TILE_SIZE * 16
#define SCREEN_HEIGHT TILE_SIZE * 9
void editTileEquates(mapPack* workingPack)
{
    closeSDL();
    initSDL("Gateway to Legend Map-Pack Wizard", workingPack->tilesetFilePath, FONT_FILE_NAME, SCREEN_WIDTH, SCREEN_HEIGHT, 24);
    SDL_RenderClear(mainRenderer);
    int numbers[MAX_SPRITE_MAPPINGS];
    numbers[0] = -1;
    sprite chooser;
    initSprite(&chooser, 0, TILE_SIZE, TILE_SIZE, TILE_SIZE, 0, 0, SDL_FLIP_NONE, type_player);
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
    bool quit = false, whiteBG = true;
    SDL_Event e;
    while(numArrayTracker < MAX_SPRITE_MAPPINGS && !quit)
    {
        if (whiteBG)
            SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        else
            SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(mainRenderer);
        SDL_RenderCopy(mainRenderer, tilesetTexture, NULL, &((SDL_Rect) {.x = 0, .y = TILE_SIZE, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT - TILE_SIZE}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0x1C, 0xC6, 0xFF);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->x, .y= playerSprite->y, .w = playerSprite->w, .h = playerSprite->h}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        drawText(text[numArrayTracker], 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0xFF * (!whiteBG), 0xFF * (!whiteBG), 0xFF * (!whiteBG)}, true);
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
            if (keyStates[SDL_SCANCODE_LSHIFT])
                whiteBG = !whiteBG;
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

