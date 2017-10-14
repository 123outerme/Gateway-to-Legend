#include "outermeSDL.h"

#define MAX_MAP_PACK_DATA 5

char* uniqueReadLine(char* output[], int outputLength, char* filePath, int lineNum);
void strPrepend(char* input, const char* prepend);

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

            printf("%s\n", mapPackData[wizardState - 1]);

            if (wizardState == 5)
            {
                strPrepend((char*) mapPackData[4], "saves/");
                printf("%s\n", mapPackData[wizardState - 1]);
                quit = true;
            }
            else
                wizardState++;
            break;
        case 6:
            strcpy(mapPackData[0], getString);
            strPrepend(mapPackData[0], "map-packs/");
            for(int i = 1; i < MAX_MAP_PACK_DATA; i++)
                uniqueReadLine((char**) &mapPackData[i], 128, mapPackData[0], i - 1);
            quit = true;
            break;
        }
    }
    initSDL(mapPackData[3]);
    SDL_RenderClear(mainRenderer);
    SDL_RenderCopy(mainRenderer, tilesetTexture, NULL, &((SDL_Rect) {.x = 0, .y = TILE_SIZE, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT - TILE_SIZE}));
    drawText("Choose the main character tile", 0, 0, SCREEN_WIDTH, TILE_SIZE, (SDL_Color){0, 0, 0}, true);
    //SDL_RenderPresent(mainRenderer);
    waitForKey();
    return 0;
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
