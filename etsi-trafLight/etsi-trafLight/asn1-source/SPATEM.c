/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "SPATEM-PDU-Descriptions"
 * 	found in "./asn1/SPATEM-PDU-Descriptions.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#include "SPATEM.h"

static asn_TYPE_member_t asn_MBR_SPATEM_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SPATEM, header),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ItsPduHeader,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"header"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SPATEM, spat),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SPAT,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"spat"
		},
};
static const ber_tlv_tag_t asn_DEF_SPATEM_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_SPATEM_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* header */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* spat */
};
static asn_SEQUENCE_specifics_t asn_SPC_SPATEM_specs_1 = {
	sizeof(struct SPATEM),
	offsetof(struct SPATEM, _asn_ctx),
	asn_MAP_SPATEM_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_SPATEM = {
	"SPATEM",
	"SPATEM",
	&asn_OP_SEQUENCE,
	asn_DEF_SPATEM_tags_1,
	sizeof(asn_DEF_SPATEM_tags_1)
		/sizeof(asn_DEF_SPATEM_tags_1[0]), /* 1 */
	asn_DEF_SPATEM_tags_1,	/* Same as above */
	sizeof(asn_DEF_SPATEM_tags_1)
		/sizeof(asn_DEF_SPATEM_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_SPATEM_1,
	2,	/* Elements count */
	&asn_SPC_SPATEM_specs_1	/* Additional specs */
};

