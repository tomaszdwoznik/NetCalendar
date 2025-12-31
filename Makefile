CC = gcc
CFLAGS = -Wall
LIBS = -lssl -lcrypto
SRCS = server/src/main.c \
       server/include/c/common_utils.c \
       server/include/c/network_utils.c \
       server/include/c/ssl_methods.c \
       server/include/c/cJSON.c \
	server/include/c/calendar.c
TARGET = server_app

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)