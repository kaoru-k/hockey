#*************************************
# Makefile
# 徳島大学 工学部 知能情報工学科 27班
#*************************************

LDLIBS  = -lSDL -lGL -lGLU -lnsl -lm -lrt
TARGET1 = server
TARGET2 = client
OBJS1   = server/server.o server/server_game.o server/server_net.o
OBJS2   = client/client.o client/client_game.o client/client_graphic.o client/client_net.o client/client_sound.o
HEADS1  = server/server.h
HEADS2  = client/client.h
.PHONY: all test clean

# default
all: $(TARGET1) $(TARGET2)

# server
$(TARGET1): $(OBJS1)
	$(CC) -o server/$(TARGET1) $(OBJS1) $(LDLIBS)

# client
$(TARGET2): $(OBJS2)
	$(CC) -o client/$(TARGET2) $(OBJS2) $(LDLIBS)

# test
test: $(OBJS2) server/server_game.o
	$(CC) $(CFLAGS) -o client/$(TARGET2)_test $(OBJS2) server/server_game.o $(LDLIBS)

# clean
clean : 
	$(RM) server/$(TARGET1) client/$(TARGET2) client/$(TARGET2)_test $(OBJS1) $(OBJS2)
