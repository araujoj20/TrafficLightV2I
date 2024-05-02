/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "./asn1/ISO-19091.asn"
 * 	`asn1c -gen-PER -pdu=auto -fincludes-quoted -fcompound-names`
 */

#include "Position3D.h"

static int
memb_regional_constraint_1(const asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size >= 1 && size <= 4)) {
		/* Perform validation of the inner elements */
		return SEQUENCE_OF_constraint(td, sptr, ctfailcb, app_key);
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_oer_constraints_t asn_OER_type_regional_constr_5 CC_NOTUSED = {
	{ 0, 0 },
	-1	/* (SIZE(1..4)) */};
static asn_per_constraints_t asn_PER_type_regional_constr_5 CC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 2,  2,  1,  4 }	/* (SIZE(1..4)) */,
	0, 0	/* No PER value map */
};
static asn_oer_constraints_t asn_OER_memb_regional_constr_5 CC_NOTUSED = {
	{ 0, 0 },
	-1	/* (SIZE(1..4)) */};
static asn_per_constraints_t asn_PER_memb_regional_constr_5 CC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 2,  2,  1,  4 }	/* (SIZE(1..4)) */,
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_regional_5[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_RegionalExtension_369P0,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		""
		},
};
static const ber_tlv_tag_t asn_DEF_regional_tags_5[] = {
	(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_regional_specs_5 = {
	sizeof(struct Position3D__regional),
	offsetof(struct Position3D__regional, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_regional_5 = {
	"regional",
	"regional",
	&asn_OP_SEQUENCE_OF,
	asn_DEF_regional_tags_5,
	sizeof(asn_DEF_regional_tags_5)
		/sizeof(asn_DEF_regional_tags_5[0]) - 1, /* 1 */
	asn_DEF_regional_tags_5,	/* Same as above */
	sizeof(asn_DEF_regional_tags_5)
		/sizeof(asn_DEF_regional_tags_5[0]), /* 2 */
	{ &asn_OER_type_regional_constr_5, &asn_PER_type_regional_constr_5, SEQUENCE_OF_constraint },
	asn_MBR_regional_5,
	1,	/* Single element */
	&asn_SPC_regional_specs_5	/* Additional specs */
};

asn_TYPE_member_t asn_MBR_Position3D_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Position3D, lat),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Latitude,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"lat"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Position3D, Long),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Longitude,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"long"
		},
	{ ATF_POINTER, 2, offsetof(struct Position3D, elevation),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Elevation,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"elevation"
		},
	{ ATF_POINTER, 1, offsetof(struct Position3D, regional),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		0,
		&asn_DEF_regional_5,
		0,
		{ &asn_OER_memb_regional_constr_5, &asn_PER_memb_regional_constr_5,  memb_regional_constraint_1 },
		0, 0, /* No default value */
		"regional"
		},
};
static const int asn_MAP_Position3D_oms_1[] = { 2, 3 };
static const ber_tlv_tag_t asn_DEF_Position3D_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_Position3D_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* lat */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* long */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* elevation */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* regional */
};
asn_SEQUENCE_specifics_t asn_SPC_Position3D_specs_1 = {
	sizeof(struct Position3D),
	offsetof(struct Position3D, _asn_ctx),
	asn_MAP_Position3D_tag2el_1,
	4,	/* Count of tags in the map */
	asn_MAP_Position3D_oms_1,	/* Optional members */
	2, 0,	/* Root/Additions */
	4,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_Position3D = {
	"Position3D",
	"Position3D",
	&asn_OP_SEQUENCE,
	asn_DEF_Position3D_tags_1,
	sizeof(asn_DEF_Position3D_tags_1)
		/sizeof(asn_DEF_Position3D_tags_1[0]), /* 1 */
	asn_DEF_Position3D_tags_1,	/* Same as above */
	sizeof(asn_DEF_Position3D_tags_1)
		/sizeof(asn_DEF_Position3D_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_Position3D_1,
	4,	/* Elements count */
	&asn_SPC_Position3D_specs_1	/* Additional specs */
};

