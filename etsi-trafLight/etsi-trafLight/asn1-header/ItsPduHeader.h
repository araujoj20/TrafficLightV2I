/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./asn1/ITS-Container.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#ifndef	_ItsPduHeader_H_
#define	_ItsPduHeader_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "StationID.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ItsPduHeader__messageID {
	ItsPduHeader__messageID_denm	= 1,
	ItsPduHeader__messageID_cam	= 2,
	ItsPduHeader__messageID_poi	= 3,
	ItsPduHeader__messageID_spatem	= 4,
	ItsPduHeader__messageID_mapem	= 5,
	ItsPduHeader__messageID_ivim	= 6,
	ItsPduHeader__messageID_ev_rsr	= 7,
	ItsPduHeader__messageID_tistpgtransaction	= 8,
	ItsPduHeader__messageID_srem	= 9,
	ItsPduHeader__messageID_ssem	= 10,
	ItsPduHeader__messageID_evcsn	= 11,
	ItsPduHeader__messageID_saem	= 12,
	ItsPduHeader__messageID_rtcmem	= 13
} e_ItsPduHeader__messageID;

/* ItsPduHeader */
typedef struct ItsPduHeader {
	long	 protocolVersion;
	long	 messageID;
	StationID_t	 stationID;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ItsPduHeader_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ItsPduHeader;
extern asn_SEQUENCE_specifics_t asn_SPC_ItsPduHeader_specs_1;
extern asn_TYPE_member_t asn_MBR_ItsPduHeader_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _ItsPduHeader_H_ */
#include "asn_internal.h"
