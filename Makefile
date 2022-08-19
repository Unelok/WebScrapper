#
# Variables:
#
CC= gcc
BIN = "scrapper"
SRC = main.c 
OBJ = main.o


#
# Construct program
#
all: $(BIN)

$(BIN): $(SRC)
	$(CC) -c $(SRC) -lcurl
	$(CC) -o $(BIN) $(OBJ) -lcurl

#
# Clean
#
clean:
	rm -f *.o *~ core $(BIN)

run : $(BIN)
	./ $(BIN)
