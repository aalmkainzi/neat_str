#include <string.h>
#include <ctype.h>
#define NEAT_STR_PREFIX
#include "../include/neat_str.h"

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
    unsigned char *str_end = memchr(chars, '\0', cap);
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

static unsigned int neat_mutstr_ref_len(Neat_Mut_String_Ref str)
{
    if(str.len != NULL)
    {
        return *str.len;
    }
    else
    {
        return neat_chars_strlen(str.chars, str.cap);
    }
}

bool neat_is_strv_intersect(Neat_String_View base, Neat_String_View sub)
{
    uintptr_t begin = (uintptr_t) base.chars;
    uintptr_t end   = ((uintptr_t) base.chars) + base.len;
    uintptr_t sub_begin = (uintptr_t) sub.chars;
    return sub_begin >= begin && sub_begin <= end;
}

NEAT_NODISCARD("discarding a new DString will cause memory leak") Neat_DString neat_dstr_new(unsigned int cap, Neat_Allocator allocator)
{
    if(allocator.ctx == NULL)
        allocator.init(&allocator.ctx, NULL);
    
    Neat_DString ret = { 0 };
    
    ret.allocator = allocator;
    size_t actual_allocated_cap;
    
    ret.chars = neat_alloc(allocator, unsigned char, cap, &actual_allocated_cap);
    ret.cap = actual_allocated_cap;
    
    if(ret.chars != NULL)
    {
        ret.chars[0] = '\0';
    }
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

void neat_dstr_maybe_grow(Neat_DString *dstr, unsigned int len_to_append)
{
    if(dstr->cap - dstr->len <= len_to_append)
    {
        // grow
        unsigned int new_cap = neat_uint_max(dstr->cap * 2, dstr->cap + len_to_append);
        
        size_t actual_allocated_cap;
        dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, new_cap, &actual_allocated_cap);
        dstr->cap = actual_allocated_cap;
    }
}

void neat_dstr_append_strv(Neat_DString *dstr, Neat_String_View str)
{
    Neat_String_View to_append = str;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), to_append))
    {
        unsigned int begin_idx = to_append.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, to_append.len);
        to_append = (Neat_String_View){
            .len   = to_append.len,
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        neat_dstr_maybe_grow(dstr, to_append.len);
    }
    
    memmove(dstr->chars + dstr->len, to_append.chars, to_append.len * sizeof(unsigned char));
    
    dstr->len += to_append.len;
    dstr->chars[dstr->len] = '\0';
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

NEAT_NODISCARD("dstr_insert returns error, true if success, false if fail") bool neat_dstr_insert_tostr_(Neat_DString *dstr, Neat_DString tostr, unsigned int idx)
{
    bool ret = neat_dstr_insert_strv(dstr, neat_strv_dstr2(tostr, 0), idx);
    neat_dstr_deinit_(&tostr);
    return ret;
}

unsigned int neat_dstr_fread_line_(Neat_DString *dstr, FILE *stream)
{
    dstr->len = 0;
    if(dstr->cap > 0)
    {
        dstr->chars[0] = '\0';
    }
    
    return neat_dstr_append_fread_line_(dstr, stream);
}

unsigned int neat_dstr_append_fread_line_(Neat_DString *dstr, FILE *stream)
{
    unsigned int prev_len = dstr->len;
    int c = 0;
    while(c != '\n' && (c=fgetc(stream)) != EOF)
    {
        unsigned char as_char = c;
        Neat_String_View as_strv = {.chars = &as_char, .len = 1};
        neat_dstr_append_strv(dstr, as_strv);
    }
    
    return dstr->len - prev_len;
}

void neat_dstr_ensure_cap_(Neat_DString *dstr, unsigned int at_least)
{
    if(dstr->cap <= at_least)
    {
        size_t actual_allocated_cap;
        dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, at_least, &actual_allocated_cap);
        dstr->cap = actual_allocated_cap;
    }
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

char *neat_dstr_ptr_as_cstr(Neat_DString *str)
{
    return (char*) str->chars;
}

char *neat_strv_as_cstr(Neat_String_View str)
{
    return (char*) str.chars;
}

char *neat_strv_ptr_as_cstr(Neat_String_View *str)
{
    return (char*) str->chars;
}

char *neat_strbuf_as_cstr(Neat_String_Buffer str)
{
    return (char*) str.chars;
}

