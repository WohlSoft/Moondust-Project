#ifndef MATHS_H
#define MATHS_H
#include <type_traits>

class Maths
{
public:
    static long roundTo(long src, long grid);
    static double roundTo(double src, double grid);

    template <typename T>
    static T max(T n1, T n2) {
        static_assert(std::is_arithmetic<T>::value, "The value for \"max\" must be arithemtic");
        return (n1>n2) ? n1 : n2;
    }

    template <typename T>
    static T min(T n1, T n2) {
        static_assert(std::is_arithmetic<T>::value, "The value for \"min\" must be arithemtic");
        return (n1<n2) ? n1 : n2;
    }

    template <typename T>
    static int sgn(T val) {
        static_assert(std::is_arithmetic<T>::value, "The value for \"val\" must be arithemtic");
        return (T(0) < val) - (val < T(0));
    }
};

#endif // MATHS_H
