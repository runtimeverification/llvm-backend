#include<assert.h>
#include<stdlib.h>
#include<iostream>

extern "C" {

// Assume assume length is passed before char *

inline int get_len(const char * a) {
    const int * head = reinterpret_cast<const int *>(a);
    return *(head - 1);
}

bool hook_STRINGS_gt(const char * a, const char * b) {
    int len_a = get_len(a);
    int len_b = get_len(b);
    for (int i = 0; i < len_a; ++i) {
        if (a[i] < b[i]) return false;
        if (a[i] > b[i]) return true;
        if (i >= len_b)  return true;
    }
    return false;
}

bool hook_STRINGS_gte(const char * a, const char * b) {
    int len_a = get_len(a);
    int len_b = get_len(b);
    for (int i = 0; i < len_a; ++i) {
        if (a[i] < b[i]) return false;
        if (a[i] > b[i]) return true;
        if (i >= len_b)  return true;
    }
    return len_a == len_b;
}

bool hook_STRINGS_lt(const char * a, const char * b) {
    int len_a = get_len(a);
    int len_b = get_len(b);
    for (int i = 0; i < len_a; ++i) {
        if (a[i] > b[i]) return false;
        if (a[i] < b[i]) return true;
        if (i >= len_b)  return false;
    }
    return len_a < len_b;
}

bool hook_STRINGS_lte(const char * a, const char * b) {
    int len_a = get_len(a);
    int len_b = get_len(b);
    for (int i = 0; i < len_a; ++i) {
        if (a[i] > b[i]) return false;
        if (a[i] < b[i]) return true;
        if (i >= len_b)  return false;
    }
    return len_a <= len_b;
}

bool hook_STRINGS_eq(const char * a, const char * b) {
    int len_a = get_len(a);
    int len_b = get_len(b);
    for (int i = 0; i < len_a; ++i) {
        if (a[i] != b[i]) return false;
        if (i >= len_b)   return false;
    }
    return len_a == len_b;
}

bool hook_STRINGS_ne(const char * a, const char * b) {
    int len_a = get_len(a);
    int len_b = get_len(b);
    for (int i = 0; i < len_a; ++i) {
        if (a[i] != b[i]) return true;
        if (i >= len_b)   return true;
    }
    return len_a != len_b;
}

}

int main() {
    // a = "hello"
    // b = "he"
    // c = "hf"
    char * a_base = static_cast<char *>(malloc(sizeof(char) * 5 + sizeof(int)));
    char * b_base = static_cast<char *>(malloc(sizeof(char) * 2 + sizeof(int)));
    char * c_base = static_cast<char *>(malloc(sizeof(char) * 2 + sizeof(int)));
    int * a_len = reinterpret_cast<int *>(a_base);
    int * b_len = reinterpret_cast<int *>(b_base);
    int * c_len = reinterpret_cast<int *>(c_base);
    *a_len = 5;
    *b_len = 2;
    *c_len = 2;
    a_base[4] = 'h';
    a_base[5] = 'e';
    a_base[6] = 'l';
    a_base[7] = 'l';
    a_base[8] = 'o';
    b_base[4] = 'h';
    b_base[5] = 'e';
    c_base[4] = 'h';
    c_base[5] = 'f';
    const char * a = &a_base[4];
    const char * b = &b_base[4];
    const char * c = &c_base[4];

    assert(hook_STRINGS_gt(a,b));
    assert(!hook_STRINGS_gt(b,a));
    assert(hook_STRINGS_gt(c,a));
    assert(hook_STRINGS_gt(c,b));
    assert(!hook_STRINGS_gt(b,c));
    assert(!hook_STRINGS_gt(a,a));
    assert(!hook_STRINGS_gt(b,b));
    assert(!hook_STRINGS_gt(c,c));

    assert(hook_STRINGS_gte(a,b));
    assert(!hook_STRINGS_gte(b,a));
    assert(hook_STRINGS_gte(c,a));
    assert(hook_STRINGS_gte(c,b));
    assert(!hook_STRINGS_gte(b,c));
    assert(hook_STRINGS_gte(a,a));
    assert(hook_STRINGS_gte(b,b));
    assert(hook_STRINGS_gte(c,c));

    assert(!hook_STRINGS_lt(a,b));
    assert(hook_STRINGS_lt(b,a));
    assert(!hook_STRINGS_lt(c,a));
    assert(!hook_STRINGS_lt(c,b));
    assert(hook_STRINGS_lt(b,c));
    assert(!hook_STRINGS_lt(a,a));
    assert(!hook_STRINGS_lt(b,b));
    assert(!hook_STRINGS_lt(c,c));

    assert(!hook_STRINGS_lte(a,b));
    assert(hook_STRINGS_lte(b,a));
    assert(!hook_STRINGS_lte(c,a));
    assert(!hook_STRINGS_lte(c,b));
    assert(hook_STRINGS_lte(b,c));
    assert(hook_STRINGS_lte(a,a));
    assert(hook_STRINGS_lte(b,b));
    assert(hook_STRINGS_lte(c,c));

    assert(!hook_STRINGS_eq(a,b));
    assert(!hook_STRINGS_eq(b,a));
    assert(!hook_STRINGS_eq(c,a));
    assert(!hook_STRINGS_eq(c,b));
    assert(!hook_STRINGS_eq(b,c));
    assert(hook_STRINGS_eq(a,a));
    assert(hook_STRINGS_eq(b,b));
    assert(hook_STRINGS_eq(c,c));

    assert(hook_STRINGS_ne(a,b));
    assert(hook_STRINGS_ne(b,a));
    assert(hook_STRINGS_ne(c,a));
    assert(hook_STRINGS_ne(c,b));
    assert(hook_STRINGS_ne(b,c));
    assert(!hook_STRINGS_ne(a,a));
    assert(!hook_STRINGS_ne(b,b));
    assert(!hook_STRINGS_ne(c,c));
	
    std::cout << "all tests passed" << std::endl;
    return 0;
}
