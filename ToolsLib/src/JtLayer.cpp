#include "JtLayer.h"

#include <assert.h>
#include  <cctype>

#include <iostream>
using namespace std;

LayerInfo ScanForLayerFilter(const vector<char>& stream)
{
	vector<char>::const_iterator It = stream.begin();
	LayerInfo layerInfo;
	

	while (It != stream.end()) {

		if (*It        == 'A' &&
			*(It + 1)  == 'C' &&
			*(It + 2)  == 'T' &&
			*(It + 3)  == 'I' &&
			*(It + 4)  == 'V' &&
			*(It + 5)  == 'E' &&
			*(It + 6)  == 'L' &&
			*(It + 7)  == 'A' &&
			*(It + 8)  == 'Y' &&
			*(It + 9)  == 'E' &&
			*(It + 10) == 'R' &&
			*(It + 11) == 'F' &&
			*(It + 12) == 'I' &&
			*(It + 13) == 'L' &&
			*(It + 14) == 'T' &&
			*(It + 15) == 'E' &&
			*(It + 16) == 'R' &&
			*(It + 17) == ':' &&
			*(It + 18) == ':' &&
			*(It + 19) == '\0'){

			// Jump to Value
			It += 20;

			layerInfo.ActiveLayerFilter.reserve(31);
			// read value
			while (*It != '\0' && It != stream.end())
				layerInfo.ActiveLayerFilter.push_back(*(It++));

			// skip '/0'
			if (It != stream.end())
				It++;

		}
		else if ( *It == 'L' &&
			*(It + 1) == 'A' &&
			*(It + 2) == 'Y' &&
			*(It + 3) == 'E' &&
			*(It + 4) == 'R' &&
			*(It + 5) == 'F' &&
			*(It + 6) == 'I' &&
			*(It + 7) == 'L' &&
			*(It + 8) == 'T' &&
			*(It + 9) == 'E' &&
			*(It + 10) == 'R' &&
			isdigit(*(It + 11)) &&
			isdigit(*(It + 12)) &&
			isdigit(*(It + 13)) &&
			*(It + 14) == ':' &&
			*(It + 15) == ':' &&
			*(It + 16) == '\0') {

			// Jump to Value
			It += 17;
			string LayerName;
			LayerName.reserve(31);
			vector<uint32_t> LayerNums;
			LayerNums.reserve(16);
			string NumBuf;
			NumBuf.reserve(31);

			// read Layer Name
			while (*It != '=' && It != stream.end())
				LayerName.push_back(*(It++));
			// skip '='
			if (It != stream.end())
				It++;

			// read the layer numbers seperated by ','
			while (*It != '\0' && It != stream.end()) {
				NumBuf.clear();
				// read next number
				while (*It != ',' && *It != '\0' && It != stream.end())
					NumBuf.push_back(*(It++));

				LayerNums.push_back(stoi(NumBuf));

				// dont skip '\0' to break out loop
				if (*It == '\0')
					continue;

				// skip ','
				if (It != stream.end())
					It++;


			}

			layerInfo.LayerMap[LayerName] = LayerNums;

			// skip '/0'
			if (It != stream.end())
				It++;
		}
		else {
			// eat up rest of key
			while (*It != '\0' && It != stream.end())
				It++;
			// skip '/0'
			if (It != stream.end())
				It++;
			// eat up value
			while (*It != '\0' && It != stream.end())
				It++;
			// skip '/0'
			if (It != stream.end())
				It++;
		}


	}

	return layerInfo;
}

vector<uint32_t> ScanForLayer(const std::vector<char>& stream)
{
	vector<uint32_t> result;
	result.reserve(16);

	vector<char>::const_iterator It = stream.begin();

	while (It != stream.end()) {

		if (      *It == 'L' &&
			*(It + 1) == 'A' &&
			*(It + 2) == 'Y' &&
			*(It + 3) == 'E' &&
			*(It + 4) == 'R' &&
			*(It + 5) == ':' &&
			*(It + 6) == ':' &&
			*(It + 7) == '\0') {

			// Jump to Value
			It += 8;

			string NumBuf;
			NumBuf.reserve(31);

			// read the layer numbers seperated by ','
			while (*It != '\0' && It != stream.end()) {
				NumBuf.clear();
				// read next number
				while (*It != ',' && *It != '\0' && It != stream.end())
					NumBuf.push_back(*(It++));

				result.push_back(stoi(NumBuf));

				// dont skip '\0' to break out loop
				if (*It == '\0')
					continue;

				// skip ','
				if (It != stream.end())
					It++;

			}
			// skip '\0'
			if (It != stream.end())
				It++;

		}
		else {
			// eat up rest of key
			while (*It != '\0' && It != stream.end())
				It++;
			// skip '/0'
			if (It != stream.end())
				It++;
			// eat up value
			while (*It != '\0' && It != stream.end())
				It++;
			// skip '/0'
			if (It != stream.end())
				It++;
		}
	}
	
	return result;
}