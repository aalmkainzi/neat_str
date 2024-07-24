#include "neat_core.h"
#include <stdalign.h>

void *neat_default_allocator_alloc(void *ctx, size_t alignment, size_t n, size_t *actual)
{
    (void) alignment;
    (void) ctx;
    if(actual) *actual = n;
    return malloc(n);
}

void neat_default_allocator_dealloc(void *ctx, void *ptr, size_t n)
{
    (void) ctx;
    (void) n;
    free(ptr);
}

void *neat_default_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size, size_t *actual)
{
    (void) ctx;
    (void) alignment;
    (void) old_size;
    if(actual) *actual = new_size;
    return realloc(ptr, new_size);
}

void neat_default_allocator_init(void **ctx, void *arg)
{
    (void) ctx;
    (void) arg;
}

void neat_default_allocator_deinit(void *ctx)
{
    (void) ctx;
}

void *neat_noop_allocator_alloc(void *ctx, size_t alignment, size_t n, size_t *actual)
{
    (void) ctx;
    (void) alignment;
    (void) n;
    (void) actual;
    return NULL;
}

void neat_noop_allocator_dealloc(void *ctx, void *ptr, size_t n)
{
    (void) ctx;
    (void) ptr;
    (void) n;
}

void *neat_noop_allocator_realloc(void *ctx, void *ptr, size_t alignment, size_t old_size, size_t new_size, size_t *actual)
{
    (void) ctx;
    (void) ptr;
    (void) alignment;
    (void) old_size;
    (void) new_size;
    (void) actual;
    return NULL;
}

void neat_noop_allocator_init(void **ctx, void *arg)
{
    (void) ctx;
    (void) arg;
}

void neat_noop_allocator_deinit(void *ctx)
{
    (void) ctx;
}

void *neat_allocator_invoke_alloc(Neat_Allocator allocator, size_t alignment, size_t obj_size, size_t nb, size_t *actual)
{
    return allocator.alloc(allocator.ctx, alignment, nb * obj_size, actual);
}

void neat_allocator_invoke_dealloc(Neat_Allocator allocator, void *ptr, size_t obj_size, size_t nb)
{
    allocator.dealloc(allocator.ctx, ptr, nb * obj_size);
}

void *neat_allocator_invoke_realloc(Neat_Allocator allocator, void *ptr, size_t alignment, size_t obj_size, size_t old_nb, size_t new_nb, size_t *actual)
{
    return allocator.realloc(allocator.ctx, ptr, alignment, old_nb * obj_size, new_nb * obj_size, actual);
}


Neat_Allocator neat_get_default_allocator()
{
    return
    (Neat_Allocator){
        .alloc   = neat_default_allocator_alloc,
        .dealloc = neat_default_allocator_dealloc,
        .realloc = neat_default_allocator_realloc,
        .init    = neat_default_allocator_init,
        .deinit  = neat_default_allocator_deinit,
    };
}

Neat_Allocator neat_get_noop_allocator()
{
    return
    (Neat_Allocator){
        .alloc   = neat_noop_allocator_alloc,
        .dealloc = neat_noop_allocator_dealloc,
        .realloc = neat_noop_allocator_realloc,
        .init    = neat_noop_allocator_init,
        .deinit  = neat_noop_allocator_deinit,
    };
}
