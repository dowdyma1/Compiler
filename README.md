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

# Output
Here is the output from running:
```
./prog input.txt -a -v -l
```

```
Source Program:
var x,y,z,v,w;
procedure a;
  var x,y,u,v;
  procedure b;
	var y,z,v;
    procedure c;
	  var y,z;
      begin
        z:=1;
        x:=y+z+w;
      end;
    begin
      y:=x+u+w;
      call c
    end;
  begin
    z:=2;
    u:=z+w;
    call b
  end;
begin
  x:=1; y:=2; z:=3; v:=4; w:=5;
  x:=v+w;
  write z;
  call a;
end.


lexeme			token type
var			29
x			2
,			17
y			2
,			17
z			2
,			17
v			2
,			17
w			2
;			18
procedure			30
a			2
;			18
var			29
x			2
,			17
y			2
,			17
u			2
,			17
v			2
;			18
procedure			30
b			2
;			18
var			29
y			2
,			17
z			2
,			17
v			2
;			18
procedure			30
c			2
;			18
var			29
y			2
,			17
z			2
;			18
begin			21
z			2
:=			20
1			3
;			18
x			2
:=			20
y			2
+			4
z			2
+			4
w			2
;			18
end			22
;			18
begin			21
y			2
:=			20
x			2
+			4
u			2
+			4
w			2
;			18
call			27
c			2
end			22
;			18
begin			21
z			2
:=			20
2			3
;			18
u			2
:=			20
z			2
+			4
w			2
;			18
call			27
b			2
end			22
;			18
begin			21
x			2
:=			20
1			3
;			18
y			2
:=			20
2			3
;			18
z			2
:=			20
3			3
;			18
v			2
:=			20
4			3
;			18
w			2
:=			20
5			3
;			18
x			2
:=			20
v			2
+			4
w			2
;			18
write			31
z			2
;			18
call			27
a			2
;			18
end			22
.			19

Lexeme List:
29 2 x 17 2 y 17 2 z 17 2 v 17 2 w 18 30 2 a 18 29 2 x 17 2 y 17 2 u 17 2 v 18 30 2 b 18 29 2 y 17 2 z 17 2 v 18 30 2 c 18 29 2 y 17 2 z 18 21 2 z 20 3 1 18 2 x 20 2 y 4 2 z 4 2 w 18 22 18 21 2 y 20 2 x 4 2 u 4 2 w 18 27 2 c 22 18 21 2 z 20 3 2 18 2 u 20 2 z 4 2 w 18 27 2 b 22 18 21 2 x 20 3 1 18 2 y 20 3 2 18 2 z 20 3 3 18 2 v 20 3 4 18 2 w 20 3 5 18 2 x 20 2 v 4 2 w 18 31 2 z 18 27 2 a 18 22 19 

Symbolic representation:
varsym identsym x commasym identsym y commasym identsym z commasym identsym v commasym identsym w semicolonsym procsym identsym a semicolonsym varsym identsym x commasym identsym y commasym identsym u commasym identsym v semicolonsym procsym identsym b semicolonsym varsym identsym y commasym identsym z commasym identsym v semicolonsym procsym identsym c semicolonsym varsym identsym y commasym identsym z semicolonsym beginsym identsym z becomessym numbersym 1 semicolonsym identsym x becomessym identsym y plussym identsym z plussym identsym w semicolonsym endsym semicolonsym beginsym identsym y becomessym identsym x plussym identsym u plussym identsym w semicolonsym callsym identsym c endsym semicolonsym beginsym identsym z becomessym numbersym 2 semicolonsym identsym u becomessym identsym z plussym identsym w semicolonsym callsym identsym b endsym semicolonsym beginsym identsym x becomessym numbersym 1 semicolonsym identsym y becomessym numbersym 2 semicolonsym identsym z becomessym numbersym 3 semicolonsym identsym v becomessym numbersym 4 semicolonsym identsym w becomessym numbersym 5 semicolonsym identsym x becomessym identsym v plussym identsym w semicolonsym writesym identsym z semicolonsym callsym identsym a semicolonsym endsym periodsym 

No errors, program is syntactically correct.

Line OP  R L M
0    jmp 0 0 32
1    jmp 0 0 23
2    jmp 0 0 14
3    jmp 0 0 4
4    inc 0 0 5
5    lit 0 0 1
6    sto 0 0 4
7    lod 0 0 3
8    lod 1 0 4
9    add 0 0 1
10   lod 1 3 7
11   add 0 0 1
12   sto 0 2 3
13   rtn 0 0 0
14   inc 0 0 6
15   lod 0 1 3
16   lod 1 1 5
17   add 0 0 1
18   lod 1 2 7
19   add 0 0 1
20   sto 0 0 3
21   cal 0 0 4
22   rtn 0 0 0
23   inc 0 0 7
24   lit 0 0 2
25   sto 0 1 5
26   lod 0 1 5
27   lod 1 1 7
28   add 0 0 1
29   sto 0 0 5
30   cal 0 0 14
31   rtn 0 0 0
32   inc 0 0 8
33   lit 0 0 1
34   sto 0 0 3
35   lit 0 0 2
36   sto 0 0 4
37   lit 0 0 3
38   sto 0 0 5
39   lit 0 0 4
40   sto 0 0 6
41   lit 0 0 5
42   sto 0 0 7
43   lod 0 0 6
44   lod 1 0 7
45   add 0 0 1
46   sto 0 0 3
47   lod 0 0 5
48   sys 0 0 1
49   cal 0 0 23
50   sys 0 0 3
					pc	bp	sp
					0	999	1000
Initial values
Registers: 0 0 0 0 0 0 0 0
Stack: 

					        pc	bp	sp
0 jmp 0 0 32				32	999	1000
Registers: 0 0 0 0 0 0 0 0
Stack: 

					        pc	bp	sp
32 inc 0 0 8				33	999	992
Registers: 0 0 0 0 0 0 0 0
Stack: 0 0 0 0 0 0 0 0 

					        pc	bp	sp
33 lit 0 0 1				34	999	992
Registers: 1 0 0 0 0 0 0 0
Stack: 0 0 0 0 0 0 0 0 

					        pc	bp	sp
34 sto 0 0 3				35	999	992
Registers: 1 0 0 0 0 0 0 0
Stack: 0 0 0 1 0 0 0 0 

					        pc	bp	sp
35 lit 0 0 2				36	999	992
Registers: 2 0 0 0 0 0 0 0
Stack: 0 0 0 1 0 0 0 0 

					        pc	bp	sp
36 sto 0 0 4				37	999	992
Registers: 2 0 0 0 0 0 0 0
Stack: 0 0 0 1 2 0 0 0 

					        pc	bp	sp
37 lit 0 0 3				38	999	992
Registers: 3 0 0 0 0 0 0 0
Stack: 0 0 0 1 2 0 0 0 

					        pc	bp	sp
38 sto 0 0 5				39	999	992
Registers: 3 0 0 0 0 0 0 0
Stack: 0 0 0 1 2 3 0 0 

					        pc	bp	sp
39 lit 0 0 4				40	999	992
Registers: 4 0 0 0 0 0 0 0
Stack: 0 0 0 1 2 3 0 0 

					        pc	bp	sp
40 sto 0 0 6				41	999	992
Registers: 4 0 0 0 0 0 0 0
Stack: 0 0 0 1 2 3 4 0 

					        pc	bp	sp
41 lit 0 0 5				42	999	992
Registers: 5 0 0 0 0 0 0 0
Stack: 0 0 0 1 2 3 4 0 

					        pc	bp	sp
42 sto 0 0 7				43	999	992
Registers: 5 0 0 0 0 0 0 0
Stack: 0 0 0 1 2 3 4 5 

					        pc	bp	sp
43 lod 0 0 6				44	999	992
Registers: 4 0 0 0 0 0 0 0
Stack: 0 0 0 1 2 3 4 5 

					        pc	bp	sp
44 lod 1 0 7				45	999	992
Registers: 4 5 0 0 0 0 0 0
Stack: 0 0 0 1 2 3 4 5 

					        pc	bp	sp
45 add 0 0 1				46	999	992
Registers: 9 5 0 0 0 0 0 0
Stack: 0 0 0 1 2 3 4 5 

					        pc	bp	sp
46 sto 0 0 3				47	999	992
Registers: 9 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 3 4 5 

					        pc	bp	sp
47 lod 0 0 5				48	999	992
Registers: 3 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 3 4 5 

Register 0: 3

					        pc	bp	sp
48 sys 0 0 1				49	999	992
Registers: 3 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 3 4 5 

					        pc	bp	sp
49 cal 0 0 23				23	991	992
Registers: 3 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 3 4 5 

					        pc	bp	sp
23 inc 0 0 7				24	991	985
Registers: 3 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 3 4 5 | 999 999 50 0 0 0 0 

					        pc	bp	sp
24 lit 0 0 2				25	991	985
Registers: 2 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 3 4 5 | 999 999 50 0 0 0 0 

					        pc	bp	sp
25 sto 0 1 5				26	991	985
Registers: 2 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 0 0 

					        pc	bp	sp
26 lod 0 1 5				27	991	985
Registers: 2 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 0 0 

					        pc	bp	sp
27 lod 1 1 7				28	991	985
Registers: 2 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 0 0 

					        pc	bp	sp
28 add 0 0 1				29	991	985
Registers: 7 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 0 0 

					        pc	bp	sp
29 sto 0 0 5				30	991	985
Registers: 7 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 

					        pc	bp	sp
30 cal 0 0 14				14	984	985
Registers: 7 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 

					        pc	bp	sp
14 inc 0 0 6				15	984	979
Registers: 7 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 0 0 0 

					        pc	bp	sp
15 lod 0 1 3				16	984	979
Registers: 0 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 0 0 0 

					        pc	bp	sp
16 lod 1 1 5				17	984	979
Registers: 0 7 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 0 0 0 

					        pc	bp	sp
17 add 0 0 1				18	984	979
Registers: 7 7 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 0 0 0 

					        pc	bp	sp
18 lod 1 2 7				19	984	979
Registers: 7 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 0 0 0 

					        pc	bp	sp
19 add 0 0 1				20	984	979
Registers: 12 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 0 0 0 

					        pc	bp	sp
20 sto 0 0 3				21	984	979
Registers: 12 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 

					        pc	bp	sp
21 cal 0 0 4				4	978	979
Registers: 12 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 

					    pc	bp	sp
4 inc 0 0 5				5	978	974
Registers: 12 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 0 

					    pc	bp	sp
5 lit 0 0 1				6	978	974
Registers: 1 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 0 

					    pc	bp	sp
6 sto 0 0 4				7	978	974
Registers: 1 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 1 

					    pc	bp	sp
7 lod 0 0 3				8	978	974
Registers: 0 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 1 

					    pc	bp	sp
8 lod 1 0 4				9	978	974
Registers: 0 1 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 1 

					    pc	bp	sp
9 add 0 0 1				10	978	974
Registers: 1 1 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 1 

					        pc	bp	sp
10 lod 1 3 7				11	978	974
Registers: 1 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 1 

					        pc	bp	sp
11 add 0 0 1				12	978	974
Registers: 6 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 0 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 1 

					        pc	bp	sp
12 sto 0 2 3				13	978	974
Registers: 6 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 6 0 7 0 | 991 991 31 12 0 0 | 984 984 22 0 1 

					        pc	bp	sp
13 rtn 0 0 0				22	984	979
Registers: 6 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 6 0 7 0 | 991 991 31 12 0 0 

					        pc	bp	sp
22 rtn 0 0 0				31	991	985
Registers: 6 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 | 999 999 50 6 0 7 0 

					        pc	bp	sp
31 rtn 0 0 0				50	999	992
Registers: 6 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 

					        pc	bp	sp
50 sys 0 0 3				51	999	992
Registers: 6 5 0 0 0 0 0 0
Stack: 0 0 0 9 2 2 4 5 
```
