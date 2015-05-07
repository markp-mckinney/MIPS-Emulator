CPP=gcc
CPPFLAGS=-Wall

DEPS=decode.o execute.o fetch.o memAccess.o writeback.o

TARGET=emu

it: $(DEPS)
	$(CPP) $(CPPFLAGS) emulator.c $(DEPS) -o $(TARGET)

%.o: %.c %.h
	$(CPP) $(CPPFLAGS) -c $<

clean:
	touch $(TARGET)
	touch $(DEPS)
	touch a.gch
	rm *.o $(TARGET)
	rm *.gch