char *neat_strbuf_ptr_as_cstr(Neat_String_Buffer *str)
{
    return (char*) str->chars;
}

char *neat_sstr_ref_as_cstr(Neat_SString_Ref str)
{
    return (char*) str.sstring->chars;
}

char *neat_mutstr_ref_as_cstr(Neat_Mut_String_Ref str)
{
    return (char*) str.chars;
}

unsigned char neat_cstr_char_at(char *str, unsigned int idx)
{
    return str[idx];
}

unsigned char neat_ucstr_char_at(unsigned char *str, unsigned int idx)
{
    return str[idx];
}

unsigned char neat_dstr_char_at(Neat_DString str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_dstr_ptr_char_at(Neat_DString *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_strv_char_at(Neat_String_View str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_strv_ptr_char_at(Neat_String_View *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_strbuf_char_at(Neat_String_Buffer str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned char neat_strbuf_ptr_char_at(Neat_String_Buffer *str, unsigned int idx)
{
    return str->chars[idx];
}

unsigned char neat_sstr_ref_char_at(Neat_SString_Ref str, unsigned int idx)
{
    return str.sstring->chars[idx];
}

unsigned char neat_mutstr_ref_char_at(Neat_Mut_String_Ref str, unsigned int idx)
{
    return str.chars[idx];
}

unsigned int neat_dstr_cap(Neat_DString str)
{
    return str.cap;
}

unsigned int neat_dstr_ptr_cap(Neat_DString *str)
{
    return str->cap;
}

unsigned int neat_strbuf_cap(Neat_String_Buffer str)
{
    return str.cap;
}

unsigned int neat_strbuf_ptr_cap(Neat_String_Buffer *str)
{
    return str->cap;
}
unsigned int neat_sstr_ref_cap(Neat_SString_Ref str)
{
    return str.cap;
}

unsigned int neat_mutstr_ref_cap(Neat_Mut_String_Ref str)
{
    return str.cap;
}

unsigned int neat_buf_cap(Neat_Buffer buf)
{
    return buf.size;
}

Neat_String_View neat_strv_strv2(Neat_String_View str, unsigned int begin)
{
    return neat_strv_strv_ptr2(&str, begin);
}

void neat_dstr_prepend_strv(Neat_DString *dstr, Neat_String_View src)
{
    Neat_String_View to_prepend = src;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), src))
    {
        unsigned int begin_idx = src.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, src.len);
        to_prepend = (Neat_String_View){
            .len = src.len, 
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        neat_dstr_maybe_grow(dstr, to_prepend.len);
    }
    
    memmove(dstr->chars + src.len, dstr->chars, dstr->len);
    memmove(dstr->chars, src.chars, src.len);
    
    dstr->len += src.len;
    dstr->chars[dstr->len] = '\0';
}

NEAT_NODISCARD("dstr_insert returns error, true if success, false if fail") bool neat_dstr_insert_strv(Neat_DString *dstr, Neat_String_View src, unsigned int idx)
{
    if(idx > dstr->len)
    {
        return false;
    }
    
    Neat_String_View to_insert = src;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), src))
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

unsigned int neat_mutstr_ref_insert_strv(Neat_Mut_String_Ref dst, Neat_String_View src, unsigned int idx)
{
    unsigned int len = neat_mutstr_ref_len(dst);
    
    if(idx > len)
    {
        return 0;
    }
    
    unsigned int nb_chars_to_insert = neat_uint_min(dst.cap - len - 1, src.len);
    
    // shift right
    memmove(dst.chars + idx + nb_chars_to_insert, dst.chars + idx, len - idx);
    
    // insert the src
    memmove(dst.chars + idx, src.chars, nb_chars_to_insert);
    
    len += nb_chars_to_insert;
    
    if(dst.len != NULL)
    {
        *dst.len = len;
    }
    
    return nb_chars_to_insert;
}

void neat_dstr_shrink_to_fit_(Neat_DString *dstr)
{
    size_t actual_new_cap;
    dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, dstr->len + 1, &actual_new_cap);
    dstr->cap = actual_new_cap;
}

bool neat_strv_equal(Neat_String_View str1, Neat_String_View str2)
{
    return
    (str1.len == str2.len) &&
    (memcmp(str1.chars, str2.chars, str1.len) == 0);
}

