#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdio>

void dump_file(char *fileName, char format);
std::string charToBin(unsigned char c);
void printChar(unsigned char c);
void printHex(unsigned char c);
void printOct(unsigned char c);
void printBin(unsigned char c);

int main(int argc, char *argv[]) {

  if (argc == 1) {
    printf("\ndump_file.out <fileName> <format>");
    printf("\nformat:");
    printf("\n0) Bin");
    printf("\n1) Oct");
    printf("\n2) Hex");
    printf("\n3) Char");
    printf("\n\n");
    return 0;
  }

  dump_file(argv[1], argv[2][0]);
}


void dump_file(char *fileName, char format) {

  std::ifstream inFile(fileName, std::ios::in);
  if (inFile.fail()) {
    std::cout << "\ndump_file(): file " << fileName << " doesn't exist.\n";
    return;
  }

  inFile.seekg(0, std::ios_base::end);
  int fileSize = inFile.tellg();
  inFile.seekg(0);

  char c;
  for (int i = 0; i < fileSize; i++) {

    if (i % 10 == 0)
      printf("\n%04d", i);

    inFile.get(c);
    switch (format) {

    case '0':
      printBin(c);
      break;

    case '1':
      printOct(c);
      break;

    case '2':
      printHex(c);
      break;

    case '3':
      printChar(c);
      break;
    }
  }
  
  printf("\n\n");

  inFile.close();
}


std::string charToBin(unsigned char c) {
  unsigned char mask = 0x80;
  char binArr[9];
  for (int i = 0; i < 8; i++) {
    binArr[i] = (c & mask) ? '1' : '0';
    mask = mask >> 1;
  }
  binArr[8] = '\0';
  std::string result(binArr);
  return result;
}


void printChar(unsigned char c) {
  switch (c) {

  case ' ':
    printf("   _");
    break;

  case '\n':
    printf("  \\n");
    break;

  default:
    printf("%4c", c);
  }
}


void printHex(unsigned char c) {
  switch(c) {
    
  case ' ':
    printf("  __");
    break;
    
  case '\n':
    printf("  \\n");
    break;

  default:
    printf("  %02X", (int)c);
  }
}


void printOct(unsigned char c) {
  switch(c) {
    
  case ' ':
    printf("  ___");
    break;

  case '\n':
    printf("  \\n ");
    break;
    
  default:
    printf("  %03o", (int)c);
  }
}


void printBin(unsigned char c) {
  switch(c) {
    
  case ' ':
    printf("  ________");
    break;

  case '\n':
    printf("  ___\\n___");
    break;
    
  default:
    printf("%10s", charToBin(c).c_str());
  }
}
