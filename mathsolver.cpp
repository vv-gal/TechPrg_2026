#include "mathsolver.h"

bool MathSolver::check(int taskNumber, const QString& variant, const QString& answer)
{
    switch (taskNumber) {
    case 32:
        return checkTask32(variant, answer);
    default:
        qDebug() << "Unknown task number:" << taskNumber;
        return false;
    }
}

bool MathSolver::parseVariant(const QString& variant, double& x, double& b)
{
    // Поддерживаемые форматы:
    // 1. "x=-0.5;b=2"
    // 2. "-0.5;2"
    // 3. "-0.5&2"

    QString v = variant.trimmed();

    // Формат 1: x=...;b=...
    if (v.contains("x=") && v.contains("b=")) {
        QStringList parts = v.split(';');
        for (const QString& part : parts) {
            if (part.contains("x=")) {
                x = part.split('=')[1].toDouble();
            } else if (part.contains("b=")) {
                b = part.split('=')[1].toDouble();
            }
        }
        return true;
    }

    // Формат 2 и 3: разделитель ; или &
    QChar separator = v.contains(';') ? ';' : '&';
    QStringList parts = v.split(separator);

    if (parts.size() >= 2) {
        x = parts[0].toDouble();
        b = parts[1].toDouble();
        return true;
    }

    qDebug() << "Failed to parse variant:" << variant;
    return false;
}

double MathSolver::calculateF(double x, double b)
{
    // Проверка области определения
    if (x <= -1) {
        return NAN;  // x <= -1: первый кусок не определён
    }

    if (x > -1 && x < 0) {
        // Первый кусок: ln(x + b)
        double arg = x + b;
        if (arg <= 0) {
            return NAN;  // Логарифм от неположительного числа
        }
        return std::log(arg);
    }
    else if (x >= 0 && x <= 2) {
        // Второй кусок: x^3 - 3x
        return x * x * x - 3 * x;
    }
    else if (x > 2) {
        // Третий кусок: 1/(x-2)
        return 1.0 / (x - 2);
    }
    else {  // x == 2
        return NAN;  // В точке x=2 разрыв
    }
}

bool MathSolver::checkTask32(const QString& variant, const QString& answer)
{
    double x, b;
    if (!parseVariant(variant, x, b)) {
        qDebug() << "Invalid variant format";
        return false;
    }

    bool answerOk = false;
    double userAnswer = answer.toDouble(&answerOk);

    if (!answerOk) {
        qDebug() << "Invalid answer format";
        return false;
    }

    double correctAnswer = calculateF(x, b);

    // Проверка на особые случаи (NAN)
    if (std::isnan(correctAnswer)) {
        qDebug() << "Function undefined for x=" << x << ", b=" << b;
        return false;
    }

    bool isCorrect = std::abs(userAnswer - correctAnswer) < 1e-6;

    qDebug() << "Task32: x=" << x << " b=" << b
             << " user=" << userAnswer
             << " correct=" << correctAnswer
             << " result=" << (isCorrect ? "OK" : "FAIL");

    return isCorrect;
}
