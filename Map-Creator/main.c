#include "outermeSDL.h"

char* readStringInput(char* str, int limit);
char* uniqueReadLine(char* output[], int outputLength, const char* filePath, int lineNum);

int main(int argc, char* argv[])
{
    char* mainFilePath = calloc(100 + 1, sizeof(char));
    char mapFilePath[100];
    char tileFilePath[100];
    mainFilePath = readStringInput(mainFilePath, 100);
    uniqueReadLine(&mapFilePath, 100, mainFilePath, 1);
    uniqueReadLine(&tileFilePath, 100, mainFilePath, 2);
    initSDL(tileFilePath);
    waitForKey();
    closeSDL();
}

char* readStringInput(char* str, int limit)
{
	printf("Enter a string (Limit of %d characters): ", limit);
	//get input using a getc() loop and terminate upon a newline
	int i = 0;
	char c;
	while (i < limit)
	{
		c = getc(stdin);
		if (c == '\n')
			break;
		str[i] = c;
		i++;

	}
	if (i >= limit)
		i = limit;
	str = realloc(str, sizeof(char[++i]));
	str[i] = '\0';
	return str;
	//this works when i < limit because apparently you can just increase the size of arrays
	//by storing a new value at [dim + 1]
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
