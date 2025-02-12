#ifndef _RANDOM_H_
#define _RANDOM_H_
#define _RANDOM_LEVEL_ 1

#include <random>
#include <chrono>

/*
random() - between 0.0 and 1.0
random(max) - between 0.0 and max
random(min, max) - returns a random float between min and max
*/

#ifndef _RANDOM_SEED_TYPE_
#define _RANDOM_SEED_TYPE_ 0
#endif/*
#if _RANDOM_SEED_TYPE_ < 0
#error "Random seed must be greater than or equal to 0"
#endif//*/
#define _RANDOM_SEED_TYPE_MAX_ 3
#if _RANDOM_SEED_TYPE_ > _RANDOM_SEED_TYPE_MAX_
#undef _RANDOM_SEED_TYPE_
#define _RANDOM_SEED_TYPE_ _RANDOM_SEED_TYPE_MAX_
#endif
#undef _RANDOM_SEED_TYPE_MAX_
namespace Rand {
#if _RANDOM_SEED_TYPE_ < 0
    unsigned int seed() { return -_RANDOM_SEED_TYPE_; }
#elif _RANDOM_SEED_TYPE_ == 0
    unsigned int seed() {
        return std::random_device()();
    }
#elif _RANDOM_SEED_TYPE_ == 1
    #ifndef _RANDOM_SEED_STRING_
    #define _RANDOM_SEED_STRING_ "std::random_device"
    #endif
    unsigned int seed() {
        return std::random_device(_RANDOM_SEED_STRING_)();
    }
#elif _RANDOM_SEED_TYPE_ == 2
    unsigned int seed() {
        return std::chrono::system_clock::now().time_since_epoch().count();
    }
#elif _RANDOM_SEED_TYPE_ == 3
    unsigned int seed() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
#endif
}

#ifndef _RANDOM_LEVEL_
#define _RANDOM_LEVEL_ 0
#endif
#if _RANDOM_LEVEL_ < 0
#error "Random level must be greater than or equal to 0"
#endif
#define _RANDOM_LEVEL_MAX_ 1
#if _RANDOM_LEVEL_ > _RANDOM_LEVEL_MAX_
#undef _RANDOM_LEVEL_
#define _RANDOM_LEVEL_ _RANDOM_LEVEL_MAX_
#endif
#undef _RANDOM_LEVEL_MAX_
namespace Rand {
#if _RANDOM_LEVEL_ == 0
    const unsinged long long int max = 0x7FFF, min = 0x0000;
    #define _SET_SEED_
        void setSeed(unsigned int seed) {
            srand(seed);
        }
    #define _RAND_
        float random() {
            return (float)rand() / RAND_MAX;
        }
    #define _RAND_RUN_
        void run() { rand(); }
#elif _RANDOM_LEVEL_ == 1
    std::mt19937_64 engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    const unsigned long long int max = engine.max(), min = engine.min();
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    #define _SET_SEED_
        void setSeed(unsigned int seed) {
            engine.seed(seed);
        }
    #define _RAND_
        float random() {
            return dist(engine);
        }
    #define _RAND_RUN_
        void run() { engine(); }
    #define _RAND_DISCARD_
        void discard(unsigned long long n) {
            engine.discard(n);
        }
#endif
}

namespace Rand {
    #ifndef _RAND_
        #error "Random base function not defined"
    #endif
    #ifndef _RAND_MAX_
    #define _RAND_MAX_
        template <typename T>
        T random(T max) {
            return T(T(random()) * T(max));
        }
    #endif
    #ifndef _RAND_RUN_
        void run() { random(); }
    #endif
    #ifndef _RAND_MIN_MAX_
    #define _RAND_MIN_MAX_
        template <typename T>
        T random(T min, T max) {
            return T(T(T(random()) * T(max - min)) + T(min));
        }
    #endif
    #ifndef _RAND_DISCARD_
    #define _RAND_DISCARD_
        template <typename T = unsigned long long, T step = 1>
        void discard(T n) {
            for(T i = 0; i < n; i++) {
                random();
            }
        }
    #endif
    static struct __SET_SEED { __SET_SEED() { setSeed(seed()); } } __set_seed;
}

#endif // _RANDOM_H_