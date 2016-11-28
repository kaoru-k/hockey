# Makefile
# 徳島大学 工学部 知能情報工学科 27班

LDLIBS  = -lSDL -lGL -lGL -lnsl
TARGET1 = server
TARGET2 = client
OBJS1   = server/server.o server/server_game.o server/server_net.o
OBJS2   = client/client.o client/client_game.o client/client_net.o client/client_sound.o
HEADS1  = server/server.h
HEADS2  = client/client.h

# default
.PHONY: all
all: $(TARGET1) $(TARGET2)

# server
$(TARGET1): $(OBJS1)
	gcc -o server/$(TARGET1) $(OBJS1) $(LDLIBS)

# client
$(TARGET2): $(OBJS2)
	gcc -o client/$(TARGET2) $(OBJS2) $(LDLIBS)

# clean
.PHONY: clean
clean : 