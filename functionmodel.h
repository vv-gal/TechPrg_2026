#ifndef FUNCTIONMODEL_H
#define FUNCTIONMODEL_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QPointF>
#include <cmath>

class FunctionModel : public QObject
{
    Q_OBJECT
public:
    explicit FunctionModel(QObject *parent = nullptr);

    double calculate(double x, double a, double b) const;

    // НОВЫЙ МЕТОД ДЛЯ ГРАФИКА
    QVector<QPointF> generatePlotData(double a, double b,
                                      double xMin = -2.5,
                                      double xMax = 4.5,
                                      int points = 2000) const;

    struct FunctionInfo {
        QString domain;
        QString asymptotes;
        QString extrema;
        QString specialPoints;
    };

    FunctionInfo getFunctionInfo(double a, double b) const;
};

#endif
