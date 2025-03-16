#include "neat_str.h"

typedef struct Fixed_Mut_String_Ref
{
    unsigned char *chars;
    unsigned int *len;
    unsigned int cap;
} Fixed_Mut_String_Ref;

Fixed_Mut_String_Ref neat_buf_as_fmutstr_ref(Neat_Buffer buf, unsigned int *len_ptr)
{
    *len_ptr = strlen((char*) buf.ptr);
    Fixed_Mut_String_Ref ret = {
        .chars = buf.ptr,
        .cap = *len_ptr,
        .len = len_ptr
    };
    return ret;
}

Fixed_Mut_String_Ref neat_sstr_ref_as_fmutstr_ref(Neat_SString_Ref sstr_ref)
{
    Fixed_Mut_String_Ref ret = {
        .chars = sstr_ref.sstr->chars,
        .cap = sstr_ref.cap,
        .len = &sstr_ref.sstr->len
    };
    return ret;
}

Fixed_Mut_String_Ref neat_strbuf_as_fmutstr_ref(Neat_String_Buffer *strbuf)
{
    Fixed_Mut_String_Ref ret = {
        .chars = strbuf->chars,
        .cap = strbuf->cap,
        .len = &strbuf->len
    };
    return ret;
}

static const long long neat_ten_pows[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000,
};

static const unsigned long long neat_ten_pows_ull[] = {
    1ull,
    10ull,
    100ull,
    1000ull,
    10000ull,
    100000ull,
    1000000ull,
    10000000ull,
    100000000ull,
    1000000000ull,
    10000000000ull,
    100000000000ull,
    1000000000000ull,
    10000000000000ull,
    100000000000000ull,
    1000000000000000ull,
    10000000000000000ull,
    100000000000000000ull,
    1000000000000000000ull,
    10000000000000000000ull,
};

// Neat_String_View neat_strv_fmutstr_ref2(Neat_Fixed_Mut_String_Ref str, unsigned int begin);
// Neat_String_View neat_strv_fmutstr_ref3(Neat_Fixed_Mut_String_Ref str, unsigned int begin, unsigned int end);

unsigned int neat_numstr_len(long long num)
{
    unsigned int len = 1;
    for(unsigned int i = 1 ; i < NEAT_CARR_LEN(neat_ten_pows) && num >= neat_ten_pows[i++] ; len++);
    return len;
}

unsigned int neat_numstr_len_ull(unsigned long long num)
{
    unsigned int len = 1;
    for(unsigned int i = 1 ; i < NEAT_CARR_LEN(neat_ten_pows_ull) && num >= neat_ten_pows_ull[i++] ; len++);
    return len;
}

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

static inline unsigned int neat_uint_min(unsigned int a, unsigned int b)
{
    return a < b ? a : b;
}

static inline unsigned int neat_uint_max(unsigned int a, unsigned int b)
{
    return a > b ? a : b;
}

static unsigned int neat_chars_strlen(unsigned char *chars, unsigned int cap)
{
    unsigned char *str_end = (unsigned char*) memchr(chars, '\0', cap);
    unsigned int len;
    
    if(str_end != NULL)
    {
        len = str_end - chars;
    }
    else
    {
        len = cap - 1;
    }
    
    return len;
}

// static unsigned int neat_mutstr_ref_len(Neat_Mut_String_Ref str)
// {
//     switch(str.ty)
//     {
//         case NEAT_DSTR_TY        : return str.str.dstr->len;
//         case NEAT_FMUTSTR_REF_TY : return *str.str.fmutstr_ref.len;
//         case NEAT_BUF_TY      : return neat_chars_strlen(str.str.carr.ptr, str.str.carr.cap);
//     };
// }

bool neat_is_strv_within(Neat_String_View base, Neat_String_View sub)
{
    uintptr_t begin = (uintptr_t) base.chars;
    uintptr_t end   = (uintptr_t) (base.chars + base.len);
    uintptr_t sub_begin = (uintptr_t) sub.chars;
    return sub_begin >= begin && sub_begin < end;
}

NEAT_NODISCARD("discarding a new DString will cause memory leak") Neat_DString neat_dstr_new(unsigned int cap, Neat_Allocator allocator, Neat_String_Error *err)
{
    // if(allocator.ctx == NULL)
    //     allocator.init(&allocator.ctx, NULL);
    
    Neat_DString ret = { 0 };
    
    ret.allocator = allocator;
    size_t actual_allocated_cap;
    
    ret.chars = neat_alloc(allocator, unsigned char, cap, &actual_allocated_cap);
    ret.cap = actual_allocated_cap;
    
    if(err)
    {
        if(ret.chars == NULL || ret.cap < cap)
            *err = NEAT_ALLOC_ERR;
        else
            *err = NEAT_OK;
    }
    
    if(ret.chars != NULL && ret.cap > 0)
    {
        ret.chars[0] = '\0';
    }
    
    return ret;
}

Neat_DString neat_new_dstr_from(Neat_String_View str, Neat_Allocator allocator, Neat_String_Error *err)
{
    Neat_String_Error dstr_new_err;
    Neat_DString ret = neat_dstr_new(str.len + 1, allocator, &dstr_new_err);
    
    if(dstr_new_err == NEAT_OK)
        *err = neat_dstr_copy(&ret, str);
    else
        *err = dstr_new_err;
    
    return ret;
}

void neat_dstr_deinit_(Neat_DString *dstr)
{
    neat_dealloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap);
    dstr->allocator.deinit(&dstr->allocator.ctx);
    dstr->cap = 0;
    dstr->len = 0;
    dstr->chars = NULL;
}

void neat_dstr_shrink_to_fit_(Neat_DString *dstr)
{
    size_t actual_new_cap;
    dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, dstr->len + 1, &actual_new_cap);
    dstr->cap = actual_new_cap;
}

Neat_String_Error neat_dstr_maybe_grow(Neat_DString *dstr, unsigned int len_to_append)
{
    if(dstr->cap - dstr->len <= len_to_append)
    {
        // grow
        unsigned int new_cap = neat_uint_max(dstr->cap * 2, dstr->cap + len_to_append);
        
        size_t actual_allocated_cap;
        dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, new_cap, &actual_allocated_cap);
        dstr->cap = actual_allocated_cap;
        
        if(dstr->chars == NULL || dstr->cap < new_cap)
        {
            return NEAT_ALLOC_ERR;
        }
    }
    
    return NEAT_OK;
}

