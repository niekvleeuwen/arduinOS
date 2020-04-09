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

const int PROCESS_TABLE_SIZE = 10;
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
    Serial.println("Error. This process has this state already.");
  } else {
    ProcessTable[processIndex].state = state;
    Serial.println("Done. The state of this process is succesfully changed.");
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
    Serial.println("Error. No program found with this PID.");
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
    Serial.println("Error. No program found with this PID.");
    return;
  }
  // change the state to PAUSED
  changeProcessState(processIndex, 'p');
}


void resumeProcess(int pid) {
  int processIndex = pidExists(pid);
  if (processIndex == -1) {
    Serial.println("Error. No program found with this PID.");
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
      Serial.print("\nExecuting: ");
      Serial.println(ProcessTable[i].pid);
      execute(i);
    }
  }
}

// execute one line of a process
void execute(int i) {
  int address = ProcessTable[i].address;

  // for every read byte up the PC one
  Serial.print("Current PC: ");
  Serial.println(ProcessTable[i].pc);
  byte currentCommand = EEPROM.read(address + ProcessTable[i].pc);
  ProcessTable[i].pc++;
  Serial.print("currentCommand: ");
  Serial.println(currentCommand);
  switch (currentCommand) {
    case PRINT:
      Serial.println("PRINT");

      break;
    case STOP:
      Serial.print("\nProcess with pid: ");
      Serial.print(ProcessTable[i].pid);
      Serial.println(" is finished.");
      killProcess(ProcessTable[i].pid);
      break;
    case STRING:
      Serial.print("STRING: ");
      char string[12];
      int pointer = 0;
      do {
        string[pointer] = (char)EEPROM.read(address + ProcessTable[i].pc++);
        pointer++;
      } while (string[pointer - 1] != 0);
      Serial.println(string);
      pushString(ProcessTable[i].pid, string);
      break;
  }
}

// this is a function to put a Program in the memory
void putProgramInMemory() {
  // program hello
  byte prog1[] = {STRING, 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\n', 0,
                  PRINT,
                  STOP
                 };
  // fill the file info in a FAT entry
  FatEntry file = {};
  char* fileName = "prog1";

  int address = (sizeof(FatEntry) * FATSIZE) + 1;
  strcpy(file.name, fileName);
  file.position = address;
  int fileSize = sizeof(prog1);
  file.length = fileSize;

  // write the FAT entry to the EEPROM
  FAT[0] = file;
  int fatAddress = sizeof(FatEntry) * 0;
  EEPROM.put(fatAddress, file);
  noOfFiles++;

  // write data to the EEPROM
  for (int i = 0; i < fileSize; i++) {
    Serial.print("\nWriting at address: ");
    Serial.println(address);
    Serial.print("Byte: ");
    Serial.println(prog1[i]);
    EEPROM.write(address, prog1[i]);
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