Neat_String_View neat_strv_find(Neat_String_View hay, Neat_String_View needle)
{
    unsigned int end = hay.len - needle.len;
    for(unsigned int i = 0 ; i <= end ; i++)
    {
        Neat_String_View sv = neat_strv_strv_ptr3(&hay, i, i + needle.len);
        if(neat_strv_equal(needle, sv))
        {
            return sv;
        }
    }
    return (Neat_String_View){
        .len = 0,
        .chars = NULL
    };
}

unsigned int neat_mutstr_ref_copy(Neat_Mut_String_Ref dst, Neat_String_View src)
{
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - 1);
    
    memmove(dst.chars, src.chars, chars_to_copy);
    dst.chars[chars_to_copy] = '\0';
    
    if(dst.len != NULL)
        *dst.len = chars_to_copy;
    
    return chars_to_copy;
}

unsigned int neat_mutstr_ref_concat(Neat_Mut_String_Ref dst, Neat_String_View src)
{
    unsigned int dst_len = neat_mutstr_ref_len(dst);
    
    if(dst_len >= dst.cap - 1)
        return 0;
    
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - dst_len - 1);
    memmove(dst.chars + dst_len, src.chars, chars_to_copy);
    
    dst_len += chars_to_copy;
    
    if(dst.len != NULL)
        *dst.len = dst_len;
    
    dst.chars[dst_len] = '\0';
    
    return chars_to_copy;
}

NEAT_NODISCARD("str_del returns true on success, false on failure") bool neat_mutstr_ref_delete_range(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end)
{
    unsigned int len = neat_mutstr_ref_len(str);
    
    if(end > len || begin > end)
    {
        return false;
    }
    
    unsigned int substr_len = end - begin;
    
    memmove(str.chars + begin, str.chars + begin + substr_len, len - begin - substr_len);
    
    len -= substr_len;
    
    str.chars[len] = '\0';
    
    if(str.len != NULL)
    {
        *str.len = len;
    }
    
    return true;
}

Neat_String_View_Array neat_strv_arr_from_carr(Neat_String_View *carr, unsigned int nb)
{
    return (Neat_String_View_Array){
        .cap  = nb,
        .nb   = nb,
        .strs = carr
    };
}

