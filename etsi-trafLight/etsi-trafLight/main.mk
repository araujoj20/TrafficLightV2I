include Makefile.am.libasncodec
CC = gcc
LIBS += -lm
CFLAGS += -g $(ASN_MODULE_CFLAGS) -DASN_PDU_COLLECTION -I./asn1-header
ASN_LIBRARY ?= libasncodec.a
ASN_PROGRAM ?= main
ASN_PROGRAM_SRCS ?= \
    src/main.c\
    src/message.c\
    ./asn1-source/pdu_collection.c
 
ASN_MODULE_SRCS_PATCH = $(addprefix ./asn1-source/, $(ASN_MODULE_SRCS))

all: $(ASN_PROGRAM)

$(ASN_PROGRAM): $(ASN_LIBRARY) $(ASN_PROGRAM_SRCS:.c=.o)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(ASN_PROGRAM) $(ASN_PROGRAM_SRCS:.c=.o) $(LDFLAGS) $(ASN_LIBRARY) $(LIBS)

$(ASN_LIBRARY): $(ASN_MODULE_SRCS_PATCH:.c=.o)
	$(AR) rcs $@ $(ASN_MODULE_SRCS_PATCH:.c=.o)

src/main.o: src/main.c src/../inc/message.h
src/message.o: src/message.c src/../inc/message.h

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(ASN_PROGRAM) $(ASN_LIBRARY)
	rm -f $(ASN_MODULE_SRCS_PATCH:.c=.o) $(ASN_PROGRAM_SRCS:.c=.o)

regen: regenerate-from-asn1-source

regenerate-from-asn1-source:
	asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names ./asn1/SPATEM-PDU-Descriptions.asn ./asn1/ITS-Container.asn ./asn1/ISO-19091.asn ./asn1/ISO-24534-3.asn
