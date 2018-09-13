#include<stdlib.h>
#include<string.h>
#include<algorithm>
#include<cstdint>
#include<iostream>
#include<iomanip>
#include<string>
#include<sstream>

extern "C" {

struct blockheader {
	int64_t len;
};

struct string {
	blockheader b;
	char data[0];
};

bool hook_STRINGS_gt(const string * a, const string * b) {
    for (unsigned i = 0; i < a->b.len; ++i) {
        if (a->data[i] < b->data[i]) return false;
        if (a->data[i] > b->data[i]) return true;
        if (i >= b->b.len)  return true;
    }
    return false;
}

bool hook_STRINGS_gte(const string * a, const string * b) {
    for (unsigned i = 0; i < a->b.len; ++i) {
        if (a->data[i] < b->data[i]) return false;
        if (a->data[i] > b->data[i]) return true;
        if (i >= b->b.len)  return true;
    }
    return a->b.len == b->b.len;
}

bool hook_STRINGS_lt(const string * a, const string * b) {
    for (unsigned i = 0; i < a->b.len; ++i) {
        if (a->data[i] > b->data[i]) return false;
        if (a->data[i] < b->data[i]) return true;
        if (i >= b->b.len)  return false;
    }
    return a->b.len < b->b.len;
}

bool hook_STRINGS_lte(const string * a, const string * b) {
    for (unsigned i = 0; i < a->b.len; ++i) {
        if (a->data[i] > b->data[i]) return false;
        if (a->data[i] < b->data[i]) return true;
        if (i >= b->b.len)  return false;
    }
    return a->b.len <= b->b.len;
}

bool hook_STRINGS_eq(const string * a, const string * b) {
    for (unsigned i = 0; i < a->b.len; ++i) {
        if (a->data[i] != b->data[i]) return false;
        if (i >= b->b.len)   return false;
    }
    return a->b.len == b->b.len;
}

bool hook_STRINGS_ne(const string * a, const string * b) {
    for (unsigned i = 0; i < a->b.len; ++i) {
        if (a->data[i] != b->data[i]) return true;
        if (i >= b->b.len)   return true;
    }
    return a->b.len != b->b.len;
}

string * hook_STRINGS_concat(const string * a, const string * b) {
	auto len_a = a->b.len;
	auto len_b = b->b.len;
	auto newlen = len_a  + len_b;
	auto ret = static_cast<string *>(malloc(sizeof(string) + newlen));
	ret->b.len = newlen;
	for (unsigned i = 0; i < a->b.len; ++i) ret->data[i] = a->data[i];
	for (unsigned i = 0; i < b->b.len; ++i) ret->data[i+len_a] = b->data[i];
	return ret;
}

int64_t hook_STRINGS_length(const string * a) {
	return a->b.len;
}

string * hook_STRINGS_chr(const int64_t ord) {
	auto ret = static_cast<string *>(malloc(sizeof(string) + sizeof(char)));
	ret->b.len = 1;
	ret->data[0] = static_cast<char>(ord);
	return ret;
}

int64_t hook_STRINGS_ord(const string * input) {
	if (input->b.len < 1) {
		return -1;
	}
    return static_cast<int64_t>(input->data[0]);
}

// -1 means take the entire rest of the string.
string * hook_STRINGS_substr(const string * input, const int64_t start, int64_t len) {
	if (len == -1) len = input->b.len;
	auto minLen = std::max(std::min(len, input->b.len - start), 0L);
	auto ret = static_cast<string *>(malloc(sizeof(string) + sizeof(char) * minLen));
	ret->b.len = minLen;
	for (unsigned i = 0; i < minLen; ++i) ret->data[i] = input->data[i + start];
	return ret;
}

int64_t hook_STRINGS_find(const string * haystack, const string * needle, int64_t pos) {
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
int64_t hook_STRINGS_rfind(const string * haystack, const string * needle, int64_t pos) {
	auto hs = std::string(haystack->data, haystack->b.len);
	auto ns = std::string(needle->data, needle->b.len);
	return hs.rfind(ns, pos);
}

int64_t hook_STRINGS_findChar(const string * haystack, const string * needle, int64_t pos) {
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
int64_t hook_STRINGS_rfindChar(const string * haystack, const string * needle, int64_t pos) {
	if (needle->b.len > 1) return -1;
	auto hs = std::string(haystack->data, haystack->b.len);
	auto ns = std::string(needle->data, needle->b.len);
	return hs.rfind(ns, pos);
}

string * makeString(const char * input) {
	auto len = strlen(input);
	auto ret = static_cast<string *>(malloc(sizeof(string) + len));
	for (unsigned i = 0; i < len; ++i) {
		ret->data[i] = input[i];
	}
	ret->b.len = len;
	return ret;
}

string * hook_STRINGS_float2string(const double input) {
	std::stringstream out;
	out << std::setprecision(14) << input;
	return makeString(out.str().c_str());
}

double hook_STRINGS_string2float(const string * input) {
	auto si = std::string(input->data, input->b.len);
	return std::stod(si);
}

// uses std::string for memory allocation, double copy here is not the best, but at least the
// first copy is released at the end of the function.
string * hook_STRINGS_replaceAll(const string * input, const string * matcher, string * replacer) {
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
string * hook_STRINGS_replace(const string * input, const string * matcher, string * replacer, int64_t occurences) {
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
string * hook_STRINGS_replaceFirst(const string * input, const string * matcher, string * replacer) {
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

int64_t hook_STRINGS_countAllOccurences(const string * input, const string * matcher) {
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

#ifdef TEST_STRINGS
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

	/*
	 * concat tests
	 *
	 */
	auto emptyCatR = hook_STRINGS_concat(a, makeString(""));
	assert(strcmp(emptyCatR->data, a->data) == 0);
	assert(emptyCatR->b.len == a->b.len);

	auto emptyCatL = hook_STRINGS_concat(makeString(""), a);
	assert(strcmp(emptyCatL->data, a->data) == 0);
	assert(emptyCatL->b.len == a->b.len);

	auto catAll = hook_STRINGS_concat(hook_STRINGS_concat(a,b), c);
	auto expected = makeString("hellohehf");
	assert(strcmp(catAll->data, expected->data) == 0);
	assert(catAll->b.len == expected->b.len);

	/*
	 * chr
	 * Tests
	 *
	 */
	string * A = hook_STRINGS_chr(65);
	string * space = hook_STRINGS_chr(32);
	assert(A->data[0] == 'A');
	assert(A->b.len == 1);
	assert(space->data[0] == ' ');
	assert(space->b.len == 1);

	/*
	 * ord
	 * Tests
	 *
	 */
	assert(hook_STRINGS_ord(A) == 65);
	assert(hook_STRINGS_ord(space) == 32);
	assert(hook_STRINGS_ord(makeString("")) == -1);
	// is this what we want?? This is what the ocaml does, anyway.
	// Actually, seems like the ocaml will crash on 0 length strings.
	assert(hook_STRINGS_ord(makeString("AA")) == 65);

	/*
	 * substr
	 * Tests
	 *
	 */
	assert(strcmp(hook_STRINGS_substr(catAll, 2, -1)->data, "llohehf") == 0);
	assert(strcmp(hook_STRINGS_substr(catAll, 2, 4)->data, "lloh") == 0);
	assert(strcmp(hook_STRINGS_substr(catAll, 0, 4)->data, "hell") == 0);
	assert(strcmp(hook_STRINGS_substr(catAll, 6, 4)->data, "ehf") == 0);
	assert(strcmp(hook_STRINGS_substr(catAll, 7, 40)->data, "hf") == 0);
	assert(strcmp(hook_STRINGS_substr(catAll, 8, 40)->data, "f") == 0);
	assert(strcmp(hook_STRINGS_substr(catAll, 8, -1)->data, "f") == 0);
	assert(strcmp(hook_STRINGS_substr(catAll, 10, 40)->data, "") == 0);
	assert(strcmp(hook_STRINGS_substr(catAll, 10124, 4024)->data, "") == 0);

	/*
	 * find
	 * Tests
	 *
	 */
	auto haystack = makeString("hellollo");
	auto needle = makeString("llo");
	assert(hook_STRINGS_find(haystack, needle, 0) == 2);
	assert(hook_STRINGS_find(haystack, needle, 1) == 2);
	assert(hook_STRINGS_find(haystack, needle, 2) == 2);
	assert(hook_STRINGS_find(haystack, needle, 3) == 5);
	assert(hook_STRINGS_find(haystack, needle, 4) == 5);
	assert(hook_STRINGS_find(haystack, needle, 5) == 5);
	assert(hook_STRINGS_find(haystack, needle, 6) == -1);
	assert(hook_STRINGS_find(haystack, c, 3) == -1);

	/*
	 * rfind
	 * Tests
	 *
	 */
	assert(hook_STRINGS_rfind(haystack, needle, 0) == -1);
	assert(hook_STRINGS_rfind(haystack, needle, 1) == -1);
	assert(hook_STRINGS_rfind(haystack, needle, 2) == 2);
	assert(hook_STRINGS_rfind(haystack, needle, 3) == 2);
	assert(hook_STRINGS_rfind(haystack, needle, 4) == 2);
	assert(hook_STRINGS_rfind(haystack, needle, 5) == 5);
	assert(hook_STRINGS_rfind(haystack, needle, 6) == 5);
	assert(hook_STRINGS_rfind(haystack, c, 6) == -1);

	/*
	 * float2string
	 * Test
	 */
	assert(hook_STRINGS_eq(hook_STRINGS_float2string(3.1415926535898), makeString("3.1415926535898")));

	/*
	 * string2float
	 * Test
	 */
	assert(hook_STRINGS_string2float(makeString("3.14159265")) == 3.14159265);

	/*
	 * replace
	 * Tests
	 */
	auto replacee = makeString("hello world hello world hello world he worl");
	auto matcher = makeString("hello");
	auto replacer = makeString("goodbye");
	assert(hook_STRINGS_eq(hook_STRINGS_replaceAll(replacee, matcher, replacer),
				makeString("goodbye world goodbye world goodbye world he worl")));

	assert(hook_STRINGS_eq(hook_STRINGS_replace(replacee, matcher, replacer, 0),
				makeString("hello world hello world hello world he worl")));
	assert(hook_STRINGS_eq(hook_STRINGS_replace(replacee, matcher, replacer, 1),
				makeString("goodbye world hello world hello world he worl")));
	assert(hook_STRINGS_eq(hook_STRINGS_replace(replacee, matcher, replacer, 2),
				makeString("goodbye world goodbye world hello world he worl")));
	assert(hook_STRINGS_eq(hook_STRINGS_replace(replacee, matcher, replacer, 3),
				makeString("goodbye world goodbye world goodbye world he worl")));
	assert(hook_STRINGS_eq(hook_STRINGS_replace(replacee, matcher, replacer, 4),
				makeString("goodbye world goodbye world goodbye world he worl")));

	assert(hook_STRINGS_eq(hook_STRINGS_replaceFirst(replacee, matcher, replacer),
				makeString("goodbye world hello world hello world he worl")));

	/*
	 * countAllOccurences
	 * Test
	 */
	assert(hook_STRINGS_countAllOccurences(replacee, matcher) == 3);

    std::cout << "all tests passed" << std::endl;
    return 0;
}
#endif
