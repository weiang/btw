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
#include <regex>
#include <dirent.h>

using namespace std;

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

// Code in different programming languages have differnet forms
class Form {
	public:
		virtual bool is_code_line(const string&) = 0;
		virtual bool is_blank_line(const string&) = 0;
		virtual bool is_comment_line(const string&) = 0;
};

class CForm : public Form {
	public:
		virtual bool is_code_line(const string&);
		virtual bool is_blank_line(const string&);
		virtual bool is_comment_line(const string&);

};

class PythonForm : public Form {
	public:
		virtual bool is_code_line(const string&);
		virtual bool is_blank_line(const string&);
		virtual bool is_comment_line(const string&);
};

class FormFactory {
	public:
		FormFactory() : _form{nullptr} { }
		virtual Form* make_form() = 0;

	protected:
		Form* _form;
};

class CFormFactory : public FormFactory {
	public:
		virtual Form* make_form() 
		{
			if (_form == nullptr) {
				_form = new CForm();
			}
			return _form;
		}
};

class PythonFormFactory : public FormFactory {
	public:
		virtual Form* make_form()
		{
			if (_form == nullptr) {
				_form = new PythonForm();
			}
			return _form;
		}
};

bool CForm::is_code_line(const string& str)
{
	int result = 0;
	char c;

	auto iter = str.begin();
	while (iter != str.end()) {
		if (!isblank(*iter))
			break;
		iter ++;
	}
	while (iter != str.end()) {
		c = *iter;
		if (isdigit(c) or isalpha(c)) {
			break;
		}
		if (c == '/') {
			if ((iter + 1) != str.end() and *(iter + 1) == '/')
				return false;
		}
		iter ++;
	}
	while (iter != str.end()) {
		c = *iter;
		if (isdigit(c) or isalpha(c)) {
			result += 1;
		}
		if (result >= 2)
			return true;
	}
	return false;
}

bool CForm::is_blank_line(const string& str)
{
	int result = 0;
	for (auto c : str) {
		if (isdigit(c) or isalpha(c)) {
			result += 1;
		}
	}
	if (result <= 1)
		return true;
	return false;
}

bool CForm::is_comment_line(const string& str)
{
	bool flag = false;
	char c;
	int result = 0;

	auto iter = str.begin();
	while (iter != str.end()) {
		if (!isblank(*iter))
			break;
		iter ++;
	}
	while (iter != str.end()) {
		c = *iter;
		if (isdigit(c) or isalpha(c)) 
			break;
		if (c == '/') {
			if ((iter + 1) != str.end() and *(iter + 1) == '/') {
				flag = true;
				iter ++;
				iter ++;
				break;
			}
		}
		iter ++;
	}
	while (iter != str.end()) {
		c = *iter;
		if (isdigit(c) or isalpha(c)) {
			result += 1;
		}
		if (result >= 2)
			return false;
	}
	return true;
}

bool PythonForm::is_code_line(const string& str)
{
	return false;
}

bool PythonForm::is_blank_line(const string& str)
{
	return false;
}

bool PythonForm::is_comment_line(const string& str)
{
	return false;
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
		
		void set_form(Form* f) 
		{
			_form = f;
		}
		virtual void execute();
		virtual void show();

	private:
		int _codeLineCount;
		int _blankLineCount;
		int _commentLineCount;
		Form* _form;
};

class SOption : public Option {
	public:
		SOption() : Option{string()} { }
		~SOption()
		{
			for (auto option : _options) {
				delete option;
			}
		}
		
		void add_option(Option *o)
		{
			_options.push_back(o);
		}
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
	ifstream in(_filename);

	if (!in.is_open()) {
		cout << "Error open file: " << _filename << endl;
		exit(1);
	}
	else {
		_codeLineCount = 0;
		_blankLineCount = 0;
		_commentLineCount = 0;

		while (!in.eof()) {
			string buffer;
			getline(in, buffer);
			if (_form->is_code_line(buffer)) {
				_codeLineCount += 1;
			}
			else if (_form->is_blank_line(buffer)) {
				_blankLineCount += 1;
			}
			else if (_form->is_comment_line(buffer)) {
				_commentLineCount += 1;
			}
		}
	}
}

void AOption::show()
{
	format_output("code line count", _filename, _codeLineCount);
	format_output("blank line count", _filename, _blankLineCount);
	format_output("comment line count", _filename, _commentLineCount);
}

void SOption::execute()
{
	for (auto& option : _options) {
		option->execute();
	}
}

void SOption::show()
{
	for (auto& option : _options) {
		option->show();
	}
}

Form* make_form(const string& filePattern)
{
	auto iter = filePattern.begin();
	auto lastIndex = filePattern.end();
	while (iter != filePattern.end()) {
		// Find the last position of '.' in filePattern
		if (*iter == '.') {
			lastIndex = iter;
		}
		iter ++;
	}

	string postfix(lastIndex + 1, filePattern.end());
	if (postfix == string("py")) {
//		cout << "Form: " << "Py" << endl;
		return new PythonForm();
	}
	else {
//		cout << "Form: " << "C" << endl;
		return new CForm();
	}
}

/**
 * Get files matching the regular expression
 */
