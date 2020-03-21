/*
  Author: Niek van Leeuwen
  Date: 15-03-2020
*/
#define STACKSIZE 32
byte stack[STACKSIZE];
byte sp = 0;

const int MEMORY_SIZE = 256;
const int MEMORY_TABLE_SIZE = 25;
int noOfVars = 0;

struct MemoryEntry {
  char name;
  char type[5];
  int address;
  int size;
  int pid;
};

MemoryEntry MemoryTable[MEMORY_TABLE_SIZE];
byte Memory[MEMORY_SIZE];

void pushByte(byte b) {
  stack[sp++] = b;
}

byte popByte() {
  return stack[sp--];
}

// find a value in the Memory Table and return it address (and -1 if not found)
int valueExists(byte name, int pid) {
  MemoryEntry tmp;
  for (int i = 0; i < noOfVars; i++) {
    if (MemoryTable[i].name == name) {
      return MemoryTable[i].address;
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

  // check how many bytes we need
  // pop stuff
  int valueSize = 0;
  int value = 0;

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
  //entry.type = IDK; // char
  entry.address = address;
  //entry.size = IDK;
  entry.pid = pid;

  MemoryTable[noOfVars] = entry;

  // store the value in the memory
  for (int i = 0; i < valueSize; i++) {
    Memory[address] = value;
  }

  noOfVars++;
}

void free(byte name, int pid) {
  // check if value exists in the memory table
  if (valueExists(name, pid) == -1) {
    // delete value

  }
  noOfVars--;
}

byte retrieve(byte name, int pid) {
  // check if value exists in the memory table
  int address = valueExists(name, pid);
  if (address != -1) {
    // retrieve value
    return address;
  } else {
    Serial.println("Ërror. Value not found.");
    return NULL;
  }
}

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
