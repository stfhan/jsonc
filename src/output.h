#ifndef __JSONC_OUTPUT_H__
#define __JSONC_OUTPUT_H__

#include "json.struct.h"

enum {
	JSONC_OUT_ECHO,
	JSONC_OUT_FLAT,
	JSONC_OUT_HTML,
	JSONC_OUT_XML,
	JSONC_OUT_MD,
	JSONC_OUT_DOXYGEN,

	JSONC_OUT_HTML_CSSONLY,

	JSONC_OUTPUTMODE__TOTAL,
};

extern void jsonc_print_value(const JSONC_Value*);
extern void jsonc_print_struct(const JSONC_Struct*);
extern void jsonc_print_array(const JSONC_Array*);

extern void jsonc_print_flat_struct(const JSONC_Struct*);
extern void jsonc_print_flat(const JSONC_Array*);

extern void jsonc_print_html_css();
extern void jsonc_print_html_struct(const JSONC_Struct*);
extern void jsonc_print_html(const JSONC_Array*);

extern void jsonc_print_xml_struct(const JSONC_Struct*);
extern void jsonc_print_xml(const JSONC_Array*);

extern void jsonc_print_md_struct(const JSONC_Struct*);
extern void jsonc_print_md(const JSONC_Array*);

extern void jsonc_print_doxygen_struct(const JSONC_Struct*);
extern void jsonc_print_doxygen(const JSONC_Array*);

#endif