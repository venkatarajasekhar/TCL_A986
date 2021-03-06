
#ifndef _SS_HASHTAB_H_
#define _SS_HASHTAB_H_

#define HASHTAB_MAX_NODES	0xffffffff

struct hashtab_node {
	void *key;
	void *datum;
	struct hashtab_node *next;
};

struct hashtab {
	struct hashtab_node **htable;	/* hash table */
	u32 size;			/* number of slots in hash table */
	u32 nel;			/* number of elements in hash table */
	u32 (*hash_value)(struct hashtab *h, const void *key);
					/* hash function */
	int (*keycmp)(struct hashtab *h, const void *key1, const void *key2);
					/* key comparison function */
};

struct hashtab_info {
	u32 slots_used;
	u32 max_chain_len;
};

struct hashtab *hashtab_create(u32 (*hash_value)(struct hashtab *h, const void *key),
			       int (*keycmp)(struct hashtab *h, const void *key1, const void *key2),
			       u32 size);

int hashtab_insert(struct hashtab *h, void *k, void *d);

void *hashtab_search(struct hashtab *h, const void *k);

void hashtab_destroy(struct hashtab *h);

int hashtab_map(struct hashtab *h,
		int (*apply)(void *k, void *d, void *args),
		void *args);

/* Fill info with some hash table statistics */
void hashtab_stat(struct hashtab *h, struct hashtab_info *info);

#endif	/* _SS_HASHTAB_H */
