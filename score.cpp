#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using std::cout;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;
using std::vector;

static const char *const filePath  = "score1.byte";/**/
size_t fileSize(fstream &file) {
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	return size;
};/*/
#include <sys/stat.h>
int fileSize(const char* dstFileName){    struct stat statbuf;    stat(dstFileName,&statbuf);    int size=statbuf.st_size; return size;}/**/
void clearFile(const char*const dstFileName = filePath) {
	ofstream ofs(dstFileName, std::ios::trunc);
	ofs.close();
}
typedef unsigned char uchar;
typedef signed char schar;
typedef uchar byte;
template <typename T>
inline const T read(byte *buffer) {
	T res = *(T*)buffer;
	buffer += sizeof(T);
	return res;
}
inline bool isdigit(char c) { return '0'<=c && c<='9'; }
schar readNum(char const*&s) {
	uchar i=0; schar a=0;
	while(isdigit(s[0]))
	{ a += s[0]-'0'; s++; }
	return a;
}/*
int strlen(char const*s) {
	int i = 0;
	while(!s[i]);
	return i;
}*/
struct Date {
	uchar month, day;
	Date(int a, int b) : month(a), day(b) {}
	friend bool operator==(Date const&a, Date const&b) {
		return a.month == b.month && a.day == b.day;
	}
	friend bool operator!=(Date const&a, Date const&b) {
		return a.month != b.month || a.day != b.day;
	}
	friend bool operator<(Date const&a, Date const&b) {
		return a.month < b.month || (a.month == b.month && a.day < b.day);
	}
	friend bool operator>(Date const&a, Date const&b) {
		return a.month > b.month || (a.month == b.month && a.day > b.day);
	}
	friend bool operator<=(Date const&a, Date const&b) {
		return a.month < b.month || (a.month == b.month && a.day <= b.day);
	}
	friend bool operator>=(Date const&a, Date const&b) {
		return a.month > b.month || (a.month == b.month && a.day >= b.day);
	}
	Date &operator=(Date const&a) {
		month = a.month;
		day = a.day;
		return *this;
	}
	friend std::ostream &operator<<(std::ostream &os, Date const&a) {
		os << (int)a.month << "/" << (int)a.day;
		return os;
	}
	friend Date operator+(Date const&a, Date const&b) {
		Date res = a;
		res.month += b.month;
		res.day += b.day;
		if(res.day > 31) {
			res.day -= 31;
			res.month++;
		}
		if(res.month > 12) {
			res.month -= 12;
		}
		return res;
	}
	friend Date operator-(Date const&a, Date const&b) {
		Date res = a;
		res.month -= b.month;
		res.day -= b.day;
		if(res.day < 1) {
			res.day += 31;
			res.month--;
		}
		if(res.month < 1) {
			res.month += 12;
		}
		return res;
	}
	friend Date &operator++(Date &a) {
		a.day++;
		if(a.day > 31) {
			a.day -= 31;
			a.month++;
		}
		if(a.month > 12) {
			a.month -= 12;
		}
		return a;
	}
	friend Date operator++(Date &a, int) {
		Date res = a;
		a.day++;
		if(a.day > 31) {
			a.day -= 31;
			a.month++;
		}
		if(a.month > 12) {
			a.month -= 12;
		}
		return res;
	}
	friend Date operator--(Date &a) {
		a.day--;
		if(a.day < 1) {
			a.day += 31;
			a.month--;
		}
		if(a.month < 1) {
			a.month += 12;
		}
		return a;
	}
	friend Date operator--(Date &a, int) {
		Date res = a;
		a.day--;
		if(a.day < 1) {
			a.day += 31;
			a.month--;
		}
		if(a.month < 1) {
			a.month += 12;
		}
		return res;
	}
	friend Date operator+(Date const&a, schar b) {
		Date res = a;
		res.day += b;
		if(res.day > 31) {
			res.day -= 31;
			res.month++;
		}
		if(res.month > 12) {
			res.month -= 12;
		}
		return res;
	}
	friend Date operator-(Date const&a, schar b) {
		Date res = a;
		res.day -= b;
		if(res.day < 1) {
			res.day += 31;
			res.month--;
		}
		if(res.month < 1) {
			res.month += 12;
		}
		return res;
	}
};
struct Student {
	schar final=0, cnt=0;
	struct Add {
		Add(Date date, string const&reson, schar add) : date(date), reson(reson), add(add) {}
		Date date;
		string reson;
		schar add;
	};
	vector<Add> score;
};
int main(int argc, char *argv[]) {
	string s;
	s += "Äã"[0];
	s += "Äã"[1];
	//cout << s; return 0;
	fstream file(filePath, std::ios::binary | std::ios::app);
	if(!file) {
		cout << "Error opening file\n";
		return -1;
	}
	if(argc == 1) {
		int len = fileSize(file);
		cout << "File size: " << len << " bytes\n";
		//if(len == 0) { cout << "empty\n"; return 0; }
		file.seekg(0, std::ios::beg);
		char *buffer = new char[len];
		file.read(buffer, len);
		for(int i = 0; i < len; i++) {
			cout << short(buffer[i]) << ' ';
		}
		if(len < 2) return 0;
		buffer += 2;
		int i=0;
		#define read() (i<len ? buffer[i++] : (i++,0))
		Student students[37];
		while(i < len) {
			Date date(read(), read()); read();
			while(true) {
				string s; char c = read();
				if((uchar)c == 0) break;
				if((uchar)c == 1) ;//reset
				do s += c;
				while(c = read());
				char id;
				while(id = read()) {
					students[id].score.push_back(
						Student::Add(date,s,(schar)read())
					);
				}
			}
		}
		#undef read
		delete[] buffer;
	} else {
		char const*arg = argv[1];
		if(arg[0] == 'c' && arg[1] == '\0') { clearFile(); return 0; }
		uchar a = readNum(arg);
		if(arg[0] == '.') { //Date 
			file << '\0' << '\0' << a << readNum(++arg);
		} else if(a == 0) {
			file << '\0' << argv[1] << '\0';
		} else {
			schar b;
			sscanf(arg, "%d", &b);
			file << a << b;
		}
	}
	file.close(); 
    return 0;
}




















