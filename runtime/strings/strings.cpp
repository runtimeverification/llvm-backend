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
	if (pos >= haystack->b.len) {
		return -1;
	}
	auto out = std::search(haystack->data + pos * sizeof(KCHAR), haystack->data + haystack->b.len * sizeof(KCHAR),
			               needle->data,   needle->data   + needle->b.len * sizeof(KCHAR));
	int64_t ret = (out - haystack->data) / sizeof(KCHAR);
	// search returns the end of the range if it is not found, but we want -1 in such a case.
	return (ret < haystack->b.len)?ret:-1;
}

int64_t hook_STRING_rfind(const string * haystack, const string * needle, int64_t pos) {
	// The semantics of rfind position are strange, it is the last position at which
	// the match can _start_, which means the end of the haystack needs to be pos + len(needle),
	// or the end of the haystack, if that's less.
	pos += needle->b.len;
	if (pos <= needle->b.len) {
		return -1;
	}
	auto end = (pos < haystack->b.len)?pos:haystack->b.len;
	auto out = std::find_end(&haystack->data[0], &haystack->data[end],
                  &needle->data[0], &needle->data[needle->b.len]);
	auto ret = &*out - &haystack->data[0];
	return (ret < end)?ret:-1;
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
