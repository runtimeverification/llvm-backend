#include<stdlib.h>
#include<string.h>
#include<algorithm>
#include<cstdint>
#include<iostream>
#include<iomanip>
#include<string>
#include<sstream>

extern "C" {

#define KCHAR char

struct blockheader {
	int64_t len;
};

struct string {
	blockheader b;
	KCHAR data[0];
};

bool hook_STRING_gt(const string * a, const string * b) {
	auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
	return res > 0 || (res == 0 && a->b.len > b->b.len);
}

bool hook_STRING_gte(const string * a, const string * b) {
	auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
	return (res > 0 || (res == 0 && a->b.len >= b->b.len));
}

bool hook_STRING_lt(const string * a, const string * b) {
	auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
	return res < 0 || (res == 0 && a->b.len < b->b.len);
}

bool hook_STRING_lte(const string * a, const string * b) {
	auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
	return (res < 0 || (res == 0 && a->b.len <= b->b.len));
}

bool hook_STRING_eq(const string * a, const string * b) {
	auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
	return (res == 0 && a->b.len == b->b.len);
}

bool hook_STRING_ne(const string * a, const string * b) {
	auto res = memcmp(a->data, b->data, std::min(a->b.len, b->b.len));
	return (res != 0 || a->b.len != b->b.len);
}

string * hook_STRING_concat(const string * a, const string * b) {
	auto len_a = a->b.len;
	auto len_b = b->b.len;
	auto newlen = len_a  + len_b;
	auto ret = static_cast<string *>(malloc(sizeof(string) + newlen));
	ret->b.len = newlen;
	memcpy(&(ret->data), &(a->data), a->b.len * sizeof(KCHAR));
	memcpy(&(ret->data[a->b.len]), &(b->data), b->b.len * sizeof(KCHAR));
	return ret;
}

int64_t hook_STRING_length(const string * a) {
	return a->b.len;
}

string * hook_STRING_chr(const int64_t ord) {
	auto ret = static_cast<string *>(malloc(sizeof(string) + sizeof(KCHAR)));
	ret->b.len = 1;
	ret->data[0] = static_cast<KCHAR>(ord);
	return ret;
}

int64_t hook_STRING_ord(const string * input) {
	if (input->b.len < 1) {
		return -1;
	}
    return static_cast<int64_t>(input->data[0]);
}

// -1 means take the entire rest of the string.
string * hook_STRING_substr(const string * input, const int64_t start, int64_t len) {
	if (len == -1) len = input->b.len;
	auto minLen = std::max(std::min(len, input->b.len - start), 0L);
	auto ret = static_cast<string *>(malloc(sizeof(string) + sizeof(KCHAR) * minLen));
	ret->b.len = minLen;
	memcpy(&(ret->data), &(input->data[start]), minLen * sizeof(KCHAR));
	return ret;
}

int64_t hook_STRING_find(const string * haystack, const string * needle, int64_t pos) {
	pos = std::max(pos, 0L);
	for (auto i = pos; i < haystack->b.len; ++i) {
		if (haystack->data[i] == needle->data[0]) {
			auto found = false;
			for (auto j = 0; j < needle->b.len; ++j) {
				if (i+j >= haystack->b.len) {
					found = false;
					break;
				}
				if  (haystack->data[i+j] != needle->data[j]) {
					found = false;
					break;
				}
				found = true;
			}
			if (found) return i;
		}
	}
	return -1L;
}

// rfind will be used much less than find; it is ok to be a little less efficient and create std::strings.
int64_t hook_STRING_rfind(const string * haystack, const string * needle, int64_t pos) {
	auto hs = std::string(haystack->data, haystack->b.len);
	auto ns = std::string(needle->data, needle->b.len);
	return hs.rfind(ns, pos);
}

int64_t hook_STRING_findChar(const string * haystack, const string * needle, int64_t pos) {
	if (needle->b.len > 1) return -1;
	pos = std::max(pos, 0L);
	for (auto i = pos; i < haystack->b.len; ++i) {
		if (haystack->data[i] == needle->data[0]) {
			auto found = false;
			for (auto j = 0; j < needle->b.len; ++j) {
				if (i+j >= haystack->b.len) {
					found = false;
					break;
				}
				if  (haystack->data[i+j] != needle->data[j]) {
					found = false;
					break;
				}
				found = true;
			}
			if (found) return i;
		}
	}
	return -1L;
}

// rfind will be used much less than find; it is ok to be a little less efficient and create std::strings.
int64_t hook_STRING_rfindChar(const string * haystack, const string * needle, int64_t pos) {
	if (needle->b.len > 1) return -1;
	auto hs = std::string(haystack->data, haystack->b.len);
	auto ns = std::string(needle->data, needle->b.len);
	return hs.rfind(ns, pos);
}

string * makeString(const KCHAR * input) {
	auto len = strlen(input);
	auto ret = static_cast<string *>(malloc(sizeof(string) + len));
	for (unsigned i = 0; i < len; ++i) {
		ret->data[i] = input[i];
	}
	ret->b.len = len;
	return ret;
}

string * hook_STRING_float2string(const double input) {
	std::stringstream out;
	out << std::setprecision(14) << input;
	return makeString(out.str().c_str());
}

double hook_STRING_string2float(const string * input) {
	auto si = std::string(input->data, input->b.len);
	return std::stod(si);
}

// uses std::string for memory allocation, double copy here is not the best, but at least the
// first copy is released at the end of the function.
string * hook_STRING_replaceAll(const string * input, const string * matcher, const string * replacer) {
	std::string ret;
	auto rs = std::string(replacer->data, replacer->b.len);
	for (auto i = 0; i < input->b.len;) {
		if (input->data[i] == matcher->data[0]) {
			auto found = false;
			for (auto j = 0; j < matcher->b.len; ++j) {
				if (i+j >= input->b.len) {
					found = false;
					break;
				}
				if  (input->data[i+j] != matcher->data[j]) {
					found = false;
					break;
				}
				found = true;
			}
			if (found) {
				ret += rs;
				i += matcher->b.len;
			} else {
				ret.push_back(input->data[i]);
				++i;
			}
		} else {
			ret.push_back(input->data[i]);
			++i;
		}
	}
	return makeString(ret.c_str());
}

// uses std::string for memory allocation, double copy here is not the best, but at least the
// first copy is released at the end of the function.
string * hook_STRING_replace(const string * input, const string * matcher, const string * replacer, int64_t occurences) {
	std::string ret;
	auto rs = std::string(replacer->data, replacer->b.len);
	for (auto i = 0; i < input->b.len;) {
		if (occurences == 0) {
			ret.push_back(input->data[i]);
			++i;
			continue;
		}
		if (input->data[i] == matcher->data[0]) {
			auto found = false;
			for (auto j = 0; j < matcher->b.len; ++j) {
				if (i+j >= input->b.len) {
					found = false;
					break;
				}
				if  (input->data[i+j] != matcher->data[j]) {
					found = false;
					break;
				}
				found = true;
			}
			if (found) {
				ret += rs;
				i += matcher->b.len;
				--occurences;
			} else {
				ret.push_back(input->data[i]);
				++i;
			}
		} else {
			ret.push_back(input->data[i]);
			++i;
		}
	}
	return makeString(ret.c_str());
}

// uses std::string for memory allocation, double copy here is not the best, but at least the
// first copy is released at the end of the function.
string * hook_STRING_replaceFirst(const string * input, const string * matcher, const string * replacer) {
	std::string ret;
	auto rs = std::string(replacer->data, replacer->b.len);
	auto replaced = false;
	for (auto i = 0; i < input->b.len;) {
		if (replaced) {
			ret.push_back(input->data[i]);
			++i;
			continue;
		}
		if (input->data[i] == matcher->data[0]) {
			auto found = false;
			for (auto j = 0; j < matcher->b.len; ++j) {
				if (i+j >= input->b.len) {
					found = false;
					break;
				}
				if  (input->data[i+j] != matcher->data[j]) {
					found = false;
					break;
				}
				found = true;
			}
			if (found) {
				ret += rs;
				i += matcher->b.len;
				replaced = true;
			} else {
				ret.push_back(input->data[i]);
				++i;
			}
		} else {
			ret.push_back(input->data[i]);
			++i;
		}
	}
	return makeString(ret.c_str());
}

int64_t hook_STRING_countAllOccurences(const string * input, const string * matcher) {
	int64_t ret = 0;
	for (auto i = 0; i < input->b.len; ++i) {
		if (input->data[i] == matcher->data[0]) {
			auto found = false;
			for (auto j = 0; j < matcher->b.len; ++j) {
				if (i+j >= input->b.len) {
					found = false;
					break;
				}
				if  (input->data[i+j] != matcher->data[j]) {
					found = false;
					break;
				}
				found = true;
			}
			if (found) {
				++ret;
			}
		}
	}
	return ret;
}

}

