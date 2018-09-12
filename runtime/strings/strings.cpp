#include<assert.h>
#include<stdlib.h>
#include<iostream>
#include<cstdint>

extern "C" {

struct blockheader {
	int64_t len;
};

struct string {
	blockheader b;
	char data[0];
};

bool hook_STRINGS_gt(const string * a, const string * b) {
    for (int i = 0; i < a->b.len; ++i) {
        if (a->data[i] < b->data[i]) return false;
        if (a->data[i] > b->data[i]) return true;
        if (i >= b->b.len)  return true;
    }
    return false;
}

bool hook_STRINGS_gte(const string * a, const string * b) {
    for (int i = 0; i < a->b.len; ++i) {
        if (a->data[i] < b->data[i]) return false;
        if (a->data[i] > b->data[i]) return true;
        if (i >= b->b.len)  return true;
    }
    return a->b.len == b->b.len;
}

bool hook_STRINGS_lt(const string * a, const string * b) {
    for (int i = 0; i < a->b.len; ++i) {
        if (a->data[i] > b->data[i]) return false;
        if (a->data[i] < b->data[i]) return true;
        if (i >= b->b.len)  return false;
    }
    return a->b.len < b->b.len;
}

bool hook_STRINGS_lte(const string * a, const string * b) {
    for (int i = 0; i < a->b.len; ++i) {
        if (a->data[i] > b->data[i]) return false;
        if (a->data[i] < b->data[i]) return true;
        if (i >= b->b.len)  return false;
    }
    return a->b.len <= b->b.len;
}

bool hook_STRINGS_eq(const string * a, const string * b) {
    for (int i = 0; i < a->b.len; ++i) {
        if (a->data[i] != b->data[i]) return false;
        if (i >= b->b.len)   return false;
    }
    return a->b.len == b->b.len;
}

bool hook_STRINGS_ne(const string * a, const string * b) {
    for (int i = 0; i < a->b.len; ++i) {
        if (a->data[i] != b->data[i]) return true;
        if (i >= b->b.len)   return true;
    }
    return a->b.len != b->b.len;
}

}

int main() {
    // a = "hello"
    // b = "he"
    // c = "hf"
    string * a = static_cast<string *>(malloc(sizeof(char) * 5 + sizeof(blockheader)));
    string * b = static_cast<string *>(malloc(sizeof(char) * 2 + sizeof(blockheader)));
    string * c = static_cast<string *>(malloc(sizeof(char) * 2 + sizeof(blockheader)));
    a->b.len = 5;
    b->b.len = 2;
    c->b.len = 2;
    a->data[0] = 'h';
    a->data[1] = 'e';
    a->data[2] = 'l';
    a->data[3] = 'l';
    a->data[4] = 'o';
    b->data[0] = 'h';
    b->data[1] = 'e';
    c->data[0] = 'h';
    c->data[1] = 'f';

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
