struct Process{
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

void startProcess(char name){
  // check if the process table has enough space
  if(noOfProcesses >= PROCESS_TABLE_SIZE){
    Serial.println("Error. No more space in process table.");
    return;
  }

  // check if the file to execute exists
  int pc = fileExists(name);
  if(pc == -1){
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


void killProcess(int pid){

}


void pauseProcess(int pid){
  
}


void resumeProcess(int pid){
  
}