#ifdef TEST_STRING
#include<assert.h>

int main() {
	auto a = makeString("hello");
	auto b = makeString("he");
	auto c = makeString("hf");

	/*
	 * Comparison
	 * Operators
	 * Test
	 *
	 */
    assert(hook_STRING_gt(a,b));
    assert(!hook_STRING_gt(b,a));
    assert(hook_STRING_gt(c,a));
    assert(hook_STRING_gt(c,b));
    assert(!hook_STRING_gt(b,c));
    assert(!hook_STRING_gt(a,a));
    assert(!hook_STRING_gt(b,b));
    assert(!hook_STRING_gt(c,c));

    assert(hook_STRING_gte(a,b));
    assert(!hook_STRING_gte(b,a));
    assert(hook_STRING_gte(c,a));
    assert(hook_STRING_gte(c,b));
    assert(!hook_STRING_gte(b,c));
    assert(hook_STRING_gte(a,a));
    assert(hook_STRING_gte(b,b));
    assert(hook_STRING_gte(c,c));

    assert(!hook_STRING_lt(a,b));
    assert(hook_STRING_lt(b,a));
    assert(!hook_STRING_lt(c,a));
    assert(!hook_STRING_lt(c,b));
    assert(hook_STRING_lt(b,c));
    assert(!hook_STRING_lt(a,a));
    assert(!hook_STRING_lt(b,b));
    assert(!hook_STRING_lt(c,c));

    assert(!hook_STRING_lte(a,b));
    assert(hook_STRING_lte(b,a));
    assert(!hook_STRING_lte(c,a));
    assert(!hook_STRING_lte(c,b));
    assert(hook_STRING_lte(b,c));
    assert(hook_STRING_lte(a,a));
    assert(hook_STRING_lte(b,b));
    assert(hook_STRING_lte(c,c));

    assert(!hook_STRING_eq(a,b));
    assert(!hook_STRING_eq(b,a));
    assert(!hook_STRING_eq(c,a));
    assert(!hook_STRING_eq(c,b));
    assert(!hook_STRING_eq(b,c));
    assert(hook_STRING_eq(a,a));
    assert(hook_STRING_eq(b,b));
    assert(hook_STRING_eq(c,c));

    assert(hook_STRING_ne(a,b));
    assert(hook_STRING_ne(b,a));
    assert(hook_STRING_ne(c,a));
    assert(hook_STRING_ne(c,b));
    assert(hook_STRING_ne(b,c));
    assert(!hook_STRING_ne(a,a));
    assert(!hook_STRING_ne(b,b));
    assert(!hook_STRING_ne(c,c));

	/*
	 * concat tests
	 *
	 */
	auto emptyCatR = hook_STRING_concat(a, makeString(""));
	assert(strcmp(emptyCatR->data, a->data) == 0);
	assert(emptyCatR->b.len == a->b.len);

	auto emptyCatL = hook_STRING_concat(makeString(""), a);
	assert(strcmp(emptyCatL->data, a->data) == 0);
	assert(emptyCatL->b.len == a->b.len);

	auto catAll = hook_STRING_concat(hook_STRING_concat(a,b), c);
	auto expected = makeString("hellohehf");
	assert(strcmp(catAll->data, expected->data) == 0);
	assert(catAll->b.len == expected->b.len);

	/*
	 * chr
	 * Tests
	 *
	 */
	string * A = hook_STRING_chr(65);
	string * space = hook_STRING_chr(32);
	assert(A->data[0] == 'A');
	assert(A->b.len == 1);
	assert(space->data[0] == ' ');
	assert(space->b.len == 1);

	/*
	 * ord
	 * Tests
	 *
	 */
	assert(hook_STRING_ord(A) == 65);
	assert(hook_STRING_ord(space) == 32);
	assert(hook_STRING_ord(makeString("")) == -1);
	// is this what we want?? This is what the ocaml does, anyway.
	// Actually, seems like the ocaml will crash on 0 length strings.
	assert(hook_STRING_ord(makeString("AA")) == 65);

	/*
	 * substr
	 * Tests
	 *
	 */
	assert(strcmp(hook_STRING_substr(catAll, 2, -1)->data, "llohehf") == 0);
	assert(strcmp(hook_STRING_substr(catAll, 2, 4)->data, "lloh") == 0);
	assert(strcmp(hook_STRING_substr(catAll, 0, 4)->data, "hell") == 0);
	assert(strcmp(hook_STRING_substr(catAll, 6, 4)->data, "ehf") == 0);
	assert(strcmp(hook_STRING_substr(catAll, 7, 40)->data, "hf") == 0);
	assert(strcmp(hook_STRING_substr(catAll, 8, 40)->data, "f") == 0);
	assert(strcmp(hook_STRING_substr(catAll, 8, -1)->data, "f") == 0);
	assert(strcmp(hook_STRING_substr(catAll, 10, 40)->data, "") == 0);
	assert(strcmp(hook_STRING_substr(catAll, 10124, 4024)->data, "") == 0);

	/*
	 * find
	 * Tests
	 *
	 */
	auto haystack = makeString("hellollo");
	auto needle = makeString("llo");
	assert(hook_STRING_find(haystack, needle, 0) == 2);
	assert(hook_STRING_find(haystack, needle, 1) == 2);
	assert(hook_STRING_find(haystack, needle, 2) == 2);
	assert(hook_STRING_find(haystack, needle, 3) == 5);
	assert(hook_STRING_find(haystack, needle, 4) == 5);
	assert(hook_STRING_find(haystack, needle, 5) == 5);
	assert(hook_STRING_find(haystack, needle, 6) == -1);
	assert(hook_STRING_find(haystack, c, 3) == -1);

	/*
	 * rfind
	 * Tests
	 *
	 */
	assert(hook_STRING_rfind(haystack, needle, 0) == -1);
	assert(hook_STRING_rfind(haystack, needle, 1) == -1);
	assert(hook_STRING_rfind(haystack, needle, 2) == 2);
	assert(hook_STRING_rfind(haystack, needle, 3) == 2);
	assert(hook_STRING_rfind(haystack, needle, 4) == 2);
	assert(hook_STRING_rfind(haystack, needle, 5) == 5);
	assert(hook_STRING_rfind(haystack, needle, 6) == 5);
	assert(hook_STRING_rfind(haystack, c, 6) == -1);

	/*
	 * float2string
	 * Test
	 */
	assert(hook_STRING_eq(hook_STRING_float2string(3.1415926535898), makeString("3.1415926535898")));

	/*
	 * string2float
	 * Test
	 */
	assert(hook_STRING_string2float(makeString("3.14159265")) == 3.14159265);

	/*
	 * replace
	 * Tests
	 */
	auto replacee = makeString("hello world hello world hello world he worl");
	auto matcher = makeString("hello");
	auto replacer = makeString("goodbye");
	assert(hook_STRING_eq(hook_STRING_replaceAll(replacee, matcher, replacer),
				makeString("goodbye world goodbye world goodbye world he worl")));

	assert(hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 0),
				makeString("hello world hello world hello world he worl")));
	assert(hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 1),
				makeString("goodbye world hello world hello world he worl")));
	assert(hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 2),
				makeString("goodbye world goodbye world hello world he worl")));
	assert(hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 3),
				makeString("goodbye world goodbye world goodbye world he worl")));
	assert(hook_STRING_eq(hook_STRING_replace(replacee, matcher, replacer, 4),
				makeString("goodbye world goodbye world goodbye world he worl")));

	assert(hook_STRING_eq(hook_STRING_replaceFirst(replacee, matcher, replacer),
				makeString("goodbye world hello world hello world he worl")));

	/*
	 * countAllOccurences
	 * Test
	 */
	assert(hook_STRING_countAllOccurences(replacee, matcher) == 3);

    std::cout << "all tests passed" << std::endl;
    return 0;
}
#endif
