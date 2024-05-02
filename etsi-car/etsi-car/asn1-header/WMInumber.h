/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./asn1/ITS-Container.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#ifndef	_WMInumber_H_
#define	_WMInumber_H_


#include "asn_application.h"

/* Including external dependencies */
#include "IA5String.h"

#ifdef __cplusplus
extern "C" {
#endif

/* WMInumber */
typedef IA5String_t	 WMInumber_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_WMInumber_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_WMInumber;
asn_struct_free_f WMInumber_free;
asn_struct_print_f WMInumber_print;
asn_constr_check_f WMInumber_constraint;
ber_type_decoder_f WMInumber_decode_ber;
der_type_encoder_f WMInumber_encode_der;
xer_type_decoder_f WMInumber_decode_xer;
xer_type_encoder_f WMInumber_encode_xer;
oer_type_decoder_f WMInumber_decode_oer;
oer_type_encoder_f WMInumber_encode_oer;
per_type_decoder_f WMInumber_decode_uper;
per_type_encoder_f WMInumber_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _WMInumber_H_ */
#include "asn_internal.h"
