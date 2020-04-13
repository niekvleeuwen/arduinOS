/*
  Author: Niek van Leeuwen
  Date: 27-02-2020
*/

const int BUFSIZE = 12;

/* CLI */
void commandLine();
// array to store the input from the CLI
static char cliBuffer[4][BUFSIZE];
static int cliBufferCounter = 0;
static int cliBufferCounterArguments = 0;

/* File System */
void writeFile(char* fileName, int fileSize, char* data);
void readFile(char* fileName);
void eraseFile(char* fileName);
void listFiles();
void loadFAT();
void freeSpace();

// list of all commands
void store();
void retrieve();
void erase();
void files();
void freespace();
void run();
void list();
void suspend();
void resume();
void kill();

typedef struct {
  char name[BUFSIZE];
  void * func;
} commandType;

static commandType command [] = {
  {"store" , &store },
  {"retrieve" , &retrieve },
  {"erase" , &erase },
  {"files" , &files },
  {"freespace" , &freespace },
  {"run" , &run },
  {"list" , &list },
  {"suspend" , &suspend },
  {"resume" , &resume },
  {"kill" , &kill },
};

static int commandTypeSize = sizeof(command) / sizeof(commandType);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(-1);
  // load FAT table in memory
  loadFAT();
  // give ready signal
  Serial.println("\nArduinOS 0.1 ready.\n==================");
}

void loop() {
  commandLine();
  runProcesses();
}

void store() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0 && cliBuffer[2][0] > 0) {
    writeFile(cliBuffer[1], atoi(cliBuffer[2]));
  }else{
    printError();
  }
}

void retrieve() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    readFile(cliBuffer[1]);
  }else{
    printError();
  }
}

void erase() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    eraseFile(cliBuffer[1]);
  }else{
    printError();
  }
}

void files() {
  listFiles();
}

void freespace() {
  freeSpace();
}

void run() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    startProcess(cliBuffer[1]);
  } else {
    printError();
  }
}

void list() {
  listProcesses();
}

void suspend() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    pauseProcess(atoi(cliBuffer[1]));
  } else {
    printError();
  }
}

void resume() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    resumeProcess(atoi(cliBuffer[1]));
  } else {
    printError();
  }
}

void kill() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    killProcess(atoi(cliBuffer[1]));
  } else {
    printError();
  }
}

void printError(){
  Serial.println("Wrong usage.");
}