NEAT_NODISCARD("str_split returns new String_View_Array") Neat_String_View_Array neat_strv_split(Neat_String_View str, Neat_String_View delim, Neat_Allocator allocator)
{
    if(allocator.ctx == NULL)
        allocator.init(&allocator.ctx, NULL);
    
    if(delim.len > str.len)
    {
        size_t alloced_size;
        Neat_String_View *entire_str = neat_alloc(allocator, Neat_String_View, 1, &alloced_size);
        if(alloced_size < sizeof(Neat_String_View) || entire_str == NULL)
        {
            return (Neat_String_View_Array){
                .cap  = 0,
                .nb   = 0,
                .strs = NULL
            };
        }
        else
        {
            *entire_str = str;
            return (Neat_String_View_Array){
                .cap  = alloced_size,
                .nb   = 1,
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
                .nb   = 0,
                .strs = NULL
            };
        }
        else
        {
            Neat_String_View_Array ret = {
                .cap  = alloced_size,
                .nb   = str.len,
                .strs = strs
            };
            
            for(unsigned int i = 0 ; i < ret.nb ; i++)
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
                .nb   = 0,
                .strs = NULL
            };
        }
        else
        {
            Neat_String_View_Array ret = {
                .cap = alloced_size,
                .nb  = nb_delim + 1,
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

NEAT_NODISCARD("str_join_new returns new DString, discarding will cause memory leak") Neat_DString neat_strv_arr_join_new(Neat_String_View_Array strs, Neat_String_View delim, Neat_Allocator allocator)
{
    Neat_DString ret = neat_dstr_new(16, allocator);
    
    if(strs.nb > 0)
        neat_dstr_append_strv(&ret, strs.strs[0]);
    
    for(unsigned int i = 1 ; i < strs.nb ; i++)
    {
        neat_dstr_append_strv(&ret, delim);
        neat_dstr_append_strv(&ret, strs.strs[i]);
    }
    
    return ret;
}

unsigned int neat_strv_arr_join(Neat_Mut_String_Ref dst, Neat_String_View_Array strs, Neat_String_View delim)
{
    unsigned int chars_copied = 0;
    
    Neat_String_Buffer dst_as_buf = { 0 };
    dst_as_buf.cap = dst.cap;
    dst_as_buf.len = 0;
    dst_as_buf.chars = dst.chars;
    
    Neat_Mut_String_Ref dst2 = neat_mutstr_ref_to_strbuf_ptr(&dst_as_buf);
    
    if(strs.nb > 0)
    {
        chars_copied += neat_mutstr_ref_concat(dst2, strs.strs[0]);
    }
    for(unsigned int i = 1 ; i < strs.nb ; i++)
    {
        chars_copied += neat_mutstr_ref_concat(dst2, delim);
        chars_copied += neat_mutstr_ref_concat(dst2, strs.strs[i]);
    }
    
    if(dst.len != NULL)
        *dst.len = chars_copied;
    
    dst.chars[chars_copied] = '\0';
    
    return chars_copied;
}

unsigned int neat_mutstr_ref_replace(Neat_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    unsigned int replacements = 0;
    
    unsigned int len;
    if(str.len == NULL)
    {
        len = neat_chars_strlen(str.chars, str.cap);
        str.len = &len;
    }
    
    if(target.len == 0)
    {
        unsigned int total_inserted = 0;
        unsigned int inserted = 1;
        for(unsigned int i = 0 ; i <= *str.len && inserted ; i += replacement.len + 1)
        {
            inserted = neat_mutstr_ref_insert_strv(str, replacement, i);
            total_inserted += inserted;
        }
        return total_inserted;
    }
    
    if(target.len < replacement.len)
    {
        for(unsigned int i = 0 ; i <= *str.len - target.len; )
        {
            Neat_String_View match = neat_strv_find(neat_strv_mutstr_ref2(str, i), target);
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
                    replacements++;
                }
                else
                {
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
            Neat_String_View match = neat_strv_find(neat_strv_mutstr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                // shift left
                memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*str.len - idx - target.len) * sizeof(unsigned char));
                
                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                *str.len -= (target.len - replacement.len);
                
                i = idx + replacement.len;
                replacements++;
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
            Neat_String_View match = neat_strv_find(neat_strv_mutstr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                i = idx + replacement.len;
                replacements++;
            }
            else
            {
                break;
            }
        }
    }
    
    str.chars[*str.len] = '\0';
    return replacements;
}

bool neat_mutstr_ref_replace_first(Neat_Mut_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    bool replaced = false;
    
    unsigned int *len_p;
    unsigned int len;
    if(str.len != NULL)
    {
        len_p = str.len;
    }
    else
    {
        len = neat_chars_strlen(str.chars, str.cap);
        len_p = &len;
    }
    
    Neat_String_View match = neat_strv_find(neat_strv_mutstr_ref2(str, 0), target);
    if(match.chars != NULL)
    {
        if(target.len < replacement.len)
        {
            if(str.cap > *len_p + (replacement.len - target.len))
            {
                unsigned int idx = match.chars - str.chars;
                
                // shift right
                memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*len_p - idx - target.len) * sizeof(unsigned char));
                
                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                *len_p += (replacement.len - target.len);
                
                replaced = true;
            }
        }
        else if(target.len > replacement.len)
        {
            unsigned int idx = match.chars - str.chars;
            
            // shift left
            memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*len_p - idx - target.len) * sizeof(unsigned char));
            
            // put the replacement
            memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
            
            *len_p -= (target.len - replacement.len);
            
            replaced = true;
        }
        else
        {
            unsigned int idx = match.chars - str.chars;
            
            // put the replacement
            memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
            
            replaced = true;
        }
    }
    
    str.chars[*len_p] = '\0';
    return replaced;
}

