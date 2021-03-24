# Week 2 Notes
## 4 important things that would be learnt in this class :
1. Compiler : compiler is used for programs that translate code from high-level programming language to a lower level language so it become an executable program.
2. Assembler : a program that would translate a low -level language into machine language
3. Virtual Machine : is an emulation that allow us to run an operating system that behaves like a completely separate computer.
4. Operating System (OS) : is a collection of software that manages computer hardware, software resources, and provides services for computer programs.

## Syntax
The term syntax come from Greek, means "arrange together". It refers to the arrangement of words in senteces, clauses, and phrases.

### Syntax Terminology
* A lexeme is a sequence of characters that matching the pattern of a token (e.x. *, else, if, sum), later it would be converted into token by lexical analyzer (also known as lexer).
* A token is a sequence of characters which represents a unit of information in the source program.

### Role of the Parser
<image src="Parser.png" width="700" title="Parser" alt="Parser">

1. first the parser will send request for token to the lexer.
2. The lexer then will scan input until it finds the next token and returns it into parser.

Lexer will skips whitespaces and comments. If it detect any error, it will correlate that error with the source file and line number.

### Formal Methods of Describing Syntax

1. Grammars
2. Parse Trees
3. Syntax Diagrams

### Context Free Grammar BNF and EBNF
1. BNF (Backus-Naur Form) : Backus-Naur is a context-free grammar that commonly used by programming language developer to specify the syntax rules of a language.
```
e.x.

for a single digit
<digit> ::= 0|1|2|3|4|5|6|7|8|9
the <digit> is LHS (left hand side) while 0|1|2|... is RHS (right hand side)
this BNF defines that the entity named digit can transform into 0 or 1 or 2 and so on.

Below is a simple example of using BNF to represent 780
<natural> ::= <digit>
        | <nonzero> <digits>
<digits> ::= <digit>
           | <digit> <digits>
<digit> ::= 0 | <nonzero>
<nonzero> ::= 1|2|3|4|5|6|7|8|9

<natural> ::= <nonzero> <digits>
<natural> ::=     7     <digit> <digits>
<natural> ::=     7     <nonzero> <digit>
<natural> ::=     7         8        0
```
2. EBNF (Extended Backus-Naur Form) : is a few simple extensions to BNF that make expressing grammars more convenient. 
* "*" : means something can be repeated any number of times (and possibly be skipped altogether)
* "+" : means that something can appear one or more times
* "?" : means that the symbol to the left of the operator is optional

Below is the difference between BNF and EBNF.
```
BNF
<expr> ::= <expr> + <term>
        |  <expr> - <term>
        |  <term>
<term> ::= <term> * <factor>
        |  <term> / <factor>
        |  <factor>

EBNF
<expr> ::= <term> {(+|-)<term>}
<term> ::= <factor> {(*|/) <factor>}
```
```
BNF
<signed int> ::= + <int> | - <int>
<int> ::= <digit> | <int> <digit>

EBNF
<signed int> ::= [+|-] <digit> {<digit>}*
```