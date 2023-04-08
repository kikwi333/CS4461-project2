all:
	gcc dv.c node0.c node1.c node2.c node3.c -o dv

clean:
	rm -f dv