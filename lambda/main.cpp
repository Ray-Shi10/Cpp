#include <iostream>
#include <string>
#include <vector>
#include <stack>
using std::cout;
using std::string;
using std::vector;
using std::stack;

class Node {
 public:
  Node *left;
  Node *right;
  Node() : left(nullptr), right(nullptr) {}
};
class Token {
 public:
  enum Type {
    Func,
    LBraket,
    FnArg,
    Value,
  } type;
  string val;
  Token(Type type) : type(type) {}
  Token(Type type, string val) : type(type), val(val) {}
  static string toStr(Type type) {
    #define CASE(type) case type: return #type;
    switch(type) {
      CASE(Func) CASE(LBraket) CASE(FnArg) CASE(Value)
      default: return "Unkown";
    }
  }
  friend std::ostream &operator<<(std::ostream &os, Token tk) {
    return os << toStr(tk.type) << "(" << tk.val << ")";
  }
};

bool compile(string &source, bool inFunc=0, string value="", string key="") {
  // while(source.size()>1 && source[0]=='(' && source[source.size()-1]==')') source = source.substr(1,source.size()-2);
  // source = "(" + source + ")";
  if(source.size() <= 1) {
    if(source.size() && source==key) {
      return source=value, 1;
    }
    return 0;
  }
  if(source[1] == '.') {
    const string arg = {source[0]};
    if(!key.size() && value.size()) {
      key = arg;
      source = source.substr(2);
      return compile(source, 1, value, key);
    } else if(key==arg) {
      value="", key="";
    }
    source = source.substr(2);
    compile(source, 1, value, key);
    source = arg+"."+source;
    return 1;
  }
  int braket = 0;
  vector<string> stk;
  string str;
  for(const auto &c : source) {
    if(c == '(') {
      if(!braket) {
        if(str.size()) stk.push_back(str);
        str = "";
        braket++;
        continue;
      }
      braket++;
    } else if(c == ')') {
      braket--;
      if(!braket) {
        if(str.size()) stk.push_back(str);
        str = "";
        continue;
      }
    }
    str += c;
  }
  if(!braket && str.size()) {
    stk.push_back(str);
    str = "";
  }
  if(source.find('(') == source.npos) {
    stk.clear();
    for(const auto c : source) {
      if(key == string{c}) {
        if(!value.size()) {
          value = "";
        }
        stk.push_back(value);
      }
      else stk.push_back({c});
    }
  }
  bool change = 0;
  source = "";
  int i=0;
  if(!inFunc && stk.size()>1 && stk[0][1]=='.') {
    string &str = stk[0];
    // {
    //   int i=0;
    //   while(str[i] == '(') i++;
    //   if(str[i+1] != '.') exit(0);
    // }
    change |= compile(str, 1, stk[1]);
    // cout << "Call: " << str << "   (" << stk[1] << ")\n";
    if(str[0]!='(' || str[str.size()-1]!=')')
      str = "("+str+")";
    source = str;
    i = 2;
  }
  for(; i<stk.size(); i++) {
    string &str = stk[i];
    if(!str.size()) continue;
    change |= compile(str, inFunc|1, value, key);
    if(i && (str[0]!='(' || str[str.size()-1]!=')'))
    {
      str = "("+str+")";
    }
    source += str;
    if(i) source = "("+source+")";
  }
  return change;
}
int main() {
  string source = ([](string source) -> string {
    string res;
    for(const auto c : source) {
      if(false) { }
      else if(c == ',') {
        res += ")(";
      }
      else if('0'<=c && c<='9') {
        int n = c-'0';
        res += "(f.x.";
        while(n--) res+='f';
        res += "x)";
      }
      else {
        res += c;
      }
    }
    return res;
  })
  // ("(x.y.x(yy)x)(x.x,x.xx)");
  // ("(x.x)((x.(x)(x))(x.(x)(x)))(x.x)");
  // ("3(f)(xx)");
  ("(m.n.f.x.nf(mfx))(2)(3)");
  ("ff");
  string _s;
  do { cout << source << "\n"; } while(_s=source,compile(source),source!=_s);
  // cout << source << "\n";
  // compile(source);
  // cout << source << "\n";
  return 0;
}