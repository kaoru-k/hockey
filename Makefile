# Makefile
# 徳島大学 工学部 知能情報工学科 27班

LDLIBS = -lSDL -lGL -lGL
TARGET1 = server
TARGET2 = client
# all
all :	$(TARGET1) $(TARGET2)

# server
$(TARGET1): $(OBJS)
$(OBJS1):

# client
$(TARGET2):

# clean
.PHONY: clean
clean :
	@rm -f core *~