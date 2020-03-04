/*
  Author: Niek van Leeuwen
  Date: 04-03-2020
*/

#include <EEPROM.h>

const int FATSIZE = 10;
EERef noOfFiles = EEPROM[160];
int EEPROMAddress = 0;

typedef struct {
  char name[12]; // name can be 12 chars long including terminating zero
  int position;
  int length;
} FatEntry;

FatEntry FAT[FATSIZE];
int FATCounter = 0;

// write FAT entry to the EEPROM
void writeFATEntry(char* name, int position, int length) {
  FatEntry file = {
    name,
    position,
    length
  };
  EEPROM.put(EEPROMAddress, file);
}

// read file to the EEPROM
void readFATEntry(int address) {
  FatEntry file;
  EEPROM.get(address, file);
  
  Serial.println("Read custom object from EEPROM: ");
  Serial.println(file.name);
  Serial.println(file.position);
  Serial.println(file.length);
}

// check if a file exists
bool fileExists(char* filename) {

}

// sort FAT
void sort() {

}

// finds a file in the FAT and returns it address (and -1 if not found)
int findFile(char* fileName){
  FatEntry tmp;
  for(int i = 0; i < noOfFiles; i++){
    if(strcmp(FAT[i].name, fileName)){
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
}

// read file from the filesystem
void readFile(char* fileName) {
  int address = findFile(fileName);
  Serial.print("Address: ");
  Serial.println(address);  
}

// erase file from the filesystem
void eraseFile(char* fileName) {

}

// print list of all the files with size
void listFiles() {
  Serial.print("Number of files: ");
  Serial.println(noOfFiles);
}

// print free space left on the file system
void freeSpace() {
  sort();

  // calculate the difference end of file system and the start of the next file
}
