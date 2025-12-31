CC = gcc
CFLAGS = -Wall
LIBS = -lssl -lcrypto
SRCS = main.c \
       ../include/c/common_utils.c \
       ../include/c/network_utils.c \
       ../include/c/ssl_methods.c \
       ../include/c/cJSON.c \
	../include/c/calendar.c
TARGET = server_app

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)