unsigned int neat_strv_count(Neat_String_View hay, Neat_String_View needle)
{
    if(needle.len == 0)
        return 0;
    
    unsigned int count = 0;
    for(unsigned int i = 0 ; i <= hay.len - needle.len ; )
    {
        //Neat_String_View view = neat_strv_strv3(hay, i, i + needle.len);
        if(memcmp(hay.chars + i, needle.chars, needle.len) == 0)
            //neat_strv_equal(view, needle))
        {
            count++;
            i += needle.len;
        }
        else
        {
            i++;
        }
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

void neat_mutstr_ref_tolower(Neat_Mut_String_Ref str)
{
    unsigned int len = neat_mutstr_ref_len(str);
    for(unsigned int i = 0 ; i < len ; i++)
    {
        str.chars[i] = tolower(str.chars[i]);
    }
}

void neat_mutstr_ref_toupper(Neat_Mut_String_Ref str)
{
    unsigned int len = neat_mutstr_ref_len(str);
    for(unsigned int i = 0 ; i < len ; i++)
    {
        str.chars[i] = toupper(str.chars[i]);
    }
}

Neat_Mut_String_Ref neat_mutstr_ref_to_cstr(char *str)
{
    unsigned int len = strlen(str);
    
    return (Neat_Mut_String_Ref){
        .cap   = len + 1,
        .len   = NULL,
        .chars = (unsigned char*) str
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_ucstr(unsigned char *str)
{
    unsigned int len = strlen((char*) str);
    
    return (Neat_Mut_String_Ref){
        .cap   = len + 1,
        .len   = NULL,
        .chars = str
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_dstr_ptr(Neat_DString *str)
{
    return (Neat_Mut_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_strbuf_ptr(Neat_String_Buffer *str)
{
    return (Neat_Mut_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_sstr_ref(Neat_SString_Ref str)
{
    return (Neat_Mut_String_Ref){
        .cap   = str.cap,
        .len   = &str.sstring->len,
        .chars = str.sstring->chars
    };
}

Neat_Mut_String_Ref neat_mutstr_ref_to_mutstr_ref(Neat_Mut_String_Ref str)
{
    return str;
}

Neat_String_Buffer neat_strbuf_new(unsigned int cap, Neat_Allocator allocator)
{
    if(allocator.ctx == NULL)
        allocator.init(&allocator.ctx, NULL);
    
    Neat_String_Buffer ret = { 0 };
    size_t actual_allocated_cap;
    ret.chars = neat_alloc(allocator, unsigned char, cap, &actual_allocated_cap);
    ret.cap = actual_allocated_cap;
    
    return ret;
}

Neat_String_Buffer neat_strbuf_new_default(unsigned int cap)
{
    return neat_strbuf_new(cap, neat_get_default_allocator());
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
    return neat_strbuf_from_ptr(buf.ptr, buf.size);
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
    if(begin > str.sstring->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str.sstring->len   - begin,
        .chars = str.sstring->chars + begin
    };
}

Neat_String_View neat_strv_mutstr_ref2(Neat_Mut_String_Ref str, unsigned int begin)
{
    unsigned int len = neat_mutstr_ref_len(str);
    
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
    if(begin > str.sstring->len || end > str.sstring->len || begin > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - begin,
        .chars = str.sstring->chars + begin
    };
}

Neat_String_View neat_strv_mutstr_ref3(Neat_Mut_String_Ref str, unsigned int begin, unsigned int end)
{
    unsigned int len = neat_mutstr_ref_len(str);
    
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

Neat_DString neat_tostr_all_into_new_dstr(Neat_Allocator allocator, unsigned int nb, ...)
{
    Neat_DString ret = neat_dstr_new(nb * 16, allocator);
    
    va_list args;
    va_start(args, nb);
    
    for(unsigned int i = 0 ; i < nb ; i++)
    {
        Neat_DString current = va_arg(args, Neat_DString);
        neat_dstr_append_strv(&ret, (Neat_String_View){.chars = current.chars, .len = current.len});
        neat_dstr_deinit_(&current);
    }
    
    va_end(args);
    
    return ret;
}

unsigned int neat_mutstr_ref_fread_line( Neat_Mut_String_Ref dst, FILE *stream)
{
    if(dst.cap == 0)
    {
        return 0;
    }
    
    unsigned int len = 0;
    int c = 0;
    while(len < dst.cap - 1 && c != '\n' && (c=fgetc(stream)) != EOF)
    {
        dst.chars[len] = c;
        len += 1;
    }
    
    dst.chars[len] = '\0';
    
    if(dst.len != NULL)
    {
        *dst.len = len;
    }
    
    return len;
}

unsigned int neat_mutstr_ref_concat_fread_line(Neat_Mut_String_Ref dst, FILE *stream)
{
    unsigned int dst_len = neat_mutstr_ref_len(dst);
    
    unsigned int concated_len = 0;
    
    Neat_Mut_String_Ref right = {
        .cap = dst.cap - dst_len,
        .len = &concated_len
    };
    right.chars = dst.chars + dst_len;
    
    unsigned int chars_read = neat_mutstr_ref_fread_line(right, stream);
    dst_len += chars_read;
    
    if(dst.len != NULL)
    {
        *dst.len = dst_len;
    }
    
    dst.chars[dst_len] = '\0';
    
    return chars_read;
}

unsigned int neat_fprint_strv(FILE *stream, Neat_String_View str)
{
    return fwrite(str.chars, sizeof(unsigned char), str.len, stream);
}

unsigned int neat_fprintln_strv(FILE *stream, Neat_String_View str)
{
    unsigned int written = fwrite(str.chars, sizeof(unsigned char), str.len, stream);
    written += fwrite(u8"\n", sizeof(unsigned char), 1, stream);
    return written;
}

// default tostr functions:

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_bool(bool *obj)
{
    Neat_DString ret = neat_dstr_new(6, neat_get_default_allocator());
    
    if(*obj)
        neat_str_copy(&ret, (char*) "true");
    else
        neat_str_copy(&ret, (char*) "false");
    
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_cstr(char **obj)
{
    Neat_DString ret = neat_dstr_new(15, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_ucstr(unsigned char **obj)
{
    Neat_DString ret = neat_dstr_new(15, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_char(char *obj)
{
    Neat_DString ret = neat_dstr_new(2, neat_get_default_allocator());
    char cstr[2] = {*obj, 0};
    neat_dstr_append(&ret, cstr);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_schar(signed char *obj)
{
    Neat_DString ret = neat_dstr_new(5, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%hhd", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_uchar(unsigned char *obj)
{
    Neat_DString ret = neat_dstr_new(2, neat_get_default_allocator());
    unsigned char cstr[2] = {*obj, 0};
    neat_dstr_append(&ret, cstr);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_short(short *obj)
{
    Neat_DString ret = neat_dstr_new(8, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%hd", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_ushort(unsigned short *obj)
{
    Neat_DString ret = neat_dstr_new(8, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%hu", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_int(int *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%d", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_uint(unsigned int *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%u", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_long(long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%ld", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_ulong(unsigned long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%lu", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_llong(long long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%lld", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_ullong(unsigned long long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%llu", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_float(float *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%g", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_double(double *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = snprintf((char*) ret.chars, ret.cap, "%g", *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_dstr(Neat_DString *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append(&ret, obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_dstr_ptr(Neat_DString **obj)
{
    return neat_tostr_dstr(*obj);
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_strv(Neat_String_View *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append_strv(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_strv_ptr(Neat_String_View **obj)
{
    return neat_tostr_strv(*obj);
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_strbuf(Neat_String_Buffer *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_strbuf_ptr(Neat_String_Buffer **obj)
{
    return neat_tostr_strbuf(*obj);
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_sstr_ref(Neat_SString_Ref *obj)
{
    Neat_DString ret = neat_dstr_new(obj->sstring->len, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

NEAT_NODISCARD("tostr returns a new DString, discarding will cause memory leak") Neat_DString neat_tostr_mutstr_ref(Neat_Mut_String_Ref *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

static const char neat_digit_ones[] = {
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
};

static const char neat_digit_tens[] = {
'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
'1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
'2', '2', '2', '2', '2', '2', '2', '2', '2', '2',
'3', '3', '3', '3', '3', '3', '3', '3', '3', '3',
'4', '4', '4', '4', '4', '4', '4', '4', '4', '4',
'5', '5', '5', '5', '5', '5', '5', '5', '5', '5',
'6', '6', '6', '6', '6', '6', '6', '6', '6', '6',
'7', '7', '7', '7', '7', '7', '7', '7', '7', '7',
'8', '8', '8', '8', '8', '8', '8', '8', '8', '8',
'9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
};

static const unsigned long long neat_ten_pows[] = {
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
10000000000000000000ull
};

#define neat_tostr_into_signed() \
if(dst.cap <= 1) \
    return; \
 \
typeof(*obj) i = *obj; \
int q, r; \
unsigned char size_of_str = neat_string_size_of_ll(i); \
if(size_of_str >= dst.cap) \
{ \
    i /= neat_ten_pows[size_of_str - dst.cap + 1]; \
    size_of_str = dst.cap - 1; \
} \
int charPos = size_of_str; \
 \
bool negative = i < 0; \
if (!negative) { \
    i = -i; \
} \
 \
while (i <= -100) { \
    q = i / 100; \
    r = (q * 100) - i; \
    i = q; \
    dst.chars[--charPos] = neat_digit_ones[r]; \
    dst.chars[--charPos] = neat_digit_tens[r]; \
} \
\
dst.chars[--charPos] = neat_digit_ones[-i]; \
if (i < -9) { \
    dst.chars[--charPos] = neat_digit_tens[-i]; \
} \
\
if (negative) { \
    dst.chars[--charPos] = '-'; \
} \
\
if(dst.len != NULL) \
    *dst.len = size_of_str;


static unsigned char neat_string_size_of_ll(long long x) {
    int d = 1;
    if (x >= 0) {
        d = 0;
        x = -x;
    }
    int p = -10;
    for (int i = 1; i < 10; i++) {
        if (x > p)
            return i + d;
        p = 10 * p;
    }
    return 10 + d;
}

void neat_tostr_into_bool(Neat_Mut_String_Ref dst, bool *obj)
{
    char *res = *obj ? "true" : "false";
    neat_tostr_into_cstr(dst, &res);
}

void neat_tostr_into_cstr(Neat_Mut_String_Ref dst, char **obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = (unsigned char*) *obj, .len = strlen(*obj)});
}

void neat_tostr_into_ucstr(Neat_Mut_String_Ref dst, unsigned char **obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = *obj, .len = strlen((char*) *obj)});
}

void neat_tostr_into_char(Neat_Mut_String_Ref dst, char *obj)
{
    if(dst.cap > 1)
    {
        dst.chars[0] = *obj;
        if(dst.len != NULL)
            *dst.len = 1;
    }
}

void neat_tostr_into_schar(Neat_Mut_String_Ref dst, signed char *obj)
{
    neat_tostr_into_signed();
}

void neat_tostr_into_uchar(Neat_Mut_String_Ref dst, unsigned char *obj)
{
    if(dst.cap > 1)
    {
        dst.chars[0] = *obj;
        if(dst.len != NULL)
            *dst.len = 1;
    }
}

void neat_tostr_into_short(Neat_Mut_String_Ref dst, short *obj)
{
    neat_tostr_into_signed();
}

void neat_tostr_into_ushort(Neat_Mut_String_Ref dst, unsigned short *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%hu", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_tostr_into_int(Neat_Mut_String_Ref dst, int *obj)
{
    neat_tostr_into_signed();
}

void neat_tostr_into_uint(Neat_Mut_String_Ref dst, unsigned int *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%u", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_tostr_into_long(Neat_Mut_String_Ref dst, long *obj)
{
    neat_tostr_into_signed();
}

void neat_tostr_into_ulong(Neat_Mut_String_Ref dst, unsigned long *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%lu", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_tostr_into_llong(Neat_Mut_String_Ref dst, long long *obj)
{
    neat_tostr_into_signed();
}

void neat_tostr_into_ullong(Neat_Mut_String_Ref dst, unsigned long long *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%llu", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_tostr_into_float(Neat_Mut_String_Ref dst, float *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%g", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_tostr_into_double(Neat_Mut_String_Ref dst, double *obj)
{
    snprintf((char*) dst.chars, dst.cap, "%g", *obj);
    if(dst.len != NULL)
        *dst.len = strlen((char*) dst.chars);
}

void neat_tostr_into_dstr(Neat_Mut_String_Ref dst, Neat_DString *obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->chars, .len = obj->len});
}

void neat_tostr_into_dstr_ptr(Neat_Mut_String_Ref dst, Neat_DString **obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj[0]->chars, .len = obj[0]->len});
}

void neat_tostr_into_strv(Neat_Mut_String_Ref dst, Neat_String_View *obj)
{
    neat_mutstr_ref_copy(dst, *obj);
}

void neat_tostr_into_strv_ptr(Neat_Mut_String_Ref dst, Neat_String_View **obj)
{
    neat_mutstr_ref_copy(dst, **obj);
}

void neat_tostr_into_strbuf(Neat_Mut_String_Ref dst, Neat_String_Buffer *obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->chars, .len = obj->len});
}

void neat_tostr_into_strbuf_ptr(Neat_Mut_String_Ref dst, Neat_String_Buffer **obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj[0]->chars, .len = obj[0]->len});
}

void neat_tostr_into_sstr_ref(Neat_Mut_String_Ref dst, Neat_SString_Ref *obj)
{
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->sstring->chars, .len = obj->sstring->len});
}

void neat_tostr_into_mutstr_ref(Neat_Mut_String_Ref dst, Neat_Mut_String_Ref *obj)
{
    unsigned int len = neat_mutstr_ref_len(*obj);
    neat_mutstr_ref_copy(dst, (Neat_String_View){.chars = obj->chars, .len = len});
}
