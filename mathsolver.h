#ifndef MATHSOLVER_H
#define MATHSOLVER_H

#include <QString>
#include <cmath>
#include <QDebug>

class MathSolver
{
public:
    static bool check(int taskNumber, const QString& variant, const QString& answer);

private:
    static bool checkTask32(const QString& variant, const QString& answer);
    static double calculateF(double x, double b);
    static bool parseVariant(const QString& variant, double& x, double& b);
};

#endif // MATHSOLVER_H
