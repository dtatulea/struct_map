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
*/

#include <argp.h>
#include <assert.h>
#include <stdio.h>
#include <dwarf.h>
#include "list.h"

/* Name and version of program.  */
ARGP_PROGRAM_VERSION_HOOK_DEF = dwarves_print_version;

#define MAX_FILENAME_LEN 255
char output_file[MAX_FILENAME_LEN] = "struct_map.dot";

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

static int smap__parse()
{
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
		.arg  = "OUTPUT_FILE"
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
	} while (argp)
}

static error_t smap__options_parser(int key, char *arg,
				      struct argp_state *state __unused)
{
	switch (key) {
	case 's':
		assert(1);
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

	assert(struct_list_parsed);
	return 0;
}
