all: travelMonitor Monitor

IDIR =../project1
CC=g++ -g
CFLAGS=-I$(IDIR) -std=c++11 -g #-Wall

ODIR = project1

OBJ_DIR = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ = Date.o Citizen.o Record.o HashTable.o SkipList.o BloomFilter.o Application.o AdditionFunctions.o

OBJS1 = Monitor.o Send_Get.o 

OBJS2 = travelMonitor.o TravelRequests.o Send_Get.o

EXEC1 = Monitor
EXEC2 = travelMonitor

$(EXEC1): $(OBJS1) $(OBJ_DIR) 
	$(CC) $(CFLAGS) $(OBJ_DIR) $(OBJS1) -o $(EXEC1)

$(EXEC2): $(OBJS2) $(OBJ_DIR)
	$(CC) $(CFLAGS)  $(OBJ_DIR) $(OBJS2) -o $(EXEC2)


clean:
	rm -f $(EXEC1) myfifos/* logfiles/* $(EXEC2) $(OBJS1) $(OBJS2) $(ODIR)/*.o *~ core $(IDIR)/*~ 

