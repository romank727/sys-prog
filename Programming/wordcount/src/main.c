#include "Utils/utils.h"
#include <stdio.h>
#include <string.h>

static int isAlphanum(char *string) {
	return ((*string >= 'A' && *string <= 'Z') || (*string >= 'a' && *string <= 'z') || (*string >= '0' && *string <= '9'));
}

static int wordcount(char * string) {
	int words = 0;
	while (*string) {
		if (isAlphanum(string)) {
			if (!isAlphanum(string+1)) {
				words++;
			}
		}
		string++;
	}
	return words;
}

static int wordcount2(char * string) {
	int inWord = 0;
	int words = 0;
	while(*string) {
		if (isAlphanum(string)) {
			if (!inWord) {
				inWord = 1;
				words++;
			} 
		}
		else {
			inWord = 0;
		}
		string++;
	}
	return words;
}

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	// This text contains 11 words.
	char * text = "  ,This is a word count test.\nHere is the second line oookie helloooo!";
	printf("The text is:\n%s\n", text);
	printf("It contains %d words.\n", wordcount(text));
	printf("It contains %d words.\n", wordcount2(text));

	while(1);
}
