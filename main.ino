/*
  Author: Niek van Leeuwen
  Date: 27-02-2020
*/
const int BUFSIZE = 32;

/* CLI */
void commandLine();
// Array to store the input from the CLI
static char cliBuffer[BUFSIZE];
static int cliBufferIndex = 0;
static char input[4][BUFSIZE];
static int inputIndex = 0;

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
  Serial.println(input[1]);
  Serial.print("Size: ");
  Serial.println(input[2]);
  Serial.print("Data: ");
  Serial.println(input[3]);
}

void retrieve(){
  Serial.println("\nRetrieve");
  Serial.print("File: ");
  Serial.println(input[1]);
}

void erase(){
  Serial.println("\nErase");
  Serial.print("File: ");
  Serial.println(input[1]);
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
  Serial.println(input[1]);
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
  Serial.println(input[1]);
}

void resume(){
  Serial.println("\nResume");
  Serial.print("ID: ");
  Serial.println(input[1]);
}

void kill(){
  Serial.println("\nKill");
  Serial.print("ID: ");
  Serial.println(input[1]);
}

bool cliSearchMatch(){
  // split the input string on the space char
  int j = 0;
  for(int i = 0; i < BUFSIZE; i++){
    if(cliBuffer[i] == ' '){
        inputIndex++;
        j = 0;
    }else{
        input[inputIndex][j] = cliBuffer[i];
        j++;
    }
  }
  
  bool matchFound = false;
  for(int i = 0; i < commandTypeSize; i++){
    if(strcmp(command[i].name, input[0]) == 0){
      void (*func)() = command[i].func;
      func();
      matchFound = true;
    }
  }
  return matchFound;
}

void clearBuffers(){
  // clear buffers
  for(int i = 0; i < BUFSIZE; i++){
    cliBuffer[i] = 0;
  }

  for(int i = 0; i < 2; i++){
    for(int j = 0; j < BUFSIZE; j++){
      input[i][j] = 0;
    }
  }
  
  cliBufferIndex = 0;
  inputIndex = 0;
}

void commandLine(){
    // wait for data to be available
  if (Serial.available() > 0) {
    // check for buffer overflow
    if(cliBufferIndex < BUFSIZE - 1){
        int incomingChar = Serial.read();
        if(incomingChar == 10 || incomingChar == 13){
          // check if buffer has values
          if(cliBufferIndex > 0){
            // find match for the command
            bool matchFound = cliSearchMatch();

            // show list in case of no match
            if(matchFound == false){
              Serial.print("Command '");
              Serial.print(cliBuffer);
              Serial.println("' is not a known command.");
              list();
            }

            clearBuffers();
            
            // give ready signal
            Serial.println("\nArduinOS 0.1 ready.\n==================");      
          }
        }else{
           cliBuffer[cliBufferIndex] = incomingChar; // store the incoming char
           cliBufferIndex++; // increment where to write next
        }
    }else{
      Serial.print("The maxium input is ");
      Serial.print(BUFSIZE);
      Serial.println(" chars long.");
      clearBuffers();
    }
  }
}