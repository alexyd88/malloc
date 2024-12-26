#include <stdio.h>

struct block_meta {
    size_t size;
    struct block_meta *next;
    int free;
};

void *head = NULL;

struct block_meta *find_free_block(struct block_meta **last, size_t size) {
    struct block_meta *cur = head;
    while (cur) {
        if (cur->free && cur->size >= size)
            return cur;
        *last = cur;
        cur = cur->next;
    }
    //couldn't find enough space
    assert(0);
}

struct block_meta *request_space(struct block_meta *last, size_t size) {
    struct block_meta *block = sbrk(size);

    //sbrk failed
    if (block == (void*) -1) {
        return NULL;
    }
    if (last) {
        last->next = block;
    }
    block->size = size;
    block->free = 0;
    block->next = NULL;
    return block;
}

void *malloc(size_t size) {
    if (size <= 0)
        return NULL;
    struct block_meta *block;
    if (!head) {
        block = request_space(NULL, size);
        head = block;
        return block+1;
    }
    struct block_meta *cur = find_free_block(head, size);
    if (cur) {
        // TODO split block
        cur->free = 0;
        return cur+1;
    }
    return request_space(cur, size)+1;
}

void free(void *ptr) {
    if (!ptr)
        return;
    struct block_meta *meta_data_pointer = ptr;
    meta_data_pointer--;
    meta_data_pointer->free = 1;
    // TODO merge block
}

void *realloc(void *ptr, size_t size) {
    if (!ptr)
        return malloc(size);
    struct block_meta *block = ptr-1;
    if (block->size >= size) {
        // TODO shrink size and free some
        return ptr;
    }
    void *new = malloc(size);
    if (!new) {
        return NULL; // TODO errno??
    }
    memcpy(new, ptr, block->size);
    free(ptr);
    return ptr;
}

void *calloc(size_t num_elements, size_t element_size) {
    size_t size = num_elements * element_size;
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

int main(void) {
    printf("yo\n");
}