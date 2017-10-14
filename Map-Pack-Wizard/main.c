#include "outermeSDL.h"

#define MAX_MAP_PACK_DATA 5
#define PIXELS_MOVED 48

#define PICK_MESSAGES_ARRAY {"Pick the main character tile.", "Pick the cursor.", "Pick the fully-transparent tile.", "Pick button 1.", "Pick button 2.", "Pick button 3.", "Pick door 1.", "Pick door 2.", "Pick door 3.", "Pick the teleporter.", "Pick the damaging hazard."}

int* mainLoop(sprite* playerSprite);
char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);
void strPrepend(char* input, const char* prepend);
SDL_Keycode getKey();

const int maxArraySize = 11;

int main(int argc, char* argv[])
{
    char getString[128];
    char mapPackData[MAX_MAP_PACK_DATA][128];
    int wizardState = 0;
    bool quit = false;
	while (!quit)
    {
        switch(wizardState)
        {
        case 0:
            printf("Create new file? (y/n) ");
            break;
        case 1:
        case 6:
            printf("File name? map-packs/");
            break;
        case 2:
            printf("Title of map pack? ");
            break;
        case 3:
            printf("Path for maps file? maps/");
            break;
        case 4:
            printf("Path for tileset file? tileset/");
            break;
        case 5:
            printf("Path for savefile? saves/");
            break;
        }
        gets(getString);
        switch(wizardState)
        {
        case 0:
            if (getString[0] == 'y')
                wizardState = 1;
            else
                wizardState = 6;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            strcpy(mapPackData[wizardState - 1], getString);
            if (wizardState == 1)
                strPrepend(mapPackData[0], "map-packs/");

            if (wizardState == 3)
                strPrepend((char*) mapPackData[2], "maps/");

            if (wizardState == 4)
                strPrepend((char*) mapPackData[3], "tileset/");

            //printf("%s\n", mapPackData[wizardState - 1]);

            if (wizardState == 5)
            {
                strPrepend((char*) mapPackData[4], "saves/");
                //printf("%s\n", mapPackData[wizardState - 1]);
                quit = true;
            }
            else
                wizardState++;
            break;
        case 6:
            strcpy(mapPackData[0], getString);
            strPrepend(mapPackData[0], "map-packs/");
            if (!checkFile(mapPackData[0], 0))
            {
                perror("No such map-pack");
                return 1;
            }
            for(int i = 1; i < MAX_MAP_PACK_DATA; i++)
                uniqueReadLine((char**) &mapPackData[i], 128, mapPackData[0], i - 1);
            quit = true;
            break;
        }
    }
    initSDL(mapPackData[3]);
    sprite chooser;
    initSprite(&chooser, 0, TILE_SIZE, TILE_SIZE, 0, type_player);
    int* numbers = mainLoop(&chooser);
    createFile(mapPackData[0]);
    for(int i = 1; i < 5; i++)
    {
        appendLine(mapPackData[0], mapPackData[i]);
        //printf("%s\n", mapPackData[i]);
    }
    char* whoCares = "";
    for(int i = 0; i < maxArraySize; i++)
    {
        appendLine(mapPackData[0], intToString(numbers[i], whoCares));
        //printf("%d\n", numbers[i]);
    }
    closeSDL();
    return 0;
}

int* mainLoop(sprite* playerSprite)
{
    int* numArray = (int*) calloc(maxArraySize, sizeof(int));
    int numArrayTracker = 0, frame = 0;
    char* text[] = PICK_MESSAGES_ARRAY;
    bool quit = false;
    while(numArrayTracker < maxArraySize && !quit)
    {
        SDL_RenderClear(mainRenderer);
        SDL_RenderCopy(mainRenderer, tilesetTexture, NULL, &((SDL_Rect) {.x = 0, .y = TILE_SIZE, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT - TILE_SIZE}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0x1C, 0xC6, 0xFF);
        SDL_RenderDrawRect(mainRenderer, &((SDL_Rect){.x = playerSprite->x, .y= playerSprite->y, .w = playerSprite->w, .h = playerSprite->h}));
        SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        drawText(text[numArrayTracker], 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0x00, 0x0, 0x00}, true);
        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        getKey();  //editor freezes without this
        if (++frame > 0)
        {
            if (playerSprite->y > TILE_SIZE && keyStates[SDL_SCANCODE_W])
                playerSprite->y -= PIXELS_MOVED;
            if (playerSprite->y < SCREEN_HEIGHT - playerSprite->h && keyStates[SDL_SCANCODE_S])
                playerSprite->y += PIXELS_MOVED;
            if (playerSprite->x > 0 && keyStates[SDL_SCANCODE_A])
                playerSprite->x -= PIXELS_MOVED;
            if (playerSprite->x < SCREEN_WIDTH - playerSprite->w && keyStates[SDL_SCANCODE_D])
                playerSprite->x += PIXELS_MOVED;
            if (keyStates[SDL_SCANCODE_SPACE])
                numArray[numArrayTracker++] = 8 * (playerSprite->x / TILE_SIZE) + playerSprite->y / TILE_SIZE - 1;  //-1 because we don't start at y=0
            if (keyStates[SDL_SCANCODE_W] || keyStates[SDL_SCANCODE_S] || keyStates[SDL_SCANCODE_A] || keyStates[SDL_SCANCODE_D])
                frame = -6;
            else
                frame = -10;
        }

        if (keyStates[SDL_SCANCODE_ESCAPE] || keyStates[SDL_SCANCODE_RETURN])
                quit = true;

        SDL_Delay(15);  //frame cap sorta
        //SDL_RenderPresent(mainRenderer);
    }
    /*for(int i = 0; i < maxArraySize; i++)
        printf("%d\n", numArray[i]);*/
    //waitForKey();
    return numArray;
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

void strPrepend(char* input, const char* prepend)
{
    char* temp = (char*) calloc(128, sizeof(char));
    strcpy(temp, prepend);
    strcat(temp, input);
    strcpy(input, temp);
    free(temp);
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
