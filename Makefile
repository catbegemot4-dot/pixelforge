PREFIX  ?= $(HOME)/.local
DESTDIR ?=
CFLAGS  ?= -O2 -Wall -Wextra
TARGET  := pixelforge

BINDIR  := $(DESTDIR)$(PREFIX)/bin
DATADIR := $(PREFIX)/share/pixelforge
APPDIR  := $(DESTDIR)$(PREFIX)/share/applications
ICONDIR := $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps

.PHONY: all run install uninstall clean

all: $(TARGET)

$(TARGET): pixelforge.c
	$(CC) $(CFLAGS) -DDATADIR='"$(DATADIR)"' $< -o $@

# сборка и запуск редактора в окне Chromium (из текущей папки)
run: $(TARGET)
	./$(TARGET) index.html

# путь к index.html зашит в бинарник, поэтому пересобираем под выбранный PREFIX
install:
	$(MAKE) -B $(TARGET) PREFIX='$(PREFIX)'
	install -Dm755 $(TARGET) $(BINDIR)/$(TARGET)
	install -Dm644 index.html $(DESTDIR)$(DATADIR)/index.html
	install -Dm644 pixelforge.svg $(ICONDIR)/pixelforge.svg
	install -d $(APPDIR)
	sed 's|@PREFIX@|$(PREFIX)|' pixelforge.desktop.in > $(APPDIR)/pixelforge.desktop
	@echo "Установлено в $(PREFIX). Нужен chromium в PATH."

uninstall:
	rm -f $(BINDIR)/$(TARGET) $(ICONDIR)/pixelforge.svg $(APPDIR)/pixelforge.desktop
	rm -rf $(DESTDIR)$(DATADIR)

clean:
	rm -f $(TARGET)
