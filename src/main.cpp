//============================================================================
// Name        : multicycle_mips.cpp
// Author      : Raphael Ferreira
//				 Andressa Andriao
//				 Jessika Darambaris
// Version     : 1.0
// Copyright   : Open-bar for learning purposes
// Description : Multicycle CPU Mips processor
//============================================================================

#include <set>
#include <map>

#include "uc.h"

using namespace std;

#define PROGRAM_NAME "UFMips"

#define USAGE "\
Usage: " PROGRAM_NAME " [options] FILE\n\
Try \"" PROGRAM_NAME " --help\" for more informations.\n"

#define HELP "\
Usage: " PROGRAM_NAME " [options] FILE\n\
Example: ./" PROGRAM_NAME " --nodclock teste.mips\n\
Runs a Multicycle MIPS CPU over a set of instructions specified\n\
in the OPTION_FILE.\n\
\n\
Emulation Options:\n\
  --nodclock    	disable simulated clock delay\n\
  --debug    		turn on debug prints\n\
\n\
Fun fact: UFMips stands for Ultra Foda Mips.\n"

#define BAD_OPTION   "Bad option: "
#define NO_SUCH_FILE "No such file: "

using namespace std;


bool processInput(int argc, char** argv, string& option, string &filename);
void memoryPrint();

int main(int argc, char** argv) {
    string option   = "";
    string filename = "";

    if (!processInput(argc, argv, option, filename)) {
        cout << USAGE << endl;
        return EXIT_SUCCESS;
    }

    // Remove the extension
    string noExtentionFilename = filename.substr(0, filename.find_last_of("."));
    cout << SEPARATOR << "Fetching instructions from " << noExtentionFilename << "." << endl;

	if (option == "--nodclock") {
		disableClockDelay();
	}

	if (option == "--debug") {
		debugMode = true;
	}

	fetchJobFromFile(filename.c_str(), noExtentionFilename.c_str());
	if (pthread_create(&uc_handle, 0, uc_thread, NULL) != 0) {
		cout << THREAD_INIT_FAIL("UC");
		exit(0);
	}
	resourcesInit();

	while (1){
		if (!sem_trywait(&invalid_opcode)) {
			break;
		}
		sem_wait(&clock_free);
		simulateClockDelay();
		sem_post(&clock_updated);
	}

	fflush(0);
	memoryPrint();

	fflush(0);
	printf("\nPress any key to terminate.\n");
	getchar();

    return EXIT_SUCCESS;
}

void memoryPrint(){
	std::cout << "Memory Final State:" << std::endl << std::endl;
		for (int i = 0; i < memorySize; i++)
			cout << "0x" << setfill('0') << setw(8) << uppercase << hex
					<< ((memoryBank[i] & 0xFFFFFFFF) >> 0) << dec << endl;

}


bool processInput(int argc, char** argv, string& option, string &filename) {
    string s1, s2 = "";
    if (argc > 1) {
        s1 = string(argv[1]);
        filename = s1;
    }
    if (argc > 2) {
        s2 = string(argv[2]);
    }

    if (s1[0] == '-') {
        option = s1;
        filename = s2;
    }
    else if (s2[0] == '-') {
        option = s2;
        filename = s1;
    }

    if (argc < 2 || argc > 3) {
        return false;
    }

    set<string> options;
    options.insert("--nodclock");
    options.insert("--debug");

    bool isHelp = (option == "--help");
    bool badOption = (argc > 2 && !options.count(option));
    bool invalidFileName = (argc >= 2 && access(filename.c_str(), F_OK) == -1);

    if (isHelp) {
        cout << HELP << endl;
        exit(EXIT_SUCCESS);
    }

    if (badOption) {
        cout << BAD_OPTION << option << endl;
        return false;
    }
    if (invalidFileName) {
        cout << NO_SUCH_FILE << filename << endl;
        return false;
    }

    return true;
}
