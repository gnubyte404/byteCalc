
#include "calcengine.h"

#include <QChar>
#include <QtGlobal>

#include <cmath>
#include <stdexcept>

namespace {

class Parser
{
public:
    explicit Parser(const QString &expression)
        : expression_(expression)
    {
    }

    double parse()
    {
        skipSpaces();

        if (expression_.isEmpty()) {
            throw std::runtime_error("Empty expression");
        }

        const double result = parseExpression();

        skipSpaces();

        if (position_ != expression_.size()) {
            throw std::runtime_error("Invalid expression");
        }

        if (!std::isfinite(result)) {
            throw std::runtime_error("Result is not finite");
        }

        return result;
    }

private:
    QString expression_;
    qsizetype position_ = 0;

    void skipSpaces()
    {
        while (position_ < expression_.size()
               && expression_[position_].isSpace()) {
            ++position_;
        }
    }

    bool consume(QChar character)
    {
        skipSpaces();

        if (position_ < expression_.size()
            && expression_[position_] == character) {
            ++position_;
            return true;
        }

        return false;
    }

    double parseExpression()
    {
        double value = parseTerm();

        while (true) {
            if (consume('+')) {
                value += parseTerm();
            } else if (consume('-')) {
                value -= parseTerm();
            } else {
                break;
            }
        }

        return value;
    }

    double parseTerm()
    {
        double value = parseUnary();

        while (true) {
            if (consume('*')) {
                value *= parseUnary();
            } else if (consume('/')) {
                const double divisor = parseUnary();

                if (divisor == 0.0) {
                    throw std::runtime_error("Division by zero");
                }

                value /= divisor;
            } else {
                break;
            }
        }

        return value;
    }

    double parseUnary()
    {
        if (consume('+')) {
            return parseUnary();
        }

        if (consume('-')) {
            return -parseUnary();
        }

        return parsePostfix();
    }

    double parsePostfix()
    {
        double value = parsePrimary();

        while (consume('%')) {
            value /= 100.0;
        }

        return value;
    }

    double parsePrimary()
    {
        skipSpaces();

        if (consume('(')) {
            const double value = parseExpression();

            if (!consume(')')) {
                throw std::runtime_error("Missing closing parenthesis");
            }

            return value;
        }

        return parseNumber();
    }

    double parseNumber()
    {
        skipSpaces();

        const qsizetype start = position_;
        bool hasDigit = false;
        bool hasDecimalPoint = false;

        while (position_ < expression_.size()) {
            const QChar character = expression_[position_];

            if (character.isDigit()) {
                hasDigit = true;
                ++position_;
            } else if (character == '.' && !hasDecimalPoint) {
                hasDecimalPoint = true;
                ++position_;
            } else {
                break;
            }
        }

        if (!hasDigit) {
            throw std::runtime_error("Expected a number");
        }

        bool ok = false;
        const double value =
            expression_.mid(start, position_ - start).toDouble(&ok);

        if (!ok || !std::isfinite(value)) {
            throw std::runtime_error("Invalid number");
        }

        return value;
    }
};

} // namespace

double CalculatorEngine::evaluate(const QString &expression)
{
    Parser parser(expression);
    return parser.parse();
}
