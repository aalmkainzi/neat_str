#include "neat_str.h"

static inline unsigned int neat_uint_min(unsigned int a, unsigned int b)
{
    return a < b ? a : b;
}

static inline unsigned int neat_uint_max(unsigned int a, unsigned int b)
{
    return a > b ? a : b;
}

bool neat_is_strv_intersect(Neat_String_View base, Neat_String_View sub)
{
    uintptr_t start = (uintptr_t) base.chars;
    uintptr_t end   = (uintptr_t) base.chars + base.len;
    uintptr_t sub_start = (uintptr_t) sub.chars;
    return sub_start >= start && sub_start <= end;
}

Neat_DString neat_dstr_new(unsigned int cap, Neat_Allocator allocator)
{
    allocator.init(&allocator.ctx);
    Neat_DString ret = {
        .allocator = allocator,
        .cap = cap + 1,
        .len = 0,
    };
    ret.chars = neat_alloc(allocator, unsigned char, cap + 1);
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
        dstr->chars = neat_realloc(dstr->allocator, dstr->chars, unsigned char, dstr->cap, new_cap);
        dstr->cap = new_cap;
    }
}

void neat_dstr_append_strv(Neat_DString *dstr, Neat_String_View str)
{
    Neat_String_View to_append = str;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), str))
    {
        unsigned int begin_idx = str.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, str.len);
        to_append = (Neat_String_View){
            .len = str.len, 
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

bool neat_dstr_insert_tostr_(Neat_DString *dstr, Neat_DString tostr, unsigned int idx)
{
    bool ret = neat_dstr_insert_strv(dstr, neat_strv_dstr2(tostr, 0), idx);
    neat_dstr_deinit_(&tostr);
    return ret;
}

Neat_String_View neat_strv_strv2(Neat_String_View str, unsigned int start)
{
    return neat_strv_strv_ptr2(&str, start);
}

void neat_dstr_prepend_strv(Neat_DString *dstr, Neat_String_View str)
{
    Neat_String_View to_prepend = str;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), str))
    {
        unsigned int begin_idx = str.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, str.len);
        to_prepend = (Neat_String_View){
            .len = str.len, 
            .chars = dstr->chars + begin_idx
        };
    }
    else
    {
        neat_dstr_maybe_grow(dstr, to_prepend.len);
    }
    
    memmove(dstr->chars + str.len, dstr->chars, dstr->len);
    memmove(dstr->chars, str.chars, str.len);
    
    dstr->len += str.len;
    dstr->chars[dstr->len] = '\0';
}

