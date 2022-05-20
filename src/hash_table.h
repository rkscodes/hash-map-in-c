typedef struct
{
  char *key;
  char *val;
} ht_item;

typedef struct
{
  int size;
  int count;
  ht_item **items;
} ht_hash_table;

ht_hash_table *ht_new();
void ht_del_hash_table(ht_hash_table *table);
void ht_insert(ht_hash_table *ht, const char *key, const char *val);
char *ht_search(ht_hash_table *ht, const char *key);
void ht_delete(ht_hash_table *h, const char *key);