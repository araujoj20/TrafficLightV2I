#!/bin/sh
#asn1c -fnative-types -gen-PER -pdu=auto its_facilities_pdu_all.asn    # ITS_CAM_v1.3.2.asn

rm *.c *.h *.o

asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names ./asn1/SPATEM-PDU-Descriptions.asn ./asn1/ITS-Container.asn ./asn1/ISO-19091.asn ./asn1/ISO-24534-3.asn

mkdir -p asn1-source
mkdir -p asn1-header

cp /usr/local/share/asn1c/*.c ./asn1-source/
cp /usr/local/share/asn1c/*.h ./asn1-header/

mv *.c ./asn1-source/
mv *.h ./asn1-header/