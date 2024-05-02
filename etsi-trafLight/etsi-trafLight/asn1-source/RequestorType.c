/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "./asn1/ISO-19091.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#include "RequestorType.h"

asn_TYPE_member_t asn_MBR_RequestorType_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct RequestorType, role),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BasicVehicleRole,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"role"
		},
	{ ATF_POINTER, 5, offsetof(struct RequestorType, subrole),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RequestSubRole,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"subrole"
		},
	{ ATF_POINTER, 4, offsetof(struct RequestorType, request),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RequestImportanceLevel,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"request"
		},
	{ ATF_POINTER, 3, offsetof(struct RequestorType, iso3883),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Iso3833VehicleType,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"iso3883"
		},
	{ ATF_POINTER, 2, offsetof(struct RequestorType, hpmsType),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_VehicleType,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"hpmsType"
		},
	{ ATF_POINTER, 1, offsetof(struct RequestorType, regional),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RegionalExtension_369P0,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"regional"
		},
};
static const int asn_MAP_RequestorType_oms_1[] = { 1, 2, 3, 4, 5 };
static const ber_tlv_tag_t asn_DEF_RequestorType_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_RequestorType_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* role */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* subrole */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* request */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* iso3883 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* hpmsType */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* regional */
};
asn_SEQUENCE_specifics_t asn_SPC_RequestorType_specs_1 = {
	sizeof(struct RequestorType),
	offsetof(struct RequestorType, _asn_ctx),
	asn_MAP_RequestorType_tag2el_1,
	6,	/* Count of tags in the map */
	asn_MAP_RequestorType_oms_1,	/* Optional members */
	5, 0,	/* Root/Additions */
	6,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_RequestorType = {
	"RequestorType",
	"RequestorType",
	&asn_OP_SEQUENCE,
	asn_DEF_RequestorType_tags_1,
	sizeof(asn_DEF_RequestorType_tags_1)
		/sizeof(asn_DEF_RequestorType_tags_1[0]), /* 1 */
	asn_DEF_RequestorType_tags_1,	/* Same as above */
	sizeof(asn_DEF_RequestorType_tags_1)
		/sizeof(asn_DEF_RequestorType_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_RequestorType_1,
	6,	/* Elements count */
	&asn_SPC_RequestorType_specs_1	/* Additional specs */
};

