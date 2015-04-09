/*************************************************************************
	> File Name: main.cc
	> Author: Weiang
	> Mail: weiang@mail.ustc.edu.cn 
	> Created Time: 2015年04月09日 星期四 14时20分23秒
    > Describition: 
 ************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <locale>
#include <fstream>

using namespace std;

const int MAX_BUF_SIZE = 256;

// Aux functions
/**
 * Counts the number of chars in a string
 */
int get_char_count(const string& str)
{
	return str.length();
}

/**
 * Counts the number of words in a string
 */
int get_word_count(const string& str)
{
	int result = 0;

	auto iter = str.begin();
	
	while (iter != str.end()) {
		while (iter != str.end() and (isblank(*iter) or *iter == '\n')) {
			iter ++;
		}
		if (iter != str.end()) {
			result += 1;
		}
		while (iter != str.end() and !(isblank(*iter) or *iter == '\n')) {
			iter ++;
		}
	}
	return result;
}

/**
 * Counts the number of line in a string
 */
int get_line_count(const string& str)
{
	int result = 0;

	if (str == "") {
		return 0;
	}
	for (auto c : str) {
		if (c == '\n') {
			result += 1;
		}
	}
	if (str[str.length() - 1] != '\n') {
		result += 1;
	}
	return result;
}

/**
 * Format the output of the result
 */
void format_output(const string& str, const string& filename, int result)
{
	cout << str << "[" << filename << "]:" << result << endl;
}

class Option {
	public:
		Option(const string& fn) : _filename{fn} { }

		virtual void execute() = 0;
		virtual void show() = 0;
	
	protected:
		string _filename;
};

class COption : public Option {
	public:
		COption(const string& fn) : Option{fn}, _charCount{0} { }

		virtual void execute();
		virtual void show();

	private:
		int _charCount;
};

class WOption : public Option {
	public:
		WOption(const string& fn) : Option{fn}, _wordCount{0} { }

		virtual void execute();
		virtual void show();

	private:
		int _wordCount;
};

class LOption : public Option {
	public:
		LOption(const string& fn) : Option{fn}, _lineCount{0} { }

		virtual void execute();
		virtual void show();

	private:
		int _lineCount;
};

class AOption : public Option {
	public:
		AOption(const string& fn) : Option{fn}, _codeLineCount{0},
									_blankLineCount{0}, _commentLineCount{0} { }

		virtual void execute();
		virtual void show();

	private:
		int _codeLineCount;
		int _blankLineCount;
		int _commentLineCount;
};

class SOption : public Option {
	public:
		SOption() : Option{string()} { }

		virtual void execute();
		virtual void show();

	private:
		vector<Option*> _options;
};

void COption::execute()
{
	ifstream in(_filename);

	if (!in.is_open()) {
		cout << "Error open file: " << _filename << endl;
		exit(1);
	}
	else {
		_charCount = 0;
		while (!in.eof()) {
			string buffer;
			// get one line from file stream, '\n' is kept
			while (!in.eof()) {
				char c;
				c = in.get();
				// Check if eof was reached
				if (in.eof()) 
					break;
				buffer.push_back(c);
				if (c == '\n')
					break;
			}
			_charCount += get_char_count(buffer);
		}
	}

	in.close();
}

void COption::show() 
{
	format_output("char count", _filename, _charCount);
}

void WOption::execute()
{
	ifstream in(_filename);

	if (!in.is_open()) {
		cout << "Error open file: " << _filename << endl;
		exit(1);
	}
	else {
		_wordCount = 0;
		while (!in.eof()) {
			string buffer;
			while (!in.eof()) {
				char c = in.get();
				if (in.eof())
					break;
				buffer.push_back(c);
				if (c == '\n') 
					break;
			}
			_wordCount += get_word_count(buffer);
		}
	}

	in.close();
}

void WOption::show()
{
	format_output("word count", _filename, _wordCount);
}

void LOption::execute()
{
	ifstream in(_filename);

	if (!in.is_open()) {
		cout << "Error open file: " << _filename << endl;
		exit(1);
	}
	else {
		_lineCount = 0;
		while (!in.eof()) {
			string buffer;
			while (!in.eof()) {
				char c = in.get();
				if (in.eof())
					break;
				buffer.push_back(c);
				if (c == '\n') 
					break;
			}
			_lineCount += get_line_count(buffer);
		}
	}

	in.close();
}

void LOption::show()
{
	format_output("line count", _filename, _lineCount);
}

void AOption::execute()
{
}

void AOption::show()
{
	format_output("code line count", _filename, _codeLineCount);
	format_output("blank line count", _filename, _blankLineCount);
	format_output("comment line count", _filename, _commentLineCount);
}

void SOption::execute()
{
}

void SOption::show()
{
	for (auto& option : _options) {
		option->show();
	}
}

void test_char()
{
	string str1 {""};
	string str2 {"a "};
	string str3 {"a    bb\tc  d  \n"};
	string str4 {"abcdef"};
	string str5 {"   abc  \t  "};
	string str6 {" \n  a  bc \n \t d ef "};

	assert(get_char_count(str1) == 0);
	assert(get_char_count(str2) == 2);
//	cout << str3 << ":" << get_char_count(str3) << endl;
	assert(get_char_count(str3) == 15);
	assert(get_char_count(str4) == 6);
	assert(get_char_count(str5) == 11);
	assert(get_char_count(str6) == 19);
}

void test_word()
{
	string str1 {""};
	string str2 {"a "};
	string str3 {"a    bb\tc  d  \n"};
	string str4 {"abcdef"};
	string str5 {"   abc  \t  "};
	string str6 {" \n  a  bc \n \t d ef "};

	assert(get_word_count(str1) == 0);
	assert(get_word_count(str2) == 1);
//	cout << strword ":" << get_char_count(str3) << endl;
	assert(get_word_count(str3) == 4);
	assert(get_word_count(str4) == 1);
	assert(get_word_count(str5) == 1);
	assert(get_word_count(str6) == 4);
}

void test_line()
{
	string str1 {""};
	string str2 {"a "};
	string str3 {"a    bb\tc  d  \n"};
	string str4 {"abcdef"};
	string str5 {"   abc  \t\n  "};
	string str6 {" \n  a  bc \n \t d ef "};

	assert(get_line_count(str1) == 0);
	assert(get_line_count(str2) == 1);
//	cout << strline ":" << get_char_count(str3) << endl;
	assert(get_line_count(str3) == 1);
	assert(get_line_count(str4) == 1);
	assert(get_line_count(str5) == 2);
	assert(get_line_count(str6) == 3);
}

void test_COption()
{ COption c1 {"1.txt"};
	COption c2 {"2.txt"};
	COption c3 {"3.txt"};


	c1.execute();
	c1.show();
	c2.execute();
	c2.show();
	c3.execute();
	c3.show();
}

void test_WOption()
{
	WOption w1 {"1.txt"};
	WOption w2 {"2.txt"};
	WOption w3 {"3.txt"};


	w1.execute();
	w1.show();
	w2.execute();
	w2.show();
	w3.execute();
	w3.show();
}

void test_LOption()
{
	LOption l1 {"1.txt"};
	LOption l2 {"2.txt"};
	LOption l3 {"3.txt"};


	l1.execute();
	l1.show();
	l2.execute();
	l2.show();
	l3.execute();
	l3.show();
}

void test_main()
{
//	test_char();
//	test_word();
//  test_line();
	
	test_COption();
	test_WOption();
	test_LOption();
}

int main(int argc, char*argv[])
{
	test_main();	
	return 0;
}
