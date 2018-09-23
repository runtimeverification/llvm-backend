#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <string.h>
#include <iostream>

#define KCHAR char
extern "C" {
    struct blockheader {
        int64_t len;
    };

    struct string {
        blockheader b;
        KCHAR data[0];
    };

	bool hook_STRING_gt(const string *, const string *);
	bool hook_STRING_gte(const string *, const string *);
	bool hook_STRING_lt(const string *, const string *);
	bool hook_STRING_lte(const string *, const string *);
	bool hook_STRING_eq(const string *, const string *);
	bool hook_STRING_ne(const string *, const string *);
	string * hook_STRING_concat(const string *, const string *);
	int64_t hook_STRING_length(const string *);
	string * hook_STRING_chr(const int64_t);
	int64_t hook_STRING_ord(const string *);
	string * hook_STRING_substr(const string *, const int64_t, int64_t);
	int64_t hook_STRING_find(const string *, const string *, int64_t);
	int64_t hook_STRING_rfind(const string *, const string *, int64_t);
	int64_t hook_STRING_findChar(const string *, const string *, int64_t);
	int64_t hook_STRING_rfindChar(const string *, const string *, int64_t);
	string * makeString(const KCHAR *);
    string * hook_STRING_float2string(const double);
    double hook_STRING_string2float(const string *);
	string * hook_STRING_replaceAll(const string *, const string *, const string *);
	string * hook_STRING_replace(const string *, const string *, const string *, int64_t);
	string * hook_STRING_replaceFirst(const string *, const string *, const string *);
	int64_t hook_STRING_countAllOccurences(const string *, const string *);
	string * makeString(const KCHAR *);
}

BOOST_AUTO_TEST_SUITE(StringTest)

