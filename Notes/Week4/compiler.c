#include <assert.h>
#include "compiler.h"

int E();
void STMT();
void IF();
void BLOCK();

int tempIdx = 0, labelIdx = 0;

#define nextTemp() (tempIdx++)
#define nextLabel() (labelIdx++)
#define emit printf

int isNext(char *set) {
  char eset[SMAX], etoken[SMAX]; // SMAX = 100000, defined in compiler.h
  sprintf(eset, " %s ", set); //put set into eset
  sprintf(etoken, " %s ", tokens[tokenIdx]); //put tokens[tokenIdx] into etoken
  return (tokenIdx < tokenTop && strstr(eset, etoken) != NULL); //strstr(eset, etoken) will try to find whether eset contain etoken or not
}

int isEnd() {
  return tokenIdx >= tokenTop;
}

char *next() {
  // printf("token[%d]=%s\n", tokenIdx, tokens[tokenIdx]);
  return tokens[tokenIdx++];
}

char *skip(char *set) {
  if (isNext(set)) {
    return next();
  } else {
    printf("skip(%s) got %s fail!\n", set, next());
    assert(0);
  }
}

// F = (E) | Number | Id
int F() {
  int f;
  //emit("this is F function\n");
  if (isNext("(")) { // '(' E ')'
    next(); // (
    f = E();
    next(); // )
  }
  else { // Number | Id | Increment | Decrement
    f = nextTemp(); //tempIdx++
    char *item = next();
    if (isNext("++")) {
      next();
      emit("%s = %s + 1\n", item, item);
    }
    else if (isNext("--")) {
      next();
      emit("%s = %s - 1\n", item, item);
    }
    else emit("t%d = %s\n", f, item);
  }
  return f;
}

// E = F (op E)*
int E() {
  int i1 = F();
  while (isNext("+ - * / & | ! < > = <= >= == !=")) {
    char *op = next();
    int i2 = E();
    int i = nextTemp();
    emit("t%d = t%d %s t%d\n", i, i1, op, i2);
    i1 = i;
  }
  return i1;
}

// ASSIGN = id '=' E;
void ASSIGN() {
  char *id = next();
  skip("=");
  int e = E();
  skip(";");
  emit("%s = t%d\n", id, e);
}

// WHILE = while (E) STMT
void WHILE() {
  int whileBegin = nextLabel(); //labelIdx++
  int whileEnd = nextLabel(); //labelIdx++
  emit("(L%d)\n", whileBegin);
  skip("while");
  skip("(");
  int e = E();
  emit("if not t%d goto L%d\n", e, whileEnd);
  F();
  skip(")");
  STMT();
  emit("goto L%d\n", whileBegin);
  emit("(L%d)\n", whileEnd);
}

// if (EXP) STMT (else STMT)?
int ifEnd = 2; // to define only one gate to go to the last line of code
void IF() {
  if (labelIdx == 0){ // only will print "(L0)" at the first line of parse
    int ifBegin = nextLabel(); //labelIdx++
    emit("(L%d)\n",ifBegin);
  }
  int ifMid = nextLabel(); //labelIdx++
  if (labelIdx <= 2){ //to skip 2 from ifMid, because it's already been used by ifEnd
    nextLabel(); //labelIdx++
  }
  skip("if");
  skip("(");
  int e= E();
  emit("[mid] if not t%d goto L%d\n",e,ifMid);
  skip(")");
  STMT();
  emit("[end] goto L%d\n",ifEnd);
  emit("[mid] (L%d)\n", ifMid);
  if (isNext("else")) {
    skip("else");
    //emit("if L%d goto L%d\n",ifMid,ifEnd);
    STMT();
  }
  if(ifEnd == 2){
    emit("[end](L%d)",ifEnd);
    ifEnd = 0;
  }
}

void FOR() {
  int forBegin = nextLabel(); // mark label for the loop start point
  int forEnd = nextLabel(); // mark label for the end of loop
  skip("for");
  skip("(");
  ASSIGN(); // read for the initialization statement
  emit("(L%d)\n", forBegin);
  int e = E(); // read for the test expression
  emit("if not t%d, goto L%d\n", e, forEnd);
  skip(";");
  F(); // read for the update statement
  skip(")");
  STMT(); // read the content in curl bracket
  emit("goto L%d\n", forBegin);
  emit("(L%d)\n", forEnd);
}

// STMT = WHILE | BLOCK | ASSIGN
// STMT stand for statment
void STMT() {
  if (isNext("while"))
    return WHILE();
  else if (isNext("if"))
    IF();
  else if (isNext("for"))
    FOR();
  else if (isNext("{"))
    BLOCK();
  else
    ASSIGN();
}

// STMTS = STMT*
void STMTS() {
  while (!isEnd() && !isNext("}")) {
    STMT();
  }
}

// BLOCK = { STMTS }
void BLOCK() {
  skip("{");
  STMTS();
  skip("}");
}

// PROG = STMTS
void PROG() {
  STMTS();
}

void parse() {
  printf("============ parse =============\n");
  tokenIdx = 0;
  PROG();
}