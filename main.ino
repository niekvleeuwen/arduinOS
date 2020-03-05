/*
  Author: Niek van Leeuwen
  Date: 27-02-2020
*/

const int BUFSIZE = 12;

/* CLI */
void commandLine();
// array to store the input from the CLI
static char cliBuffer[5][BUFSIZE];
static int cliBufferCounter = 0;
static int cliBufferCounterArguments = 0;

/* File System */
void writeFile(char* fileName, int fileSize, char* data);
void readFile(char* fileName);
void eraseFile(char* fileName);
void listFiles();
void loadFAT();
void freeSpace();
void writeFATEntry(char* name, int position, int length, int address);

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
  // load FAT table in memory
  loadFAT();
  // give ready signal
  Serial.println("\nArduinOS 0.1 ready.\n==================");
}

void loop() {
  commandLine();
}

void store(){
  Serial.println("\nStoring file..");
  writeFile(cliBuffer[1], atoi(cliBuffer[2]), cliBuffer[3]);
}

void retrieve(){
  Serial.print("\nContent: ");
  readFile(cliBuffer[1]);
}

void erase(){
  Serial.println("\nErase");
  eraseFile(cliBuffer[1]);
}

void files(){
  Serial.println("\nFiles\n=====");
  listFiles();
}

void freespace(){
  freeSpace();
  Serial.println("\nDone sorting.");
}

void run(){
  Serial.println("\nFAT ENTRY TMP");
  writeFATEntry(cliBuffer[1], atoi(cliBuffer[2]), atoi(cliBuffer[3]), atoi(cliBuffer[4]));
}

void list(){
  Serial.println("\nAvailable commands");
  Serial.println("==================");
  for(int i = 0; i < commandTypeSize; i++){
      Serial.println(command[i].name);
  }
  Serial.print("\n");
}

void suspend(){
  Serial.println("\nSuspend");
  Serial.print("ID: ");
  Serial.println(cliBuffer[1]);
}

void resume(){
  Serial.println("\nResume");
  Serial.print("ID: ");
  Serial.println(cliBuffer[1]);
}

void kill(){
  Serial.println("\nKill");
  Serial.print("ID: ");
  Serial.println(cliBuffer[1]);
}
