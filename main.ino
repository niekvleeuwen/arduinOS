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
void help();

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
  {"help" , &help },
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
    Serial.println("\nStoring file..");
    writeFile(cliBuffer[1], atoi(cliBuffer[2]));
  } else {
    Serial.println("Usage: store filename length data");
  }
}

void retrieve() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    readFile(cliBuffer[1]);
  } else {
    Serial.println("Usage: retrieve filename");
  }
}

void erase() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    Serial.print("\nErasing ");
    Serial.println(cliBuffer[1]);
    eraseFile(cliBuffer[1]);
    Serial.println("Done.");
  } else {
    Serial.println("Usage: retrieve filename");
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
    Serial.println("Usage: run filename");
  }
}

void list() {
  listProcesses();
}

void help() {
  Serial.println("\nAvailable commands");
  Serial.println("==================");
  for (int i = 0; i < commandTypeSize; i++) {
    Serial.println(command[i].name);
  }
  Serial.print("\n");
}

void suspend() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    pauseProcess(atoi(cliBuffer[1]));
  } else {
    Serial.println("Usage: suspend pid");
  }
}

void resume() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    resumeProcess(atoi(cliBuffer[1]));
  } else {
    Serial.println("Usage: resume pid");
  }
}

void kill() {
  // check if all parameters are filled
  if (cliBuffer[1][0] > 0) {
    killProcess(atoi(cliBuffer[1]));
  } else {
    Serial.println("Usage: kill pid");
  }
}
