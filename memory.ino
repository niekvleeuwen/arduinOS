#define STACKSIZE 32
byte stack[STACKSIZE];
byte sp = 0;

const int MEMORY_TABLE_SIZE = 10;
int noOfVars = 0;

struct MemoryEntry
{
    char name[12]; // name can be 12 chars long including terminating zero
    char type[5];
    int address;
    int size;
    int pid;
};

MemoryEntry MemoryTable[MEMORY_TABLE_SIZE];

void pushByte(byte b)
{
    stack[sp++] = b;
}

byte popByte()
{
    return stack[sp--];
}

void allocate(int pid)
{
}

void free(int pid)
{
}

void retrieve(int pid)
{
}

void clearProcess(int pid)
{
}