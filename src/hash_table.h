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