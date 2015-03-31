/*
 * utils.cc
 *
 *  Created on: Mar 30, 2015
 *      Author: rferreira
 */

#include "utils.h"

bool nodClock = false;

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
		sleep(CLOCK_DELAY_TIME);
}

void disableClockDelay() {
	nodClock = true;
}
