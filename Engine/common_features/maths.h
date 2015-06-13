#ifndef MATHS_H
#define MATHS_H

class Maths
{
public:
    static long roundTo(long src, long grid);
    static double roundTo(double src, double grid);

    template <typename T>
    static int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }
};

#endif // MATHS_H
