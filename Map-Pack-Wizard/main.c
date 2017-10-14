#include "outermeSDL.h"

int main(int argc, char* argv[])
{
    char getString[128];
    char mapPackData[5][128];
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
            printf("File name? (format map-packs/...) ");
            break;
        case 2:
            printf("Title of map pack? ");
            break;
        case 3:
            printf("Path for maps file? (format maps/...) ");
            break;
        case 4:
            printf("Path for tileset file? (format tileset/...) ");
            break;
        case 5:
            printf("Path for savefile? (format saves/...) ");
            break;
        }
        scanf("%s", getString);
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
            if (wizardState == 5)
                quit = true;
            else
                wizardState++;
            break;
        case 6:
            strcpy(mapPackData[0], getString);
            //read the rest of the data here
            quit = true;
            break;
        }
    }
    for(int i = 0; i < 5; i++)
    {
        printf("%s\n", mapPackData[i]);
    }
    return 0;
}
