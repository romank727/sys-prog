#include "Utils/utils.h"
#include <stdio.h>

static int wordcount(char * string) {
	int words = 0;

	

	return words;
}

int main(void) {
	
	configClock();
	configUSART2(38400);
	
	// This text contains 11 words.
	char * text = "This is a  word count test.\nHere is the second line !";
	printf("The text is:\n%s\n", text);
	printf("It contains %d words.\n", wordcount(text));

	while(1);
}
