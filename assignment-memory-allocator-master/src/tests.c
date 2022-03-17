#include "tests.h"
#include <stdio.h>
#include "mem.h"
#include "mem_internals.h"
#include "util.h"
#include <unistd.h>

#define HEAP_INIT_SIZE 10000
#define DATA_INIT_SIZE 1000

static void* heap_init_test() {
	void *heap = heap_init(HEAP_INIT_SIZE);
    	if (heap == NULL) {
        	err("Error of initialized heap for tests.");
    	}
    	return heap;
}

static struct block_header* get_block_header(void* contents){
	return (struct block_header*) ((uint8_t*) contents - offsetof(struct block_header, contents));
}

static struct block_header *get_block_from_allocated_data(void *data) {
	return (struct block_header*) ((uint8_t*) data - offsetof(struct block_header, contents));
}

static void allocate_pages_after_block(struct block_header *block) {
	void *addr = (uint8_t*) block + size_from_capacity(block->capacity).bytes;
	addr = (uint8_t*) (getpagesize() * ((size_t) addr / getpagesize() + (((size_t) addr % getpagesize()) > 0 )));
	addr = map_pages(addr, 1024, MAP_FIXED);
}

/* Обычное успешное выделение памяти */
static void test_1(struct block_header *heap) {
	printf("---------------- TEST 1 ----------------\n");
	
	void *data = _malloc(DATA_INIT_SIZE);
	
	debug_heap(stdout, heap);
	
	if (data == NULL) err("_malloc returned NULL!");
	if (heap->is_free) err("_malloc returned empty block!");
	if (heap->capacity.bytes != DATA_INIT_SIZE) err("_malloc returned incorrect capacity");
	
	debug_heap(stdout, heap);
	
	_free(data);
	if (!heap->is_free) err("block is not empty after test!");
	
	debug_heap(stdout, heap);
	
	printf("\nTEST 1 status: ALL RIGHT\n\n");
}

/* Освобождение одного блока из нескольких выделенных */
static void test_2(struct block_header *heap) {
	printf("---------------- TEST 2 ----------------\n");
	
	void *data_1 = _malloc(DATA_INIT_SIZE);
	void *data_2 = _malloc(DATA_INIT_SIZE);
	void *data_3 = _malloc(DATA_INIT_SIZE);
	if (data_1 == NULL || data_2 == NULL || data_3 == NULL) {
		err("_malloc returned NULL!");
	}
	
	struct block_header* data_1_block = get_block_header(data_1);
	struct block_header* data_2_block = get_block_header(data_2);
	struct block_header* data_3_block = get_block_header(data_3);
	
	debug_heap(stdout, heap);

	_free(data_1);
	
	if (!data_1_block->is_free) err("First block is not empty after _free");
	if (data_2_block->is_free) err("Second block is empty, it is wrong");
	if (data_3_block->is_free) err("Third block is empty, it is wrong");
	
	debug_heap(stdout, heap);
	
	_free(data_2);
	_free(data_3);
	
	debug_heap(stdout, heap);
	
	printf("\nTEST 2 status: ALL RIGHT\n\n");
}

/* Освобождение двух блоков из нескольких выделенных */
void test_3(struct block_header *heap) {
	printf("---------------- TEST 3 ----------------\n");
	
	void *data_1 = _malloc(DATA_INIT_SIZE);
	void *data_2 = _malloc(DATA_INIT_SIZE);
	void *data_3 = _malloc(DATA_INIT_SIZE);
	if (data_1 == NULL || data_2 == NULL || data_3 == NULL) {
		err("_malloc returned NULL!");
	}
	
	struct block_header* data_1_block = get_block_header(data_1);
	struct block_header* data_2_block = get_block_header(data_2);
	struct block_header* data_3_block = get_block_header(data_3);
	
	debug_heap(stdout, heap);

	_free(data_1);
	_free(data_2);
	
	debug_heap(stdout, heap);
	
	if (!data_1_block->is_free) err("First block is not empty after _free");
	if (!data_2_block->is_free) err("Second block is not empty after _free");
	if (data_3_block->is_free) err("Third block is empty, it is wrong");
	
	_free(data_3);
	
	debug_heap(stdout, heap);
	
	printf("\nTEST 3 status: ALL RIGHT\n\n");
}

/* Память закончилась, новый регион памяти расширяет старый */
void test_4(struct block_header *heap) {
	printf("---------------- TEST 4 ----------------\n");
	
	void *data_1 = _malloc(DATA_INIT_SIZE * 10);
	void *data_2 = _malloc(DATA_INIT_SIZE * 10);
	if (data_1 == NULL || data_2 == NULL) err("_malloc returned NULL!");
	
	struct block_header* data_1_block = get_block_header(data_1);
	struct block_header* data_2_block = get_block_header(data_2);
	
	debug_heap(stdout, heap);

	_free(data_1);
	_free(data_2);
	
	debug_heap(stdout, heap);
	
	printf("\nTEST 4 status: ALL RIGHT\n\n");
}

/* Память закончилась, старый регион памяти не расширить из-за другого выделенного диапазона адресов, новый регион выделяется в другом месте */
void test_5(struct block_header *heap) {
	printf("---------------- TEST 5 ----------------\n");
	
	void *data_1 = _malloc(DATA_INIT_SIZE * 10);
	if (data_1 == NULL) err("_malloc returned NULL!");
	
	struct block_header *heap_addr = heap;
	while(heap_addr->next != NULL) heap_addr = heap_addr->next;
	
	allocate_pages_after_block(heap_addr);
	
	debug_heap(stdout, heap);
	
	void *data_2 = _malloc(DATA_INIT_SIZE * 50);
	
	struct block_header *data_block = get_block_from_allocated_data(data_2);
	
	if (data_block == heap_addr) err ("Created block is next to the first heap!");
	
	debug_heap(stdout, heap);
	
	_free(data_1);
	_free(data_2);
	
	debug_heap(stdout, heap);
	
	printf("\nTEST 5 status: ALL RIGHT\n\n");
}

/* Запуск всех тестов */
void testing_all_tests() {
	struct block_header *heap = heap_init_test();
	test_1(heap);
	test_2(heap);
	test_3(heap);
	test_4(heap);
	test_5(heap);
	printf("ALL TESTS status: ALL RIGHT\n");
}
