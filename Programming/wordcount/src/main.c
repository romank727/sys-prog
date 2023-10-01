#include "Utils/utils.h"
#include <stdio.h>
#include <string.h>

static int wordcount(char * string) {
	int words = 0;
	char character, prevChar = ' ';
	while (*string != '\0') {
		character = *string;
		if ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9')) {
			// is alphanumeric
		}
		else if (character == ' ' || character == '.' || character == ',' || character == '!' || character == '?') {
			if (prevChar != ' ' && prevChar != '.' && prevChar != ',' && prevChar != '!' && prevChar != '?') {
				words += 1;
			}
		}
		prevChar = character;
		string++;
	}
	
	return words;
}

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	// This text contains 11 words.
	char * text = "This is a word count test.\nHere is the second line !";
	printf("The text is:\n%s\n", text);
	printf("It contains %d words.\n", wordcount(text));

	while(1);
}
