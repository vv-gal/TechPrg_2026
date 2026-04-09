#ifndef FUNCTIONSOLVER_H
#define FUNCTIONSOLVER_H

#include <QString>
#include <QVector>
#include <cmath>

struct SolutionPoint {
    double x;
    double y;
    int piece;
    QString description;
};

class FunctionSolver
{
public:
    FunctionSolver(double a = 0.0, double b = 0.0);

    double evaluate(double x) const;
    int getPiece(double x) const;
    double solveBisection(double left, double right, double tolerance = 1e-7, int maxIter = 1000);
    QVector<SolutionPoint> findAllRoots();
    void setParameters(double a, double b);
    double getA() const { return m_a; }
    double getB() const { return m_b; }

private:
    double piece1(double x) const;  // ln(x+b)
    double piece2(double x) const;  // x³ - 3x + a
    double piece3(double x) const;  // 1/(x-2) - b
    int sign(double x) const;

    double m_a;
    double m_b;
};

#endif
