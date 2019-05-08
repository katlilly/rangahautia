/*
	HASH_TABLE.C
	------------
	Copyright (c) 2019 Andrew Trotman
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*
	HASH_ITERATE_CALLBACK
	---------------------
*/
typedef void (*hash_iterate_callback)(char *key, void *value);

/*
	HASH_ENTRY
	----------
*/
typedef struct
	{
	char *key;
	void *value;
	} hash_entry;

/*
	HASH_TABLE
	----------
*/
typedef struct _hash_table
	{
	void (*destroy)(struct _hash_table *this);
	bool (*add)(struct _hash_table *this, char *key, void *value);
	void *(*find)(struct _hash_table *this, char *key);
	void (*iterate)(struct _hash_table *this, hash_iterate_callback callback);

	int table_size;
	hash_entry *table;
	} hash_table;

/*
	Forward declarations
	--------------------
*/
void hash_destroy(hash_table *this);
bool hash_add(hash_table *this, char *key, void *value);
void *hash_find(hash_table *this, char *key);
void hash_iterate(hash_table *this, hash_iterate_callback callback);

/*
	HASH_STRING()
	-------------
*/
uint32_t hash_string(hash_table *this, char *key)
	{
	uint32_t value = 0;
	for (char *ch = key; *ch != '\0'; ch++)
		value = value * 32 + *ch;

	return value % this->table_size;
	}

/*
	HASH_CREATE()
	-------------
*/
hash_table *hash_create(uint32_t table_size)
	{
	size_t table_size_in_bytes;
	hash_table *new = malloc(sizeof(*new));

	new->destroy = hash_destroy;
	new->add = hash_add;
	new->find = hash_find;
	new->iterate = hash_iterate;

	new->table_size = table_size;
	table_size_in_bytes = sizeof(*new->table) * new->table_size;
	new->table = malloc(table_size_in_bytes);
	memset(new->table, 0, table_size_in_bytes);

	return new;
	}

/*
	HASH_DESTROY()
	--------------
*/
void hash_destroy(hash_table *this)
	{
	uint32_t index;

	for (index = 0; index < this->table_size; index++)
		if (this->table[index].key != NULL)
			free(this->table[index].key);

	free(this->table);
	free(this);
	}

/*
	HASH_ADD()
	----------
*/
bool hash_add(hash_table *this, char *key, void *value)
	{
	uint32_t index;
	uint32_t initial;
	bool inserted = false;

	initial = index = hash_string(this, key);
	do
		if (this->table[index].key == NULL)
			{						// enter into the hash table here.
			this->table[index].key = strdup(key);
	 		this->table[index].value = value;
	 		inserted = true;
	 		break;
	 		}
		else if (strcmp(this->table[index].key, key) == 0)
			return false;		// this key already exists in the hash table
		else
			index = (index + 1) % this->table_size;			// hash table collision so use direct chaining
	while (inserted == false && index != initial);

	return inserted;
	}

/*
	HASH_FIND()
	-----------
*/
void *hash_find(hash_table *this, char *key)
	{
	uint32_t index;
	uint32_t initial;

	initial = index = hash_string(this, key);
	do
		if (this->table[index].key == NULL)
			return NULL;			// nothing at this point in the table
		else if (strcmp(key, this->table[index].key) == 0)
			return this->table[index].value;		// found it!
		else
			index = (index + 1) % this->table_size;
	while (index != initial);

	return NULL;			// table is full
	}

/*
	HASH_ITERATE()
	--------------
*/
void hash_iterate(hash_table *this, hash_iterate_callback callback)
	{
	uint32_t index;

	for (index = 0; index < this->table_size; index++)
		if (this->table[index].key != NULL)
			callback(this->table[index].key, this->table[index].value);
	}

/*
	MAIN_CALLBACK()
	---------------
*/
void main_callback(char *key, void *value)
	{
	printf("%s -> %s\n", key, (char *)value);
	}

/*
	MAIN()
	------
*/
int main(int argc, const char * argv[])
	{
	int table_size = 32;
	char buffer[32];
	char random_key[32];
	char nine[32];		// keep the key for the 9th insertion (to check it later)
	uint32_t trial;
	hash_table *hash = hash_create(table_size);

	for (trial = 0; trial < table_size * 2; trial++)
		{
		sprintf(buffer, "%d", trial);
		sprintf(random_key, "%d", rand());
		if (trial == 9)
			strcpy(nine, random_key);

		if (!hash->add(hash, random_key, strdup(buffer)))
			{
			printf("failed to add:%d\n", trial);
			break;
			}
		}

	void *should_be_nine = hash->find(hash, nine);
	printf("Should be nine:%s\n\n", (char*)should_be_nine);


	void *should_fail = hash->find(hash, "nine");
	if (should_fail != NULL)
		printf("Found something when it should not have\n");

	hash->iterate(hash, main_callback);
	hash->destroy(hash);

	return 0;
	}

