/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./asn1/ITS-Container.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#ifndef	_Traces_H_
#define	_Traces_H_


#include "asn_application.h"

/* Including external dependencies */
#include "asn_SEQUENCE_OF.h"
#include "constr_SEQUENCE_OF.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct PathHistory;

/* Traces */
typedef struct Traces {
	A_SEQUENCE_OF(struct PathHistory) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Traces_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Traces;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "PathHistory.h"

#endif	/* _Traces_H_ */
#include "asn_internal.h"
