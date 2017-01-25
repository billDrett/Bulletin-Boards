# In  order  to  execute  this  "Makefile " just  type  "make "
OBJS1     = board.o boardClient.o boardServer.o stringFuctions.o List.o Node.o chanel.o
OBJS2     = boardPostTest.o boardPost.o stringFuctions.o
SOURCE   = test.cpp bankAccount.cpp graph.cpp hashtable.cpp List.cpp Node.cpp edge.cpp
HEADER1  = boardServer.h List.h chanel.h stringFuctions.h Node.h boardClient.h
HEADER2  = boardPost.h stringFuctions.h
OUT      = board boardPost
CC       = g++
FLAGS    = -g -c
# -g  option  enables  debugging  mode
# -c flag  generates  object  code  for  separate  files
all: board boardPost
	
#  create/ compile  the  individual  files  >> separately <<
board: $(OBJS1)
	$(CC) -g $(OBJS1) -o board
boardPost: $(OBJS2)
	$(CC) -g $(OBJS2) -o boardPost
	
board.o: board.cpp $(HEADER1)
	$(CC) $(FLAGS) board.cpp
	
boardPostTest.o: boardPostTest.cpp $(HEADER2)
	$(CC) $(FLAGS) boardPostTest.cpp

boardClient.o: boardClient.cpp boardClient.h
	$(CC) $(FLAGS) boardClient.cpp

boardPost.o: boardPost.cpp boardPost.h
	$(CC) $(FLAGS) boardPost.cpp

boardServer.o: boardServer.cpp boardServer.h
	$(CC) $(FLAGS) boardServer.cpp

stringFuctions.o: stringFuctions.cpp stringFuctions.h
	$(CC) $(FLAGS) stringFuctions.cpp

List.o: List.cpp List.h
	$(CC) $(FLAGS) List.cpp

Node.o: Node.cpp Node.h
	$(CC) $(FLAGS) Node.cpp

chanel.o: chanel.cpp chanel.h
	$(CC) $(FLAGS) chanel.cpp

#  clean  house
clean :
	rm -f $(OBJS1) $(OBJS2) $(OUT)
