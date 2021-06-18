//MIT License
//
//Copyright(c) 2021 Richard Bamford
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this softwareand associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <cstring>
#include <fcntl.h>
#include <assert.h>
#include <unordered_set>
#include <set>

#include <regex>
//msvc __cplusplus 199711l
#ifdef __linux__
#if __cplusplus >= 201103L &&                             \
    (!defined(__GLIBCXX__) || (__cplusplus >= 201402L) || \
        (defined(_GLIBCXX_REGEX_DFS_QUANTIFIERS_LIMIT) || \
         defined(_GLIBCXX_REGEX_STATE_LIMIT)           || \
             (defined(_GLIBCXX_RELEASE)                && \
             _GLIBCXX_RELEASE > 4)))
#define HAVE_WORKING_REGEX 1
#else
#define HAVE_WORKING_REGEX 0
#endif
#else
#define HAVE_WORKING_REGEX 1
#endif

#define LOGERROR(fmt, ...)\
	fprintf(stderr,"[ERROR] function:[%s] line:[%d]: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOGWARNING(fmt, ...)\
	fprintf(stderr,"[WARNING] function:[%s] line:[%d]: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define NEW_POINT(f,b,name)\
std::vector<std::string> tag;\
Split(b, tag, ',');\
if (tag.size() >= 1) {\
	fields_table[*tag.begin()] = new FieldConverter##name<StructType, FieldType>(f, *tag.begin(), tag);\
}else{LOGERROR("Insufficient parameters");}

#define INIT(r) r()

#ifdef SPLIT_CHECK_EMPEY
#define fast_split(a,b,c) split_c(a,b,c)
#else
#define fast_split(a,b,c) split(a,b,c)
#endif

const static int initRowSize = 1024;
const static int initBufSize = 128;
const static int initLineBufSize = 2048;

//Standard C ++ String Split is convenient, but not the most efficient
static void Split(const std::string& source, std::vector<std::string>& tokens, const char delimiters) {
	tokens.clear();
	for (auto itStart = source.begin(), itDelim = itStart, itEnd = source.end(); itStart < itEnd; itStart = itDelim + 1)
	{
		itDelim = find(itStart, itEnd, delimiters);
		std::string str(itStart, itDelim);
		if (str == "")
			continue;
		tokens.emplace_back(str);
		if (itDelim == itEnd)
			break;
	}
}

//Specialized for efficient slicing of strings
class StringRef
{
private:
	const char* m_itBeg;
	int m_iSize;
	int m_col;
public:
	int size() const { return m_iSize; }
	const char* begin() { return m_itBeg; }
	const char* end() const { return m_itBeg + m_iSize; }
	bool operator==(const std::string& str) {
		if (m_iSize != str.size()) {
			return false;
		}
		for (int i = 0; i < m_iSize; i++) {
			if (m_itBeg[i] != str[i]) {
				return false;
			}
		}
		return true;
	}
	const int getCol() { return m_col; }
	StringRef() = delete;
	StringRef(const char*& beg, const char*& nd,const int col)
		: m_itBeg(beg)
		, m_iSize(nd - beg)
		, m_col(col)
	{}
};

//This split does not check if the split string is empty
void split(std::vector<StringRef>& ret, const std::string& s,const char delimiters)
{
	int col = 0;
	for (auto ps = &s[0], pd = ps, pend = ps + s.size(); ps < pend , pd < pend; ps = pd + 1 )
	{
		ret.emplace_back(StringRef(ps, pd = std::find(ps, pend, delimiters), col++));
	}
}

//This split checks to see if the split string is empty
void split_c(std::vector<StringRef>& ret, const std::string& s, const char delimiters)
{
	int col = 0;
	for (auto ps = &s[0], pd = ps, pend = ps + s.size(); ps < pend, pd < pend; ps = pd + 1)
	{
		pd = std::find(ps, pend, delimiters);
		assert(pd - ps);
		ret.emplace_back(StringRef(ps, pd, col++));
	}
}

//If you want to use regular expressions your GCC version must be higher than 4.9
//see https://stackoverflow.com/questions/12530406/is-gcc-4-8-or-earlier-buggy-about-regular-expressions
bool isRegexMatch(std::string reg, std::string value) {
	return std::regex_match(reg.begin(), reg.end(), std::regex(value));
}

template<typename _Pr>
bool read_table_if(const char* path, const char delimiters, _Pr pr) {
	std::ifstream f(path);
	if (f.fail())
	{
		LOGERROR("Failed to open file:%s ", path);
		return false;
	}

	int row = 0;
	int first_col = 0;
	std::string line(initLineBufSize, 0);
	while (std::getline(f,line))
	{
		std::vector<StringRef> w;
		if (first_col) {
			w.reserve(first_col);
		}

		fast_split(w, line, delimiters);
		if (row == 0) {
			first_col = w.size();
		}
		if (first_col != 0 && first_col != w.size()) {
			LOGERROR("the current row number parameter is inconsistent at line:%d ", row);
			return false;
		}

		if (!pr(row, w)) {
			LOGERROR("parse table error:%d", row);
			return false;
		}
		
		row++;
	}
	return true;
}

#define EVTYPE_KEY      0x01
#define EVTYPE_REGEX    0x02
#define EVTYPE_SPLIT    0x04

class ParseFile;

template <typename FieldType>
class ConvertToStandard
{
public:
	static constexpr void Convert(FieldType* dest, std::string& value)
	{
		if (dest)
		{
			std::stringstream s_stream;
			s_stream << value;
			s_stream >> *dest;
		}
	}

	static constexpr void Convert(FieldType* dest,const char* str,int size)
	{
		if (dest && str && size > 0) {
			std::stringstream s_stream;
			for (int i = 0; i < size; i++) {
				s_stream.put(str[i]);
			}
			s_stream >> *dest;
		}
	}
};

template<typename FieldType>
void covert(FieldType* field, std::string& value) {
	ConvertToStandard<FieldType>::Convert(field, value);
}

template<typename FieldType>
void covert(FieldType* field, const char* str,int size) {
	ConvertToStandard<FieldType>::Convert(field, str,size);
}

template<typename FieldType>
void covert_to_vector(std::vector<FieldType>* field, const char* str, int size) {
	FieldType res;
	covert(&res, str, size);
	field->push_back(res);
}

template<typename FieldType>
void covert_to_set(std::unordered_set<FieldType>* field, const char* str, int size) {
	FieldType res;
	covert(&res, str, size);
	field->insert(res);
}


template<typename FieldType>
void covert_to_set(std::set<FieldType>* field, const char* str, int size) {
	FieldType res;
	covert(&res, str, size);
	field->insert(res);
}

template <typename StructType>
class FieldConverterBase {
public:
	virtual ~FieldConverterBase() = default;
	virtual void Copy(StructType& obj, const char* str,int size) = 0;
	virtual int GetType() = 0;
	virtual std::string GetField() = 0;
	virtual void SetPlace(int col) = 0;
	virtual std::vector<int>& GetPlace() = 0;
	virtual void CheckPlace(std::vector<StringRef>& f) = 0;
};


template <typename StructType>
class FieldConverterComponment : public FieldConverterBase<StructType> {
public:
	FieldConverterComponment() :type(0) {
	}

	int GetType()override {
		return type;
	}

	std::string GetField()override {
		return bind_field;
	}

	void SetParameter(const std::string& field, const std::vector<std::string>& t) {
		bind_field = field;
		tag = t;
		for_each(tag.begin(), tag.end(), [&](std::string& data) {
			transform(data.begin(), data.end(), data.begin(), ::tolower);

			if (data.find("key") != std::string::npos) {
				type |= EVTYPE_KEY;
			}

			// A variable may contain multiple field types
			// may be [1,2,3,4],need add "pkg" keyword,If you don't want to save some unexpected value add "ex:number"
			else if (data.find("pkg") != std::string::npos) {
#if HAVE_WORKING_REGEX
				type |= EVTYPE_REGEX;		
#else
				LOGERROR("you <regex> doesn't work");
#endif
			}

			//// split a|b 
			//// This version will not be added
			//if (data.find("sp") != std::string::npos) {
			//	type |= EVTYPE_SPLIT;
			//}
			});
	}

	void CheckPlace(std::vector<StringRef>& f)override {
		if (f.empty()) {
			return;
		}
		if (!col_pos.empty()) {
			return;
		}
		int col = 0;
		col_pos.clear();
		for (auto& Iter : f) {
			if (GetType() & EVTYPE_REGEX) {
				std::string str(Iter.begin(), Iter.size());
				if (isRegexMatch(str, bind_field)) {
					SetPlace(col);
				}
			}
			else
			{
				if (Iter == bind_field) {
					SetPlace(col);
					return;
				}
			}
			col++;
		}
		if (col_pos.empty()) {
			LOGWARNING("The marked field could not be found -> [%s]", GetField().c_str());
		}
	}

	void SetPlace(int c)override {
		col_pos.push_back(c);
	}

	std::vector<int>& GetPlace()override {
		return col_pos;
	}
private:
	int type;
	std::vector<int> col_pos;
	std::vector<std::string> tag;
	std::string bind_field;
};

template <typename StructType, typename FieldType>
class FieldConverterSubTypeNone : public FieldConverterComponment<StructType> {
	typedef  FieldConverterComponment<StructType> Super;
	static_assert(!std::is_same<FieldType, const char*>::value, "if to string, dest must be std::string");
	static_assert(!std::is_same<FieldType, char*>::value, "if to string, dest must be std::string");
public:
	FieldConverterSubTypeNone(
		FieldType StructType::* pointer,
		const std::string field,
		const std::vector<std::string>& tag) {
		Super::SetParameter(field, tag);
		this->pointer = pointer;
	}

	void Copy(StructType& obj, const char* str, int size) {
		covert(&(obj.*pointer), str, size);
	}
private:
	FieldType StructType::* pointer;
};

template <typename StructType, typename FieldType>
class FieldConverterSubTypeVector : public FieldConverterComponment<StructType> {
	typedef  FieldConverterComponment<StructType> Super;
public:
	FieldConverterSubTypeVector(std::vector<FieldType> StructType::* pointer,
		const std::string field,
		const std::vector<std::string>& tag) {
		Super::SetParameter(field, tag);
		this->pointer = pointer;
	}

	void Copy(StructType& obj, const char* str, int size)override {
		covert_to_vector(&(obj.*pointer), str, size);
	}
private:
	std::vector<FieldType> StructType::* pointer;
};

template <typename StructType, typename FieldType>
class FieldConverterSubTypeSet : public FieldConverterComponment<StructType> {
	typedef  FieldConverterComponment<StructType> Super;
public:
	FieldConverterSubTypeSet(std::set<FieldType> StructType::* pointer,
		const std::string field,
		const std::vector<std::string>& tag) {
		Super::SetParameter(field, tag);
		this->pointer = pointer;
	}

	void Copy(StructType& obj, const char* str,int size)override {
		covert_to_set(&(obj.*pointer), str, size);
	}
private:
	std::set<FieldType> StructType::* pointer;
};

template <typename StructType, typename FieldType>
class FieldConverterSubTypeUSet : public FieldConverterComponment<StructType> {
	typedef  FieldConverterComponment<StructType> Super;
public:
	FieldConverterSubTypeUSet(std::unordered_set<FieldType> StructType::* pointer,
		const std::string field,
		const std::vector<std::string>& tag) {
		Super::SetParameter(field, tag);
		this->pointer = pointer;
	}

	void Copy(StructType& obj, const char*str, int size)override {
		covert_to_set(&(obj.*pointer), str, size);
	}
private:
	std::unordered_set<FieldType> StructType::* pointer;
};

template <typename StructType>
class StructValueConverter {
public:
	StructValueConverter() = delete;

	StructValueConverter(const char* file_name, const char d = '\t') :
		filepath(file_name),
		delimiters(d){
	}

	~StructValueConverter()
	{
		Destory();
	}

	//To create a table structure object with a structure as its template, you pass in the path to the table and the split character
	static StructValueConverter<StructType>* CreateTable(const char* file_name, const char d = '\t') {
		StructValueConverter<StructType>* newConvert = new StructValueConverter<StructType>(file_name, d);
		do {
			if (newConvert == nullptr) {
				break;
			}
			return newConvert;
		} while (0);

		if (newConvert) {
			delete newConvert;
			newConvert = nullptr;
		}
		return newConvert;
	}

	//Export as  STL map/unordered_map  data structure
	template<typename KeyType>
	bool Unmarshal(std::unordered_map<KeyType, StructType>& save) {
		return fast_parse(save);
	}

	//Export as  STL map/unordered_map  data structure,and support for lambda to check data easily
	template<typename KeyType,typename _Pr>
	bool Unmarshal(std::unordered_map<KeyType, StructType>& save, _Pr pr) {
		return fast_parse(save, pr);
	}

	//Export as  STL map/unordered_map  data structure
	template<typename KeyType>
	bool Unmarshal(std::map<KeyType, StructType>& save) {
		return fast_parse(save);
	}

	//Export as  STL map/unordered_map  data structure,and support for lambda to check data easily
	template<typename KeyType, typename _Pr>
	bool Unmarshal(std::map<KeyType, StructType>& save, _Pr pr) {
		return fast_parse(save, pr);
	}

	//Export as  STL vector  data structure
	bool Unmarshal(std::vector<StructType>& save) {
		return fast_parse(save);
	}

	//Export as  STL vector  data structure,and support for lambda to check data easily
	template<typename _Pr>
	bool Unmarshal(std::vector<StructType>& save, _Pr pr) {
		return fast_parse(save, pr);
	}

	/** Register
	Support for C ++ regular variables
	stl Support vector set
	nonsupport char*
	*/
	//Register a store of the underlying type
	template <typename FieldType>
	void Register(FieldType StructType::* field_pointer,
		std::string bind_field) {
		newPointer(field_pointer, bind_field);
	}

	//Register a store for std::vector,
	template <typename FieldType>
	void Register(std::vector<FieldType> StructType::* field_pointer,
		std::string bind_field) {
		newPointer(field_pointer, bind_field);
	}

	//Register a store for std::set/unordered_set,
	template <typename FieldType>
	void Register(std::set<FieldType> StructType::* field_pointer,
		std::string bind_field) {
		newPointer(field_pointer, bind_field);
	}

	//Register a store for std::set/unordered_set,
	template <typename FieldType>
	void Register(std::unordered_set<FieldType> StructType::* field_pointer,
		std::string bind_field) {
		newPointer(field_pointer, bind_field);
	}
	
	bool IsEmpty() {
		return fields_table.empty();
	}
private:
	void Destory() {
		for (auto& Iter : fields_table) {
			if (Iter.second != nullptr) {
				delete Iter.second;
			}
		}
		fields_table.clear();
		delimiters = '\0';
	}

	template <typename FieldType>
	void newPointer(FieldType StructType::* field_pointer,
		std::string bind_field) {
		NEW_POINT(field_pointer, bind_field, SubTypeNone);
	}

	template <typename FieldType>
	void newPointer(std::vector<FieldType> StructType::* field_pointer,
		std::string bind_field) {
		NEW_POINT(field_pointer, bind_field, SubTypeVector);
	}

	template <typename FieldType>
	void newPointer(std::set<FieldType> StructType::* field_pointer,
		std::string bind_field) {
		NEW_POINT(field_pointer, bind_field, SubTypeSet);
	}

	template <typename FieldType>
	void newPointer(std::unordered_set<FieldType> StructType::* field_pointer,
		std::string bind_field) {
		NEW_POINT(field_pointer, bind_field, SubTypeUSet);
	}

	bool fast_parse(std::vector<StructType>& save) {
		return read_table_if(filepath.c_str(), delimiters, [&](int row,
			std::vector<StringRef>& row_data) {
				StructType obj;
				if(row == 0) return to(save, obj, row_data, row);
				if (to(save, obj, row_data, row)) {
					save.push_back(obj);
					return true;
				}
				else { return false; } return false;	});
	}

	template<typename _Pr>
	bool fast_parse(std::vector<StructType>& save, _Pr pr) {
		return read_table_if(filepath.c_str(), delimiters, [&](int row, 
			std::vector<StringRef>& row_data) {
				StructType obj;
				if (row == 0) return to(save, obj, row_data, row);
				if (to(save, obj, row_data, row) && pr(obj)) {
					save.push_back(obj);
					return true;
				}
				else { return false; } return false;	});
	}

	template<typename KeyType>
	bool fast_parse(std::map<KeyType, StructType>& save) {
		return read_table_if(filepath.c_str(), delimiters, [&](int row,
			std::vector<StringRef>& row_data) {
				StructType obj;
				KeyType k = KeyType();
				if (row == 0) return to(k, obj, row_data, row);
				if (to(k, obj, row_data, row)) {
					save.insert(std::make_pair(k,obj));
					return true;
				}
				else { return false; } return false;	});
	}

	template<typename KeyType,typename _Pr>
	bool fast_parse(std::map<KeyType, StructType>& save, _Pr pr) {
		return read_table_if(filepath.c_str(), delimiters, [&](int row,
			std::vector<StringRef>& row_data) {
				StructType obj;
				KeyType k = KeyType();
				if (row == 0) return to(k, obj, row_data, row);
				if (to(k, obj, row_data, row) && pr(obj)) {
					save.insert(std::make_pair(k, obj));
					return true;
				}
				else { return false; } return false;	});
	}

	template<typename KeyType>
	bool fast_parse(std::unordered_map<KeyType, StructType>& save) {
		return read_table_if(filepath.c_str(), delimiters, [&](int row,
			std::vector<StringRef>& row_data) {
				StructType obj;
				KeyType k = KeyType();
				if (row == 0) return to(k, obj, row_data, row);
				if (to(k, obj, row_data, row)) {
					save.insert(std::make_pair(k, obj));
					return true;
				}
				else { return false; } return false;	});
	}

	template<typename KeyType, typename _Pr>
	bool fast_parse(std::unordered_map<KeyType, StructType>& save,_Pr pr) {
		return read_table_if(filepath.c_str(), delimiters, [&](int row,
			std::vector<StringRef>& row_data) {
				StructType obj;
				KeyType k = KeyType();
				if (row == 0) return to(k, obj, row_data, row);
				if (to(k, obj, row_data, row) && pr(obj)) {
					save.insert(std::make_pair(k, obj));
					return true;
				}
				else { return false; } return false;	});
	}

	bool to(std::vector<StructType>& save, StructType& temp,std::vector<StringRef>& row_data,int row) {
		if (row == 0) {
			for (auto& Iter : fields_table) {
				Iter.second->CheckPlace(row_data);
			}
			return true;
		}
		save.reserve(row_data.size());
		for (auto& fieldBase : fields_table) {
			FieldConverterBase<StructType>* base = fieldBase.second;
			if (base->GetType() & EVTYPE_REGEX) {
				for (int& col_location : base->GetPlace()) {
					if (col_location < 0 || col_location >= (int)row_data.size()) continue;
					StringRef& strRef = row_data[col_location];
					base->Copy(temp, strRef.begin(), strRef.size());
				}
			}
			else
			{
				if (base->GetPlace().empty()) {
					return true;
				}
				int col_location = base->GetPlace()[0];
				if (col_location < 0 || col_location >= (int)row_data.size()) continue;
				StringRef& strRef = row_data[col_location];
				base->Copy(temp, strRef.begin(), strRef.size());
			}
		}
		return true;
	}

	template<typename KeyType>
	bool to(KeyType& k, StructType& temp, std::vector<StringRef>& row_data,int row) {
		if (row == 0) {
			for (auto& Iter : fields_table) {
				Iter.second->CheckPlace(row_data);
			}
			return true;
		}

		for (auto& fieldBase : fields_table) {
			FieldConverterBase<StructType>* base = fieldBase.second;
			if (base->GetType() & EVTYPE_REGEX) {
				for (int& col_location : base->GetPlace()) {
					if (col_location < 0 || col_location >= (int)row_data.size()) continue;
					StringRef& strRef = row_data[col_location];
					base->Copy(temp, strRef.begin(), strRef.size());
				}
			}
			else
			{
				if (base->GetPlace().empty()) {
					return true;
				}
				int col_location = base->GetPlace()[0];
				if (col_location < 0 || col_location >= (int)row_data.size()) continue;
				StringRef& strRef = row_data[col_location];
				base->Copy(temp, strRef.begin(), strRef.size());
				if (base->GetType() & EVTYPE_KEY) {
					covert(&k, strRef.begin(), strRef.size());
				}
			}
		}
		return true;
	}

	void ClearPlace() {
		for (auto& Iter : fields_table) {
			Iter.second->GetPlace().clear();
		}
	}
private:
	std::unordered_map<std::string, FieldConverterBase<StructType>*> fields_table;
	std::string filepath;
	char delimiters;
};