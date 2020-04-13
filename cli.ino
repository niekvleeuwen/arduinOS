/*
  Author: Niek van Leeuwen
  Date: 27-02-2020
*/

// this function looks for a match for the given command
bool cliSearchMatch() {
  bool matchFound = false;
  for (int i = 0; i < commandTypeSize; i++) {
    if (strcmp(command[i].name, cliBuffer[0]) == 0) {
      // call the matched function
      void (*func)() = command[i].func;
      func();
      matchFound = true;
    }
  }
  return matchFound;
}

// this function clears the cli buffers
void clearBuffers() {
  for (int i = 0; i < 4; i++) {
    memset(cliBuffer[i], NULL, BUFSIZE);
  }

  cliBufferCounter = 0;
  cliBufferCounterArguments = 0;
}

// main command line function
void commandLine() {
  // wait for data to be available
  if (Serial.available() > 0) {
    int incomingChar = Serial.read();

    // split the buffer on the space char
    if (incomingChar == ' ' && cliBufferCounter > 0) {
      cliBuffer[cliBufferCounterArguments][cliBufferCounter] = '\0'; // null terminate string
      cliBufferCounterArguments++;
      cliBufferCounter = 0;
    } else if (incomingChar == 13 || incomingChar == 10) {
      if (cliBufferCounter > 0) {
        // find match for the command
        bool matchFound = cliSearchMatch();

        // show list in case of no match
        if (matchFound == false) {
          Serial.print("Command '");
          Serial.print(cliBuffer[0]);
          Serial.println("' is not a known command.");
        }

        clearBuffers();

        // give ready signal
        Serial.println(F("\nArduinOS 0.1 ready.\n=================="));
      }
    } else {
      cliBuffer[cliBufferCounterArguments][cliBufferCounter] = incomingChar; // store the incoming char
      cliBufferCounter++; // increment where to write next
    }
  }
}
