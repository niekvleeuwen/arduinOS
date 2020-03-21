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
    Serial.println("Error. The state of this process is succesfully changed.");
  }
}

void startProcess(char name) {
  // check if the process table has enough space
  if (noOfProcesses >= PROCESS_TABLE_SIZE) {
    Serial.println("Error. No more space in process table.");
    return;
  }

  // check if the file to execute exists
  int pc = fileExists(name);
  if (pc == -1) {
    Serial.println("Error. File does not exists.");
    return;
  }

  Process p;
  strcpy(p.name, name);
  p.pid = noOfFiles;
  p.state = 'r';
  p.pc = pc;
  p.sp = 0;

  ProcessTable[noOfFiles] = p;
  noOfFiles++;

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

  Serial.println("\nProcess\t\tPID\tState\n======================");
  for (int i = 0; i < noOfProcesses; i++) {
    if (ProcessTable[i].state != 0) {
      Serial.print(ProcessTable[i].name);
      Serial.print("\t\t");
      Serial.print(ProcessTable[i].pid);
      Serial.print("\t");
      Serial.print(ProcessTable[i].state);
    }
  }
}
