#include "model/functionSolver.h"
#include <QDebug>
#include <float.h>

FunctionSolver::FunctionSolver(double a, double b) : m_a(a), m_b(b) {}

double FunctionSolver::piece1(double x) const
{
    double arg = x + m_b;
    if (arg <= 0) return DBL_MAX;
    return log(arg);
}

double FunctionSolver::piece2(double x) const
{
    return x * x * x - 3.0 * x + m_a;
}

double FunctionSolver::piece3(double x) const
{
    if (fabs(x - 2.0) < 1e-10) return DBL_MAX;
    return 1.0 / (x - 2.0) - m_b;
}

double FunctionSolver::evaluate(double x) const
{
    if (x > -1.0 && x < 0.0) return piece1(x);
    else if (x >= 0.0 && x <= 2.0) return piece2(x);
    else if (x > 2.0) return piece3(x);
    return DBL_MAX;
}

int FunctionSolver::getPiece(double x) const
{
    if (x > -1.0 && x < 0.0) return 1;
    if (x >= 0.0 && x <= 2.0) return 2;
    if (x > 2.0) return 3;
    return -1;
}

int FunctionSolver::sign(double x) const
{
    if (x > 1e-12) return 1;
    if (x < -1e-12) return -1;
    return 0;
}

double FunctionSolver::solveBisection(double left, double right, double tolerance, int maxIter)
{
    double fLeft = evaluate(left);
    double fRight = evaluate(right);

    if (sign(fLeft) == sign(fRight)) return DBL_MAX;

    double mid = left;
    for (int i = 0; i < maxIter; i++) {
        mid = (left + right) / 2.0;
        double fMid = evaluate(mid);

        if (fabs(fMid) < tolerance || (right - left) / 2.0 < tolerance) {
            return mid;
        }

        if (sign(fMid) == sign(fLeft)) {
            left = mid;
            fLeft = fMid;
        } else {
            right = mid;
            fRight = fMid;
        }
    }
    return mid;
}

QVector<SolutionPoint> FunctionSolver::findAllRoots()
{
    QVector<SolutionPoint> roots;

    // Поиск на (-1, 0)
    double left1 = -1.0 + 1e-10;
    double right1 = 0.0 - 1e-10;

    if (left1 + m_b > 0) {
        if (sign(evaluate(left1)) != sign(evaluate(right1))) {
            double root = solveBisection(left1, right1);
            if (root != DBL_MAX) {
                SolutionPoint p;
                p.x = root;
                p.y = evaluate(root);
                p.piece = 1;
                p.description = QString("Корень в куске 1 (логарифм): x = %1, f(x)=%2").arg(root).arg(p.y);
                roots.append(p);
            }
        }
    }

    // Поиск на [0, 2]
    double left2 = 0.0;
    double right2 = 2.0;
    double fLeft2 = evaluate(left2);
    double fRight2 = evaluate(right2);

    if (fabs(fLeft2) < 1e-10) {
        SolutionPoint p;
        p.x = left2;
        p.y = fLeft2;
        p.piece = 2;
        p.description = QString("Корень в точке x=0: f(x)=%1").arg(p.y);
        roots.append(p);
    } else if (fabs(fRight2) < 1e-10) {
        SolutionPoint p;
        p.x = right2;
        p.y = fRight2;
        p.piece = 2;
        p.description = QString("Корень в точке x=2: f(x)=%1").arg(p.y);
        roots.append(p);
    } else if (sign(fLeft2) != sign(fRight2)) {
        double root = solveBisection(left2, right2);
        if (root != DBL_MAX) {
            SolutionPoint p;
            p.x = root;
            p.y = evaluate(root);
            p.piece = 2;
            p.description = QString("Корень в куске 2 (кубический): x = %1, f(x)=%2").arg(root).arg(p.y);
            roots.append(p);
        }
    }

    // Поиск на (2, +∞)
    double left3 = 2.0 + 1e-10;
    double right3 = 10.0;

    while (sign(evaluate(left3)) == sign(evaluate(right3)) && right3 < 1000) {
        right3 *= 2;
    }

    if (sign(evaluate(left3)) != sign(evaluate(right3))) {
        double root = solveBisection(left3, right3);
        if (root != DBL_MAX) {
            SolutionPoint p;
            p.x = root;
            p.y = evaluate(root);
            p.piece = 3;
            p.description = QString("Корень в куске 3 (гипербола): x = %1, f(x)=%2").arg(root).arg(p.y);
            roots.append(p);
        }
    }

    return roots;
}

void FunctionSolver::setParameters(double a, double b)
{
    m_a = a;
    m_b = b;
}
