#include "functionmodel.h"
#include <cmath>
#include <QDebug>

FunctionModel::FunctionModel(QObject *parent) : QObject(parent) {}

double FunctionModel::calculate(double x, double a, double b) const
{
    // Проверка на корректность входных данных
    if (std::isnan(x) || std::isinf(x)) {
        return 0.0;
    }

    // Первая ветка: логарифмическая часть
    if (x > -1 && x < 0) {
        double arg = x + b;
        if (arg > 0) {
            double result = log(arg);
            if (std::isnan(result) || std::isinf(result)) {
                return 0.0;
            }
            return result;
        }
        return 0.0;  // Возвращаем 0 вместо NaN
    }
    // Вторая ветка: кубическая часть
    else if (x >= 0 && x <= 2) {
        double result = pow(x, 3) - 3 * x + a;
        if (std::isnan(result) || std::isinf(result)) {
            return 0.0;
        }
        return result;
    }
    // Третья ветка: дробная часть
    else if (x > 2) {
        if (fabs(x - 2) > 0.001) {  // Не приближаемся слишком близко к асимптоте
            double result = 1.0 / (x - 2) - b;
            // Ограничиваем значения для отображения
            if (result > 100) return 100;
            if (result < -100) return -100;
            if (std::isnan(result) || std::isinf(result)) {
                return 0.0;
            }
            return result;
        }
        return 0.0;
    }

    return 0.0;
}

QVector<QPointF> FunctionModel::generatePlotData(double a, double b,
                                                 double xMin, double xMax,
                                                 int points) const
{
    QVector<QPointF> data;
    data.reserve(points);

    double step = (xMax - xMin) / points;

    for (int i = 0; i <= points; ++i) {
        double x = xMin + i * step;
        double y = calculate(x, a, b);

        // Пропускаем точки разрыва (создаём разрыв в графике)
        // Проверяем особые точки
        bool isDiscontinuity = false;

        // Асимптота в x = -1
        if (fabs(x + 1) < 0.01) {
            isDiscontinuity = true;
        }
        // Асимптота в x = 2
        if (fabs(x - 2) < 0.01) {
            isDiscontinuity = true;
        }
        // Граница между ветками
        if (fabs(x - 0) < 0.005 || fabs(x - 2) < 0.005) {
            isDiscontinuity = true;
        }

        if (isDiscontinuity) {
            // Добавляем точку разрыва (NaN для разрыва линии)
            data.append(QPointF(x, NAN));
        } else {
            data.append(QPointF(x, y));
        }
    }

    return data;
}

FunctionModel::FunctionInfo FunctionModel::getFunctionInfo(double a, double b) const
{
    FunctionInfo info;
    info.domain = "x ∈ (-1, 0) ∪ [0, 2] ∪ [2, +∞)\nВертикальные асимптоты: x = -1, x = 2";

    info.asymptotes = QString("Вертикальные асимптоты: x = -1, x = 2\nГоризонтальная асимптота: y = -%1").arg(b);

    double maxY = pow(-1, 3) - 3*(-1) + a;
    double minY = pow(1, 3) - 3*1 + a;

    info.extrema = QString("Максимум: (-1, %1)\nМинимум: (1, %2)").arg(maxY).arg(minY);

    double f0 = calculate(0, a, b);
    info.specialPoints = QString("f(0) = %1\nРазрыв в x = 2").arg(f0);

    return info;
}
