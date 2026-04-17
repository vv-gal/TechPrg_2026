/**
 * @file functionSolver.cpp
 * @brief Реализация класса FunctionSolver
 * @author Developer
 * @date 2026
 *
 * Содержит реализацию методов для решения кусочно-заданного уравнения f(x)=0
 * с использованием метода бисекции.
 */

#include "model/functionSolver.h"
#include <QDebug>
#include <float.h>

/**
 * @brief Конструктор класса FunctionSolver
 * @param a Параметр a функции
 * @param b Параметр b функции
 *
 * Инициализирует решатель с заданными параметрами a и b.
 */
FunctionSolver::FunctionSolver(double a, double b)
    : m_a(a), m_b(b)
{
}

/**
 * @brief Вычисление первой кусочной функции: ln(x + b)
 * @param x Аргумент функции
 * @return Значение функции или NaN, если x вне области определения
 *
 * Область определения: x + b > 0 => x > -b
 */
double FunctionSolver::piece1(double x) const
{
    // Область определения: x + b > 0 => x > -b
    if (x + m_b <= 0) {
        return NAN;
    }
    return log(x + m_b);
}

/**
 * @brief Вычисление второй кусочной функции: x³ - 3x + a
 * @param x Аргумент функции
 * @return Значение функции
 *
 * Область определения: все действительные числа
 */
double FunctionSolver::piece2(double x) const
{
    return pow(x, 3) - 3 * x + m_a;
}

/**
 * @brief Вычисление третьей кусочной функции: 1/(x-2) - b
 * @param x Аргумент функции
 * @return Значение функции или NaN, если x = 2
 *
 * Область определения: x ≠ 2
 */
double FunctionSolver::piece3(double x) const
{
    // Область определения: x != 2
    if (fabs(x - 2) < 1e-10) {
        return NAN;
    }
    return 1.0 / (x - 2) - m_b;
}

/**
 * @brief Определение номера куска для заданного x
 * @param x Аргумент функции
 * @return Номер куска:
 *         1 - для ln(x+b) на интервале (-b; 2)
 *         2 - для x³ - 3x + a (все остальные x)
 *         3 - для 1/(x-2) - b на интервале (2; +∞)
 */
int FunctionSolver::getPiece(double x) const
{
    // Определяем область определения для каждого куска
    if (x > -m_b && x < 2) {
        return 1;  // Первый кусок: ln(x + b)
    }
    else if (x > 2) {
        return 3;  // Третий кусок: 1/(x-2) - b
    }
    else {
        return 2;  // Второй кусок: x³ - 3x + a (определен везде)
    }
}

/**
 * @brief Вычисление значения функции f(x) с учетом кусочности
 * @param x Аргумент функции
 * @return Значение функции или NaN, если x вне области определения
 *
 * Автоматически выбирает нужный кусок функции в зависимости от x.
 */
double FunctionSolver::evaluate(double x) const
{
    int piece = getPiece(x);

    switch (piece) {
    case 1:
        return piece1(x);
    case 2:
        return piece2(x);
    case 3:
        return piece3(x);
    default:
        return NAN;
    }
}

/**
 * @brief Определение знака числа
 * @param x Число для анализа
 * @return 1 - положительное, -1 - отрицательное, 0 - ноль
 *
 * Использует эпсилон = 1e-10 для учета погрешности вычислений.
 */
int FunctionSolver::sign(double x) const
{
    if (x > 1e-10) return 1;
    if (x < -1e-10) return -1;
    return 0;
}

/**
 * @brief Проверка наличия корня на интервале
 * @param left Левая граница интервала
 * @param right Правая граница интервала
 * @return true - если функция меняет знак на интервале
 *
 * Необходимое условие для применения метода бисекции.
 */
bool FunctionSolver::hasRoot(double left, double right) const
{
    double fLeft = evaluate(left);
    double fRight = evaluate(right);

    if (std::isnan(fLeft) || std::isnan(fRight)) {
        return false;
    }

    return sign(fLeft) != sign(fRight);
}

/**
 * @brief Решение уравнения f(x)=0 методом бисекции
 * @param left Левая граница интервала
 * @param right Правая граница интервала
 * @param tolerance Точность решения (по умолчанию 1e-7)
 * @param maxIter Максимальное количество итераций (по умолчанию 1000)
 * @return Найденный корень или NaN, если корень не найден
 *
 * Метод последовательного деления интервала пополам.
 * Требует смены знака функции на интервале.
 */
