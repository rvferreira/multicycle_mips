/*
 * utils.cc
 *
 *  Created on: Mar 30, 2015
 *      Author: rferreira
 */

#include "utils.h"

bool nodClock = false;

void convertTextMIPStoBinMIPS(FILE* origin, FILE* decoded){
	char buffer[4];
	fgets(buffer, 4, origin);
	fputs(buffer, decoded);
	//TODO loop and conversion
}

void fetchJobFromFile(const char* filename, const char* noExtensionFilename){
	FILE* origin = fopen(filename, "rb");
	FILE* decoded = fopen(strcat((char*) noExtensionFilename,".bin"), "wb");

	convertTextMIPStoBinMIPS(origin, decoded);

	fclose(origin);
	fclose(decoded);
}

void simulateClockDelay(){
	if (!nodClock) sleep(CLOCK_DELAY_TIME);
}

void disableClockDelay(){
	nodClock = true;
}
