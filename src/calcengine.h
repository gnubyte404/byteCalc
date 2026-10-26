#ifndef CALCULATORENGINE_H
#define CALCULATORENGINE_H

#include <QString>

class CalculatorEngine
{
public:
    static double evaluate(const QString &expression);
};

#endif // CALCULATORENGINE_H
