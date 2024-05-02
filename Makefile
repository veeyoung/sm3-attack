SHAREFLAGS = -s -g0 -fdata-sections -ffunction-sections -Wl,--gc-sections
SOFLAGS = $(SHAREFLAGS) -march=native -msse2 -fPIC -shared
LINKFLAGS = $(SHAREFLAGS) -L. -lcommon -Wl,-rpath=.

all: rho-attack LengthAttack BirthdayAttack


LengthAttack: libcommon.so LengthAttack.cpp
	g++ LengthAttack.cpp $(LINKFLAGS) -o LengthAttack

rho-attack: libcommon.so rho-attack.cpp
	g++ rho-attack.cpp $(LINKFLAGS) -o rho-attack

BirthdayAttack: libcommon.so BirthdayAttack.cpp
	g++ BirthdayAttack.cpp $(LINKFLAGS) -o BirthdayAttack

libcommon.so: common.cpp common.h
	g++ common.cpp $(SOFLAGS) -o libcommon.so

clean:
	$(RM) rho-attack LengthAttack BirthdayAttack libcommon.so
