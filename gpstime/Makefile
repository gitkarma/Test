GPSTIME_MAIN:= gpstime.c
GPSTIME_LIBS:=
GPSTIME_EXEC:= gpstime
LIBS:= -lgps
INCLS:=

CFLAGS:= -g

gpstime:
	$(CC) $(GPSTIME_MAIN) $(GPSTIME_LIBS) -o $(GPSTIME_EXEC) $(CFLAGS) $(LIBS)

clean:
	rm -f $(GPSTIME_EXEC) *.o *~
