PKG_CONFIG = pkg-config
PKGS = gio-2.0
CFLAGS = -Wall -Wextra -O2 `$(PKG_CONFIG) --cflags $(PKGS)`
LDFLAGS = -lm `$(PKG_CONFIG) --libs $(PKGS)`
SRC = main.c sunrise_sunset.c
OBJ = $(SRC:.c=.o)
TARGET = autoLightDark

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
