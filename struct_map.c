/*
  Copyright (C) 2011 Dragos Tatulea <dragos.tatulea@gmail.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.
*/

/*
  Description: 
    This utility is supposed to generate a graph of structure dependencies
    in a dot file. The input structs will be highlited in the graph. When
    seen with graphviz, it should look like a bunch of trees with top level
    strucs as root nodes.

  Input:
    List of structures to be checked out.
    List of object files to be read.

  Output:
    .dot file containing what was mentioned in the desc.

  Todo:
    [] input struct list will be kept in a hash
    	[] decide on hash func (easier with b64 encoding)
    [] walk structure by structure in depth, mark input structures
    [] keep structure as tree (not rbtree)
    [] generate .dot file

  Q's:
    [] how to iterate through structs
    [] how to go deep in structs
    [] how to hash a struct name, or hash by smth else?
*/

#include <argp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <dwarf.h>

#include "list.h"
#include "dwarves.h"
#include "dutil.h"

/* Name and version of program.  */
ARGP_PROGRAM_VERSION_HOOK_DEF = dwarves_print_version;

#define MAX_FILENAME_LEN 255
char output_file[MAX_FILENAME_LEN] = "struct_map.dot";

static struct conf_load conf_load;
#if 0
/** smap hash **/
struct smap_struct_hash_node {
	struct hlist_head hash_node;
	char *name;
};

#define SMAP_HASH_SIZE 512
struct smap_struct_hash *smap_hash[SMAP_HASH_SIZE];

static int smap__hash_init()
{
}

static uint64_t smap__hash_fn(const char *name)
{
}

static int smap__hash_add_node(char *struct_name)
{
	struct smap_struct_hash *inode = NULL;
	struct hlist_node *pos;
	struct smap_struct_hash_node *hnode = calloc(1, sizeof(*hnode));
	if (!hnode) {
		fprintf("%s: not enogh mem\n");
		assert(1);
	}

	hnode.name = struct_name;
	smap_hash[smap__hash_fn(struct_name)];

}

/** smap hash **/
#endif

void print_variable(struct cu *cu, struct variable *var)
{
	struct ip_tag *itag = &var->ip;
	struct tag *tag  = &itag->tag;

	tag__fprintf(tag, cu, NULL, stdout);
	putchar('\n');
	printf("var::external:%d, decl:%d || ", var->external, var->declaration);
	printf("ip_tag:: addr:%p || ", itag->addr);
	printf("tag:: type:0x%x, tag:%d, visited:%d, top_level:%d, recursivity_level:%d\n",
		tag->type, tag->tag, tag->visited, tag->top_level, tag->recursivity_level);
}

static int cu_struct_iterator(struct cu *cu, void *cookie __unused)
{
	struct tag *pos;
	uint32_t id;

	cu__for_each_type(cu, id, pos) {
		struct variable *var = tag__variable(pos);
		print_variable(cu, var);
	}

	return 0;
}


static const struct argp_option smap__options[] = {
	{
		.key  = 's',
		.name = "structs",
		.arg  = "STRUCT_LIST",
		.doc  = "descend only into these structs",

	},
	{
		.key  = 'o',
		.name = "output_name",
		.arg  = "OUTPUT_FILE",
		.doc  = "set name of output file",
	},
	{
		.name = NULL,
	}
};

/* parse comma separated struct list into hash */
static smap__parse_structs(char *argp)
{
	char *p = argp;

	do {
		if (*argp == ',') {
			*argp = 0;
			p = argp + 1;
		}

		argp++;
	} while (argp);
}

static error_t smap__options_parser(int key, char *arg,
				      struct argp_state *state __unused)
{
	switch (key) {
	case 's':
		assert(0);
		break;
	case 'o':
		assert(strlen(arg) < MAX_FILENAME_LEN);
		strncpy(output_file, arg, MAX_FILENAME_LEN);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static const char smap__args_doc[] = "FILE_LIST";

static struct argp smap__argp = {
	.options  = smap__options,
	.parser	  = smap__options_parser,
	.args_doc = smap__args_doc,
};

int main(int argc, char *argv[])
{
	int err, remaining, rc = EXIT_FAILURE;

	if (argp_parse(&smap__argp, argc, argv, 0, &remaining, NULL) ||
	    remaining == argc) {
                argp_help(&smap__argp, stderr, ARGP_HELP_SEE, argv[0]);
                goto out;
	}

	if (dwarves__init(0)) {
		fputs("smap: insufficient memory\n", stderr);
		goto out;
	}

	struct cus *cus = cus__new();
	if (cus == NULL) {
		fputs("smap: insufficient memory\n", stderr);
		goto out_dwarves_exit;
	}

	printf("remaining:%d\n", remaining);
	err = cus__load_files(cus, &conf_load, argv + remaining);
	if (err != 0)
		goto out_cus_delete;

	cus__for_each_cu(cus, cu_struct_iterator, NULL, NULL);

out_cus_delete:
	cus__delete(cus);
out_dwarves_exit:
	dwarves__exit();
out:
	return 0;
}