BOOST_AUTO_TEST_CASE(gt) {
    auto a = makeString("hello");
    auto b = makeString("he");
    auto c = makeString("hf");
	auto d = makeString("");

    BOOST_CHECK_EQUAL(false, hook_STRING_gt(a,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(a,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(a,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(a,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(b,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(b,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(b,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(b,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(c,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(c,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(c,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gt(c,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(d,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(d,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(d,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_gt(d,d));
}

BOOST_AUTO_TEST_CASE(gte) {
    auto a = makeString("hello");
    auto b = makeString("he");
    auto c = makeString("hf");
	auto d = makeString("");

    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(a,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(a,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gte(a,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(a,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_gte(b,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(b,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gte(b,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(b,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(c,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(c,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(c,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(c,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_gte(d,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_gte(d,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_gte(d,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_gte(d,d));
}

BOOST_AUTO_TEST_CASE(lt) {
    auto a = makeString("hello");
    auto b = makeString("he");
    auto c = makeString("hf");
	auto d = makeString("");

    BOOST_CHECK_EQUAL(false, hook_STRING_lt(a,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(a,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lt(a,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(a,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lt(b,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(b,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lt(b,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(b,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(c,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(c,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(c,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(c,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lt(d,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lt(d,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lt(d,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_lt(d,d));
}

BOOST_AUTO_TEST_CASE(lte) {
    auto a = makeString("hello");
    auto b = makeString("he");
    auto c = makeString("hf");
	auto d = makeString("");

    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(a,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_lte(a,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(a,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_lte(a,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(b,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(b,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(b,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_lte(b,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_lte(c,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_lte(c,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(c,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_lte(c,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(d,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(d,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(d,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_lte(d,d));
}

BOOST_AUTO_TEST_CASE(eq) {
    auto a = makeString("hello");
    auto b = makeString("he");
    auto c = makeString("hf");
	auto d = makeString("");

    BOOST_CHECK_EQUAL(true,  hook_STRING_eq(a,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(a,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(a,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(a,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(b,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_eq(b,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(b,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(b,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(c,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(c,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_eq(c,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(c,d));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(d,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(d,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_eq(d,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_eq(d,d));
}

BOOST_AUTO_TEST_CASE(ne) {
    auto a = makeString("hello");
    auto b = makeString("he");
    auto c = makeString("hf");
	auto d = makeString("");

    BOOST_CHECK_EQUAL(false, hook_STRING_ne(a,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(a,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(a,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(a,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(b,a));
    BOOST_CHECK_EQUAL(false, hook_STRING_ne(b,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(b,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(b,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(c,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(c,b));
    BOOST_CHECK_EQUAL(false, hook_STRING_ne(c,c));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(c,d));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(d,a));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(d,b));
    BOOST_CHECK_EQUAL(true,  hook_STRING_ne(d,c));
    BOOST_CHECK_EQUAL(false, hook_STRING_ne(d,d));
}

BOOST_AUTO_TEST_CASE(concat) {
    auto a = makeString("hello");
    auto b = makeString("he");
    auto c = makeString("hf");
	auto d = makeString("");

    auto emptyCatR = hook_STRING_concat(a, d);
    BOOST_CHECK_EQUAL(0, memcmp(emptyCatR->data, a->data, emptyCatR->b.len));
    BOOST_CHECK_EQUAL(emptyCatR->b.len, a->b.len);

    auto emptyCatL = hook_STRING_concat(d, a);
    BOOST_CHECK_EQUAL(0, memcmp(emptyCatL->data, a->data, emptyCatL->b.len));
    BOOST_CHECK_EQUAL(emptyCatL->b.len, a->b.len);

    auto catAll = hook_STRING_concat(hook_STRING_concat(a,b), c);
    auto expected = makeString("hellohehf");
    BOOST_CHECK_EQUAL(0, memcmp(catAll->data, expected->data, catAll->b.len));
    BOOST_CHECK_EQUAL(catAll->b.len, expected->b.len);
}

BOOST_AUTO_TEST_CASE(chr) {
    string * A = hook_STRING_chr(65);
    string * space = hook_STRING_chr(32);

    BOOST_CHECK_EQUAL(A->data[0], 'A');
    BOOST_CHECK_EQUAL(A->b.len, 1);
    BOOST_CHECK_EQUAL(space->data[0], ' ');
    BOOST_CHECK_EQUAL(space->b.len, 1);
}

BOOST_AUTO_TEST_CASE(ord) {
    string * A = hook_STRING_chr(65);
    string * space = hook_STRING_chr(32);

    BOOST_CHECK_EQUAL(hook_STRING_ord(A), 65);
    BOOST_CHECK_EQUAL(hook_STRING_ord(space), 32);
    BOOST_CHECK_EQUAL(hook_STRING_ord(makeString("")), -1);
    BOOST_CHECK_EQUAL(hook_STRING_ord(makeString("AA")), 65);
}

BOOST_AUTO_TEST_CASE(substr) {
    auto catAll = makeString("hellohehf");

    BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, 2, -1)->data, "llohehf", 7), 0);
    BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, 2, 4)->data, "lloh", 4), 0);
    BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, 0, 4)->data, "hell", 4), 0);
    BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, 6, 4)->data, "ehf", 3), 0);
    BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, 7, 40)->data, "hf", 2), 0);
    BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, 8, 40)->data, "f", 1), 0);
    BOOST_CHECK_EQUAL(memcmp(hook_STRING_substr(catAll, 8, -1)->data, "f", 1), 0);
    BOOST_CHECK_EQUAL(hook_STRING_substr(catAll, 10, 40)->b.len, 0);
    BOOST_CHECK_EQUAL(hook_STRING_substr(catAll, 10124, 4024)->b.len, 0);
}

BOOST_AUTO_TEST_CASE(find) {
    auto haystack = makeString("hellollo");
    auto needle = makeString("llo");
	auto c = makeString("hf");

    BOOST_CHECK_EQUAL(hook_STRING_find(haystack, needle, 0), 2);
    BOOST_CHECK_EQUAL(hook_STRING_find(haystack, needle, 1), 2);
    BOOST_CHECK_EQUAL(hook_STRING_find(haystack, needle, 2), 2);
    BOOST_CHECK_EQUAL(hook_STRING_find(haystack, needle, 3), 5);
    BOOST_CHECK_EQUAL(hook_STRING_find(haystack, needle, 4), 5);
    BOOST_CHECK_EQUAL(hook_STRING_find(haystack, needle, 5), 5);
    BOOST_CHECK_EQUAL(hook_STRING_find(haystack, needle, 6), -1);
    BOOST_CHECK_EQUAL(hook_STRING_find(haystack, c, 3), -1);
}

BOOST_AUTO_TEST_CASE(rfind) {
    auto haystack = makeString("hellollo");
    auto needle = makeString("llo");
	auto c = makeString("hf");

	BOOST_CHECK_EQUAL(hook_STRING_rfind(haystack, needle, 0), -1);
    BOOST_CHECK_EQUAL(hook_STRING_rfind(haystack, needle, 1), -1);
    BOOST_CHECK_EQUAL(hook_STRING_rfind(haystack, needle, 2), 2);
    BOOST_CHECK_EQUAL(hook_STRING_rfind(haystack, needle, 3), 2);
    BOOST_CHECK_EQUAL(hook_STRING_rfind(haystack, needle, 4), 2);
    BOOST_CHECK_EQUAL(hook_STRING_rfind(haystack, needle, 5), 5);
    BOOST_CHECK_EQUAL(hook_STRING_rfind(haystack, needle, 6), 5);
    BOOST_CHECK_EQUAL(hook_STRING_rfind(haystack, c, 6), -1);
}

BOOST_AUTO_TEST_CASE(float2string) {
	BOOST_CHECK_EQUAL(true, hook_STRING_eq(hook_STRING_float2string(3.1415926535898), makeString("3.1415926535898")));
}

BOOST_AUTO_TEST_CASE(string2float) {
	 BOOST_CHECK_EQUAL(hook_STRING_string2float(makeString("3.14159265")), 3.14159265);
}

BOOST_AUTO_TEST_CASE(replace) {
    auto replacee = makeString("hello world hello world hello world he worl");
    auto matcher = makeString("hello");
    auto replacer = makeString("goodbye");

	BOOST_CHECK_EQUAL(true,
			    hook_STRING_eq(hook_STRING_replaceAll(replacee, matcher, replacer),
                makeString("goodbye world goodbye world goodbye world he worl")));
    BOOST_CHECK_EQUAL(true,
			    hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 0),
                makeString("hello world hello world hello world he worl")));
    BOOST_CHECK_EQUAL(true,
			    hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 1),
                makeString("goodbye world hello world hello world he worl")));
    BOOST_CHECK_EQUAL(true,
			    hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 2),
                makeString("goodbye world goodbye world hello world he worl")));
    BOOST_CHECK_EQUAL(true,
			    hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 3),
                makeString("goodbye world goodbye world goodbye world he worl")));
    BOOST_CHECK_EQUAL(true,
			    hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 4),
                makeString("goodbye world goodbye world goodbye world he worl")));
    BOOST_CHECK_EQUAL(true,
			    hook_STRING_eq(hook_STRING_replaceFirst(replacee, matcher, replacer),
                makeString("goodbye world hello world hello world he worl")));
}

BOOST_AUTO_TEST_CASE(countAllOccurences) {
    auto replacee = makeString("hello world hello world hello world he worl");
    auto matcher = makeString("hello");

    BOOST_CHECK_EQUAL(hook_STRING_countAllOccurences(replacee, matcher), 3);
}

BOOST_AUTO_TEST_SUITE_END()
