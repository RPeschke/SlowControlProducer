#include "StringUtility.hh"

using namespace deco;

std::string StringUtility::trim(const std::string &s) {
	bool x = true;
	int a = 0;
	while (x && a < s.size()) {
		if (s[a] != ' ' && s[a] != '\t') {
			x = false;
		} else {
			a++;
		}
	}

	x = true;
	int b = s.size() - 1;
	while (x && b >= 0) {
		if (s[b] != ' ' && s[b] != '\t') {
			x = false;
		} else {
			b--;
		}
	}

	std::string out = s.substr(a);
	out = out.substr(0, b + 1);

	return out;
}

std::pair<std::string, std::string> StringUtility::splitByFirstOccurrence(const std::string &s, const char c) {
	std::string first("");
	std::string second("");

	bool x = true;
	for (int i = 0; i < s.size(); i++) {
		if (x) {
			if (s[i] != c) {
				first += s[i];
			} else {
				x = false;
			}
		} else {
			second += s[i];
		}
	}

	return std::pair<std::string, std::string>(first, second);
}

std::string StringUtility::toLowerCase(const std::string &s) {
	std::string out("");
	int diff = 'A' - 'a';
	for (int i = 0; i < s.size(); i++) {
		if (s[i] >= 'A' && s[i] <= 'Z') {
			out += s[i] - diff;
		} else {
			out += s[i];
		}
	}

	return out;
}
