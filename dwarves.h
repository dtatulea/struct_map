#ifndef _PAHOLE_CLASSES_H_
#define _PAHOLE_CLASSES_H_ 1
/* 
  Copyright (C) 2006 Mandriva Conectiva S.A.
  Copyright (C) 2006 Arnaldo Carvalho de Melo <acme@mandriva.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.
*/


#include <stdint.h>
#include <dwarf.h>
#include <elfutils/libdw.h>

#include "list.h"

struct cus {
	struct list_head cus;
	struct list_head priv_definitions; /* struct type entries */
	struct list_head priv_fwd_decls;   /* struct class entries */
	struct list_head *definitions;
	struct list_head *fwd_decls;
};

struct cu {
	struct list_head node;
	struct list_head tags;
	struct list_head tool_list;	/* To be used by tools such as ctracer */
	const char	 *name;
	uint8_t		 addr_size;
	uint16_t	 language;
	uint32_t	 id;
	unsigned long	 nr_inline_expansions;
	size_t		 size_inline_expansions;
	uint32_t	 nr_functions_changed;
	uint32_t	 nr_structures_changed;
	size_t		 max_len_changed_item;
	size_t		 function_bytes_added;
	size_t		 function_bytes_removed;
};

struct tag {
	struct list_head node;
	Dwarf_Off	 type;
	Dwarf_Off	 id;
	uint16_t	 tag;
	uint16_t	 decl_line;
	const char	 *decl_file;
	uint32_t	 refcnt;
};

struct type {
	struct tag	 tag;
	struct list_head node;
	const char	 *name;
	size_t		 size;
	struct list_head members;
	uint16_t	 nr_members;
	uint8_t		 declaration; /* only one bit used */
	uint8_t		 definition_emitted:1;
	uint8_t		 fwd_decl_emitted:1;
};

static inline struct type *tag__type(const struct tag *self)
{
	return (struct type *)self;
}

struct class {
	struct type	 type;
	uint8_t		 nr_holes;
	uint8_t		 nr_bit_holes;
	uint16_t	 padding;
	uint8_t		 bit_padding;
	void		 *priv;
};

static inline struct class *tag__class(const struct tag *self)
{
	return (struct class *)self;
}

static inline struct tag *class__tag(const struct class *self)
{
	return (struct tag *)self;
}

static inline const char *class__name(const struct class *self)
{
	return self->type.name;
}

static inline uint16_t class__tag_type(const struct class *self)
{
	return self->type.tag.tag;
}

struct base_type {
	struct tag	tag;
	const char	*name;
	size_t		size;
};

static inline struct base_type *tag__base_type(const struct tag *self)
{
	return (struct base_type *)self;
}

struct array_type {
	struct tag	tag;
	uint32_t	*nr_entries;
	uint8_t		dimensions;
};

static inline struct array_type *tag__array_type(const struct tag *self)
{
	return (struct array_type *)self;
}

struct class_member {
	struct tag	 tag;
	char		 *name;
	uint16_t	 offset;
	uint8_t		 bit_offset;
	uint8_t		 bit_size;
	uint8_t		 bit_hole;	/* If there is a bit hole before the next
					   one (or the end of the struct) */
	uint8_t		 visited:1;
	uint16_t	 hole;		/* If there is a hole before the next
					   one (or the end of the struct) */
};

struct lexblock {
	struct tag	 tag;
	struct list_head tags;
	Dwarf_Addr	 low_pc;
	Dwarf_Addr	 high_pc;
	uint16_t	 nr_inline_expansions;
	uint16_t	 nr_labels;
	uint16_t	 nr_variables;
	uint16_t	 nr_lexblocks;
	size_t		 size_inline_expansions;
};

static inline struct lexblock *tag__lexblock(const struct tag *self)
{
	return (struct lexblock *)self;
}

/*
 * tag.tag can be DW_TAG_subprogram_type or DW_TAG_subroutine_type.
 */
struct ftype {
	struct tag	 tag;
	struct list_head parms;
	uint16_t	 nr_parms;
	uint8_t		 unspec_parms; /* just one bit is needed */
};

static inline struct ftype *tag__ftype(const struct tag *self)
{
	return (struct ftype *)self;
}

struct function {
	struct ftype	 proto;
	struct lexblock	 lexblock;
	const char	 *name;
	Dwarf_Off	 abstract_origin;
	size_t		 cu_total_size_inline_expansions;
	uint16_t	 cu_total_nr_inline_expansions;
	uint8_t		 inlined;	/* two bits used */
	uint8_t		 external;	/* one bit used */
	/* fields used by tools */
	struct list_head tool_node;
	void		 *priv;
};

static inline struct function *tag__function(const struct tag *self)
{
	return (struct function *)self;
}

struct parameter {
	struct tag	 tag;
	char		 *name;
	Dwarf_Off	 abstract_origin;
};

static inline struct parameter *tag__parameter(const struct tag *self)
{
	return (struct parameter *)self;
}

