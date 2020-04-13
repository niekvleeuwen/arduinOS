/*
  Author: Niek van Leeuwen
  Date: 15-03-2020
*/
const int MEMORY_SIZE = 256;
const int MEMORY_TABLE_SIZE = 10;
int noOfVars = 0;

struct MemoryEntry {
  byte name;
  char type;
  int address;
  int size;
  int pid;
};

MemoryEntry MemoryTable[MEMORY_TABLE_SIZE];
byte Memory[MEMORY_SIZE];

// find a value in the Memory Table and return it address (and -1 if not found)
int valueExists(byte name, int pid) {
  MemoryEntry tmp;
  for (int i = 0; i < noOfVars; i++) {
    if (MemoryTable[i].name == name) {
      return i;
    }
  }
  return -1;
}

// sort memory table
void sortMemory() {
  bool sorted = false;
  while (sorted == false) {
    sorted = true;
    for (int i = 0; i < noOfVars - 1; i++) {
      if (MemoryTable[i].address > MemoryTable[i + 1].address) {
        MemoryEntry tmp = MemoryTable[i];
        MemoryTable[i] = MemoryTable[i + 1];
        MemoryTable[i + 1] = tmp;
        sorted = false;
      }
    }
  }
}

// allocate a value from the stack to the memory
void allocate(byte name, int pid) {
  // check first for space in the memory table
  if (noOfVars >= MEMORY_TABLE_SIZE) {
    Serial.println("Error. No more space in memory table.");
    return;
  }

  // check if value exists in the memory table
  if (valueExists(name, pid) == -1) {
    // delete value
    free(name, pid);
  }

  // check how many bytes we need to pop
  byte type = popByte(pid);
  int valueSize = 0;
  if(type == STRING){
    valueSize = (int)popByte(pid); // pop the size from the stack
  }else{
    valueSize = (int)type;
  }

  // pop the amount of bytes needed
  byte popped[valueSize];
  for (int i = 0; i < valueSize; i++) {
    popped[i] = popByte(pid);
  }

  // search for a spot in the memory
  sortMemory();
  // check in between the blocks
  int address = -1;
  for (int i = 0; i < noOfVars - 1; i++) {
    if (valueSize < (MemoryTable[i + 1].address - (MemoryTable[i].address + MemoryTable[i].size))) {
      address = MemoryTable[i].address + MemoryTable[i].size;
    }
  }

  // check for room after the last block
  if (address == -1) {
    address = MemoryTable[noOfVars - 1].address + MemoryTable[noOfVars - 1].size;
    if ((MEMORY_SIZE - address) < valueSize) {
      Serial.println("Error. No more space in memory.");
      return;
    }
  }

  // store the entry in the memory table
  MemoryEntry entry;
  entry.name = name;
  entry.type = type;
  entry.address = address;
  entry.size = valueSize;
  entry.pid = pid;
  MemoryTable[noOfVars] = entry;

  // store the value in the memory
  int j = 0;
  for (int i = address; i < (address + valueSize); i++) {
    Memory[i] = popped[j];
    j++;
  }
  noOfVars++;
}

// free a variable
void free(byte name, int pid) {
  // check if value exists in the memory table
  int memoryTableAddress = valueExists(name, pid);
  if (memoryTableAddress != -1) {
    // move elements a spot up
    for (int i = memoryTableAddress; i < noOfVars - 1; ++i) {
      MemoryTable[i] = MemoryTable[i + 1]; // copy next element left
    }
    noOfVars--;
  } else {
    Serial.println("Error.");
  }
}

// retrieve a variable from the memory
void retrieve(byte name, int pid) {
  // check if value exists in the memory table
  int memoryTableAddress = valueExists(name, pid);
  if (MemoryTable[memoryTableAddress].address == -1) {
    Serial.println("Error.");
    return NULL;
  }

  // push the variable to the stack
  for (int i = MemoryTable[memoryTableAddress].address; i < (MemoryTable[memoryTableAddress].address + MemoryTable[memoryTableAddress].size); i++) {
    pushByte(pid, Memory[i]);
  }

  // push the type to the stack
  byte type = MemoryTable[memoryTableAddress].type;
}

// remove all the variables for a process
void clearProcess(int pid) {
  // remove all the data with the given pid
  MemoryEntry tmp = {};
  for (int i = 0; i < noOfVars; i++) {
    if (MemoryTable[i].pid == pid) {
      // clear the value from the memory
      free(MemoryTable[i].name, pid);
      //remove the memory entry
      MemoryTable[i] = tmp;
    }
  }
  Serial.print("Memory cleared of process: ");
  Serial.println(pid);
}
