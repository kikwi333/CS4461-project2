How to Compile Program:
	Type in "make" into your command line, our makefile
	should then compile it and make an executable called dv.
	If you would like to get rid of the executable, type in
	"make clean". To compile it without the makefile, type:
	"gcc dv.c node0.c node1.c node2.c node3.c -o dv"

How to Execute Program:
	Type in "./dv" into your command line to execute the binary
	exutable created by the makefile. Adding a trace number like
	"./dv 1" is also acceptable. If no trace number is provided
	("./dv") then the program will ask for one. After that, the
	program's network simulation will run on its own until it
	has finished updating all of the node's distance tables.

Packages Used:
	None