bool neat_dstr_insert_strv(Neat_DString *dstr, Neat_String_View str, unsigned int idx)
{
    if(idx > dstr->len)
    {
        return false;
    }
    
    Neat_String_View to_insert = str;
    
    if(neat_is_strv_intersect(neat_strv_dstr_ptr2(dstr, 0), str))
    {
        unsigned int begin_idx = str.chars - dstr->chars;
        neat_dstr_maybe_grow(dstr, str.len);
        to_insert = (Neat_String_View){
            .len = str.len, 
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

bool neat_strv_equal(Neat_String_View str1, Neat_String_View str2)
{
    return
    (str1.len == str2.len) &&
    (memcmp(str1.chars, str2.chars, str1.len) == 0);
}

Neat_String_View neat_strv_find(Neat_String_View hay, Neat_String_View needle)
{
    unsigned int end = hay.len - needle.len;
    for(unsigned int i = 0 ; i < end ; i++)
    {
        Neat_String_View sv = neat_strv_strv_ptr3(&hay, i, i + needle.len);
        if(neat_strv_equal(needle, sv))
        {
            return sv;
        }
    }
    return (Neat_String_View){
        .len=0,
        .chars = NULL
    };
}

Neat_SString_Ref neat_sstr_ref_from_sstr_ptr(void *sstr_ptr, unsigned int cap)
{
    Neat_SString_Ref ret = {.cap = cap};
    ret.sstring = (typeof(ret.sstring)) sstr_ptr;
    return ret;
}

unsigned int neat_anystr_ref_copy(Neat_Any_String_Ref dst, Neat_String_View src)
{
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - 1);
    
    memmove(dst.chars, src.chars, chars_to_copy);
    dst.chars[chars_to_copy] = '\0';
    
    if(dst.len != NULL)
        *dst.len = chars_to_copy;
    
    return chars_to_copy;
}

unsigned int neat_anystr_ref_concat(Neat_Any_String_Ref dst, Neat_String_View src)
{
    unsigned int len;
    if(dst.len != NULL)
    {
        len = *dst.len;
    }
    else
    {
        len = strlen((char*) dst.chars);
    }
    
    if(len >= dst.cap - 1)
        return 0;
    
    unsigned int chars_to_copy = neat_uint_min(src.len, dst.cap - len - 1);
    memmove(dst.chars + len, src.chars, chars_to_copy);
    
    len += chars_to_copy;
    
    if(dst.len != NULL)
        *dst.len = len;
    
    dst.chars[len] = '\0';
    
    return chars_to_copy;
}

Neat_DString neat_strv_concat_new(Neat_String_View str1, Neat_String_View str2, Neat_Allocator allocator)
{
    Neat_DString ret = neat_dstr_new(str1.len + str2.len + 1, allocator);
    neat_dstr_append_strv(&ret, str1);
    neat_dstr_append_strv(&ret, str2);
    return ret;
}

unsigned int neat_anystr_ref_concat_strv_arr(Neat_Any_String_Ref dst, Neat_String_View_Array src)
{
    unsigned int total_chars_copied = 0;
    unsigned int chars_copied = 1;
    
    for(unsigned int i = 0 ; i < src.nb && chars_copied != 0 ; i++)
    {
        chars_copied = neat_anystr_ref_concat(dst, src.strs[i]);
        total_chars_copied += chars_copied;
    }
    
    return total_chars_copied;
}

Neat_DString neat_anystr_ref_concat_strv_arr_new(Neat_String_View_Array src, Neat_Allocator allocator)
{
    // lazy solution, fix later.
    return neat_strv_arr_join_new(neat_strv_cstr2((char*) "", 0), src, allocator);
}

Neat_String_View_Array neat_strv_split(Neat_String_View str, Neat_String_View delim, Neat_Allocator allocator)
{
    allocator.init(&allocator.ctx);
    
    if(delim.len > str.len)
    {
        Neat_String_View *copy = neat_alloc(allocator, Neat_String_View, 1);
        *copy = str;
        return (Neat_String_View_Array){
            .nb   = 1,
            .strs = copy
        };
    }
    
    unsigned int nb_delim = 0;
    unsigned int *delim_idx = (unsigned int*) calloc(str.len, sizeof(unsigned int));
    
    for(unsigned int i = 0 ; i < str.len - delim.len ; )
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
    
    Neat_String_View_Array ret = { 0 };
    ret.strs = neat_alloc(allocator, Neat_String_View, nb_delim + 1);
    
    ret.nb = nb_delim + 1;
    
    ret.strs[0] = neat_strv_strv_ptr3(&str, 0, delim_idx[0]);
    for(unsigned int i = 1 ; i <= nb_delim ; i++)
    {
        ret.strs[i] = neat_strv_strv_ptr3(&str, delim_idx[i - 1] + delim.len, delim_idx[i]);
    }
    
    free(delim_idx);
    return ret;
}

Neat_DString neat_strv_arr_join_new(Neat_String_View delim, Neat_String_View_Array strs, Neat_Allocator allocator)
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

unsigned int neat_strv_arr_join(Neat_Any_String_Ref dst, Neat_String_View delim, Neat_String_View_Array strs)
{
    if(dst.len != NULL)
        *dst.len = 0;
    
    unsigned int chars_copied = 0;
    
    if(strs.nb > 0)
        chars_copied += neat_anystr_ref_concat(dst, strs.strs[0]);
    
    for(unsigned int i = 1 ; i < strs.nb ; i++)
    {
        chars_copied += neat_anystr_ref_concat(dst, delim);
        chars_copied += neat_anystr_ref_concat(dst, strs.strs[i]);
    }
    
    if(dst.len != NULL)
        *dst.len = chars_copied;
    
    dst.chars[chars_copied] = '\0';
    
    return chars_copied;
}

unsigned int neat_anystr_ref_replace(Neat_Any_String_Ref str, Neat_String_View target, Neat_String_View replacement)
{
    unsigned int replacements = 0;
    unsigned int *len_p;
    unsigned int len;
    if(str.len != NULL)
    {
        len_p = str.len;
    }
    else
    {
        len = strlen((char*) str.chars);
        len_p = &len;
    }
    
    if(target.len < replacement.len)
    {
        for(unsigned int i = 0 ; i <= *len_p - target.len; )
        {
            Neat_String_View match = neat_strv_find(neat_strv_anystr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                if(str.cap > *len_p + (replacement.len - target.len))
                {
                    // shift right
                    memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*len_p - idx - target.len) * sizeof(unsigned char));
                    
                    // put the replacement
                    memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                    
                    *len_p += (replacement.len - target.len);
                    
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
        for(unsigned int i = 0 ; i <= *len_p - target.len; )
        {
            Neat_String_View match = neat_strv_find(neat_strv_anystr_ref2(str, i), target);
            if(match.chars != NULL)
            {
                unsigned int idx = match.chars - str.chars;
                
                // shift left
                memmove(str.chars + idx + replacement.len, str.chars + idx + target.len, (*len_p - idx - target.len) * sizeof(unsigned char));
                
                // put the replacement
                memmove(str.chars + idx, replacement.chars, replacement.len * sizeof(unsigned char));
                
                *len_p -= (target.len - replacement.len);
                
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
        for(unsigned int i = 0 ; i <= *len_p - target.len; )
        {
            Neat_String_View match = neat_strv_find(neat_strv_anystr_ref2(str, i), target);
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
    
    str.chars[*len_p] = '\0';
    return replacements;
}

unsigned int neat_strv_count(Neat_String_View hay, Neat_String_View needle)
{
    unsigned int count = 0;
    for(unsigned int i = 0 ; i <= hay.len - needle.len ; )
    {
        Neat_String_View view = neat_strv_strv3(hay, i, i + needle.len);
        if(neat_strv_equal(view, needle))
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

Neat_Any_String_Ref neat_anystr_ref_to_cstr(char *str)
{
    unsigned int len = strlen(str);
    
    return (Neat_Any_String_Ref){
        .cap   = len,
        .len   = NULL,
        .chars = (unsigned char*) str
    };
}

Neat_Any_String_Ref neat_anystr_ref_to_ucstr(unsigned char *str)
{
    unsigned int len = strlen((char*) str);
    
    return (Neat_Any_String_Ref){
        .cap   = len,
        .len   = NULL,
        .chars = str
    };
}

Neat_Any_String_Ref neat_anystr_ref_to_dstr_ptr(Neat_DString *str)
{
    return (Neat_Any_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Neat_Any_String_Ref neat_anystr_ref_to_strv_ptr(Neat_String_View *str)
{
    return (Neat_Any_String_Ref){
        .cap   = str->len,
        .len   = &str->len,
        .chars = str->chars
    };
}

Neat_Any_String_Ref neat_anystr_ref_to_strbuf_ptr(Neat_String_Buffer *str)
{
    return (Neat_Any_String_Ref){
        .cap   = str->cap,
        .len   = &str->len,
        .chars = str->chars
    };
}

Neat_Any_String_Ref neat_anystr_ref_to_sstr_ref(Neat_SString_Ref str)
{
    return (Neat_Any_String_Ref){
        .cap   = str.cap,
        .len   = &str.sstring->len,
        .chars = str.sstring->chars
    };
}

Neat_Any_String_Ref neat_anystr_ref_to_anystr_ref(Neat_Any_String_Ref str)
{
    return str;
}

Neat_String_Buffer neat_strbuf_new(unsigned int cap, Neat_Allocator allocator)
{
    allocator.init(&allocator.ctx);
    return (Neat_String_Buffer){
        .cap   = cap + 1,
        .len   = 0,
        .chars = neat_alloc(allocator, unsigned char, cap + 1)
    };
}

Neat_String_Buffer neat_strbuf_new_default(unsigned int cap)
{
    return neat_strbuf_new(cap, neat_get_default_allocator());
}

Neat_String_Buffer neat_strbuf_of_cstr(char *str)
{
    unsigned int len = strlen(str);
    return (Neat_String_Buffer){
        .cap   = len + 1,
        .len   = len,
        .chars = (unsigned char*) str
    };
}

Neat_String_Buffer neat_strbuf_of_ucstr(unsigned char *str)
{
    unsigned int len = strlen((char*) str);
    return (Neat_String_Buffer){
        .cap   = len + 1,
        .len   = len,
        .chars = (unsigned char*) str
    };
}

Neat_String_Buffer neat_strbuf_of_dstr_ptr(Neat_DString *str)
{
    return (Neat_String_Buffer){
        .cap   = str->cap,
        .len   = str->len,
        .chars = str->chars
    };
}

Neat_String_Buffer neat_strbuf_of_strv_ptr(Neat_String_View *str)
{
    return (Neat_String_Buffer){
        .cap   = str->len,
        .len   = str->len,
        .chars = str->chars
    };
}

Neat_String_Buffer neat_strbuf_of_strbuf_ptr(Neat_String_Buffer *str)
{
    return *str;
}

Neat_String_Buffer neat_strbuf_of_sstr_ref(Neat_SString_Ref str)
{
    return (Neat_String_Buffer){
        .cap   = str.cap,
        .len   = str.sstring->len,
        .chars = str.sstring->chars
    };
}

Neat_String_Buffer neat_strbuf_of_anystr_ref(Neat_Any_String_Ref str)
{
    return (Neat_String_Buffer){
        .cap   = str.cap,
        .len   = str.len != NULL ? *str.len : (unsigned int) strlen((char*) str.chars),
        .chars = str.chars
    };
}

Neat_String_Buffer neat_strbuf_of_dstr(Neat_DString str)
{
    return neat_strbuf_of_dstr_ptr(&str);
}

Neat_String_Buffer neat_strbuf_of_strv(Neat_String_View str)
{
    return neat_strbuf_of_strv_ptr(&str);
}

Neat_String_Buffer neat_strbuf_of_strbuf(Neat_String_Buffer str)
{
    return str;
}

Neat_String_View neat_strv_cstr2(char *str, unsigned int start)
{
    unsigned int len = strlen(str);
    
    if(start > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len - start,
        .chars = (unsigned char*) str + start
    };
}

Neat_String_View neat_strv_ucstr2(unsigned char *str, unsigned int start)
{
    unsigned int len = strlen((char*) str);
    
    if(start > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len - start,
        .chars = str + start
    };
}

Neat_String_View neat_strv_dstr2(Neat_DString str, unsigned int start)
{
    return neat_strv_dstr_ptr2(&str, start);
}

Neat_String_View neat_strv_dstr_ptr2(Neat_DString *str, unsigned int start)
{
    if(start > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

Neat_String_View neat_strv_strv_ptr2(Neat_String_View *str, unsigned int start)
{
    if(start > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

Neat_String_View neat_strv_strbuf_ptr2(Neat_String_Buffer *str, unsigned int start)
{
    if(start > str->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str->len   - start,
        .chars = str->chars + start
    };
}

Neat_String_View neat_strv_strbuf2(Neat_String_Buffer str, unsigned int start)
{
    return neat_strv_strbuf_ptr2(&str, start);
}

Neat_String_View neat_strv_sstr_ref2(Neat_SString_Ref str, unsigned int start)
{
    if(start > str.sstring->len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = str.sstring->len   - start,
        .chars = str.sstring->chars + start
    };
}

Neat_String_View neat_strv_anystr_ref2(Neat_Any_String_Ref str, unsigned int start)
{
    unsigned int len = 0;
    if(str.len == NULL)
    {
        len = strlen((char*) str.chars);
    }
    else
    {
        len = *str.len;
    }
    
    if(start > len)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = len       - start,
        .chars = str.chars + start
    };
}

Neat_String_View neat_strv_cstr3(char *str, unsigned int start, unsigned int end)
{
    unsigned int len = strlen(str);
    
    if(start > len || end > len || start > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - start,
        .chars = (unsigned char*) str + start
    };
}

Neat_String_View neat_strv_ucstr3(unsigned char *str, unsigned int start, unsigned int end)
{
    unsigned int len = strlen((char*) str);
    
    if(start > len || end > len || start > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - start,
        .chars = str + start
    };
}

Neat_String_View neat_strv_dstr_ptr3(Neat_DString *str, unsigned int start, unsigned int end)
{
    if(start > str->len || end > str->len || start > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - start,
        .chars = str->chars + start
    };
}

Neat_String_View neat_strv_strv_ptr3(Neat_String_View *str, unsigned int start, unsigned int end)
{
    if(start > str->len || end > str->len || start > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - start,
        .chars = str->chars + start
    };
}

Neat_String_View neat_strv_strbuf_ptr3(Neat_String_Buffer *str, unsigned int start, unsigned int end)
{
    if(start > str->len || end > str->len || start > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - start,
        .chars = str->chars + start
    };
}

Neat_String_View neat_strv_sstr_ref3(Neat_SString_Ref str, unsigned int start, unsigned int end)
{
    if(start > str.sstring->len || end > str.sstring->len || start > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - start,
        .chars = str.sstring->chars + start
    };
}

Neat_String_View neat_strv_anystr_ref3(Neat_Any_String_Ref str, unsigned int start, unsigned int end)
{
    unsigned int len = 0;
    if(str.len == NULL)
    {
        len = strlen((char*) str.chars);
    }
    else
    {
        len = *str.len;
    }
    
    if(start > len || end > len || start > end)
    {
        return (Neat_String_View){
            .len = 0,
            .chars = NULL
        };
    }
    
    return (Neat_String_View){
        .len   = end - start,
        .chars = str.chars + start
    };
}

Neat_String_View neat_strv_dstr3(Neat_DString str, unsigned int start, unsigned int end)
{
    return neat_strv_dstr_ptr3(&str, start, end);
}

Neat_String_View neat_strv_strv3(Neat_String_View str, unsigned int start, unsigned int end)
{
    return neat_strv_strv_ptr3(&str, start, end);
}

Neat_String_View neat_strv_strbuf3(Neat_String_Buffer str, unsigned int start, unsigned int end)
{
    return neat_strv_strbuf_ptr3(&str, start, end);
}

unsigned int neat_anystr_ref_fread_line(FILE *stream, Neat_Any_String_Ref dst)
{
    unsigned int len;
    if(dst.len != NULL)
    {
        len = *dst.len;
    }
    else
    {
        len = strlen((char*) dst.chars);
    }
    
    unsigned char prev_len = len;
    int c = 0;
    while(c != '\n' && !feof(stream))
    {
        c = fgetc(stream);
        dst.chars[len] = c;
        len += 1;
    }
    
    if(dst.len != NULL)
    {
        *dst.len = len;
    }
    
    unsigned int chars_read = len - prev_len;
    return chars_read;
}

unsigned int neat_anystr_ref_concat_fread_line(FILE *stream, Neat_Any_String_Ref dst)
{
    unsigned int dst_len;
    if(dst.len != NULL)
    {
        dst_len = *dst.len;
    }
    else
    {
        dst_len = strlen((char*) dst.chars);
    }
    
    unsigned int concated_len = 0;
    
    Neat_Any_String_Ref right = {
        .cap = dst.cap - dst_len,
        .len = &concated_len
    };
    
    right.chars = dst.chars + dst_len;
    unsigned int chars_read = neat_anystr_ref_fread_line(stream, right);
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

Neat_DString neat_tostr_bool(bool *obj)
{
    Neat_DString ret = neat_dstr_new(6, neat_get_default_allocator());
    
    if(*obj)
        neat_str_copy(&ret, (char*) "true");
    else
        neat_str_copy(&ret, (char*) "false");
    
    return ret;
}

Neat_DString neat_tostr_str(char **obj)
{
    Neat_DString ret = neat_dstr_new(strlen(*obj) + 1, neat_get_default_allocator());
    neat_str_copy(&ret, *obj);
    return ret;
}

Neat_DString neat_tostr_ustr(unsigned char **obj)
{
    Neat_DString ret = neat_dstr_new(strlen((char*) *obj) + 1, neat_get_default_allocator());
    neat_str_copy(&ret, *obj);
    return ret;
}

Neat_DString neat_tostr_char(char *obj)
{
    Neat_DString ret = neat_dstr_new(5, neat_get_default_allocator());
    char cstr[2] = {*obj, 0};
    neat_dstr_append(&ret, cstr);
    return ret;
}

Neat_DString neat_tostr_schar(signed char *obj)
{
    Neat_DString ret = neat_dstr_new(5, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%hhd", *obj);
    return ret;
}

Neat_DString neat_tostr_uchar(unsigned char *obj)
{
    Neat_DString ret = neat_dstr_new(4, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%hhu", *obj);
    return ret;
}

Neat_DString neat_tostr_short(short *obj)
{
    Neat_DString ret = neat_dstr_new(8, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%hd", *obj);
    return ret;
}

Neat_DString neat_tostr_ushort(unsigned short *obj)
{
    Neat_DString ret = neat_dstr_new(8, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%hu", *obj);
    return ret;
}

Neat_DString neat_tostr_int(int *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%d", *obj);
    return ret;
}

Neat_DString neat_tostr_uint(unsigned int *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%u", *obj);
    return ret;
}

Neat_DString neat_tostr_long(long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%ld", *obj);
    return ret;
}

Neat_DString neat_tostr_ulong(unsigned long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%lu", *obj);
    return ret;
}

Neat_DString neat_tostr_llong(long long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%lld", *obj);
    return ret;
}

Neat_DString neat_tostr_ullong(unsigned long long *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%llu", *obj);
    return ret;
}

Neat_DString neat_tostr_float(float *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%g", *obj);
    return ret;
}

Neat_DString neat_tostr_double(double *obj)
{
    Neat_DString ret = neat_dstr_new(32, neat_get_default_allocator());
    ret.len = sprintf((char*) ret.chars, "%g", *obj);
    return ret;
}

Neat_DString neat_tostr_dstr(Neat_DString *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append(&ret, obj);
    return ret;
}

Neat_DString neat_tostr_dstr_ptr(Neat_DString **obj)
{
    return neat_tostr_dstr(*obj);
}

Neat_DString neat_tostr_strv(Neat_String_View *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append_strv(&ret, *obj);
    return ret;
}

Neat_DString neat_tostr_strv_ptr(Neat_String_View **obj)
{
    return neat_tostr_strv(*obj);
}

Neat_DString neat_tostr_strbuf(Neat_String_Buffer *obj)
{
    Neat_DString ret = neat_dstr_new(obj->len, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

Neat_DString neat_tostr_strbuf_ptr(Neat_String_Buffer **obj)
{
    return neat_tostr_strbuf(*obj);
}

Neat_DString neat_tostr_sstr_ref(Neat_SString_Ref *obj)
{
    Neat_DString ret = neat_dstr_new(obj->sstring->len, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

Neat_DString neat_tostr_anystr_ref(Neat_Any_String_Ref *obj)
{
    Neat_DString ret = neat_dstr_new(16, neat_get_default_allocator());
    neat_dstr_append(&ret, *obj);
    return ret;
}

void neat_tostr_into_bool(Neat_Any_String_Ref dst, bool *obj)
{
    neat_str_copy(dst, *obj ? (char*) "true" : (char*) "false");
}

void neat_tostr_into_str(Neat_Any_String_Ref dst, char **obj)
{
    neat_str_copy(dst, *obj);
}

void neat_tostr_into_ustr(Neat_Any_String_Ref dst, unsigned char **obj)
{
    neat_str_copy(dst, *obj);
}

void neat_tostr_into_char(Neat_Any_String_Ref dst, char *obj)
{
    neat_str_copy(dst, ((char[]){*obj, '\0'}));
}

void neat_tostr_into_schar(Neat_Any_String_Ref dst, signed char *obj)
{
    char tmp[5] = { 0 };
    snprintf(tmp, sizeof(tmp), "%hhd", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_uchar(Neat_Any_String_Ref dst, unsigned char *obj)
{
    neat_str_copy(dst, ((char[]){(char) *obj, '\0'}));
}

void neat_tostr_into_short(Neat_Any_String_Ref dst, short *obj)
{
    char tmp[8] = { 0 };
    snprintf(tmp, sizeof(tmp), "%hd", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_ushort(Neat_Any_String_Ref dst, unsigned short *obj)
{
    char tmp[8] = { 0 };
    snprintf(tmp, sizeof(tmp), "%hu", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_int(Neat_Any_String_Ref dst, int *obj)
{
    char tmp[16] = { 0 };
    snprintf(tmp, sizeof(tmp), "%d", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_uint(Neat_Any_String_Ref dst, unsigned int *obj)
{
    char tmp[16] = { 0 };
    snprintf(tmp, sizeof(tmp), "%u", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_long(Neat_Any_String_Ref dst, long *obj)
{
    char tmp[32] = { 0 };
    snprintf(tmp, sizeof(tmp), "%ld", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_ulong(Neat_Any_String_Ref dst, unsigned long *obj)
{
    char tmp[32] = { 0 };
    snprintf(tmp, sizeof(tmp), "%lu", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_llong(Neat_Any_String_Ref dst, long long *obj)
{
    char tmp[32] = { 0 };
    snprintf(tmp, sizeof(tmp), "%lld", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_ullong(Neat_Any_String_Ref dst, unsigned long long *obj)
{
    char tmp[32] = { 0 };
    snprintf(tmp, sizeof(tmp), "%llu", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_float(Neat_Any_String_Ref dst, float *obj)
{
    char tmp[16] = { 0 };
    snprintf(tmp, sizeof(tmp), "%g", *obj);
    
    neat_str_copy(dst, tmp);
}

void neat_tostr_into_double(Neat_Any_String_Ref dst, double *obj)
{
    char tmp[32] = { 0 };
    snprintf(tmp, sizeof(tmp), "%g", *obj);
    
    neat_str_copy(dst, tmp);
}


void neat_tostr_into_dstr(Neat_Any_String_Ref dst, Neat_DString *obj)
{
    neat_str_copy(dst, obj);
}

void neat_tostr_into_dstr_ptr(Neat_Any_String_Ref dst, Neat_DString **obj)
{
    neat_str_copy(dst, *obj);
}

void neat_tostr_into_strv(Neat_Any_String_Ref dst, Neat_String_View *obj)
{
    neat_str_copy(dst, *obj);
}

void neat_tostr_into_strv_ptr(Neat_Any_String_Ref dst, Neat_String_View **obj)
{
    neat_str_copy(dst, *obj);
}

void neat_tostr_into_strbuf(Neat_Any_String_Ref dst, Neat_String_Buffer *obj)
{
    neat_str_copy(dst, *obj);
}

void neat_tostr_into_strbuf_ptr(Neat_Any_String_Ref dst, Neat_String_Buffer **obj)
{
    neat_str_copy(dst, *obj);
}

void neat_tostr_into_sstr_ref(Neat_Any_String_Ref dst, Neat_SString_Ref *obj)
{
    neat_str_copy(dst, *obj);
}

void neat_tostr_into_anystr_ref(Neat_Any_String_Ref dst, Neat_Any_String_Ref *obj)
{
    neat_str_copy(dst, *obj);
}
