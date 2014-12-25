/*
 * archive_entry_beattr.c
 * Copyright (c) 2014 Markus Himmel
 * Distributed under the terms of the MIT license
 */

#ifdef __HAIKU__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archive_platform.h"
#include "archive_entry.h"
#include "archive_entry_private.h"

void
archive_entry_beattr_clear(struct archive_entry *entry)
{
	struct ae_beattr *a = entry->beattr_head, *b;

	while (a != NULL) {
		b = a->next;
		free(a->name);
		free(a->data);
		free(a);
		a = b;
	}

	entry->beattr_head = NULL;
}

void
archive_entry_beattr_add_entry(struct archive_entry *entry, const char *name,
	uint32_t type, int64_t size, const void *data)
{
	struct ae_beattr *tmp;
	tmp = (struct ae_beattr *)malloc(sizeof(struct ae_beattr));

	if (tmp == NULL)
		return; /* malloc failed */

	tmp->name = strdup(name);
	tmp->type = type;

	tmp->data = malloc(size);
	if (tmp->data != NULL) {
		memcpy(tmp->data, data, size);
		tmp->size = size;
	} else {
		/* if malloc fails here, do not fail, but treat the attribute as if
		 * it had no data */
		tmp->size = 0;
	}

	/* Put the new element at the beginning of the linked list */
	tmp->next = entry->beattr_head;
	entry->beattr_head = tmp;
}

int
archive_entry_beattr_count(struct archive_entry *entry)
{
	struct ae_beattr *tmp = entry->beattr_head;
	int count = 0;

	while (tmp != NULL) {
		count++;
		tmp = tmp->next;
	}

	return count;
}

int
archive_entry_beattr_reset(struct archive_entry *entry)
{
	entry->beattr_p = entry->beattr_head;

	return archive_entry_beattr_count(entry);
}

int
archive_entry_beattr_next(struct archive_entry *entry, const char **name,
	uint32_t *type, int64_t *size, const void **data)
{
	if (entry->beattr_p != NULL) {
		*name = entry->beattr_p->name;
		*type = entry->beattr_p->type;
		*size = entry->beattr_p->size;
		*data = entry->beattr_p->data;

		entry->beattr_p = entry->beattr_p->next;

		return ARCHIVE_OK;
	} else {
		*name = NULL;
		*type = 0;
		*size = 0;
		*data = NULL;

		return ARCHIVE_WARN;
	}
}
#endif /* __HAIKU__ */
