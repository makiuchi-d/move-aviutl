

EXE = move.auf

all: $(EXE)

move.auf: move.obj move.res
	bcc32 -tWD -O1 -O2 -emove.auf move.obj
	brc32 -femove.auf move.res

move.obj: move.c
	bcc32 -c -O1 -O2 move.c

move.res: move.rc *.ico
	brcc32 move.rc
