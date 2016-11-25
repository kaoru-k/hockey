# Makefile
# 徳島大学 工学部 知能情報工学科 27班

LDLIBS = -lSDL -lGL -lGL -lsocket -lnsl
TARGET1 = server
TARGET2 = client
OBJS1 = server/server_main.o server/server_game.o server/server_net.o
OBJS2 = client/client_main.o client/client_game.o client/client_net.o client/client_sound.o
HEADS1 = server/server.h
HEADS2 = client/client.h

# default
.PHONY: all
all: $(TARGET1) $(TARGET2)

# server
$(TARGET1): $(OBJS1)
$(OBJS1): $(HEADS1)

# client
$(TARGET2): $(OBJS2)
$(OBJS2): $(HEADS2)

# clean
.PHONY: clean
clean :
	@rm -f core *~