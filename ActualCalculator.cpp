#include"std_lib_facilities.h"
using namespace std;

//--------------------------------------------------------------------------------------------------------------------------------------

double expression();
double term();
double declaration();
double statement();
void calculate();

//--------------------------------------------------------------------------------------------------------------------------------------


const char let = 'L';
const char quit = 'q';
const char print = ';';
const char number = '8';
const char name = 'a';
const string declkey = "let";

//--------------------------------------------------------------------------------------------------------------------------------------
class Token{
public:
	char kind;
	double value;
	string name;
	Token(char ch)
	:kind(ch),value(0) { } //converts char ch to token type
	
	Token(char ch, double val)
	: kind(ch), value(val) { }//converts char ch and double val to token type
	
	Token(char ch, string s)
	: kind(ch), name(s) { }//converts char ch and string s to token type
};
//-----------------------------------------------------------------------------------------------------------------------------------------

class Token_stream {
public:
	Token get();
	void putback(Token t);
	void ignore(char c );
private:
	bool full{false};
	Token buffer={'0'};
};


Token Token_stream::get()
{
	if (full) {full=false;return buffer;}
	char ch;
	cin >> ch;
	switch(ch)
	{
		case 'q':
		case '(':
		case ')':
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case ';':
		case '=':
			return Token(ch);
			break;
		case '.':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			{
				cin.putback(ch);
				double val;
				cin>>val;
				return Token(number, val);
			}
		default:
		if (isalpha(ch))
		{
			string s;
			s+=ch;
			while(cin.get(ch) && (isalpha(ch)||isdigit(ch)))
				s+=ch;
			cin.putback(ch);
			if (s==declkey) return Token(let);
			return Token{name,s};
		}
		error("Bad Token");
		return 1;
	}
}

void Token_stream::putback(Token t)
{
	if (full) error("putback() into a full buffer");
    buffer = t;         // copy t to buffer
    full = true;        // buffer is now full
}

void Token_stream::ignore(char c)
{
	if (full && c==buffer.kind)
	{
		full=false;
		return;
	}
	full=false;

	char ch;
	while( cin>>ch )
		if (ch==c) return;
}
Token_stream ts;
void clean_up_mess()
{
	ts.ignore(print);
}

class variable{
public:
	string name;
	double value;
	variable(string n, double v): name(n), value(v){}
};

vector<variable>var_table;

double get_value(string s)
{
	for (const variable& v: var_table)
		if (v.name==s) return v.value;
	error("get:undefined variable",s);
	return 1;
}

void set_value(string s, double d)
{
	for (variable& v: var_table)
		if(v.name==s)
		{
			v.value=d;
			return;
		}
		error("set:undefined variable",s);
}

bool is_declared(string var)
{
	for(const variable& v: var_table)
		if (v.name==var) return true;
	return false;
}

double define_name(string var, double val)
{
	if (is_declared(var)) error(var, "declared twice");
	var_table.push_back(variable(var, val));
	return val;
}

double primary()
{
	Token t=ts.get();
	switch (t.kind)
	{
		case '(':
		{
			double d = expression();
			t=ts.get();
			if (t.kind!=')') error("')' expected");
			return d;
		}
		case'-':
		return -primary();
		break;

		case number:
		return t.value;
		break;

		case name:
		return get_value(t.name);
		break;

		default:
		error("primary expected");
		return 1;
	}
}

double term()
{
	double left = primary();
	Token t = ts.get();
	while(true)
	{
		switch(t.kind)
		{
			case'*':
			left*=primary();
			t=ts.get();
			/*at this point program stalls and expects for an intput, if we provide ; after the desired input i.e.
			1+2; then it's okay with the input then proceeds to print the statement() and in the next loop the ;
			character will be consumed, but if any other character is entered then the output you will get will be
			messy enough to waste couple of your precious hours😂😂*/
			/*program stalls only when you are about to return the result to statement() function via expression*/
			break;

			case'/':
			{
				double d=primary();
				if (d==0) error("divide by zero");
				left/=d;
				t=ts.get();
			/*at this point program stalls and expects for an intput, if we provide ; after the desired input i.e.
			1+2; then it's okay with the input then proceeds to print the statement() and in the next loop the ;
			character will be consumed, but if any other character is entered then the output you will get will be
			messy enough to waste couple of your precious hours😂😂*/
			/*program stalls only when you are about to return the result to statement() function via expression*/
				break;
			}
			default:
			ts.putback(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	Token t= ts.get();
	while (true)
	{
		switch (t.kind)
		{
			case'+':
			left+=term();
			t=ts.get();
			/*at this point program stalls and expects for an intput, if we provide ; after the desired input i.e.
			1+2; then it's okay with the input then proceeds to print the statement() and in the next loop the ;
			character will be consumed, but if any other character is entered then the output you will get will be
			messy enough to waste couple of your precious hours😂😂(The program stalls only if you are about to 
			return the value to statement function via expression()*/
			break;

			case'-':
			left-=term();
			t=ts.get();
			/*at this point program stalls and expects for an intput, if we provide ; after the desired input i.e.
			1+2; then it's okay with the input then proceeds to print the statement() and in the next loop the ;
			character will be consumed, but if any other character is entered then the output you will get will be
			messy enough to waste couple of your precious hours😂😂(The program stalls only if you are about to 
			return the value to statement function via expression()*/
			break;
		
			default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind!=name) error("name expected in declaration");
	string name = t.name;
	Token t2= ts.get();
	if (t2.kind!='=') error("= is missing in declaration of name");
	double d = expression();
	return define_name(name,d);
}

double statement()
{
	Token t = ts.get();
	switch(t.kind)
	{
		case let:
		return declaration();

		default:
		ts.putback(t);
		return expression();
	}
}

const string prompt = ">";
const string result = "=";

void calculate()
{
	define_name("pi", 3.14159);
	define_name("e", 2.718281828);

	while (cin)
		try{
			cout << prompt;
			Token t= ts.get();
			while (t.kind==print) t=ts.get();
			if (t.kind==quit) return;
			ts.putback(t);
			cout<<result<<statement()<<endl;
			/*the statement() waits for the input of ; once input is obtained program will no longer stall at the 
			following and identical parts of the program which are in term() and expression() function
			case'+':
			left+=term();
			t=ts.get();
			break;
			and value will be returned by expression() to statement() and the extra; will be consumed by the 
			program in the next loop*/

		}
		catch(runtime_error& e)
		{
			cerr<<e.what()<<endl;
			clean_up_mess();
		}
}

int main()
try{
	calculate();
	return 0;
}

catch(exception& e)
	{
		cerr<<"exception: "<<e.what()<<endl;
		char c;
		keep_window_open("0-0");
		return 1;
	}
	catch(...)
	{
		cerr<<"exception \n";
		keep_window_open("0-0");
		return 2;
	}

