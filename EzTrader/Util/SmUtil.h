#pragma once
#include <tuple>
#include <time.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <stdexcept>
#include <cmath>

#define ROUNDING(x, dig)	( floor((x) * pow(float(10), dig) + 0.5f) / pow(float(10), dig) )

namespace DarkHorse {
	enum STR2INT_ERROR { SM_SUCCESS, SM_OVERFLOW, SM_UNDERFLOW, SM_INCONVERTIBLE };

#define MAX_BUFFER 128

	using namespace std;

	class SmUtil {
	public:
		static void insert_decimal(std::string& value, const int decimal);
		static std::vector<int> IntToDate(const int& date);
		static std::vector<int> IntToTime(const int& time);
		static std::tuple<int, int, int> GetLocalTime();
		static std::vector<int> GetLocalDate();
		static std::vector<int> GetDateTime(std::string datetime_string);
		static std::vector<int> GetTime(std::string time_string);
		static std::vector<int> GetLocalDateTime();
		static std::vector<int> GetUtcDateTime();
		static std::string GetUTCDateTimeString();
		static std::string GetUTCDateTimeStringForNowMin();
		static std::string GetUTCDateTimeStringForPreMin(int previousMinLen);
		static double GetDifTimeBySeconds(std::string newTime, std::string oldTime);
		static double GetDifTimeForNow(std::string srcTime);
		static std::string Format(const char* fmt, ...);
		static std::string GetMacAddress();

		static void to_thou_sep(std::string& val)
		{
			for (int point = 0, i = (val.find_last_of('.') == -1 ? val.length() : val.find_last_of('.')); i > 0; --i, ++point)
				if (point != 0 && point % 3 == 0)
					val.insert(i, ",");
		}

		static STR2INT_ERROR str2long(long& l, char const* s, int base = 0)
		{
			char* end = (char*)s;
			errno = 0;

			l = strtol(s, &end, base);

			if ((errno == ERANGE) && (l == LONG_MAX)) {
				return SM_OVERFLOW;
			}
			if ((errno == ERANGE) && (l == LONG_MIN)) {
				return SM_UNDERFLOW;
			}
			if ((errno != 0) || (s == end)) {
				return SM_INCONVERTIBLE;
			}
			while (isspace((unsigned char)*end)) {
				end++;
			}

			if (*s == '\0' || *end != '\0') {
				return SM_INCONVERTIBLE;
			}

			return SM_SUCCESS;
		};

		static string thousandSeparator(int n)
		{
			string ans = "";

			// Convert the given integer 
			// to equivalent string 
			string num = to_string(n);

			// Initialise count 
			int count = 0;

			// Traverse the string in reverse 
			for (int i = num.size() - 1;
				i >= 0; i--) {
				count++;
				ans.push_back(num[i]);

				// If three characters 
				// are traversed 
				if (count == 3) {
					ans.push_back(',');
					count = 0;
				}
			}

			// Reverse the string to get 
			// the desired output 
			reverse(ans.begin(), ans.end());

			// If the given string is 
			// less than 1000 
			if (ans.size() % 4 == 0) {

				// Remove ',' 
				ans.erase(ans.begin());
			}

			return ans;
		}
	};
}