/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "AddGrpC"
 * 	found in "./asn1/ISO-19091.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#ifndef	_PrioritizationResponseList_H_
#define	_PrioritizationResponseList_H_


#include "asn_application.h"

/* Including external dependencies */
#include "asn_SEQUENCE_OF.h"
#include "constr_SEQUENCE_OF.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct PrioritizationResponse;

/* PrioritizationResponseList */
typedef struct PrioritizationResponseList {
	A_SEQUENCE_OF(struct PrioritizationResponse) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PrioritizationResponseList_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PrioritizationResponseList;
extern asn_SET_OF_specifics_t asn_SPC_PrioritizationResponseList_specs_1;
extern asn_TYPE_member_t asn_MBR_PrioritizationResponseList_1[1];
extern asn_per_constraints_t asn_PER_type_PrioritizationResponseList_constr_1;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "PrioritizationResponse.h"

#endif	/* _PrioritizationResponseList_H_ */
#include "asn_internal.h"
