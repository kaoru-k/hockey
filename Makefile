#*************************************
# Makefile
# 徳島大学 工学部 知能情報工学科 27班
#*************************************

LDLIBS  = -lSDL -lSDL_mixer -lGL -lGLU -lnsl -lm -lrt
TARGET1 = hockey_server
TARGET2 = hockey_client
OBJS1   = server/server.o server/server_game.o server/server_net.o
OBJS2   = client/client.o client/client_game.o client/client_graphic.o client/client_net.o client/client_sound.o
.PHONY: all clean

# default
all: $(TARGET1) $(TARGET2)

# server
$(TARGET1): $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1) $(LDLIBS)

# client
$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2) $(LDLIBS)

# clean
clean : 
	$(RM) $(TARGET1) $(TARGET2) client/$(TARGET2)_test $(OBJS1) $(OBJS2) *~ server/*~ client/*~