Neat_String_Error neat_dstr_append_strv(Neat_DString *dstr, Neat_String_View src)
{
    Neat_String_View to_append = src;
    Neat_String_Error err = NEAT_OK;
    if(neat_is_strv_within(neat_strv_dstr_ptr2(dstr, 0), to_append))
    {
        unsigned int begin_idx = to_append.chars - dstr->chars;
        err = neat_dstr_maybe_grow(dstr, to_append.len);
        to_append = (Neat_String_View){
            .len   = to_append.len,
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        err = neat_dstr_maybe_grow(dstr, to_append.len);
    }
    
    if(err == NEAT_OK)
    {
        memmove(dstr->chars + dstr->len, to_append.chars, to_append.len * sizeof(unsigned char));
        
        dstr->len += to_append.len;
        dstr->chars[dstr->len] = '\0';
    }
    
    return err;
}

Neat_String_Error neat_dstr_prepend_strv(Neat_DString *dstr, Neat_String_View src)
{
    Neat_String_View to_prepend = src;
    Neat_String_Error err = NEAT_OK;
    
    if(neat_is_strv_within(neat_strv_dstr_ptr2(dstr, 0), src))
    {
        unsigned int begin_idx = src.chars - dstr->chars;
        err = neat_dstr_maybe_grow(dstr, src.len);
        to_prepend = (Neat_String_View){
            .len = src.len, 
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        err = neat_dstr_maybe_grow(dstr, to_prepend.len);
    }
    
    if(err == NEAT_OK)
    {
        memmove(dstr->chars + to_prepend.len, dstr->chars, dstr->len);
        memmove(dstr->chars, to_prepend.chars, to_prepend.len);
        
        dstr->len += to_prepend.len;
        dstr->chars[dstr->len] = '\0';
    }
    
    return err;
}

Neat_String_Error neat_dstr_insert_strv(Neat_DString *dstr, Neat_String_View src, unsigned int idx)
{
    if(idx > dstr->len)
    {
        return NEAT_INDEX_OUT_OF_BOUNDS;
    }
    
    Neat_String_View to_insert = src;
    
    if(neat_is_strv_within(neat_strv_dstr_ptr2(dstr, 0), src))
    {
        unsigned int begin_idx = src.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, src.len);
        to_insert = (Neat_String_View){
            .len = src.len, 
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        neat_dstr_maybe_grow(dstr, to_insert.len);
    }
    
    memmove(dstr->chars + idx + to_insert.len, dstr->chars + idx, dstr->len - idx);
    memmove(dstr->chars + idx, to_insert.chars, to_insert.len);
    
    dstr->len += to_insert.len;
    dstr->chars[dstr->len] = '\0';
    
    return true;
}

void neat_dstr_append_tostr_(Neat_DString *dstr, Neat_DString tostr)
{
    neat_dstr_append_strv(dstr, neat_strv_dstr2(tostr, 0));
    neat_dstr_deinit_(&tostr);
}

void neat_dstr_prepend_tostr_(Neat_DString *dstr, Neat_DString tostr)
{
    neat_dstr_prepend_strv(dstr, neat_strv_dstr2(tostr, 0));
    neat_dstr_deinit_(&tostr);
}

Neat_String_Error neat_dstr_insert_tostr_(Neat_DString *dstr, Neat_DString tostr, unsigned int idx)
{
    Neat_String_Error ret = neat_dstr_insert_strv(dstr, neat_strv_dstr2(tostr, 0), idx);
    neat_dstr_deinit_(&tostr);
    return ret;
}

Neat_String_Error neat_dstr_ensure_cap_(Neat_DString *dstr, unsigned int at_least)
{
    if(dstr->cap < at_least)
    {
        size_t actual_allocated_cap;
        unsigned char *save = dstr->chars;
        dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, at_least, &actual_allocated_cap);
        dstr->cap = actual_allocated_cap;
        
        if(dstr->chars == NULL)
        {
            dstr->chars = save;
            return NEAT_ALLOC_ERR;
        }
        if(dstr->cap < at_least)
        {
            return NEAT_ALLOC_ERR;
        }
    }
    
    return NEAT_OK;
}

char *neat_cstr_as_cstr(char *str)
{
    return str;
}

char *neat_ucstr_as_cstr(unsigned char *str)
{
    return (char*) str;
}

char *neat_dstr_as_cstr(Neat_DString str)
{
    return (char*) str.chars;
}

char *neat_dstr_ptr_as_cstr(const Neat_DString *str)
{
    return (char*) str->chars;
}

char *neat_strv_as_cstr(const Neat_String_View str)
{
    return (char*) str.chars;
}

char *neat_strv_ptr_as_cstr(const Neat_String_View *str)
{
    return (char*) str->chars;
}

char *neat_strbuf_as_cstr(const Neat_String_Buffer str)
{
    return (char*) str.chars;
}

char *neat_strbuf_ptr_as_cstr(const Neat_String_Buffer *str)
{
    return (char*) str->chars;
}

char *neat_sstr_ref_as_cstr(const Neat_SString_Ref str)
{
    return (char*) str.sstr->chars;
}

char *neat_mutstr_ref_as_cstr(const Neat_Mut_String_Ref str)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY     : return (char*) str.str.dstr->chars;
        case NEAT_STRBUF_TY   : return (char*) str.str.strbuf.chars;
        case NEAT_SSTR_REF_TY : return (char*) str.str.sstr_ref.sstr->chars;
        case NEAT_BUF_TY      : return (char*) str.str.carr.ptr;
    };
}

unsigned char neat_cstr_char_at(const char *str, unsigned int idx)
{
    return str[idx];
}

unsigned char neat_ucstr_char_at(const unsigned char *str, unsigned int idx)
{
    return str[idx];
}

