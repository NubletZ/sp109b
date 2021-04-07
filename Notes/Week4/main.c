#include "compiler.h"

int readText(char *fileName, char *text, int size) {
  FILE *file = fopen(fileName, "r"); //this file will open the if.c
  int len = fread(text, 1, size, file); // here text is the code, this line will read if.c and put the content in code;
  //printf("TEXT = ");
  //puts(text);
  //printf("len = %d", len);
  text[len] = '\0'; 
  fclose(file);
  return len;
}

void dump(char *strTable[], int top) {
  printf("========== dump ==============\n");
  for (int i=0; i<top; i++) {
    printf("%d:%s\n", i, strTable[i]);
  }
}

int main(int argc, char * argv[]) {
  readText(argv[1], code, TMAX); //argv[1] contain the filename (e.x. if.c)
  puts(code); //print out code in the file
  lex(code); //execute program in lexer.c
             //lexer will count the total token and put the value in tokenTop
             //it also will split the program into token and put it in array tokens
  dump(tokens, tokenTop);
  parse(); //run the parse program in compiler.c
  return 0;
}