double FunctionSolver::solveBisection(double left, double right,
                                      double tolerance, int maxIter)
{
    // Проверка на смену знака
    if (!hasRoot(left, right)) {
        qDebug() << "No sign change on interval [" << left << ", " << right << "]";
        return NAN;
    }

    double fLeft = evaluate(left);
    double fRight = evaluate(right);

    // Убеждаемся, что f(left) < 0, f(right) > 0
    if (fLeft > 0 && fRight < 0) {
        std::swap(left, right);
        std::swap(fLeft, fRight);
    }

    double mid;
    double fMid;

    for (int i = 0; i < maxIter; i++) {
        mid = (left + right) / 2.0;
        fMid = evaluate(mid);

        if (std::isnan(fMid)) {
            qDebug() << "NaN encountered at x = " << mid;
            return NAN;
        }

        // Проверка на достижение точности
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

    qDebug() << "Max iterations reached";
    return mid;
}

/**
 * @brief Поиск всех корней уравнения на всех интервалах
 * @return Вектор найденных корней
 *
 * Автоматически анализирует каждый кусок функции:
 * - Для 1-го куска: аналитическое решение x = 1 - b
 * - Для 2-го куска: численный поиск на интервалах [-10, -2], [-2, 0], [0, 2], [2, 10]
 * - Для 3-го куска: аналитическое решение x = 2 + 1/b (при b ≠ 0)
 */
QVector<SolutionPoint> FunctionSolver::findAllRoots()
{
    QVector<SolutionPoint> roots;

    qDebug() << "Searching for roots with a=" << m_a << ", b=" << m_b;

    // Поиск корней для каждого куска

    // Кусок 1: ln(x + b) = 0 => x + b = 1 => x = 1 - b
    // Область определения: x > -b и x < 2
    double root1 = 1 - m_b;
    if (root1 > -m_b && root1 < 2) {
        double y = evaluate(root1);
        if (fabs(y) < 1e-7) {
            SolutionPoint point;
            point.x = root1;
            point.y = y;
            point.piece = 1;
            point.description = QString("Root in piece 1 (ln(x+%1)): x = %2, f(x) = %3")
                                    .arg(m_b).arg(root1).arg(y);
            roots.append(point);
            qDebug() << point.description;
        }
    }

    // Кусок 2: x³ - 3x + a = 0
    // Ищем корни на интервалах: (-∞, -2), (-2, 0), (0, 2), (2, ∞)
    // Но ограничимся разумными пределами [-10, 10]
    QVector<QPair<double, double>> intervals2 = {
        qMakePair(-10.0, -2.0),
        qMakePair(-2.0, 0.0),
        qMakePair(0.0, 2.0),
        qMakePair(2.0, 10.0)
    };

    for (const auto& interval : intervals2) {
        double left = interval.first;
        double right = interval.second;

        // Проверяем, есть ли корень на интервале
        if (hasRoot(left, right)) {
            double x = solveBisection(left, right);
            if (!std::isnan(x)) {
                // Проверяем, не нашли ли мы уже этот корень
                bool duplicate = false;
                for (const auto& root : roots) {
                    if (fabs(root.x - x) < 1e-7) {
                        duplicate = true;
                        break;
                    }
                }

                if (!duplicate) {
                    double y = evaluate(x);
                    SolutionPoint point;
                    point.x = x;
                    point.y = y;
                    point.piece = 2;
                    point.description = QString("Root in piece 2 (x³ - 3x + %1): x = %2, f(x) = %3")
                                            .arg(m_a).arg(x).arg(y);
                    roots.append(point);
                    qDebug() << point.description;
                }
            }
        }
    }

    // Кусок 3: 1/(x-2) - b = 0 => 1/(x-2) = b => x-2 = 1/b => x = 2 + 1/b
    // Область определения: x > 2
    if (fabs(m_b) > 1e-10) {  // b != 0
        double root3 = 2 + 1.0 / m_b;
        if (root3 > 2) {
            double y = evaluate(root3);
            if (fabs(y) < 1e-7) {
                SolutionPoint point;
                point.x = root3;
                point.y = y;
                point.piece = 3;
                point.description = QString("Root in piece 3 (1/(x-2) - %1): x = %2, f(x) = %3")
                                        .arg(m_b).arg(root3).arg(y);
                roots.append(point);
                qDebug() << point.description;
            }
        }
    } else if (fabs(m_b) < 1e-10) {
        // Если b = 0, то уравнение 1/(x-2) = 0 не имеет решений
        qDebug() << "Piece 3 has no roots when b = 0";
    }

    qDebug() << "Total roots found:" << roots.size();
    return roots;
}

/**
 * @brief Установка новых параметров a и b
 * @param a Новый параметр a
 * @param b Новый параметр b
 *
 * Позволяет изменить параметры без создания нового объекта.
 */
void FunctionSolver::setParameters(double a, double b)
{
    m_a = a;
    m_b = b;
    qDebug() << "Parameters updated: a=" << m_a << ", b=" << m_b;
}

/**
 * @brief Генерация точек для построения графика
 * @param piece Номер куска функции (1, 2 или 3)
 * @param xStart Начало интервала
 * @param xEnd Конец интервала
 * @param points Количество точек для генерации
 * @return Вектор пар (x, f(x)) для построения графика
 *
 * Генерирует равномерно распределенные точки для визуализации функции.
 * Точки вне области определения пропускаются.
 */
QVector<QPair<double, double>> FunctionSolver::generateGraphPoints(int piece,
                                                                   double xStart,
                                                                   double xEnd,
                                                                   int points)
{
    QVector<QPair<double, double>> graphPoints;

    if (points <= 0) return graphPoints;

    double step = (xEnd - xStart) / points;

    for (int i = 0; i <= points; i++) {
        double x = xStart + i * step;
        double y = 0;

        switch (piece) {
        case 1:
            if (x > -m_b && x < 2) {
                y = piece1(x);
            } else {
                continue;  // Пропускаем точки вне области определения
            }
            break;
        case 2:
            y = piece2(x);
            break;
        case 3:
            if (x > 2) {
                y = piece3(x);
            } else {
                continue;  // Пропускаем точки вне области определения
            }
            break;
        default:
            continue;
        }

        if (!std::isnan(y) && std::isfinite(y)) {
            graphPoints.append(qMakePair(x, y));
        }
    }

    return graphPoints;
}
