/*
  Author: Niek van Leeuwen
  Date: 04-03-2020
*/

#include <EEPROM.h>

const int FATSIZE = 10;
EERef noOfFiles = EEPROM[160];

struct FatEntry {
  char name[12]; // name can be 12 chars long including terminating zero
  int position;
  int length;
};

FatEntry FAT[FATSIZE];

// read FAT entry from the EEPROM
struct FatEntry readFATEntry(int address) {
  FatEntry file;
  EEPROM.get(address, file);
  // uncomment these prints to see the print FAT entries when loading FAT from EEPROm
  //  Serial.print("\nRead FAT entry from EEPROM at address: ");
  //  Serial.println(address);
  //  Serial.println(file.name);
  //  Serial.println(file.position);
  //  Serial.println(file.length);
  return file;
}

// load the FAT table in the memory
void loadFAT() {
  int address = 0;
  for (int i = 0; i < FATSIZE; i++) {
    FAT[i] = readFATEntry(address);
    address += sizeof(FatEntry);
  }
}

// write the FAT table from the memory to the EEPROM
void writeFAT() {
  int address = 0;
  for (int i = 0; i < FATSIZE; i++) {
    EEPROM.put(address, FAT[i]);
    address += sizeof(FatEntry);
  }
}

// sort FAT table
void sort() {
  bool sorted = false;
  while (sorted == false) {
    sorted = true;
    for (int i = 0; i < noOfFiles - 1; i++) {
      if (FAT[i].position > FAT[i + 1].position) {
        FatEntry tmp = FAT[i];
        FAT[i] = FAT[i + 1];
        FAT[i + 1] = tmp;
        sorted = false;
      }
    }
  }
  // write updated FAT table to the EEPROM
  writeFAT();
}

// find a file in the FAT and returns it address (and -1 if not found)
int fileExists(char* fileName) {
  FatEntry tmp;
  for (int i = 0; i < noOfFiles; i++) {
    if (strcmp(FAT[i].name, fileName) == 0) {
      return FAT[i].position;
    }
  }
  return -1;
}

// write file to the filesystem
void writeFile(char* fileName, int fileSize) {
  // verify the fat has an entry left
  if (noOfFiles >= FATSIZE) {
    Serial.println("Error. No more space left in FAT");
    return;
  }

  // verify that no file has the same name
  if (fileExists(fileName) != -1) {
    Serial.println("Error. There is already a file with this name");
    return;
  }

  // verify that enough space is available
  sort();
  int address = -1;

  // check first for space between the start of the usable memory and the first block
  int systemMemory = (sizeof(FatEntry) * FATSIZE) + 1;
  if ((FAT[0].position - systemMemory) >= fileSize) {
    address = systemMemory;
  } else {
    // check in between the blocks
    for (int i = 0; i < noOfFiles - 1; i++) {
      if (fileSize < (FAT[i + 1].position - (FAT[i].position + FAT[i].length))) {
        address = FAT[i].position + FAT[i].length;
      }
    }
  }

  if (address == -1) {
    // check if there are files at all
    if (noOfFiles > 0) {
      // check for room after the last block
      if ((EEPROM.length() - (FAT[noOfFiles - 1].position + FAT[noOfFiles - 1].length)) < fileSize) {
        Serial.println("Error. No more space left on disk");
        return;
      } else {
        address = FAT[noOfFiles - 1].position + FAT[noOfFiles - 1].length;
      }
    } else {
      // the start address of the memory
      address = systemMemory;
    }
  }

  // fill the file info in a FAT entry
  FatEntry file = {};
  strcpy(file.name, fileName);
  file.position = address;
  file.length = fileSize;

  // write the FAT entry to the EEPROM
  FAT[noOfFiles] = file;
  int fatAddress = sizeof(FatEntry) * noOfFiles;
  EEPROM.put(fatAddress, file);
  noOfFiles++;

  // write data to the EEPROM
  fileSize++;
  byte buffer[fileSize];
  while(Serial.available() == 0){}
  Serial.readBytes(buffer, fileSize);
  delay(100);
  // clear the serial buffer
  while(Serial.available()){ Serial.read();}
  
  for(int i = 1; i < fileSize; i++){
    EEPROM.write(address, buffer[i]);
    address++;
  }

  Serial.print("Filename:\t");
  Serial.println(fileName);
  Serial.print("Size:\t\t");
  Serial.println(fileSize);
  Serial.println("Storing done.");
}

// read file from the filesystem
void readFile(char* fileName) {
  int address = fileExists(fileName);
  if (address == -1) {
    Serial.println("Error. This file does not exists");
    return;
  }

  // retrieve the file
  int fatNumber = 0;
  for(int i = 0; i < noOfFiles; i++){
    if(FAT[i].position == address){
      fatNumber = i;
    }
  }

  Serial.print("\nContent: ");
  for(int i = 0; i < FAT[fatNumber].length; i++){
    Serial.print((char)EEPROM.read(address));
    address++;
  }
  Serial.print("\n");
}

// erase file from the filesystem
void eraseFile(char* fileName) {
  // verify that the file exist
  if (fileExists(fileName) == -1) {
    Serial.println("Error. There is no file with this name");
    return;
  }

  // remove fat entry and sort the FAT table
  int fatNumber;
  for (int i = 0; i < noOfFiles; i++) {
    if (strcmp(FAT[i].name, fileName) == 0) {
      fatNumber = i;
      break;
    }
  }

  // overide the old element
  for (int i = fatNumber; i < noOfFiles - 1; ++i) {
    FAT[i] = FAT[i + 1]; // copy next element left
  }
  FatEntry file = {0, 0, 0};
  FAT[noOfFiles - 1] = file;

  noOfFiles--;
  sort();
}

// print list of all the files with size
void listFiles() {
  Serial.print("Number of files: ");
  Serial.println(noOfFiles);
  loadFAT();

  Serial.println("\nFiles\t\tLength\n======================");
  for (int i = 0; i < noOfFiles; i++) {
    Serial.print(FAT[i].name);
    Serial.print("\t\t");
    Serial.println(FAT[i].length);
  }
}

// print free space left on the file system
void freeSpace() {
  sort();
  int systemMemory = (sizeof(FatEntry) * FATSIZE) + 1; // the space the FAT table takes up
  int totalSpaceAvailable = 0;

  // add the space between the start of the usable memory and the first file;
  totalSpaceAvailable += (FAT[0].position - systemMemory);

  // add the space between the blocks
  for (int i = 0; i < noOfFiles - 1; i++) {
    totalSpaceAvailable += (FAT[i + 1].position - (FAT[i].position + FAT[i].length));
  }

  // add the space between the last file and the end of the filesystem
  totalSpaceAvailable += EEPROM.length() - (FAT[noOfFiles - 1].position + FAT[noOfFiles - 1].length);

  Serial.print("Total space avaiable: ");
  Serial.println(totalSpaceAvailable);
}

// DEBUGGING FUNCTION - clears the entire FAT table
void clearFAT() {
  int address = 0;
  char name = {0};
  for (int i = 0; i < FATSIZE; i++) {
    FatEntry file = {};
    strcpy(file.name, name);
    file.position = 0;
    file.length = 0;
    EEPROM.put(address, file);
    address += sizeof(FatEntry);
  }
  noOfFiles = 0;
}
