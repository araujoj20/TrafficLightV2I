/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "AddGrpC"
 * 	found in "./asn1/ISO-19091.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#ifndef	_RejectedReason_H_
#define	_RejectedReason_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeEnumerated.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum RejectedReason {
	RejectedReason_unknown	= 0,
	RejectedReason_exceptionalCondition	= 1,
	RejectedReason_maxWaitingTimeExceeded	= 2,
	RejectedReason_ptPriorityDisabled	= 3,
	RejectedReason_higherPTPriorityGranted	= 4,
	RejectedReason_vehicleTrackingUnknown	= 5
	/*
	 * Enumeration is extensible
	 */
} e_RejectedReason;

/* RejectedReason */
typedef long	 RejectedReason_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_RejectedReason_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_RejectedReason;
extern const asn_INTEGER_specifics_t asn_SPC_RejectedReason_specs_1;
asn_struct_free_f RejectedReason_free;
asn_struct_print_f RejectedReason_print;
asn_constr_check_f RejectedReason_constraint;
ber_type_decoder_f RejectedReason_decode_ber;
der_type_encoder_f RejectedReason_encode_der;
xer_type_decoder_f RejectedReason_decode_xer;
xer_type_encoder_f RejectedReason_encode_xer;
oer_type_decoder_f RejectedReason_decode_oer;
oer_type_encoder_f RejectedReason_encode_oer;
per_type_decoder_f RejectedReason_decode_uper;
per_type_encoder_f RejectedReason_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _RejectedReason_H_ */
#include "asn_internal.h"
