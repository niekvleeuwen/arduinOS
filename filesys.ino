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

struct FileData {
  char data[255];
};

FatEntry FAT[FATSIZE];

// write FAT entries to the EEPROM
void writeFATEntry(char* name, int position, int length, int address) {
  FatEntry file = {};
  strcpy(file.name, name);
  file.position = position;
  file.length = length;
  EEPROM.put(address, file);
}

// read file from the EEPROM
struct FatEntry readFATEntry(int address) {
  FatEntry file;
  EEPROM.get(address, file);
  Serial.print("\nRead FAT entry from EEPROM at address: ");
  Serial.println(address);
  Serial.println(file.name);
  Serial.println(file.position);
  Serial.println(file.length);
  return file;
}

// this functions loads the FAT table in the memory
void loadFAT() {
  int address = 0;
  for (int i = 0; i < FATSIZE; i++) {
    FAT[i] = readFATEntry(address);
    address += sizeof(FatEntry);
  }
}

// sort FAT
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

  // write new order to the memory
  int address = 0;
  for (int i = 0; i < FATSIZE; i++) {
    EEPROM.put(address, FAT[i]);
    address += sizeof(FatEntry);
  }
}

// finds a file in the FAT and returns it address (and -1 if not found)
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
void writeFile(char* fileName, int fileSize, char* data) {
  Serial.print("file: ");
  Serial.println(fileName);
  Serial.print("Size: ");
  Serial.println(fileSize);
  Serial.print("Data: ");
  Serial.println(data);

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
  //sort();
  int address = -1;
  // check in between the blocks
  for (int i = 0; i < noOfFiles - 1; i++) {
    if (fileSize < (FAT[i + 1].position - (FAT[i].position + FAT[i].length))) {
      address = FAT[i].position + FAT[i].length + 1;
    }
  }
  if (address == -1) {
    // check for room after the last block
    if ((EEPROM.length() - (FAT[noOfFiles - 1].position + FAT[noOfFiles - 1].length)) < fileSize) {
      Serial.println("Error. No more space left on disk");
      return;
    } else {
      address = FAT[noOfFiles - 1].position + FAT[noOfFiles - 1].length + 1;
    }
  }
  Serial.print("Address: ");
  Serial.println(address);

  FatEntry file = {};
  strcpy(file.name, fileName);
  file.position = address;
  file.length = fileSize;

  FAT[noOfFiles] = file;
  Serial.print("Address to put FAT: ");
  int fatAddress = sizeof(FatEntry) * noOfFiles;
  Serial.println(fatAddress);
  EEPROM.put(fatAddress, file);
  noOfFiles++;

  FileData dat = {};
  strcpy(dat.data, data);
  EEPROM.put(address, dat);
}

// read file from the filesystem
void readFile(char* fileName) {
  int address = fileExists(fileName);
  if (address == -1) {
    Serial.println("Error. This file does not exists");
    return;
  }

  FileData dat;
  EEPROM.get(address, dat);
  Serial.println(dat.data);
}

// erase file from the filesystem
void eraseFile(char* fileName) {
  // verify that the file exist
  int address = fileExists(fileName);
  if (address == -1) {
    Serial.println("Error. There is no file with this name");
    return;
  }

  int fatNumber;
  for (int i = 0; i < noOfFiles; i++) {
    if (strcmp(FAT[i].position, address) == 0) {
      fatNumber = i;
      Serial.print(i);
      break;
    }
  }

  // override old values

}

// print list of all the files with size
void listFiles() {
  Serial.print("Number of files: ");
  Serial.println(noOfFiles);
  loadFAT();

  Serial.println("\nFiles\n==================");
  for (int i = 0; i < noOfFiles; i++) {
    Serial.print(FAT[i].name);
    Serial.print("\t\t");
    Serial.println(FAT[i].length);
  }
}

// print free space left on the file system
void freeSpace() {
  sort();

  // calculate the difference end of file system and the start of the next file

}
