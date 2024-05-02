/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./asn1/ITS-Container.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#include "ActionID.h"

static asn_TYPE_member_t asn_MBR_ActionID_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ActionID, originatingStationID),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_StationID,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"originatingStationID"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ActionID, sequenceNumber),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SequenceNumber,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"sequenceNumber"
		},
};
static const ber_tlv_tag_t asn_DEF_ActionID_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_ActionID_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* originatingStationID */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* sequenceNumber */
};
static asn_SEQUENCE_specifics_t asn_SPC_ActionID_specs_1 = {
	sizeof(struct ActionID),
	offsetof(struct ActionID, _asn_ctx),
	asn_MAP_ActionID_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_ActionID = {
	"ActionID",
	"ActionID",
	&asn_OP_SEQUENCE,
	asn_DEF_ActionID_tags_1,
	sizeof(asn_DEF_ActionID_tags_1)
		/sizeof(asn_DEF_ActionID_tags_1[0]), /* 1 */
	asn_DEF_ActionID_tags_1,	/* Same as above */
	sizeof(asn_DEF_ActionID_tags_1)
		/sizeof(asn_DEF_ActionID_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_ActionID_1,
	2,	/* Elements count */
	&asn_SPC_ActionID_specs_1	/* Additional specs */
};

