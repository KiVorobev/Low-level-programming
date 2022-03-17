#include <stdarg.h>
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"

void debug_block(struct block_header* b, const char* fmt, ... );
void debug(const char* fmt, ... );

/* Вычисляем размер блока, зная его вместимость */
extern inline block_size size_from_capacity(block_capacity cap);

/* Вычисляем вместимость блока, зная его размер */
extern inline block_capacity capacity_from_size(block_size sz);

/* Проверяем, хватит ли вместимости блока для запрошенного кол-ва байт */
static bool block_is_big_enough(size_t query, struct block_header* block) { return block->capacity.bytes >= query; }

static size_t pages_count(size_t mem) { return mem / getpagesize() + ((mem % getpagesize()) > 0); }

static size_t round_pages( size_t mem ) { return getpagesize() * pages_count( mem ) ; }

/* Инициализация блока */
static void block_init( void* restrict addr, block_size block_sz, void* restrict next ) {
  *((struct block_header*)addr) = (struct block_header) {
    .next = next,
    .capacity = capacity_from_size(block_sz),
    .is_free = true
  };
}

/* Возвращаем акутальный размер для региона памяти (запрашиваемый или минимальный) */
static size_t region_actual_size( size_t query ) { return size_max( round_pages( query ), REGION_MIN_SIZE ); }

extern inline bool region_is_invalid( const struct region* r );

void* map_pages(void const* addr, size_t length, int additional_flags) {
  return mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags , 0, 0 );
}

/*  аллоцировать регион памяти и инициализировать его блоком */
static struct region alloc_region( void const * addr, size_t query ) {
  query = region_actual_size(query);
  void *region_addr = map_pages(addr, query, MAP_FIXED_NOREPLACE);
  
  struct region region;
  
  if (region_addr == MAP_FAILED) region_addr = map_pages(addr, query, 0);
  if (region_addr == MAP_FAILED) return REGION_INVALID;
  
  region = (struct region) {region_addr, query, false};
  
  block_init(region_addr, (block_size) {query}, NULL);
  
  return region;
}

/* Возвращает ссылку на начало следующего блока */
static void* block_after( struct block_header const* block );

/* Инициализация кучи */
void* heap_init( size_t initial ) {
  const struct region region = alloc_region( HEAP_START, initial );
  if ( region_is_invalid(&region) ) return NULL;

  return region.addr;
}

#define BLOCK_MIN_CAPACITY 24

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

/* Проверяем, разделяем ли этот блок */
static bool block_splittable( struct block_header* restrict block, size_t query) {
  return block-> is_free && query + offsetof( struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}

/* Делим блок памяти, если он большой для запрашиваемого объема */
static bool split_if_too_big( struct block_header* block, size_t query ) {
  
  if (!block_splittable(block, query)) return false;
  
  const block_size second_block_size = (block_size) {size_from_capacity(block->capacity).bytes - query};
  
  block->capacity.bytes = query;
  
  void *start_of_second_block = block_after(block);
  
  block_init(start_of_second_block, second_block_size, NULL);
  
  block->next = start_of_second_block;
  
  return true;
}


/*  --- Слияние соседних свободных блоков --- */

static void* block_after( struct block_header const* block ) {
  return  (void*) (block->contents + block->capacity.bytes);
}

static bool blocks_continuous (struct block_header const* fst, struct block_header const* snd ) {
  return (void*)snd == block_after(fst);
}

/* Проверка на возможность слияния двух блоков */
static bool mergeable(struct block_header const* restrict fst, struct block_header const* restrict snd) {
  return fst->is_free && snd->is_free && blocks_continuous( fst, snd ) ;
}

static bool try_merge_with_next( struct block_header* block ) {
  if (!block->next || !mergeable(block, block->next)) return false;
  block->capacity.bytes += size_from_capacity(block->next->capacity).bytes;
  return true;
}


/*  --- ... ecли размера кучи хватает --- */

struct block_search_result {
  enum {BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED} type;
  struct block_header* block;
};

/* Перебор блоков, поиск хорошего или последнего */
static struct block_search_result find_good_or_last ( struct block_header* restrict block, size_t sz ) {
  /* Пройдемся по всем блокам */
  while (block != NULL) {
  	while(block->next != NULL) {
  		/* Делаем блок максимально большим */
  		while(try_merge_with_next(block));
  		break;
  	}
  	/* Проверяем свободен ли блок и подходит ли нам по размеру */
  	if (block->is_free && block_is_big_enough(sz, block))
  	return (struct block_search_result) {.type = BSR_FOUND_GOOD_BLOCK, .block = block};
  	
  	/* Если блок не свободен/не подходит нам по размеру и при этом следующего блока нет */
  	if (block->next == NULL)
  	return (struct block_search_result) {.type = BSR_REACHED_END_NOT_FOUND, .block = block};
  	
  	/* Если блок не последний, то переходим к следующему блоку */
  	block = block->next;
  }
  return (struct block_search_result) {.type = BSR_CORRUPTED, .block = NULL};
}

/*  Попробовать выделить память в куче начиная с блока `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
static struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block ) {
  struct block_search_result result = find_good_or_last(block, query);
  if (result.type == BSR_FOUND_GOOD_BLOCK) split_if_too_big(result.block, query);
  
  return result;
}

/* Расширение кучи */
static struct block_header* grow_heap( struct block_header* restrict last, size_t query ) {
  void *addr = size_from_capacity(last->capacity).bytes + last;
  struct region region = alloc_region(addr, query);
  
  if (region_is_invalid(&region)) return NULL;
  
  last->next = region.addr;
  
  if (try_merge_with_next(last)) return last;
  return last->next;
}

/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
static struct block_header* memalloc( size_t query, struct block_header* heap_start) {
  struct block_search_result result = try_memalloc_existing(query, heap_start);
  if (result.type == BSR_CORRUPTED) return NULL;
  if (result.type == BSR_REACHED_END_NOT_FOUND) {
  result.block = grow_heap(result.block, query);
  result.block = try_memalloc_existing(query, result.block).block;
  }
  result.block->is_free = false;
  return result.block;
}

void* _malloc( size_t query ) {
  struct block_header* const addr = memalloc( query, (struct block_header*) HEAP_START );
  if (addr) return addr->contents;
  else return NULL;
}

static struct block_header* block_get_header(void* contents) {
  return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void _free( void* mem ) {
  if (!mem) return ;
  struct block_header* header = block_get_header( mem );
  header->is_free = true;
  /* Объединение блока со всеми последующими свободных блоками */
  while (try_merge_with_next(header));
}
