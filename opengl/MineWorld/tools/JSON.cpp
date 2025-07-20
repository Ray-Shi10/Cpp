#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#define map unordered_map
#include <vector>

#define _JSON_REAL_FLOAT 1
#define _JSON_REAL_DOUBLE 2
#define _JSON_REAL_LONG_DOUBLE 3
#ifndef _JSON_REAL_DEFAULT
#define _JSON_REAL_DEFAULT _JSON_REAL_DOUBLE
#endif
#ifndef _JSON_REAL_OPT
#define _JSON_REAL_OPT _JSON_REAL_DOUBLE
#endif

namespace ____JSON {
    #define __JSON__ 1
    using std::string;
    using std::vector;
    class JSON {
    private:
        enum class Type : unsigned char {
            Undefined,
            Object,
            Array,
            Integer,
            Real,
            String,
            Boolean
        } type = Type::Undefined;
    public:
        union {
            class {
            private:
                using attMap_t = std::map<string, JSON>;
                attMap_t jsonMap;
            public:
                attMap_t::iterator begin() { return jsonMap.begin(); }
                attMap_t::iterator end() { return jsonMap.end(); }
                attMap_t::const_iterator begin() const { return begin(); }
                attMap_t::const_iterator end() const { return end(); }
                JSON& operator[](const string& key) {
                    attMap_t::iterator it = jsonMap.find(key);
                    if (it == jsonMap.end()) {
                        throw std::out_of_range("Key not found in JSON object.");
                    }
                    return it->second;
                }
                const JSON& operator[](const string& key) const {
                    attMap_t::const_iterator it = jsonMap.find(key);
                    if (it == jsonMap.end()) {
                        throw std::out_of_range("Key not found in JSON object.");
                    }
                    return it->second;
                }
                auto insert(const string &key, const JSON &value) {
                    //return jsonMap.insert(key, value);
                }
            } Object;
            vector<JSON> Array;
            long long Integer;
            #if _JSON_REAL_OPT == _JSON_REAL_FLOAT
            float Real;
            #elif _JSON_REAL_OPT == _JSON_REAL_LONG_DOUBLE
            long double Real;
            #elif _JSON_REAL_OPT == _JSON_REAL_DOUBLE
            double Real;
            #else
            #error Please define _JSON_REAL_OPT to one of the following: _JSON_REAL_FLOAT, _JSON_REAL_DOUBLE, or _JSON_REAL_LONG_DOUBLE.
            #endif
            string String;
            bool Boolean;
        };
        Type getType() const { return type; }
        JSON() : type(Type::Undefined) {}
    };
}; using namespace ____JSON;

#include "console.h"
using namespace tools;

int main() {
    //console::Stream stream;
    fprintf(stdout, "Hello, World!\n");
    return 0;
}