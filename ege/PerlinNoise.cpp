#include <iostream>
#include <functional>
#include <string>
#include <ege>
using string;
//using size_t;
constexpr int width = 40, height = 30;
constexpr int size = 20;
struct Vec2 {
	float x, y;
};
using _Hash_impl::hash;
template <typename ...Args>
size_t rand(Args ...args) {
	size_t seed = 0x114514abc;
	()
}
size_t seed = hash("seed");
int main() {
	std::srand(seed);
	ege::initgraph(width*size, height*size);
	Vec2 grad[width+1][height+1];
	for(int x=0; x<=width; x++) {
		for(int y=0; y<=height; y++) {
			grad[x][y] = Vec2(rand(seed, x, y), rand(y, seed, x));
		}
	}
	return 0;
}
