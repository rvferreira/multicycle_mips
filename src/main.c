/*   **************************************************************************** /
 *  Trabalho de Sistemas Operacionais I - CPU MIPS Multiciclo com PThreads 
 *
 * Grupo 2
 *
 * Integrantes                      
 *      Andressa Andrião            
 *      Jéssika Darambaris          
 *      Raphael Ferreira
 *
 * Professor 
 *      Paulo Sérgio Souza  
 ******************************************************************************* */


//#include <set>
//#include <map>

#include "uc.h"

//using namespace std;

#define PROGRAM_NAME "UFMips"

#define USAGE "\
Usage: " PROGRAM_NAME " [options] FILE\n\
Try \"" PROGRAM_NAME " --help\" for more informations.\n"

#define HELP "\
Usage: " PROGRAM_NAME " FILE [options] \n\
Example: ./" PROGRAM_NAME " teste.mips --nodclock\n\
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

//using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int processInput(int argc, char** argv, const char *option, const char *filename);
void memoryPrint();

int main(int argc, char** argv) {
    const char *option = "";
    const char *filename = "";
    
    if (!processInput(argc, argv, option, filename)) {
        printf("%s \n",USAGE);
        return 1;// EXIT_SUCESS
    }

    // Remove the extension
    printf("%sFetching instructions from %s\n ", SEPARATOR, argv[1]) ;


    if (!strcmp(argv[2],"--nodclock")) {
        disableClockDelay();
    }

    if (!strcmp(argv[2],"--debug")) {
        debugMode = 1;
    }

    fetchJobFromFile(argv[1],"converted.mbin");
    if (pthread_create(&uc_handle, 0, uc_thread, NULL) != 0) {
        printf(THREAD_INIT_FAIL("UC"));
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

    return 1;
}

void memoryPrint(){
    int i;
    printf("Memory Final State: \n \n");
        for ( i = 0; i < memorySize; i++)
            printf(" %#.8x \n",(memoryBank[i] & 0xFFFFFFFF)); 

}

int processInput(int argc, char** argv, const char *option, const char *filename) {
    char  *s1="", *s2="";
    if (argc > 1) {
        s1 = argv[1];
        filename = s1;
    }
    if (argc > 2) {
        s2 = argv[2];
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
        return 0;
    }

    if (!strcmp(option, "--help")) {
        printf("%s \n", HELP);
        exit(1);
    }
    return 1;
}


