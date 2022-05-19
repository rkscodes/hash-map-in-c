#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

static ht_item *ht_new_item(const char *k, const char *v)
{
  ht_item *i = malloc(sizeof(ht_item));
  i->key = strdup(k);
  i->val = strdup(v);
  return i;
}
ht_hash_table *ht_new()
{
  ht_hash_table *ht = malloc(sizeof(ht_hash_table));
  ht->size = 53;
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item *));
  return ht;
}

static void ht_del_item(ht_item *item)
{
  free(item->key);
  free(item->val);
  free(item);
}

void ht_del_hash_table(ht_hash_table *table)
{
  for (int i = 0; i < table->size; i++)
  {
    ht_item *item = table->items[i];
    if (item)
    {
      ht_del_item(item);
    }
  }
  free(table->items);
  free(table);
}

static int ht_hash(char *string, int a, int buckets)
{
  long hash = 0;
  const int len_string = sizeof(string);
  for (int i = 0; i < len_string; i++)
  {
    hash += (long)pow(a, len_string - (i + 1)) * string[i];
    hash = hash % buckets;
  }
  return (int)hash;
}