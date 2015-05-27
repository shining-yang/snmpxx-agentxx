# Makefile for libdes lite, for the full libdes distribution get
# ftp://ftp.psy.uq.oz.au/~ftp/Crypto/DES
#
# Targets
# make          - twidle the options yourself :-)
# make cc       - standard cc options
# make gcc      - standard gcc options
# make x86-elf  - linux-elf etc
# make x86-out  - linux-a.out, FreeBSD etc
# make x86-solaris
# make x86-bdsi

# If you are on a DEC Alpha, edit des.h and change the DES_LONG
# define to 'unsigned int'.  I have seen this give a %20 speedup.

OPTS0= -DLIBDES_LIT -DRAND #-DNOCONST

# There are 4 possible performance options
# -DDES_PTR
# -DDES_RISC1
# -DDES_RISC2 (only one of DES_RISC1 and DES_RISC2)
# -DDES_UNROLL
# after the initial build, run 'des_opts' to see which options are best
# for your platform.  There are some listed in options.txt
#OPTS2= -DDES_PTR 
#OPTS3= -DDES_RISC1 # or DES_RISC2
#OPTS4= -DDES_UNROLL

OPTS= $(OPTS0) $(OPTS1) $(OPTS2) $(OPTS3) $(OPTS4)

CC=cc
CFLAGS= -I. -O $(OPTS) $(CFLAG)

#CC=gcc
#CFLAGS= -O3 -fomit-frame-pointer $(OPTS) $(CFLAG)

CPP=($CC) -E

DES_ENC=des_enc.o	# normal C version
#DES_ENC=asm/dx86-elf.o	# elf format x86
#DES_ENC=asm/dx86-out.o	# a.out format x86
#DES_ENC=asm/dx86-sol.o	# solaris format x86 
#DES_ENC=asm/dx86bsdi.o	# bsdi format x86 

LIBDIR=/usr/local/lib
INCDIR=/usr/local/include
MANDIR=/usr/local/man
MAN1=1
MAN3=3
SHELL=/bin/sh
OBJS=	cbc_enc.o ncbc_enc.o ecb_enc.o ede_enc.o fcrypt.o set_key.o $(DES_ENC) 

TESTING=examples/destest.c examples/speed.c examples/des_opts.c
GENERAL=COPYRIGHT INSTALL README VERSION Makefile des_crypt.man \
	des.doc options.txt asm

DLIB=	libdes.a

all: $(DLIB) examples/destest examples/speed examples/des_opts

cc:
	make CC=cc CFLAGS="-O $(OPTS) $(CFLAG)" all

gcc:
	make CC=gcc CFLAGS="-O3 -fomit-frame-pointer $(OPTS) $(CFLAG)" all

x86-elf:
	make DES_ENC=asm/dx86-elf.o CC=gcc CFLAGS="-DELF -O3 -fomit-frame-pointer $(OPTS) $(CFLAG)" all

x86-out:
	make DES_ENC=asm/dx86-out.o CC=gcc CFLAGS="-DOUT -O3 -fomit-frame-pointer $(OPTS) $(CFLAG)" all

x86-solaris:
	make DES_ENC=asm/dx86-sol.o CFLAGS="-DSOL -O  $(OPTS) $(CFLAG)" all

x86-bsdi:
	make DES_ENC=asm/dx86bsdi.o CC=gcc CFLAGS="-DBSDI -O3 -fomit-frame-pointer $(OPTS) $(CFLAG)" all

asm/dx86-elf.o: asm/dx86-cpp.s asm/dx86unix.cpp
	$(CPP) -DELF asm/dx86unix.cpp | as -o asm/dx86-elf.o

asm/dx86-sol.o: asm/dx86-cpp.s asm/dx86unix.cpp
	$(CPP) -DSOL asm/dx86unix.cpp | as -o asm/dx86-sol.o

asm/dx86-out.o: asm/dx86-cpp.s asm/dx86unix.cpp
	$(CPP) -DOUT asm/dx86unix.cpp | as -o asm/dx86-out.o

asm/dx86bsdi.o: asm/dx86-cpp.s asm/dx86unix.cpp
	$(CPP) -DBSDI asm/dx86unix.cpp | as -o asm/dx86bsdi.o

test:	all
	examples/destest

$(DLIB): $(OBJS)
	/bin/rm -f $(DLIB)
	ar cr $(DLIB) $(OBJS)
	-if test -s /bin/ranlib; then /bin/ranlib $(DLIB); \
	else if test -s /usr/bin/ranlib; then /usr/bin/ranlib $(DLIB); \
	else exit 0; fi; fi

examples/des_opts: examples/des_opts.o libdes.a
	$(CC) $(CFLAGS) -o examples/des_opts examples/des_opts.o libdes.a

examples/destest: examples/destest.o libdes.a
	$(CC) $(CFLAGS) -o examples/destest examples/destest.o libdes.a

examples/speed: examples/speed.o libdes.a
	$(CC) $(CFLAGS) -o examples/speed examples/speed.o libdes.a

depend:
	makedepend $(LIBDES) $(DES) $(TESTING)

clean:
	/bin/rm -f *.o tags core examples/destest examples/speed $(DLIB) .nfs* *.old \
	*.bak examples/des_opts asm/*.o examples/*.o

dclean:
	sed -e '/^# DO NOT DELETE THIS LINE/ q' Makefile >Makefile.new
	mv -f Makefile.new Makefile

# Eric is probably going to choke when he next looks at this --tjh
install: $(DLIB)
	if test $(INSTALLTOP); then \
	    echo SSL style install; \
	    cp $(DLIB) $(INSTALLTOP)/lib; \
	    if test -s /bin/ranlib; then \
	        /bin/ranlib $(INSTALLTOP)/lib/$(DLIB); \
	    else \
		if test -s /usr/bin/ranlib; then \
		/usr/bin/ranlib $(INSTALLTOP)/lib/$(DLIB); \
	    fi; fi; \
	    chmod 644 $(INSTALLTOP)/lib/$(DLIB); \
	    cp des.h $(INSTALLTOP)/include; \
	    chmod 644 $(INSTALLTOP)/include/des.h; \
	else \
	    echo Standalone install; \
	    cp $(DLIB) $(LIBDIR)/$(DLIB); \
	    if test -s /bin/ranlib; then \
	      /bin/ranlib $(LIBDIR)/$(DLIB); \
	    else \
	      if test -s /usr/bin/ranlib; then \
		/usr/bin/ranlib $(LIBDIR)/$(DLIB); \
	      fi; \
	    fi; \
	    chmod 644 $(LIBDIR)/$(DLIB); \
	    cp des_crypt.man $(MANDIR)/man$(MAN3)/des_crypt.$(MAN3); \
	    chmod 644 $(MANDIR)/man$(MAN3)/des_crypt.$(MAN3); \
	    cp des.man $(MANDIR)/man$(MAN1)/des.$(MAN1); \
	    chmod 644 $(MANDIR)/man$(MAN1)/des.$(MAN1); \
	    cp des.h $(INCDIR)/des.h; \
	    chmod 644 $(INCDIR)/des.h; \
	fi
# DO NOT DELETE THIS LINE -- make depend depends on it.
