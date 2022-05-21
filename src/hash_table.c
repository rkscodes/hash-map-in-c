#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "prime.h"

#include "hash_table.h"
#define HT_PRIME1 137
#define HT_PRIME2 149
#define HT_INTIAL_BASE_SIZE 53

static ht_item *ht_new_item(const char *k, const char *v)
{
  ht_item *i = malloc(sizeof(ht_item));
  i->key = strdup(k);
  i->val = strdup(v);
  return i;
}
ht_hash_table *ht_new()
{
  return ht_new_sized(HT_INTIAL_BASE_SIZE);
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

static int ht_hash(const char *string, int a, int buckets)
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

static int ht_get_hash(const char *string, const int buckets, int collision)
{
  int h1 = ht_hash(string, HT_PRIME1, buckets);
  int h2 = ht_hash(string, HT_PRIME2, buckets);
  return (h1 + (h2 + 1) * collision) % buckets;
}

static ht_item HT_DELETED_ITEM = {NULL, NULL};

void ht_insert(ht_hash_table *ht, const char *key, const char *val)
{
  const int load = ht->count * 100 / ht->size;
  if (load > 70)
  {
    ht_resize_up(ht);
  }
  ht_item *item = ht_new_item(key, val);
  int index = ht_get_hash(item->key, ht->size, 0);
  ht_item *curr_item = ht->items[index];
  int i = 1;
  while (curr_item != NULL && curr_item != &HT_DELETED_ITEM)
  {
    // if with same index we found inserted key, we will delete it, so that in effect that value will
    //  be updated
    if (strcmp(curr_item->key, key) == 0)
    {
      ht_del_item(curr_item);
      ht->items[index] = item;
      return;
    }

    index = ht_get_hash(item->key, ht->size, i++);
    curr_item = ht->items[index];
  }
  ht->items[index] = item;
  ht->count++;
}

char *ht_search(ht_hash_table *ht, const char *key)
{

  int index = ht_get_hash(key, ht->size, 0);
  ht_item *curr_item = ht->items[index];
  int i = 1;
  while (curr_item != NULL)
  {
    if (curr_item)
    {
      if (strcmp(curr_item->key, key) == 0)
        return curr_item->val;
      index = ht_get_hash(key, ht->size, i++);
      curr_item = ht->items[index];
    }
  }
  return NULL;
}

// The item we want to delete could be part of collision chain,
// so if we delete it it maybe the case we would never reach other item at last location.
// so instead we would simply mark that item as deleted using global sential item

void ht_delete(ht_hash_table *ht, const char *key)
{
  const int load = ht->count * 100 / ht->size;
  if (load < 10)
  {
    ht_resize_down(ht);
  }
  int index = ht_get_hash(key, ht->size, 0);
  ht_item *curr_item = ht->items[index];
  int i = 1;
  while (curr_item)
  {
    if (curr_item != &HT_DELETED_ITEM)
    {
      if (strcmp(curr_item->key, key) == 0)
      {
        ht_del_item(curr_item);
        ht->items[index] = &HT_DELETED_ITEM;
        break;
      }
    }
    index = ht_get_hash(key, ht->size, i++);
    curr_item = ht->items[index];
  }
  ht->count--;
}

static ht_hash_table *ht_new_sized(const int base_size)
{
  ht_hash_table *ht = malloc(sizeof(ht_hash_table));
  ht->size = base_size;
  ht->size = next_prime(ht->size);
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item *));
  return ht;
}

static void ht_resize(ht_hash_table *ht, const int base_size)
{
  if (base_size < HT_INTIAL_BASE_SIZE)
    return;
  ht_hash_table *new_ht = ht_new_sized(base_size);
  for (int i = 0; i < ht->size; i++)
  {
    ht_item *item = ht->items[i];

    // not sure about && item != &HT_DELETED_ITEM
    if (item != NULL && item != &HT_DELETED_ITEM)
    {
      ht_insert(new_ht, item->key, item->val);
    }
    ht->size = new_ht->size;
    ht->count = new_ht->count;

    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item **tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
  }
}

static void ht_resize_up(ht_hash_table *ht)
{
  const int new_size = ht->size * 2;
  ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table *ht)
{
  const int new_size = ht->size / 2;
  ht_resize(ht, new_size);
}