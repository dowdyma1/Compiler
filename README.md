# Introduction
This project is a simulated compiler for the PL/0 programming language. It was written throughout the Fall 2020 semester for my Systems Software class in college.
This compiler is a top down recursive descent compiler. 

## How this PL/0 Compiler works
The **lexical analyzer** takes in an input file and groups characters into tokens, which are easier to use later in the compiler. ('i' 'f' -> 'if' aka 23)  
The **driver** then passes the tokens to the parser  
The **parser** checks to make sure the input file followed the correct syntax, which is specified in Extended Backus-Naur Form (EBNF). It also adds variables, constants and procedures to the symbol table.  
If the program is syntactically correct, the **driver** passes the symbol table from the parser to codegen.  
**Codegen** uses the symbol table and the input file to construct the assembly instructions for the virtual machine.  
The **driver** then passes the instructions to the virtual machine.  
The **virtual machine** reads the instructions and manipulates the registers and the stack accordingly.  

# Parser Walkthrough
![Parser tutorial.](/Parser_Tutorial/parser_tutorial.gif)

# Codegen Walkthrough
![Codegen tutorial.](/Codegen_Tutorial/codegen_tutorial.gif)

# Usage
You can download this project for yourself by doing 
```
git clone https://github.com/dowdyma1/Compiler
```

There are three options for printing:
```
-l prints output for lexeme list
-a prints the assembly instructions frome codegen
-v shows the virtual machine stack and registers
```

## Running
```
make
./prog [input.txt] [-a -l -v]
```
to remove the object file:
```
make clean
```
