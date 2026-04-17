#ifndef FUNCTIONSOLVER_H
#define FUNCTIONSOLVER_H

#include <QString>
#include <QVector>
#include <cmath>

/**
 * @brief Структура для хранения найденного корня уравнения
 */
struct SolutionPoint {
    double x;               ///< Координата x корня
    double y;               ///< Значение функции в точке (должно быть 0)
    int piece;              ///< Номер куска функции (1, 2 или 3)
    QString description;    ///< Текстовое описание корня
};

/**
 * @brief Класс для решения кусочно-заданного уравнения f(x)=0
 *
 * Функция определена на трех интервалах:
 * 1. (-b; 2)        : f(x) = ln(x + b)
 * 2. (-∞; +∞)       : f(x) = x³ - 3x + a
 * 3. (2; +∞)        : f(x) = 1/(x-2) - b
 *
 * Параметры a и b могут изменяться
 */
class FunctionSolver
{
public:
    /**
     * @brief Конструктор
     * @param a параметр a
     * @param b параметр b
     */
    FunctionSolver(double a = 0.0, double b = 0.0);

    /**
     * @brief Вычисление значения функции f(x) с учетом кусочности
     * @param x аргумент
     * @return значение функции
     */
    double evaluate(double x) const;

    /**
     * @brief Определение номера куска для заданного x
     * @param x аргумент
     * @return номер куска (1, 2 или 3), 0 если x вне области определения
     */
    int getPiece(double x) const;

    /**
     * @brief Решение уравнения f(x)=0 методом бисекции на интервале
     * @param left левая граница
     * @param right правая граница
     * @param tolerance точность
     * @param maxIter максимальное число итераций
     * @return найденный корень, NaN если корень не найден
     */
    double solveBisection(double left, double right,
                          double tolerance = 1e-7, int maxIter = 1000);

    /**
     * @brief Поиск всех корней уравнения на всех интервалах
     * @return вектор найденных корней
     */
    QVector<SolutionPoint> findAllRoots();

    /**
     * @brief Установка новых параметров a и b
     * @param a новый параметр a
     * @param b новый параметр b
     */
    void setParameters(double a, double b);

    /**
     * @brief Получение значения параметра a
     */
    double getA() const { return m_a; }

    /**
     * @brief Получение значения параметра b
     */
    double getB() const { return m_b; }

    /**
     * @brief Генерация точек для построения графика
     * @param piece номер куска (1, 2 или 3)
     * @param xStart начало интервала
     * @param xEnd конец интервала
     * @param points количество точек
     * @return вектор пар (x, f(x))
     */
    QVector<QPair<double, double>> generateGraphPoints(int piece,
                                                       double xStart,
                                                       double xEnd,
                                                       int points = 1000);

private:
    /**
     * @brief Первая кусочная функция: ln(x + b)
     * @param x аргумент (должен быть > -b)
     */
    double piece1(double x) const;

    /**
     * @brief Вторая кусочная функция: x³ - 3x + a
     * @param x аргумент
     */
    double piece2(double x) const;

    /**
     * @brief Третья кусочная функция: 1/(x-2) - b
     * @param x аргумент (x != 2)
     */
    double piece3(double x) const;

    /**
     * @brief Определение знака числа
     * @param x число
     * @return -1, 0 или 1
     */
    int sign(double x) const;

    /**
     * @brief Проверка смены знака на интервале
     * @param left левая граница
     * @param right правая граница
     * @return true если знаки разные
     */
    bool hasRoot(double left, double right) const;

    double m_a;  ///< Параметр a
    double m_b;  ///< Параметр b
};

#endif // FUNCTIONSOLVER_H
