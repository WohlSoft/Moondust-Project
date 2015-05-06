#ifndef MATHS_H
#define MATHS_H

class Maths
{
public:
    Maths();
    ~Maths();
    static long roundTo(long src, long grid);
    static double roundTo(double src, double grid);
};

#endif // MATHS_H
