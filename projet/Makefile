TARGET = zoul
TARGET_LIBFILES = -lm

CONTIKI = ../..

CONTIKI_PROJECT = sensor.upload server.upload
CONTIKI_TARGET_SOURCEFILES += rgb-bl-lcd.c
CONTIKI_WITH_RIME = 1

all: $(CONTIKI_PROJECT)
sensor: sensor.upload
server: server.upload

include $(CONTIKI)/Makefile.include