vector<string> get_all_files(const string& path, const string& regularExpression)
{
	DIR* dp = opendir(path.c_str());
	if (dp == nullptr) {
		cout << "Can't open directory: " << path << endl;
		exit(1);
	}

	vector<string> result;
	struct dirent* dirp;
	// Walk through the directory to find the matched files
	while ((dirp = readdir(dp)) != nullptr) {
		if (dirp->d_type == DT_DIR) { // Directory
			if (dirp->d_name[0] == '.') // Ignore file or directory named with prefix '.' 
				continue;
	//		cout << "Dir: " << dirp->d_name << endl;
			string subPath = path + string(string("/") + string(dirp->d_name));
			vector<string> subResult = get_all_files(subPath, regularExpression);
			for (auto e : subResult) {
				result.push_back(e);
			}
		}
		else if (dirp->d_type == DT_REG) { // Regular file 
	//		cout << "File: " << dirp->d_name << endl;
			regex regObj(regularExpression, regex::icase);
			if (regex_match(dirp->d_name, regObj)) {
				result.push_back(dirp->d_name);
			}
		}
	}
	return result;
}

string get_path(const string& str)
{
	auto iter = str.begin();
	auto lastIter = str.end();

	while (iter != str.end()) {
		if (*iter == '/') {
			lastIter = iter;
		}
		iter ++;
	}
	string path;
	if (lastIter == str.end()) { // No '/' contained in str
		path = string(".");
	}
	else {
		path = string(str.begin(), lastIter);
	}
	return path;
}

string get_regex(const string& str)
{
	auto iter = str.begin();
	auto lastIter = str.end();

	while (iter != str.end()) {
		if (*iter == '/') {
			lastIter = iter;
		}
		iter ++;
	}
	string regex;
	if (lastIter == str.end()) {
		regex = str;
	}
	else {
		regex = string(lastIter + 1, str.end());
	}
	return regex;
}

Option* parse_option(int argc, char* argv[]) 
{
	bool sFlag = false;
	bool aFlag = false;
	bool cFlag = false;
	bool wFlag = false;
	bool lFlag = false;

	for (int i = 1; i != argc; i ++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 's':
					sFlag = true;
					break;
				case 'a':
					aFlag = true;
					break;
				case 'c':
					cFlag = true;
					break;
				case 'w':
					wFlag = true;
					break;
				case 'l':
					lFlag = true;
					break;
			}
		}
	}

	vector<string> files;
	if (sFlag == true) {
		auto str = argv[argc-1];
		// Todo: match files from regula expression
		auto path = get_path(str);
		auto re = get_regex(str);	
		files = get_all_files(path, re);
	}
	else {
		files.push_back(argv[argc-1]);
	}
	
	Form* form = nullptr;
	if (aFlag == true) {
		// Todo: create form instance according to file's postfix
		form = make_form(argv[argc-1]);
	}

	cout << "Debug: " << cFlag;
	cout << " " << wFlag;
	cout << " " << lFlag;
	cout << " " << aFlag;
	cout << " " << sFlag << endl;

	SOption* s_opt = new SOption();
	for (auto file : files) {
		if (aFlag) {
			AOption* a_opt = new AOption(file);
			a_opt->set_form(form);
			s_opt->add_option(a_opt);
		}
		if (cFlag) {
			s_opt->add_option(new COption(file));
		}
		if (wFlag) {
			s_opt->add_option(new WOption(file));
		}
		if (lFlag) {
			s_opt->add_option(new LOption(file));
		}
	}
	return s_opt; 
}

/**
 * Test Module
 */
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

void test_AOption()
{
	AOption a1 {"1.c"};
	AOption a2 {"2.c"};
	AOption a3 {"3.c"};

	Form* form = new CForm();
	a1.set_form(form);
	a2.set_form(form);
	a3.set_form(form);
	a1.execute();
	a1.show();
	a2.execute();
	a2.show();
	a3.execute();
	a3.show();

	delete form;
}

void test_parse_option()
{
	char* arg1[] {"./a.out", "-c", "1.txt"};
	char* arg2[] {"./a.out", "-w", "2.txt"};
	char* arg3[] {"./a.out", "-l", "3.txt"};
	char* arg4[] {"./a.out", "-a", "2.c"};
	char* arg5[] {"./a.out", "-s", "-c", "*.txt"};
	char* arg6[] {"./a.out", "-a", "-s", "*.c"};
	char* arg7[] {"./a.out", "-a", "-s", "/home/angwei/tmp/*.py"};

	parse_option(3, arg1);
	parse_option(3, arg2); 
	parse_option(3, arg3); 
	parse_option(3, arg4); 
	parse_option(4, arg5); 
	parse_option(4, arg6); 
	parse_option(4, arg7); 
}

void show_vector(const vector<string>& v)
{
	cout << "vector:";
	for (auto e : v)
		cout << " " << e;
	cout << endl;
}

void test_path_regex_files()
{
	string str1 {"a.c"};
	string str2 {"./*.c"};
	string str3 {"/home/angwei/c/c.c"};
	string str4 {"/home/angwei/c/*.c"};

	assert(get_path(str1) == string("."));
	assert(get_path(str2) == string("."));
	assert(get_path(str3) == string("/home/angwei/c"));
	assert(get_path(str4) == string("/home/angwei/c"));
	assert(get_regex(str1) == string("a.c"));
	assert(get_regex(str2) == string("*.c"));
	assert(get_regex(str3) == string("c.c"));
	assert(get_regex(str4) == string("*.c"));

	show_vector(get_all_files(string("."), string("1.c")));
	show_vector(get_all_files(string("."), string(".*\\.c")));
	show_vector(get_all_files(string("/home/angwei/books/pci"), string(".*\\.py")));
	show_vector(get_all_files(string("/home/angwei/books/pci/chap2"), string("u.data")));
}

void test_main()
{
//	test_char();
//	test_word();
//  test_line();
	
//	test_COption();
//	test_WOption();
//	test_LOption();
//	test_AOption();
//	test_path_regex_files();
	test_parse_option();
}

int main(int argc, char*argv[])
{
	test_main();	

//	Option* option = parse_option(argc, argv);
//	option->execute();
//	option->show();
//	
//	delete option;
	return 0;
}
