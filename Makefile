SHAREFLAGS = -s -g0 -fdata-sections -ffunction-sections -Wl,--gc-sections
SOFLAGS = $(SHAREFLAGS) -march=native -msse2 -fPIC -shared
LINKFLAGS = $(SHAREFLAGS) -L. -lcommon -Wl,-rpath=.

all: rho-attack.out LengthAttack.out BirthdayAttack.out


LengthAttack.out: libcommon.so LengthAttack.cpp common.h
	g++ LengthAttack.cpp $(LINKFLAGS) -o LengthAttack.out

rho-attack.out: libcommon.so rho-attack.cpp common.h
	g++ rho-attack.cpp $(LINKFLAGS) -o rho-attack.out

BirthdayAttack.out: libcommon.so BirthdayAttack.cpp common.h
	g++ BirthdayAttack.cpp $(LINKFLAGS) -o BirthdayAttack.out

libcommon.so: common.cpp common.h
	g++ common.cpp $(SOFLAGS) -o libcommon.so

clean:
	$(RM) *.out libcommon.so
