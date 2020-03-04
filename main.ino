/*
  Author: Niek van Leeuwen
  Date: 27-02-2020
*/

const int BUFSIZE = 32;

/* CLI data  */
void commandLine();
// array to store the input from the CLI
static char cliBuffer[4][BUFSIZE];
static int cliBufferCounter = 0;
static int cliBufferCounterArguments = 0;

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
}

void loop() {
  commandLine();
}

void store(){
  Serial.println("\nStore");
  Serial.print("File: ");
  Serial.println(cliBuffer[1]);
  Serial.print("Size: ");
  Serial.println(cliBuffer[2]);
  Serial.print("Data: ");
  Serial.println(cliBuffer[3]);
}

void retrieve(){
  Serial.println("\nRetrieve");
  Serial.print("File: ");
  Serial.println(cliBuffer[1]);
}

void erase(){
  Serial.println("\nErase");
  Serial.print("File: ");
  Serial.println(cliBuffer[1]);
}

void files(){
  Serial.println("\nFiles");
}

void freespace(){
  Serial.println("\nFree space");
}

void run(){
  Serial.println("\nRun");
  Serial.print("File: ");
  Serial.println(cliBuffer[1]);
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
