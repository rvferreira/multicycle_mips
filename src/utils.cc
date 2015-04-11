/*
 * utils.cc
 *
 *  Created on: Mar 30, 2015
 *      Author: rferreira
 */

#include "utils.h"

bool nodClock = false;
bool debugMode = false;

void convertTextMIPStoBinMIPS(FILE* origin, FILE* decoded) {
	fseek(origin, 0, SEEK_END);
	int size = (int) ftell(origin);
	rewind(origin);

	if (size) {
		char *buffer = (char *) malloc(sizeof(char) * size);
		fread(buffer, 1, size, origin);

		fputs(buffer, decoded);
		free(buffer);
		//TODO iterate through lines and convert
	}
	else {
		std::cout << "Empty origin file" << std::endl;
		exit(0);
	}
}

void fetchJobFromFile(const char* filename, const char* noExtensionFilename) {
	FILE* origin = fopen(filename, "rb");
	bincode = fopen(strcat((char*) noExtensionFilename, ".bin"), "wb+");

	convertTextMIPStoBinMIPS(origin, bincode);

	fclose(origin);
}

void simulateClockDelay() {
	if (!nodClock)
		for (long int i = 0; i < CLOCK_DELAY_TIME*1000; i++){}
}

void disableClockDelay() {
	nodClock = true;
}

int hexToInt(char c){
	if ((int) c < 58) return ((int) (c - 48));
	return (int) (c - 65 + 10);
}