struct variable {
	struct tag	 tag;
	char		 *name;
	Dwarf_Off	 abstract_origin;
};

static inline struct variable *tag__variable(const struct tag *self)
{
	return (struct variable *)self;
}

struct inline_expansion {
	struct tag	 tag;
	size_t		 size;
	Dwarf_Addr	 low_pc;
	Dwarf_Addr	 high_pc;
};

static inline struct inline_expansion *
				tag__inline_expansion(const struct tag *self)
{
	return (struct inline_expansion *)self;
}

struct label {
	struct tag	 tag;
	char		 *name;
	Dwarf_Addr	 low_pc;
};

struct enumerator {
	struct tag	 tag;
	const char	 *name;
	uint32_t	 value;
};

#define DEFAULT_CACHELINE_SIZE 32

extern void class__find_holes(struct class *self, const struct cu *cu);
extern void tag__print(const struct tag *self, const struct cu *cu,
		       const char *prefix, const char *suffix);

extern const char *function__name(struct function *self, const struct cu *cu);
extern void function__print(const struct tag *tag_self, const struct cu *cu);
extern void function__print_stats(const struct tag *tag_self,
				  const struct cu *cu);

extern struct cus *cus__new(struct list_head *definitions,
			    struct list_head *fwd_decls);
extern int cus__load(struct cus *self, const char *filename);
extern int cus__load_dir(struct cus *self, const char *dirname,
			 const char *filename_mask, const int recursive);
extern struct cu *cus__find_cu_by_name(const struct cus *self,
				       const char *name);
extern struct tag *cus__find_struct_by_name(const struct cus *self,
					    struct cu **cu,
					    const char *name);
extern struct tag *cus__find_function_by_name(const struct cus *self,
					      struct cu **cu,
					      const char *name);
extern int cus__emit_ftype_definitions(struct cus *self, struct cu *cu,
				       struct ftype *ftype);
extern int cus__emit_struct_definitions(struct cus *self, struct cu *cu,
					struct tag *tag, const char *prefix,
					const char *suffix);
extern int cus__emit_fwd_decl(struct cus *self, struct type *ctype);

extern struct tag *cu__find_tag_by_id(const struct cu *self,
				      const Dwarf_Off id);
extern struct tag *cu__find_struct_by_name(const struct cu *cu,
					   const char *name);
extern int tag__is_struct(const struct tag *self, struct tag **typedef_alias,
			  const struct cu *cu);
extern void	    cu__account_inline_expansions(struct cu *self);
extern int	    cu__for_each_tag(struct cu *self,
				     int (*iterator)(struct tag *tag,
					     	     struct cu *cu,
						     void *cookie),
				     void *cookie,
				     struct tag *(*filter)(struct tag *tag,
							   struct cu *cu,
							   void *cookie));
extern void	    cus__for_each_cu(struct cus *self,
				     int (*iterator)(struct cu *cu,
						     void *cookie),
				     void *cookie,
				     struct cu *(*filter)(struct cu *cu));

extern const struct class_member *
		class__find_bit_hole(const struct class *self,
				     const struct class_member *trailer,
				     const size_t bit_hole_size);

extern struct tag *cu__find_function_by_name(const struct cu *cu,
					     const char *name);

static inline size_t function__size(const struct function *self)
{
	return self->lexblock.high_pc - self->lexblock.low_pc;
}

static inline int function__declared_inline(const struct function *self)
{
	return (self->inlined == DW_INL_declared_inlined ||
	        self->inlined == DW_INL_declared_not_inlined);
}

static inline int function__inlined(const struct function *self)
{
	return (self->inlined == DW_INL_inlined ||
	        self->inlined == DW_INL_declared_inlined);
}

extern size_t ftype__snprintf(const struct ftype *self, const struct cu *cu,
			      char *bf, const size_t len,
			      const char *name, const int inlined,
			      const int is_pointer,
			      const size_t type_spacing);
extern int ftype__has_parm_of_type(const struct ftype *self,
				   const struct tag *target,
				   const struct cu *cu);

extern const char *tag__name(const struct tag *self, const struct cu *cu,
			     char *bf, size_t len);
extern size_t tag__size(const struct tag *self, const struct cu *cu);

extern struct class_member *type__find_member_by_name(const struct type *self,
						      const char *name);

static inline struct class_member *
	class__find_member_by_name(const struct class *self, const char *name)
{
	return type__find_member_by_name(&self->type, name);
}

static inline uint16_t class__nr_members(const struct class *self)
{
	return self->type.nr_members;
}

static inline size_t class__size(const struct class *self)
{
	return self->type.size;
}

static inline int class__is_declaration(const struct class *self)
{
	return self->type.declaration;
}

extern size_t cacheline_size;

extern const char *variable__name(const struct variable *self,
				  const struct cu *cu);
extern const char *variable__type_name(const struct variable *self,
				       const struct cu *cu,
				       char *bf, size_t len);

extern const char *dwarf_tag_name(const uint32_t tag);
#endif /* _PAHOLE_CLASSES_H_ */