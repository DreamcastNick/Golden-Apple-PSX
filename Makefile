TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
       src/mutil.c \
       src/random.c \
       src/archive.c \
       src/font.c \
       src/trans.c \
       src/loadscr.c \
       src/menu.c \
       src/stage.c \
       src/psx/psx.c \
       src/psx/io.c \
       src/psx/gfx.c \
       src/psx/audio.c \
       src/psx/pad.c \
       src/psx/timer.c \
       src/psx/movie.c \
       src/stage/dummy.c \
       src/stage/week0.c \
       src/stage/week1.c \
       src/stage/week2.c \
       src/stage/week3.c \
       src/stage/week4.c \
       src/stage/week5.c \
       src/stage/week8.c \
       src/stage/week9.c \
       src/animation.c \
       src/character.c \
       src/character/title.c \
       src/character/bf.c \
       src/character/tdbf.c \
       src/character/wfbf.c \
	   src/character/dave.c \
       src/character/bandu.c \
       src/character/disrupt.c \
       src/character/unfair.c \
       src/character/cripple.c \
       src/character/wfdave.c \
       src/character/badai.c \
       src/character/speaker.c \
       src/character/ogdave.c \
       src/character/garett.c \
       src/character/diaman.c \
       src/character/robot.c \
       src/character/sugar.c \
       src/character/origin.c \
       src/character/ringi.c \
       src/character/bambom.c \
       src/character/bendu.c \
       src/character/cycles.c \
       src/character/ntbandu.c \
       src/character/pooper.c \
       src/character/png.c \
       src/character/bambi.c \
       src/character/rpa.c \
       src/character/rpb.c \
       src/character/rpc.c \
	   src/character/sart.c \
       src/character/gf.c \
       src/character/gfb.c \
       src/character/bestgf.c \
       src/object.c \
       src/object/combo.c \
       src/object/splash.c \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -Wextra -pedantic -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
#LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
#LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
#LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk
