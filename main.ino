int incomingChar = 0; // for incoming serial data
const int BUFSIZE = 12;

/* CLI */
void commandLine();
// Array to store the input from the CLI
static char cliBuffer[BUFSIZE];
int cliBufferIndex = 0;

// list of all commands
void store();
void retrieve();
void help();

typedef struct {
  char name[BUFSIZE];
  void * func;
} commandType;

static commandType command [] = {
  {"store" , &store },
  {"retrieve" , &retrieve },
  {"help" , &help },
};

static int commandTypeSize = sizeof(command) / sizeof(commandType);

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  commandLine();
}

void processCommand(int incomingByte){
   Serial.println("\nprocessCommand..");
}

void retrieve(){
  Serial.println("\nRetrieve..");
}

void store(){
  Serial.println("\nStore..");
}

void help(){
  Serial.println("\nAvailable commands");
  Serial.println("==================");
  for(int i = 0; i < commandTypeSize; i++){
      Serial.println(command[i].name);
  }
  Serial.print("\n");
}

void commandLine(){
    // wait for data to be available
  if (Serial.available() > 0) {
    // check for buffer overflow
    if(cliBufferIndex < BUFSIZE - 1){
        incomingChar = Serial.read();

        // Check for enter
        if(incomingChar == 10 || incomingChar == 13){
          // check if buffer has values
          if(cliBufferIndex > 0){
            // find match for the command
            bool matchFound = false;
            for(int i = 0; i < commandTypeSize; i++){
              if(strcmp(command[i].name, cliBuffer) == 0){
                void (*func)() = command[i].func;
                func();
                matchFound = true;
              }
            }

            // show help in case of no match
            if(matchFound == false){
              Serial.print("Command '");
              Serial.print(cliBuffer);
              Serial.println("' is not a know command.");
              help();
            }
            
            // give ready signal
            Serial.println("ArduinOS 0.1 ready.\n==================");
          }
          
          // clear buffer
          for(int i = 0; i < BUFSIZE; i++){
            //Serial.println(cliBuffer[i], DEC);
            cliBuffer[i] = 0;
          }
          cliBufferIndex = 0;
        }else{
           cliBuffer[cliBufferIndex] = incomingChar; // Store it
           cliBufferIndex++; // Increment where to write next
           //cliBuffer[cliBufferIndex] = '\0'; // Null terminate the string
        }
       
    }else{
      Serial.println("The maxium input is 12 chars long.");
    }
  }
}
