#define STACKSIZE 32
byte stack[1][STACKSIZE];

void pushByte(int processIndex, byte b) {
  stack[processIndex][ProcessTable[processIndex].sp++] = b;
}

byte popByte(int processIndex) {
  ProcessTable[processIndex].sp--;
  return stack[processIndex][ProcessTable[processIndex].sp];
}

// this function pushes a float to the stack
void pushFloat(int processIndex, float f) {
  byte b[4];
  float *pf = (float *)b;
  *pf = f;
  for(int i = 4; i > 0; i--){
    pushByte(processIndex, b[i]);
  }
}

// this function pops a float from the stack
float* popFloat(int processIndex) {
  byte b[4];
  for (int i = 0; i < 4; i++) {
    b[i] = popByte(processIndex);
  }
  float *pf = (float *)b;
  return pf;
}

// this function pushes an int to the stack
void pushInt(int processIndex, int i) {
  pushByte(processIndex, highByte(i));
  pushByte(processIndex, lowByte(i));
}

// this function pops a int from the stack
int popInt(int processIndex) {
  byte low = popByte(processIndex);
  byte high = popByte(processIndex);
  int result = word(high, low);
  return result;
}

// this function pushes a string to the stack
void pushString(int processIndex, char* s) {
  int i = 0;
  do {
    pushByte(processIndex, (byte)s[i]);
    i++;
  } while (s[i] != '\0');
  // push the terminating zero
  pushByte(processIndex, '\0');
  // push the size of the string
  pushByte(processIndex, (byte)i + 1);
  // push the type
  //pushByte(processIndex, (byte)'S');
}

// this function pops a string from the stack
char* popString(int processIndex) {
  // first pop the size
  int stringSize = (int)popByte(processIndex);
  char s[stringSize];
  for (int i = stringSize; i > 0; i--) {
    s[i] = (char)popByte(processIndex);
  }
  return s;
}
