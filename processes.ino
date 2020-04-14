/*
  Author: Niek van Leeuwen
  Date: 19-03-2020
*/
struct Process {
  char name[12];
  int pid;
  char state; // RUNNING(’r’),PAUSED(’p’),TERMINATED(0)
  int pc;
  int fp;
  int sp;
  int address;
};

const int PROCESS_TABLE_SIZE = 5;
Process ProcessTable[PROCESS_TABLE_SIZE];
int noOfProcesses = 0;

int pidExists(int pid) {
  // check if ID exists and the program is not Terminated
  int process = -1;
  for (int i = 0; i < noOfProcesses; i++) {
    if (ProcessTable[i].pid == pid) {
      // check if process is not terminated
      if (ProcessTable[i].state != 0) {
        process = i;
      }
    }
  }
  return process;
}

void changeProcessState(int processIndex, char state) {
  char currentState = ProcessTable[processIndex].state;
  if (currentState == state) {
    Serial.println("Error.");
  } else {
    ProcessTable[processIndex].state = state;
    Serial.println("Done. The state is changed.");
  }
}

void startProcess(char* fileName) {
  // check if the process table has enough space
  if (noOfProcesses >= PROCESS_TABLE_SIZE) {
    Serial.println("Error. No more space in process table.");
    return;
  }

  // check if the file to execute exists
  int address = fileExists(fileName);
  if (address == -1) {
    Serial.println("Error. File does not exists.");
    return;
  }

  Process p;
  strcpy(p.name, fileName);
  p.pid = noOfProcesses;
  p.state = 'r';
  p.pc = 0;
  p.sp = 0;
  p.address = address;

  ProcessTable[noOfProcesses] = p;
  noOfProcesses++;

  Serial.print("Program started with pid: ");
  Serial.println(p.pid);
}

void killProcess(int pid) {
  int processIndex = pidExists(pid);
  if (processIndex == -1) {
    Serial.println("Error. Invalid PID.");
    return;
  }
  // delete all variables from memory
  clearProcess(pid);
  // change the state to TERMINATED
  changeProcessState(processIndex, 0);
  noOfProcesses--;
}

void pauseProcess(int pid) {
  int processIndex = pidExists(pid);
  if (processIndex == -1) {
    Serial.println("Error. Invalid PID.");
    return;
  }
  // change the state to PAUSED
  changeProcessState(processIndex, 'p');
}


void resumeProcess(int pid) {
  int processIndex = pidExists(pid);
  if (processIndex == -1) {
    Serial.println("Error. Invalid PID.");
    return;
  }
  // change the state to RUNNING
  changeProcessState(processIndex, 'r');
}

void listProcesses() {
  Serial.print("Number of processes: ");
  Serial.println(noOfProcesses);

  Serial.println("\nProcess\t\tPID\tState\n=============================");
  for (int i = 0; i < noOfProcesses; i++) {
    if (ProcessTable[i].state != 0) {
      Serial.print(ProcessTable[i].name);
      Serial.print("\t\t");
      Serial.print(ProcessTable[i].pid);
      Serial.print("\t");
      Serial.println(ProcessTable[i].state);
    }
  }
}

// loop trough the process table and execute one instruction
void runProcesses() {
  for (int i = 0; i < noOfProcesses; i++) {
    if (ProcessTable[i].state == 'r') {
      Serial.print("\nProcess: ");
      Serial.println(ProcessTable[i].pid);
      execute(i);
    }
  }
}

// execute one line of a process
void execute(int i) {
  int address = ProcessTable[i].address;

  // for every read byte up the PC one
  Serial.print("PC: ");
  Serial.println(ProcessTable[i].pc);
  byte currentCommand = EEPROM.read(address + ProcessTable[i].pc);
  ProcessTable[i].pc++;
  Serial.print("CMD: ");
  Serial.println(currentCommand);
  switch (currentCommand) {
    case PRINT: {
        printStack(ProcessTable[i].pid, address);
        break;
      }
    case STOP: {
        Serial.print("\nProcess with pid: ");
        Serial.print(ProcessTable[i].pid);
        Serial.println(" is finished.");
        killProcess(ProcessTable[i].pid);
        break;
      }
    case STRING: {

        char string[12];
        int pointer = 0;
        do {
          string[pointer] = (char)EEPROM.read(address + ProcessTable[i].pc++);
          pointer++;
        } while (string[pointer - 1] != 0);
        pushString(ProcessTable[i].pid, string);
        break;
      }
    case CHAR: {
 
        break;
      }
    case INT: {

        int first = EEPROM.read(address + ProcessTable[i].pc++);
        int second = EEPROM.read(address + ProcessTable[i].pc++);
        int result = (first * 256) + second;
        pushInt(ProcessTable[i].pid, result);
        Serial.println(result);
        break;
      }
    case FLOAT: {

        break;
      }
    case SET: {
        char nameSet = (char)EEPROM.read(address + ProcessTable[i].pc++);
        Serial.println(nameSet);
        allocate(nameSet, ProcessTable[i].pid);
        break;
      }
    case GET: {
        char nameGet = (char)EEPROM.read(address + ProcessTable[i].pc++);
        Serial.println(nameGet);
        retrieve(nameGet, ProcessTable[i].pid);
        break;
      }
  }
}

void printStack(int pid, int address) {
  // check how many bytes we need to pop
  byte type = popByte(pid);
  switch (type) {
    case STRING:
      char* r = popString(pid);
      Serial.println(r);
      free(r);
      break;
    case INT:
      break;
    case CHAR:
      break;
    case FLOAT:
      break;
  }
}

// this is a function to put a program in the memory
void putProgramInMemory() {
  byte prog6[] = {INT, 0, LED_BUILTIN, SET, 'p',
                  GET, 'p', INT, 0, OUTPUT, PINMODE,
                  LOOP,
                  GET, 'p', INT, 0, HIGH, DIGITALWRITE,
                  MILLIS, INT, 1, 244, PLUS, // 500
                  DELAYUNTIL,
                  GET, 'p', INT, 0, LOW, DIGITALWRITE,
                  MILLIS, INT, 1, 244, PLUS, // 500
                  DELAYUNTIL,
                  ENDLOOP
                 };
  // fill the file info in a FAT entry
  FatEntry file = {};
  char* fileName = "blink";

  int address = (sizeof(FatEntry) * FATSIZE) + 1 + 16;
  strcpy(file.name, fileName);
  file.position = address;
  int fileSize = sizeof(prog6);
  file.length = fileSize;

  // write the FAT entry to the EEPROM
  FAT[1] = file;
  int fatAddress = sizeof(FatEntry) * 1;
  EEPROM.put(fatAddress, file);
  noOfFiles = 2;

  // write data to the EEPROM
  for (int i = 0; i < fileSize; i++) {
    Serial.print("\nWriting at address: ");
    Serial.println(address);
    Serial.print("Byte: ");
    Serial.println(prog6[i]);
    EEPROM.write(address, prog6[i]);
    address++;
  }

  Serial.print("Filename:\t");
  Serial.println(fileName);
  Serial.print("Size:\t\t");
  Serial.println(fileSize);
  Serial.println("Storing done.");

  Serial.println("Readback: ");
  // write data to the EEPROM
  for (int i = 161; i < 177; i++) {
    Serial.println(EEPROM.read(i));
  }
}