unsigned char neat_dstr_char_at(const Neat_DString str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_dstr_ptr_char_at(const Neat_DString *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_strv_char_at(const Neat_String_View str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_strv_ptr_char_at(const Neat_String_View *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_strbuf_char_at(Neat_String_Buffer str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_strbuf_ptr_char_at(const Neat_String_Buffer *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_sstr_ref_char_at(Neat_SString_Ref str, unsigned int idx)
{
    return str.sstr->chars[idx];
}

unsigned char neat_mutstr_ref_char_at(Neat_Mut_String_Ref str, unsigned int idx)
{
    return neat_mutstr_ref_as_cstr(str)[idx];
}

unsigned int neat_dstr_cap(Neat_DString str)
{
    return str.cap;
}

unsigned int neat_dstr_ptr_cap(const Neat_DString *str)
{
    return str->cap;
}

unsigned int neat_strbuf_cap(Neat_String_Buffer str)
{
    return str.cap;
}

unsigned int neat_strbuf_ptr_cap(const Neat_String_Buffer *str)
{
    return str->cap;
}

unsigned int neat_sstr_ref_cap(const Neat_SString_Ref str)
{
    return str.cap;
}

unsigned int neat_buf_cap(const Neat_Buffer buf)
{
    return buf.cap;
}

unsigned int neat_mutstr_ref_cap(Neat_Mut_String_Ref str)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY     : return str.str.dstr->cap;
        case NEAT_STRBUF_TY   : return str.str.strbuf->cap;
        case NEAT_SSTR_REF_TY : return str.str.sstr_ref.cap;
        case NEAT_BUF_TY      : return str.str.carr.cap;
    };
}

Neat_String_Error neat_fmutstr_ref_insert(Fixed_Mut_String_Ref dst, Neat_String_View src, unsigned int idx)
{
    unsigned int len = *dst.len;
    if(idx > len)
    {
        return NEAT_INDEX_OUT_OF_BOUNDS;
    }
    
    unsigned int nb_chars_to_insert = neat_uint_min(dst.cap - len - 1, src.len);
    
    // shift right
    memmove(dst.chars + idx + nb_chars_to_insert, dst.chars + idx, len - idx);
    
    // insert the src
    memmove(dst.chars + idx, src.chars, nb_chars_to_insert);
    
    len += nb_chars_to_insert;
    
    *dst.len = len;
    
    return nb_chars_to_insert == src.len ? NEAT_OK : NEAT_DST_TOO_SMALL;
}

Neat_String_Error neat_buf_insert(Neat_Buffer dst, Neat_String_View src, unsigned int idx)
{
    unsigned int len;
    Fixed_Mut_String_Ref as_fmutstr_ref = neat_buf_as_fmutstr_ref(dst, &len);
    return neat_fmutstr_ref_insert(as_fmutstr_ref, src, idx);
}

Neat_String_Error neat_sstr_ref_insert(Neat_SString_Ref dst, Neat_String_View src, unsigned int idx)
{
    Fixed_Mut_String_Ref as_fmutstr_ref = neat_sstr_ref_as_fmutstr_ref(dst);
    return neat_fmutstr_ref_insert(as_fmutstr_ref, src, idx);
}

Neat_String_Error neat_strbuf_insert(Neat_String_Buffer *dst, Neat_String_View src, unsigned int idx)
{
    Fixed_Mut_String_Ref as_fmutstr_ref = neat_strbuf_as_fmutstr_ref(dst);
    return neat_fmutstr_ref_insert(as_fmutstr_ref, src, idx);
}

Neat_String_Error neat_mutstr_ref_insert(Neat_Mut_String_Ref dst, Neat_String_View src, unsigned int idx)
{
    switch(dst.ty)
    {
        case NEAT_DSTR_TY     : return neat_dstr_insert_strv(dst.str.dstr, src, idx);
        case NEAT_STRBUF_TY   : return neat_strbuf_insert(dst.str.strbuf, src, idx);
        case NEAT_SSTR_REF_TY : return neat_sstr_ref_insert(dst.str.sstr_ref, src, idx);
        case NEAT_BUF_TY      : return neat_buf_insert(dst.str.carr, src, idx);
    };
}

bool neat_strv_equal(Neat_String_View str1, Neat_String_View str2)
{
    return
    (str1.len == str2.len) &&
    (memcmp(str1.chars, str2.chars, str1.len) == 0);
}

Neat_String_View neat_strv_memmem(const Neat_String_View hay, const Neat_String_View needle)
{
    if(hay.chars == NULL || needle.chars == NULL || hay.len == 0 || needle.len > hay.len)
        return (Neat_String_View){.chars = NULL, .len = 0};
    if(needle.len == 0)
        return (Neat_String_View){.chars = hay.chars, .len = 0};
    
    unsigned char *max_possible_ptr = &hay.chars[hay.len] - needle.len;
    unsigned char *first_char = hay.chars;
    unsigned int remaining_len = hay.len;
    
    while(first_char && first_char <= max_possible_ptr)
    {
        if(memcmp(first_char, needle.chars, needle.len) == 0)
            return (Neat_String_View){.chars = first_char, .len = needle.len};
        first_char = memchr(first_char + 1, needle.chars[0], remaining_len - 1);
        remaining_len = hay.len - (first_char - hay.chars);
    }
    
    return (Neat_String_View){.chars = NULL, .len = 0};
}

Neat_String_Error neat_fmutstr_ref_copy(Fixed_Mut_String_Ref dst, Neat_String_View src)
{
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - 1);
    
    memmove(dst.chars, src.chars, chars_to_copy * sizeof(unsigned char));
    dst.chars[chars_to_copy] = '\0';
    
    *dst.len = chars_to_copy;
    
    return chars_to_copy == src.len ? NEAT_OK : NEAT_DST_TOO_SMALL;
}

Neat_String_Error neat_dstr_copy(Neat_DString *dstr, Neat_String_View src)
{
    Neat_String_Error err = neat_dstr_ensure_cap_(dstr, src.len + 1);
    
    if(err == NEAT_OK)
    {
        memmove(dstr->chars, src.chars, src.len * sizeof(unsigned char));
        
        dstr->len = src.len;
        dstr->chars[dstr->len] = '\0';
    }
    
    return err;
}

Neat_String_Error neat_buf_copy(Neat_Buffer dst, Neat_String_View src)
{
    unsigned int len;
    return neat_fmutstr_ref_copy(neat_buf_as_fmutstr_ref(dst, &len), src);
}

Neat_String_Error neat_strbuf_copy(Neat_String_Buffer *dst, Neat_String_View src)
{
    return neat_fmutstr_ref_copy(neat_strbuf_as_fmutstr_ref(dst), src);
}

Neat_String_Error neat_sstr_ref_copy(Neat_SString_Ref dst, Neat_String_View src)
{
    return neat_fmutstr_ref_copy(neat_sstr_ref_as_fmutstr_ref(dst), src);
}

Neat_String_Error neat_mutstr_ref_copy(Neat_Mut_String_Ref dst, Neat_String_View src)
{
    switch(dst.ty)
    {
        case NEAT_DSTR_TY     : return neat_dstr_copy(dst.str.dstr, src);
        case NEAT_STRBUF_TY   : return neat_strbuf_copy(dst.str.strbuf, src);
        case NEAT_SSTR_REF_TY : return neat_sstr_ref_copy(dst.str.sstr_ref, src);
        case NEAT_BUF_TY      : return neat_buf_copy(dst.str.carr, src);
    };
}

Neat_String_Error neat_dstr_putc(Neat_DString *dst, unsigned char c)
{
    Neat_String_Error err = neat_dstr_ensure_cap_(dst, dst->len + 2);
    if(err != NEAT_OK)
        return err;
    
    dst->chars[dst->len] = c;
    dst->chars[dst->len + 1] = '\0';
    dst->len += 1;
    
    return NEAT_OK;
}

Neat_String_Error neat_fmutstr_ref_putc(Fixed_Mut_String_Ref dst, unsigned char c)
{
    if(dst.cap - *dst.len <= 1)
    {
        return NEAT_DST_TOO_SMALL;
    }
    
    // TODO check if valid 7 bit ascii
    
    dst.chars[*dst.len] = c;
    dst.chars[*dst.len + 1] = '\0';
    *dst.len += 1;
    
    return NEAT_OK;
}

Neat_String_Error neat_strbuf_putc(Neat_String_Buffer *dst, unsigned char c)
{
    return neat_fmutstr_ref_putc(neat_strbuf_as_fmutstr_ref(dst), c);
}

Neat_String_Error neat_sstr_ref_putc(Neat_SString_Ref dst, unsigned char c)
{
    return neat_fmutstr_ref_putc(neat_sstr_ref_as_fmutstr_ref(dst), c);
}

Neat_String_Error neat_buf_putc(Neat_Buffer dst, unsigned char c)
{
    return neat_fmutstr_ref_putc(neat_buf_as_fmutstr_ref(dst, &(unsigned int){0}), c);
}

Neat_String_Error neat_mutstr_ref_putc(Neat_Mut_String_Ref dst, unsigned char c)
{
    switch(dst.ty)
    {
        case NEAT_DSTR_TY     : return neat_dstr_putc(dst.str.dstr, c);
        case NEAT_STRBUF_TY   : return neat_strbuf_putc(dst.str.strbuf, c);
        case NEAT_SSTR_REF_TY : return neat_sstr_ref_putc(dst.str.sstr_ref, c);
        case NEAT_BUF_TY      : return neat_buf_putc(dst.str.carr, c);
    }
}

Neat_String_Error neat_fmutstr_ref_concat(Fixed_Mut_String_Ref dst, Neat_String_View src)
{
    unsigned int dst_len = *dst.len;
    
    if(dst_len >= dst.cap - 1)
        return 0;
    
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - dst_len - 1);
    memmove(dst.chars + dst_len, src.chars, chars_to_copy);
    
    dst_len += chars_to_copy;
    
    if(dst.len != NULL)
        *dst.len = dst_len;
    
    dst.chars[dst_len] = '\0';
    
    return chars_to_copy == src.len ? NEAT_OK : NEAT_DST_TOO_SMALL;
}

Neat_String_Error neat_buf_concat(Neat_Buffer dst, Neat_String_View src)
{
    return neat_fmutstr_ref_concat(neat_buf_as_fmutstr_ref(dst, &(unsigned int){0}), src);
}

Neat_String_Error neat_strbuf_concat(Neat_String_Buffer *dst, Neat_String_View src)
{
    return neat_fmutstr_ref_concat(neat_strbuf_as_fmutstr_ref(dst), src);
}

Neat_String_Error neat_sstr_ref_concat(Neat_SString_Ref dst, Neat_String_View src)
{
    return neat_fmutstr_ref_concat(neat_sstr_ref_as_fmutstr_ref(dst), src);
}

// TODO rename dstr_ functions to match others
// also rename lib to STS

Neat_String_Error neat_mutstr_ref_concat(Neat_Mut_String_Ref dst, Neat_String_View src)
{
    switch(dst.ty)
    {
        case NEAT_DSTR_TY     : return neat_dstr_append_strv(dst.str.dstr, src);
        case NEAT_STRBUF_TY   : return neat_strbuf_concat(dst.str.strbuf, src);
        case NEAT_SSTR_REF_TY : return neat_sstr_ref_concat(dst.str.sstr_ref, src);
        case NEAT_BUF_TY      : return neat_buf_concat(dst.str.carr, src);
    };
}

Neat_String_Error neat_fmutstr_ref_delete_range(Fixed_Mut_String_Ref str, unsigned int begin, unsigned int end)
{
    unsigned int len = *str.len;
    
    if(end > len || begin > end)
    {
        return NEAT_BAD_RANGE;
    }
    
    unsigned int substr_len = end - begin;
    
    memmove(str.chars + begin, str.chars + begin + substr_len, len - begin - substr_len);
    
    len -= substr_len;
    
    str.chars[len] = '\0';
    
    if(str.len != NULL)
    {
        *str.len = len;
    }
    
    return NEAT_OK;
}

Neat_String_Error neat_buf_delete_range(Neat_Buffer buf, unsigned int begin, unsigned int end)
{
    unsigned int len = (unsigned int) strlen((char*) buf.ptr);
    return neat_fmutstr_ref_delete_range(neat_buf_as_fmutstr_ref(buf, &len), begin, end);
}

Neat_String_Error neat_mutstr_ref_delete_range(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY        : return neat_fmutstr_ref_delete_range(neat_dstr_as_fmutstr_ref(str.str.dstr), begin, end);
        case NEAT_FMUTSTR_REF_TY : return neat_fmutstr_ref_delete_range(str.str.fmutstr_ref, begin, end);
        case NEAT_BUF_TY      : return neat_buf_delete_range(str.str.carr, begin, end);
    };
}

Neat_String_View_Array neat_strv_arr_from_carr(Neat_String_View *carr, unsigned int nb)
{
    return (Neat_String_View_Array){
        .cap  = nb,
        .len   = nb,
        .strs = carr
    };
}

NEAT_NODISCARD("str_split returns new String_View_Array") Neat_String_View_Array neat_strv_split(Neat_String_View str, Neat_String_View delim, Neat_Allocator allocator)
{
    // if(allocator.ctx == NULL)
    //     allocator.init(&allocator.ctx, NULL);
    
    if(delim.len > str.len)
    {
        size_t alloced_size;
        Neat_String_View *entire_str = neat_alloc(allocator, Neat_String_View, 1, &alloced_size);
        if(alloced_size < sizeof(Neat_String_View) || entire_str == NULL)
        {
            return (Neat_String_View_Array){
                .cap  = 0,
                .len   = 0,
                .strs = NULL
            };
        }
        else
        {
            *entire_str = str;
            return (Neat_String_View_Array){
                .cap  = (unsigned int) alloced_size,
                .len   = 1,
                .strs = entire_str
            };
        }
    }
    else if(delim.len == 0)
    {
        size_t alloced_size;
        Neat_String_View *strs = neat_alloc(allocator, Neat_String_View, str.len, &alloced_size);
        
        if(alloced_size < (str.len * sizeof(Neat_String_View)) || strs == NULL)
        {
            return (Neat_String_View_Array){
                .cap  = 0,
                .len   = 0,
                .strs = NULL
            };
        }
        else
        {
            Neat_String_View_Array ret = {
                .cap  = (unsigned int) alloced_size,
                .len   = str.len,
                .strs = strs
            };
            
            for(unsigned int i = 0 ; i < ret.len ; i++)
            {
                ret.strs[i] = neat_strv_strv3(str, i, i + 1);
            }
            
            return ret;
        }
    }
    else
    {
        unsigned int nb_delim = 0;
        unsigned int *delim_idx = (unsigned int*) calloc(str.len, sizeof(unsigned int));
        
        for(unsigned int i = 0 ; i <= str.len - delim.len ; )
        {
            Neat_String_View sub = neat_strv_strv3(str, i, i + delim.len);
            if(neat_strv_equal(sub, delim))
            {
                delim_idx[nb_delim] = i;
                nb_delim += 1;
                i += delim.len;
            }
            else
            {
                i += 1;
            }
        }
        
        delim_idx[nb_delim] = str.len;
        
        size_t alloced_size;
        Neat_String_View *strs = neat_alloc(allocator, Neat_String_View, nb_delim + 1, &alloced_size);
        
        if(alloced_size < ((nb_delim + 1) * sizeof(Neat_String_View))  || strs == NULL)
        {
            free(delim_idx);
            
            return (Neat_String_View_Array){
                .cap  = 0,
                .len   = 0,
                .strs = NULL
            };
        }
        else
        {
            Neat_String_View_Array ret = {
                .cap = (unsigned int) alloced_size,
                .len  = nb_delim + 1,
                .strs = strs
            };
            
            ret.strs[0] = neat_strv_strv_ptr3(&str, 0, delim_idx[0]);
            for(unsigned int i = 1 ; i <= nb_delim ; i++)
            {
                ret.strs[i] = neat_strv_strv_ptr3(&str, delim_idx[i - 1] + delim.len, delim_idx[i]);
            }
            
            free(delim_idx);
            return ret;
        }
    }
}

Neat_String_Error neat_strv_arr_join_into_dstr(Neat_DString *dstr, Neat_String_View_Array strs, Neat_String_View delim)
{
    Neat_String_Error err = NEAT_OK;
    
    if(strs.len > 0)
        err = neat_dstr_copy(dstr, strs.strs[0]);
    
    for(unsigned int i = 1 ; i < strs.len && err == NEAT_OK ; i++)
    {
        neat_dstr_append_strv(dstr, delim);
        err = neat_dstr_append_strv(dstr, strs.strs[i]);
    }
    
    return err;
}

Neat_String_Error neat_strv_arr_join_into_fmutstr_ref(Neat_Fixed_Mut_String_Ref dst, Neat_String_View_Array strs, Neat_String_View delim)
{
    Neat_String_Error err = NEAT_OK;
    
    if(strs.len > 0)
        err = neat_fmutstr_ref_copy(dst, strs.strs[0]);
    
    for(unsigned int i = 1 ; i < strs.len && err == NEAT_OK; i++)
    {
        neat_fmutstr_ref_concat(dst, delim);
        err = neat_fmutstr_ref_concat(dst, strs.strs[i]);
    }
    
    return err;
}

Neat_String_Error neat_strv_arr_join_into_buf(Neat_Buffer dst, Neat_String_View_Array strs, Neat_String_View delim)
{
    unsigned int len = (unsigned int) strlen((char*) dst.ptr);
    Neat_Fixed_Mut_String_Ref str = {
        .chars = dst.ptr,
        .len = &len,
        .cap = dst.cap
    };
    return neat_strv_arr_join_into_fmutstr_ref(str, strs, delim);
}

Neat_String_Error neat_strv_arr_join(Neat_Mut_String_Ref dst, Neat_String_View_Array strs, Neat_String_View delim)
{
    switch(dst.ty)
    {
        case NEAT_DSTR_TY        : return neat_strv_arr_join_into_dstr(dst.str.dstr, strs, delim);
        case NEAT_FMUTSTR_REF_TY : return neat_strv_arr_join_into_fmutstr_ref(dst.str.fmutstr_ref, strs, delim);
        case NEAT_BUF_TY      : return neat_strv_arr_join_into_buf(dst.str.carr, strs, delim);
    };
}

Neat_String_Error neat_dstr_replace_range(Neat_DString *dstr, unsigned int begin, unsigned int end, const Neat_String_View replacement)
{
    if(begin >= dstr->len)
        return NEAT_INDEX_OUT_OF_BOUNDS;
    if(begin > end || end > dstr->len)
        return NEAT_BAD_RANGE;
    if(neat_is_strv_within(neat_strv_dstr_ptr2(dstr, 0), replacement))
        return NEAT_ALIASING_NOT_SUPPORTED;
    
    unsigned int len_to_delete = end - begin;
    if(len_to_delete > replacement.len)
    {
        // shift left
        memmove(dstr->chars + begin + replacement.len, dstr->chars + end, dstr->len - end);
        // insert the replacement
        memmove(dstr->chars + begin, replacement.chars, replacement.len);
    }
    else if(len_to_delete < replacement.len)
    {
        neat_dstr_ensure_cap_(dstr, dstr->len + replacement.len - len_to_delete + 1);
        unsigned int shift_by = replacement.len - len_to_delete;
        
        // shift right
        memmove(dstr->chars + end + shift_by, dstr->chars + end, shift_by);
        // insert the replacement
        memmove(dstr->chars + begin, replacement.chars, len_to_delete + shift_by);
    }
    else
    {
        memmove(dstr->chars + begin, replacement.chars, replacement.len);
    }
    
    return NEAT_OK;
}

void neat_fmutstr_ref_replace_range_unsafe(Neat_Fixed_Mut_String_Ref str, unsigned int begin, unsigned int end, Neat_String_View replacement)
{
    unsigned int len_to_delete = end - begin;
    if(len_to_delete > replacement.len)
    {
        // shift left
        memmove(str.chars + begin + replacement.len, str.chars + end, *str.len - end);
        // insert the replacement
        memmove(str.chars + begin, replacement.chars, replacement.len);
    }
    else if(len_to_delete < replacement.len)
    {
        unsigned int shift_by = neat_uint_min(replacement.len - len_to_delete, str.cap - *str.len - 1);
        
        // shift right
        memmove(str.chars + end + shift_by, str.chars + end, shift_by);
        // insert the replacement
        memmove(str.chars + begin, replacement.chars, len_to_delete + shift_by);
    }
    else
    {
        memmove(str.chars + begin, replacement.chars, replacement.len);
    }
}

Neat_String_Error neat_fmutstr_ref_replace_range(Neat_Fixed_Mut_String_Ref str, unsigned int begin, unsigned int end, Neat_String_View replacement)
{
    if(begin >= *str.len)
        return NEAT_INDEX_OUT_OF_BOUNDS;
    if(begin > end || end > *str.len)
        return NEAT_BAD_RANGE;
    if(neat_is_strv_within(neat_strv_fmutstr_ref2(str, 0), replacement))
        return NEAT_ALIASING_NOT_SUPPORTED;
    
    Neat_String_Error err = (*str.len - (end - begin) + replacement.len) >= str.cap ? NEAT_DST_TOO_SMALL : NEAT_OK;
    neat_fmutstr_ref_replace_range_unsafe(str, begin, end, replacement);
    
    return err;
}

Neat_String_Error neat_buf_replace_range(Neat_Buffer buf, unsigned int begin, unsigned int end, Neat_String_View replacement)
{
    unsigned int len = (unsigned int) strlen((char*) buf.ptr);
    return neat_fmutstr_ref_replace_range(neat_buf_as_fmutstr_ref(buf, &len), begin, end, replacement);
}

Neat_String_Error neat_mustr_ref_replace_range(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end, const Neat_String_View replacement)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY        : return neat_dstr_replace_range(str.str.dstr, begin, end, replacement);
        case NEAT_FMUTSTR_REF_TY : return neat_fmutstr_ref_replace_range(str.str.fmutstr_ref, begin, end, replacement);
        case NEAT_BUF_TY      : return neat_buf_replace_range(str.str.carr, begin, end, replacement);
    };
}

