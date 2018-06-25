CC = g++
CFLAGS = -std=c++11 -Wall -g

all: test_vector test_list test_deque test_tree test_hashtable

test_vector: test_vector.cpp profiler.o 
	$(CC) $(CFLAGS) test_vector.cpp profiler.o -o test_vector 

test_list: test_list.cpp profiler.o
	$(CC) $(CFLAGS) test_list.cpp profiler.o -o test_list

test_deque: test_deque.cpp profiler.o
	$(CC) $(CFLAGS) test_deque.cpp profiler.o -o test_deque 

test_tree: test_tree.cpp profiler.o
	$(CC) $(CFLAGS) test_tree.cpp profiler.o -o test_tree 

test_hashtable: test_hashtable.cpp 
	$(CC) $(CFLAGS) test_hashtable.cpp -o test_hashtable 

profiler.o: profiler.cpp 
	$(CC) $(CFLAGS) -c profiler.cpp 

clean:
	rm profiler.o test_vector test_list test_deque \
	   test_tree test_hashtable 