Neat_String_View neat_strv_fmutstr_ref2(Neat_Fixed_Mut_String_Ref str, unsigned int begin);

Neat_String_Error neat_fmutstr_ref_replace(Neat_Fixed_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    Neat_String_View as_strv = neat_strv_fmutstr_ref2(str, 0);
    if(neat_is_strv_within(as_strv, target) || neat_is_strv_within(as_strv, replacement))
    {
        return NEAT_ALIASING_NOT_SUPPORTED;
    }
    
    Neat_String_Error err = NEAT_OK;
    unsigned int len = *str.len;
    
    if(target.len == 0)
    {
        for(unsigned int i = 0 ; i <= *str.len && err == NEAT_OK ; i += replacement.len + 1)
        {
            err = neat_fmutstr_ref_insert(str, replacement, i);
        }
        return err;
    }
    
    if(target.len < replacement.len)
    {
        for(unsigned int i = 0 ; i <= *str.len - target.len; )
        {
            Neat_String_View match = neat_strv_memmem(neat_strv_fmutstr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                if(str.cap > *str.len + (replacement.len - target.len))
                {
                    // shift right
                    memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));
                    
                    // put the replacement
                    memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                    
                    *str.len += (replacement.len - target.len);
                    
                    i = idx + replacement.len;
                }
                else
                {
                    err = NEAT_DST_TOO_SMALL;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    else if(target.len > replacement.len)
    {
        for(unsigned int i = 0 ; i <= *str.len - target.len; )
        {
            Neat_String_View match = neat_strv_memmem(neat_strv_fmutstr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                // shift left
                memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));
                
                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                *str.len -= (target.len - replacement.len);
                
                i = idx + replacement.len;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for(unsigned int i = 0 ; i <= *str.len - target.len; )
        {
            Neat_String_View match = neat_strv_memmem(neat_strv_fmutstr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                i = idx + replacement.len;
            }
            else
            {
                break;
            }
        }
    }
    
    if(str.cap > 0)
        str.chars[*str.len] = '\0';
    return err;
}

Neat_String_Error neat_dstr_replace(Neat_DString *dstr, Neat_String_View target, Neat_String_View replacement)
{
    Neat_String_View as_strv = neat_strv_dstr_ptr2(dstr, 0);
    if(neat_is_strv_within(as_strv, target) || neat_is_strv_within(as_strv, replacement))
    {
        return NEAT_ALIASING_NOT_SUPPORTED;
    }
    
    Neat_String_Error err = NEAT_OK;
    unsigned int len = dstr->len;
    
    if(target.len == 0)
    {
        unsigned int total_inserted = 0;
        unsigned int inserted = 1;
        for(unsigned int i = 0 ; i <= dstr->len && err == NEAT_OK ; i += replacement.len + 1)
        {
            err = neat_dstr_insert_strv(dstr, replacement, i);
        }
        return err;
    }
    
    if(target.len < replacement.len)
    {
        for(unsigned int i = 0 ; i <= dstr->len - target.len; )
        {
            Neat_String_View match = neat_strv_memmem(neat_strv_dstr_ptr2(dstr, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - dstr->chars;
                
                neat_dstr_ensure_cap_(dstr, dstr->len + (replacement.len - target.len) + 1);
                
                // shift right
                memmove(dstr->chars + idx + replacement.len, dstr->chars + idx + target.len, (dstr->len - idx - target.len) * sizeof(unsigned char));
                
                // put the replacement
                memmove(dstr->chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                dstr->len += (replacement.len - target.len);
                
                i = idx + replacement.len;
            }
        }
    }
    else if(target.len > replacement.len)
    {
        for(unsigned int i = 0 ; i <= dstr->len - target.len ; )
        {
            Neat_String_View match = neat_strv_memmem(neat_strv_dstr_ptr2(dstr, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - dstr->chars;
                
                // shift left
                memmove(dstr->chars + idx + replacement.len, dstr->chars + idx + target.len, (dstr->len - idx - target.len) * sizeof(unsigned char));
                
                // put the replacement
                memmove(dstr->chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                dstr->len -= (target.len - replacement.len);
                
                i = idx + replacement.len;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for(unsigned int i = 0 ; i <= dstr->len - target.len; )
        {
            Neat_String_View match = neat_strv_memmem(neat_strv_dstr_ptr2(dstr, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - dstr->chars;
                
                // put the replacement
                memmove(dstr->chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                i = idx + replacement.len;
            }
            else
            {
                break;
            }
        }
    }
    
    dstr->chars[dstr->len] = '\0';
    
    return err;
}

Neat_String_Error neat_buf_replace(Neat_Buffer str, Neat_String_View target, Neat_String_View replacement)
{
    unsigned int len = strlen((char*) str.ptr);
    Neat_Fixed_Mut_String_Ref s = neat_buf_as_fmutstr_ref(str, &len);
    return neat_fmutstr_ref_replace(s, target, replacement);
}

Neat_String_Error neat_mutstr_ref_replace(Neat_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY        : return neat_dstr_replace(str.str.dstr, target, replacement);
        case NEAT_FMUTSTR_REF_TY : return neat_fmutstr_ref_replace(str.str.fmutstr_ref, target, replacement);
        case NEAT_BUF_TY      : return neat_buf_replace(str.str.carr, target, replacement);
    };
}

Neat_String_Error neat_dstr_replace_first(Neat_DString *dstr, Neat_String_View target, Neat_String_View replacement)
{
    Neat_String_Error err = NEAT_NOT_FOUND;
    
    unsigned int len = 0;
    
    Neat_String_View match = neat_strv_memmem(neat_strv_dstr_ptr2(dstr, 0), target);
    if(match.chars != NULL)
    {
        if(dstr->cap > 0 && dstr->cap - 1 > dstr->len + (replacement.len - target.len))
        {
            unsigned int begin = match.chars - dstr->chars;
            unsigned int end = begin + match.len;
            err = neat_dstr_replace_range(dstr, begin, end, replacement);
        }
    }
    
    if(dstr->cap > 0)
        dstr->chars[dstr->len] = '\0';
    
    return err;
}

Neat_String_Error neat_fmutstr_ref_replace_first(Neat_Fixed_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    Neat_String_Error err = NEAT_NOT_FOUND;
    
    unsigned int len = 0;
    
    Neat_String_View match = neat_strv_memmem(neat_strv_fmutstr_ref2(str, 0), target);
    if(match.chars != NULL)
    {
        if(str.cap > 0 && str.cap - 1 > *str.len + (replacement.len - target.len))
        {
            unsigned int idx = match.chars - str.chars;
            
            // shift
            memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));
            
            // put the replacement
            memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
            
            *str.len += (replacement.len - target.len);
            
            err = NEAT_OK;
        }
        else
        {
            err = NEAT_DST_TOO_SMALL;
        }
    }
    
    if(str.cap > 0)
        str.chars[*str.len] = '\0';
    
    return err;
}

Neat_String_Error neat_buf_replace_first(Neat_Buffer str, Neat_String_View target, Neat_String_View replacement)
{
    unsigned int len = strlen((char*) str.ptr);
    Neat_Fixed_Mut_String_Ref fmutstr_ref = {
        .chars = str.ptr,
        .cap = str.cap,
        .len = &len
    };
    return neat_fmutstr_ref_replace_first(fmutstr_ref, target, replacement);
}

Neat_String_Error neat_mutstr_ref_replace_first(Neat_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY        : return neat_dstr_replace_first(str.str.dstr, target, replacement);
        case NEAT_FMUTSTR_REF_TY : return neat_fmutstr_ref_replace_first(str.str.fmutstr_ref, target, replacement);
        case NEAT_BUF_TY      : return neat_buf_replace_first(str.str.carr, target, replacement);
    };
}

unsigned int neat_strv_count(Neat_String_View hay, Neat_String_View needle)
{
    if(needle.len == 0)
        return 0;
    
    unsigned int count = 0;
    Neat_String_View found = neat_strv_memmem(hay, needle);
    
    while(found.chars != NULL)
    {
        count += 1;
        found = neat_strv_memmem(neat_strv_strv2(hay, (found.chars - hay.chars) + 1), needle);
    }
    
    return count;
}

bool neat_strv_starts_with(Neat_String_View hay, Neat_String_View needle)
{
    return (needle.len <= hay.len) && (memcmp(hay.chars, needle.chars, needle.len) == 0);
}

bool neat_strv_ends_with(Neat_String_View hay, Neat_String_View needle)
{
    return (needle.len <= hay.len) && (memcmp(hay.chars + hay.len - needle.len, needle.chars, needle.len) == 0);
}

// TODO toupper/tolower need to ignore non-ascii chars
// check if the current char is only 1 byte before calling tolower
// this can be done by making a generic 'neat_foreach_utf8(any_str, callback)'
// typedef bool(*callback)(UTF8_Char_Ref);
// return false to stop foreach

void neat_chars_tolower(unsigned char *chars, unsigned int len)
{
    for(unsigned int i = 0 ; i < len ; i++)
    {
        chars[i] = tolower(chars[i]);
    }
}

void neat_chars_toupper(unsigned char *chars, unsigned int len)
{
    for(unsigned int i = 0 ; i < len ; i++)
    {
        chars[i] = toupper(chars[i]);
    }
}

// TODO this should return NEAT_UTF8_ERR if invalid utf8
Neat_String_Error neat_mutstr_ref_toupper(Neat_Mut_String_Ref str)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY:
            neat_chars_toupper(str.str.dstr->chars, str.str.dstr->len);
            break;
        case NEAT_FMUTSTR_REF_TY:
            neat_chars_toupper(str.str.fmutstr_ref.chars, *(str.str.fmutstr_ref.len));
            break;
        case NEAT_BUF_TY:
            neat_chars_toupper(str.str.carr.ptr, strlen((char*) str.str.carr.ptr));
            break;
    }
    return NEAT_OK;
}

Neat_String_Error neat_mutstr_ref_clear(Neat_Mut_String_Ref str)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY:
            str.str.dstr->len = 0;
            break;
        case NEAT_FMUTSTR_REF_TY:
            *str.str.fmutstr_ref.len = 0;
            break;
        case NEAT_BUF_TY:
            str.str.carr.ptr[0] = '\0';
            break;
    }
    return NEAT_OK;
}

Neat_Mut_String_Ref neat_cstr_as_mutstr_ref(char *str)
{
    unsigned int len = (unsigned int) strlen(str);
    
    Neat_Buffer asbuf = {
        .ptr = (unsigned char*) str,
        .cap = len + 1
    };
    
    return neat_buf_as_mutstr_ref(asbuf);
}

Neat_Mut_String_Ref neat_ucstr_as_mutstr_ref(unsigned char *str)
{
    unsigned int len = (unsigned int) strlen((char*) str);
    
    Neat_Buffer asbuf = {
        .ptr = str,
        .cap = len + 1
    };
    
    return neat_buf_as_mutstr_ref(asbuf);
}

Neat_Mut_String_Ref neat_buf_as_mutstr_ref(Neat_Buffer str)
{
    return (Neat_Mut_String_Ref){
        .ty = NEAT_BUF_TY,
        .str.carr = str
    };
}

Neat_Mut_String_Ref neat_dstr_ptr_as_mutstr_ref(Neat_DString *str)
{
    return (Neat_Mut_String_Ref){
        .ty = NEAT_DSTR_TY,
        .str.dstr = str
    };
}

Neat_Mut_String_Ref neat_strbuf_ptr_as_mutstr_ref(Neat_String_Buffer *str)
{
    return (Neat_Mut_String_Ref){
        .ty = NEAT_DSTR_TY,
        .str.fmutstr_ref = {.chars = str->chars, .len = &str->len, .cap = str->cap}
    };
}

Neat_Mut_String_Ref neat_sstr_ref_as_mutstr_ref(Neat_SString_Ref str)
{
    return (Neat_Mut_String_Ref){
        .ty = NEAT_DSTR_TY,
        .str.fmutstr_ref = {.chars = str.sstr->chars, .len = &str.sstr->len, .cap = str.cap}
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_as_mutstr_ref(Neat_Mut_String_Ref str)
{
    return str;
}

Neat_String_Buffer neat_strbuf_new(unsigned int cap, Neat_Allocator allocator, Neat_String_Error *err)
{
    // if(allocator.ctx == NULL)
    //     allocator.init(&allocator.ctx, NULL);
    
    Neat_String_Buffer ret = { 0 };
    size_t actual_allocated_cap;
    ret.chars = neat_alloc(allocator, unsigned char, cap, &actual_allocated_cap);
    ret.cap = actual_allocated_cap;
    
    if(err)
    {
        if(ret.chars == NULL || ret.cap < cap)
            *err = NEAT_ALLOC_ERR;
        else
            *err = NEAT_OK;
    }
    
    return ret;
}

Neat_String_Buffer neat_strbuf_new_default(unsigned int cap, Neat_String_Error *err)
{
    return neat_strbuf_new(cap, neat_get_default_allocator(), err);
}

Neat_String_Buffer neat_strbuf_from_ptr(void *ptr, unsigned int cap)
{
    unsigned int len = neat_chars_strlen(ptr, cap);
    
    return (Neat_String_Buffer){
        .cap = cap,
        .len = len,
        .chars = ptr
    };
}

Neat_String_Buffer neat_strbuf_from_buf(Neat_Buffer buf)
{
    return neat_strbuf_from_ptr(buf.ptr, buf.cap);
}

Neat_String_View neat_strv_cstr2(char *str, unsigned int begin)
{
    unsigned int len = strlen(str);
    
    if(begin > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len - begin,
        .chars = (unsigned char*) str + begin
    };
}

Neat_String_View neat_strv_ucstr2(unsigned char *str, unsigned int begin)
{
    unsigned int len = strlen((char*) str);
    
    if(begin > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len - begin,
        .chars = str + begin
    };
}

Neat_String_View neat_strv_dstr2(Neat_DString str, unsigned int begin)
{
    return neat_strv_dstr_ptr2(&str, begin);
}

Neat_String_View neat_strv_dstr_ptr2(Neat_DString *str, unsigned int begin)
{
    if(begin > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strv_ptr2(Neat_String_View *str, unsigned int begin)
{
    if(begin > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strv2(Neat_String_View str, unsigned int begin)
{
    return neat_strv_strv_ptr2(&str, begin);
}

Neat_String_View neat_strv_strbuf_ptr2(Neat_String_Buffer *str, unsigned int begin)
{
    if(begin > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strbuf2(Neat_String_Buffer str, unsigned int begin)
{
    return neat_strv_strbuf_ptr2(&str, begin);
}

Neat_String_View neat_strv_sstr_ref2(Neat_SString_Ref str, unsigned int begin)
{
    if(begin > str.sstr->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str.sstr->len   - begin,
        .chars = str.sstr->chars + begin
    };
}

Neat_String_View neat_strv_mutstr_ref2(Neat_Mut_String_Ref str, unsigned int begin)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY:
            return neat_strv_dstr_ptr2(str.str.dstr, begin);
        case NEAT_FMUTSTR_REF_TY:
            return neat_strv_fmutstr_ref2(str.str.fmutstr_ref, begin);
        case NEAT_BUF_TY:
        {
            unsigned int len = strlen((char*)(str.str.carr.ptr));
            Neat_Fixed_Mut_String_Ref as_fixed = {
                .chars = str.str.carr.ptr,
                .len = &len,
                .cap = str.str.carr.cap
            };
            return neat_strv_fmutstr_ref2(as_fixed, begin);
        }
    }
}

Neat_String_View neat_strv_fmutstr_ref2(Neat_Fixed_Mut_String_Ref str, unsigned int begin)
{
    unsigned int len = *str.len;
    
    if(begin > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len       - begin,
        .chars = str.chars + begin
    };
}

Neat_String_View neat_strv_fmutstr_ref3(Neat_Fixed_Mut_String_Ref str, unsigned int begin, unsigned int end)
{
    unsigned int len = *str.len;
    
    if(begin > len || end > len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str.chars + begin
    };
}

Neat_String_View neat_strv_cstr3(char *str, unsigned int begin, unsigned int end)
{
    unsigned int len = strlen(str);
    
    if(begin > len || end > len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = (unsigned char*) str + begin
    };
}

Neat_String_View neat_strv_ucstr3(unsigned char *str, unsigned int begin, unsigned int end)
{
    unsigned int len = strlen((char*) str);
    
    if(begin > len || end > len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str + begin
    };
}

Neat_String_View neat_strv_dstr_ptr3(Neat_DString *str, unsigned int begin, unsigned int end)
{
    if(begin > str->len || end > str->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strv_ptr3(Neat_String_View *str, unsigned int begin, unsigned int end)
{
    if(begin > str->len || end > str->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_strbuf_ptr3(Neat_String_Buffer *str, unsigned int begin, unsigned int end)
{
    if(begin > str->len || end > str->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str->chars + begin
    };
}

Neat_String_View neat_strv_sstr_ref3(Neat_SString_Ref str, unsigned int begin, unsigned int end)
{
    if(begin > str.sstr->len || end > str.sstr->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str.sstr->chars + begin
    };
}

Neat_String_View neat_strv_mutstr_ref3(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end)
{
    switch(str.ty)
    {
        case NEAT_DSTR_TY:
            return neat_strv_dstr_ptr3(str.str.dstr, begin, end);
        case NEAT_FMUTSTR_REF_TY:
            return neat_strv_fmutstr_ref3(str.str.fmutstr_ref, begin, end);
        case NEAT_BUF_TY:
        {
            unsigned int len = strlen((char*)(str.str.carr.ptr));
            Neat_Fixed_Mut_String_Ref as_fixed = {
                .chars = str.str.carr.ptr,
                .len = &len,
                .cap = str.str.carr.cap
            };
            return neat_strv_fmutstr_ref3(as_fixed, begin, end);
        }
    }
}

Neat_String_View neat_strv_dstr3(Neat_DString str, unsigned int begin, unsigned int end)
{
    return neat_strv_dstr_ptr3(&str, begin, end);
}

Neat_String_View neat_strv_strv3(Neat_String_View str, unsigned int begin, unsigned int end)
{
    return neat_strv_strv_ptr3(&str, begin, end);
}

Neat_String_View neat_strv_strbuf3(Neat_String_Buffer str, unsigned int begin, unsigned int end)
{
    return neat_strv_strbuf_ptr3(&str, begin, end);
}

// Neat_DString neat_tostr_all_into_new_dstr(Neat_Allocator allocator, unsigned int nb, ...)
// {
//     Neat_DString ret = neat_dstr_new(nb * 16, allocator);
//     
//     va_list args;
//     va_start(args, nb);
//     
//     for(unsigned int i = 0 ; i < nb ; i++)
//     {
//         Neat_DString current = va_arg(args, Neat_DString);
//         neat_dstr_append_strv(&ret, (Neat_String_View){.chars = current.chars, .len = current.len});
//         neat_dstr_deinit_(&current);
//     }
//     
//     va_end(args);
//     
//     return ret;
// }

Neat_String_Error neat_dstr_fread_line_(Neat_DString *dstr, FILE *stream)
{
    dstr->len = 0;
    if(dstr->cap > 0)
    {
        dstr->chars[0] = '\0';
    }
    
    return neat_dstr_append_fread_line_(dstr, stream);
}

Neat_String_Error neat_dstr_append_fread_line_(Neat_DString *dstr, FILE *stream)
{
    // TODO optimize this
    unsigned int prev_len = dstr->len;
    int c = 0;
    while(c != '\n' && (c=fgetc(stream)) != EOF)
    {
        unsigned char as_char = c;
        Neat_String_View as_strv = {.chars = &as_char, .len = 1};
        Neat_String_Error err = neat_dstr_append_strv(dstr, as_strv);
        if(err != NEAT_OK) return err;
    }
    
    return NEAT_OK;
}

Neat_String_Error neat_fmutstr_ref_fread_line(Neat_Fixed_Mut_String_Ref dst, FILE *stream)
{
    if(dst.cap == 0)
    {
        return NEAT_DST_TOO_SMALL;
    }
    
    unsigned int len = 0;
    int c = 0;
    while(len < dst.cap - 1 && c != '\n' && (c=fgetc(stream)) != EOF)
    {
        dst.chars[len] = c;
        len += 1;
    }
    
    dst.chars[len] = '\0';
    *dst.len = len;
    
    bool dst_too_small = (len == dst.cap - 1) && (c != '\n') && (c != EOF);
    
    if(dst_too_small)
        return NEAT_DST_TOO_SMALL;
    else
        return NEAT_OK;
}

Neat_String_Error neat_mutstr_ref_fread_line(Neat_Mut_String_Ref dst, FILE *stream)
{
    switch(dst.ty)
    {
        case NEAT_DSTR_TY:
            return neat_dstr_fread_line_(dst.str.dstr, stream);
        case NEAT_FMUTSTR_REF_TY:
            return neat_fmutstr_ref_fread_line(dst.str.fmutstr_ref, stream);
        case NEAT_BUF_TY:
        {
            unsigned int len = (unsigned int) strlen((char*) dst.str.carr.ptr);
            Neat_Fixed_Mut_String_Ref as_fixed = {
                .chars = dst.str.carr.ptr,
                .len = &len,
                .cap = dst.str.carr.cap
            };
            return neat_fmutstr_ref_fread_line(as_fixed, stream);
        }
    };
}

Neat_String_Error neat_fmutstr_ref_append_fread_line(Neat_Fixed_Mut_String_Ref dst, FILE *stream)
{
    if(dst.cap == 0)
        return false;
    
    unsigned int concated_len = 0;
    
    Neat_Fixed_Mut_String_Ref right = {
        .cap = dst.cap - *dst.len,
        .len = &concated_len
    };
    
    right.chars = dst.chars + *dst.len;
    
    Neat_String_Error err = neat_fmutstr_ref_fread_line(right, stream);
    
    *dst.len += *right.len;
    
    dst.chars[*dst.len] = '\0';
    
    return err;
}

Neat_String_Error neat_mutstr_ref_append_fread_line(Neat_Mut_String_Ref dst, FILE *stream)
{
    switch(dst.ty)
    {
        case NEAT_DSTR_TY:
            return neat_dstr_append_fread_line_(dst.str.dstr, stream);
        case NEAT_FMUTSTR_REF_TY:
            return neat_fmutstr_ref_append_fread_line(dst.str.fmutstr_ref, stream);
        case NEAT_BUF_TY:
        {
            unsigned int len = (unsigned int) strlen((char*) dst.str.carr.ptr);
            Neat_Fixed_Mut_String_Ref as_fixed = {
                .chars = dst.str.carr.ptr,
                .len = &len,
                .cap = dst.str.carr.cap
            };
            return neat_fmutstr_ref_append_fread_line(as_fixed, stream);
        }
    };
}

unsigned int neat_fprint_strv(FILE *stream, Neat_String_View str)
{
    return fwrite(str.chars, sizeof(unsigned char), str.len, stream);
}

unsigned int neat_fprintln_strv(FILE *stream, Neat_String_View str)
{
    unsigned int written = fwrite(str.chars, sizeof(unsigned char), str.len, stream);
    int err = fputc('\n', stream);
    if(err == EOF)
        return written;
    else
        return written + 1;
}

#define neat_sinteger_min(ty) \
_Generic((ty){0},             \
signed char: SCHAR_MIN,   \
short      : SHRT_MIN,    \
int        : INT_MIN,     \
long       : LONG_MIN,    \
long long  : LLONG_MIN    \
)

#define neat_min_tostr(ty) \
_Generic((ty){0},          \
signed char: neat_schar_min_into_fmutstr_ref, \
short      : neat_short_min_into_fmutstr_ref, \
int        : neat_int_min_into_fmutstr_ref,   \
long       : neat_long_min_into_fmutstr_ref,  \
long long  : neat_llong_min_into_fmutstr_ref  \
)

Neat_String_Error neat_schar_min_into_fmutstr_ref(Neat_Fixed_Mut_String_Ref dst)
{
    if(SCHAR_MIN == -128)
    {
        const char *numstr = "-128";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        return neat_fmutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[16] = {0};
        int len = snprintf(temp, sizeof(temp), "%hhd", SCHAR_MIN);
        return neat_fmutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

Neat_String_Error neat_short_min_into_fmutstr_ref(Neat_Fixed_Mut_String_Ref dst)
{
    if(SHRT_MIN == -32768)
    {
        const char *numstr = "-32768";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        return neat_fmutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[16] = {0};
        int len = snprintf(temp, sizeof(temp), "%hd", SHRT_MIN);
        return neat_fmutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

Neat_String_Error neat_int_min_into_fmutstr_ref(Neat_Fixed_Mut_String_Ref dst)
{
    if(INT_MIN == -2147483648)
    {
        const char *numstr = "-2147483648";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        return neat_fmutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len = snprintf(temp, sizeof(temp), "%d", INT_MIN);
        return neat_fmutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

Neat_String_Error neat_long_min_into_fmutstr_ref(Neat_Fixed_Mut_String_Ref dst)
{
    if(LONG_MIN == INT_MIN)
    {
        return neat_int_min_into_fmutstr_ref(dst);
    }
    else if(LONG_MIN == -9223372036854775807 - 1)
    {
        const char *numstr = "-9223372036854775808";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        return neat_fmutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len = snprintf(temp, sizeof(temp), "%ld", LONG_MIN);
        return neat_fmutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

Neat_String_Error neat_llong_min_into_fmutstr_ref(Neat_Fixed_Mut_String_Ref dst)
{
    if(LLONG_MIN == LONG_MIN)
    {
        return neat_long_min_into_fmutstr_ref(dst);
    }
    else if(LLONG_MIN == -9223372036854775807 - 1)
    {
        const char *numstr = "-9223372036854775808";
        Neat_String_View s = {.chars = (unsigned char*) numstr, .len = strlen(numstr)};
        return neat_fmutstr_ref_copy(dst, s);
    }
    else
    {
        char temp[32] = {0};
        int len = snprintf(temp, sizeof(temp), "%lld", LLONG_MIN);
        return neat_fmutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) temp, .len = len});
    }
}

#define neat_sintger_tostr_fmutstr_ref(fmutstr) \
do { \
    if(*fmutstr.len <= 1) \
        return NEAT_DST_TOO_SMALL; \
        if(obj == neat_sinteger_min(typeof(obj))) \
        { \
            return neat_min_tostr(typeof(obj))(fmutstr); \
        } \
        typeof(obj) num = obj; \
        \
        bool isneg = num < 0; \
        if(isneg) \
        { \
            num *= -1; \
            if(fmutstr.cap > 1) \
            { \
                fmutstr.chars[0] = '-'; \
            } \
        } \
        unsigned int numstr_len = neat_numstr_len(num); \
        unsigned int chars_to_copy = neat_uint_min(fmutstr.cap - (1 + isneg), numstr_len); \
        num /= neat_ten_pows[numstr_len - chars_to_copy]; \
        for (unsigned int i = 0; i < chars_to_copy ; i++) \
        { \
            unsigned int rem = num % 10; \
            num = num / 10; \
            fmutstr.chars[isneg + chars_to_copy - (i + 1)] = rem + '0'; \
        } \
        \
        *fmutstr.len = chars_to_copy; \
} while(0)

// TODO: optimize this
#define neat_sinteger_tostr_dstr(dstr) \
do { \
    unsigned int numlen = neat_numstr_len(obj); \
    err = neat_dstr_ensure_cap_(dstr, numlen + 1); \
    if(err != NEAT_OK) \
        return err; \
        \
        Neat_Fixed_Mut_String_Ref as_fixed = { \
            .chars = dstr->chars, \
            .len = &dstr->len \
        }; \
        neat_sintger_tostr_fmutstr_ref(as_fixed); \
} while(0)

#define neat_sinteger_tostr() \
do { \
    Neat_String_Error err = NEAT_OK; \
    switch(dst.ty) \
    { \
        case NEAT_DSTR_TY: \
        { \
            neat_sinteger_tostr_dstr(dst.str.dstr); \
            return err; \
        } \
        case NEAT_FMUTSTR_REF_TY: \
        { \
            neat_sintger_tostr_fmutstr_ref(dst.str.fmutstr_ref); \
            return err; \
        } \
        case NEAT_BUF_TY: \
        { \
            unsigned int len = (unsigned int) strlen((char*) dst.str.carr.ptr); \
            Neat_Fixed_Mut_String_Ref as_fixed = { \
                .chars = dst.str.carr.ptr, \
                .len = &len, \
                .cap = dst.str.carr.cap \
            }; \
            neat_sintger_tostr_fmutstr_ref(as_fixed); \
            return err; \
        } \
    } \
} while(0)

// TODO optimize this
#define neat_uinteger_tostr_dstr(dstr) \
do { \
    unsigned int numlen = neat_numstr_len_ull(obj); \
    err = neat_dstr_ensure_cap_(dstr, numlen + 1); \
    if(err != NEAT_OK) \
        return err; \
        \
        Neat_Fixed_Mut_String_Ref as_fixed = { \
            .chars = dstr->chars, \
            .len = &dstr->len \
        }; \
        neat_uintger_tostr_fmutstr_ref(as_fixed); \
} while(0)

#define neat_uintger_tostr_fmutstr_ref(fmutstr) \
do { \
    if(fmutstr.cap <= 1) \
        return NEAT_DST_TOO_SMALL; \
        typeof(obj) num = obj; \
        unsigned int numstr_len = neat_numstr_len_ull(num); \
        unsigned int chars_to_copy = neat_uint_min(fmutstr.cap - 1, numstr_len); \
        num /= neat_ten_pows[numstr_len - chars_to_copy]; \
        for (unsigned int i = 0; i < chars_to_copy ; i++) \
        { \
            unsigned int rem = num % 10; \
            num = num / 10; \
            fmutstr.chars[chars_to_copy - (i + 1)] = rem + '0'; \
        } \
        \
        *fmutstr.len = chars_to_copy; \
} while(0)

#define neat_uinteger_tostr() \
do { \
    Neat_String_Error err = NEAT_OK; \
    switch(dst.ty) \
    { \
        case NEAT_DSTR_TY: \
        { \
            neat_uinteger_tostr_dstr(dst.str.dstr); \
            return err; \
        } \
        case NEAT_FMUTSTR_REF_TY: \
        { \
            neat_uintger_tostr_fmutstr_ref(dst.str.fmutstr_ref); \
            return err; \
        } \
        case NEAT_BUF_TY: \
        { \
            unsigned int len = (unsigned int) strlen((char*) dst.str.carr.ptr); \
            Neat_Fixed_Mut_String_Ref as_fixed = { \
                .chars = dst.str.carr.ptr, \
                .len = &len, \
                .cap = dst.str.carr.cap \
            }; \
            neat_uintger_tostr_fmutstr_ref(as_fixed); \
            return err; \
        } \
    } \
} while(0)

Neat_String_Error neat_bool_tostr(Neat_Mut_String_Ref dst, bool obj)
{
    char *res = obj ? "true" : "false";
    return neat_cstr_tostr(dst, res);
}

Neat_String_Error neat_cstr_tostr(Neat_Mut_String_Ref dst, char *obj)
{
    return neat_mutstr_ref_copy(
        dst,
        (Neat_String_View){
            .chars = (unsigned char*) obj,
                                .len = strlen(obj)
        }
    );
}

Neat_String_Error neat_ucstr_tostr(Neat_Mut_String_Ref dst, unsigned char *obj)
{
    return neat_mutstr_ref_copy(
        dst,
        (Neat_String_View){
            .chars = obj,
            .len = strlen((char*) obj)
        }
    );
}

Neat_String_Error neat_char_tostr(Neat_Mut_String_Ref dst, char obj)
{
    neat_mutstr_ref_clear(dst);
    return neat_mutstr_ref_putc(dst, obj);
}

Neat_String_Error neat_schar_tostr(Neat_Mut_String_Ref dst, signed char obj)
{
    neat_sinteger_tostr();
}

Neat_String_Error neat_uchar_tostr(Neat_Mut_String_Ref dst, unsigned char obj)
{
    neat_mutstr_ref_clear(dst);
    return neat_mutstr_ref_putc(dst, obj);
}

Neat_String_Error neat_short_tostr(Neat_Mut_String_Ref dst, short obj)
{
    neat_sinteger_tostr();
}

Neat_String_Error neat_ushort_tostr(Neat_Mut_String_Ref dst, unsigned short obj)
{
    neat_uinteger_tostr();
}

Neat_String_Error neat_int_tostr(Neat_Mut_String_Ref dst, int obj)
{
    neat_sinteger_tostr();
}

Neat_String_Error neat_uint_tostr(Neat_Mut_String_Ref dst, unsigned int obj)
{
    neat_uinteger_tostr();
}

Neat_String_Error neat_long_tostr(Neat_Mut_String_Ref dst, long obj)
{
    neat_sinteger_tostr();
}

Neat_String_Error neat_ulong_tostr(Neat_Mut_String_Ref dst, unsigned long obj)
{
    neat_uinteger_tostr();
}

Neat_String_Error neat_llong_tostr(Neat_Mut_String_Ref dst, long long obj)
{
    neat_sinteger_tostr();
}

Neat_String_Error neat_ullong_tostr(Neat_Mut_String_Ref dst, unsigned long long obj)
{
    neat_uinteger_tostr();
}

Neat_String_Error neat_float_tostr(Neat_Mut_String_Ref dst, float obj)
{
    char tmp[32] = { 0 };
    int len = snprintf(tmp, sizeof(tmp), "%g", obj);
    return neat_mutstr_ref_copy(
        dst,
        (Neat_String_View){
            .chars = (unsigned char*) tmp,
                                .len = len
        }
    );
}

Neat_String_Error neat_double_tostr(Neat_Mut_String_Ref dst, double obj)
{
    char tmp[32] = { 0 };
    int len = snprintf(tmp, sizeof(tmp), "%g", obj);
    return neat_mutstr_ref_copy(
        dst,
        (Neat_String_View){
            .chars = (unsigned char*) tmp,
                                .len = len
        }
    );
}

// TODO make all tostr functions take val not ptr

Neat_String_Error neat_dstr_tostr(Neat_Mut_String_Ref dst, Neat_DString obj)
{
    return neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj.chars, .len = obj.len});
}

Neat_String_Error neat_dstr_ptr_tostr(Neat_Mut_String_Ref dst, Neat_DString *obj)
{
    return neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->chars, .len = obj->len});
}

Neat_String_Error neat_strv_tostr(Neat_Mut_String_Ref dst, Neat_String_View obj)
{
    return neat_mutstr_ref_copy(dst, obj);
}

Neat_String_Error neat_strv_ptr_tostr(Neat_Mut_String_Ref dst, Neat_String_View *obj)
{
    return neat_mutstr_ref_copy(dst, *obj);
}

Neat_String_Error neat_strbuf_tostr(Neat_Mut_String_Ref dst, Neat_String_Buffer obj)
{
    return neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj.chars, .len = obj.len});
}

Neat_String_Error neat_strbuf_ptr_tostr(Neat_Mut_String_Ref dst, Neat_String_Buffer *obj)
{
    return neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->chars, .len = obj->len});
}

Neat_String_Error neat_sstr_ref_tostr(Neat_Mut_String_Ref dst, Neat_SString_Ref obj)
{
    return neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj.sstr->chars, .len = obj.sstr->len});
}

Neat_String_Error neat_mutstr_ref_tostr(Neat_Mut_String_Ref dst, Neat_Mut_String_Ref obj)
{
    return neat_mutstr_ref_copy(dst, neat_strv_mutstr_ref2(obj, 0));
}
