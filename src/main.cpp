
#include "calcengine.h"

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <QString>
#include <stdexcept>

class CalculatorWindow : public QMainWindow
{
public:
    CalculatorWindow()
    {
        setWindowTitle("byteCalc");
        setMinimumSize(320, 460);
        resize(360, 520);

        auto *central = new QWidget(this);
        auto *mainLayout = new QVBoxLayout(central);

        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(10);

        expressionDisplay = new QLabel("", central);
        expressionDisplay->setAlignment(
            Qt::AlignRight | Qt::AlignBottom
        );
        expressionDisplay->setMinimumHeight(30);
        expressionDisplay->setStyleSheet(
            "color: #9A9EAA; background: transparent;"
        );

        display = new QLabel("0", central);
        display->setAlignment(
            Qt::AlignRight | Qt::AlignVCenter
        );
        display->setMinimumHeight(75);

        QFont displayFont = display->font();
        displayFont.setPointSize(34);
        display->setFont(displayFont);

        mainLayout->addWidget(expressionDisplay);
        mainLayout->addWidget(display);

        auto *grid = new QGridLayout();
        grid->setSpacing(8);

        const QString buttons[5][4] = {
            {"AC", "±", "%", "÷"},
            {"7",  "8", "9", "×"},
            {"4",  "5", "6", "−"},
            {"1",  "2", "3", "+"},
            {"⌫",  "0", ".", "="}
        };

        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col < 4; ++col) {
                const QString text = buttons[row][col];
                auto *button = new QPushButton(text, central);

                button->setMinimumSize(55, 55);
                button->setFocusPolicy(Qt::NoFocus);
                button->setCursor(Qt::PointingHandCursor);

                QFont buttonFont = button->font();
                buttonFont.setPointSize(17);
                button->setFont(buttonFont);

                const bool isOperator =
                    text == "÷" || text == "×" ||
                    text == "−" || text == "+" ||
                    text == "=";

                const bool isUtility =
                    text == "AC" || text == "±" ||
                    text == "%" || text == "⌫";

                QString background = "#272A31";
                QString foreground = "#FFFFFF";

                if (isOperator) {
                    background = "#D99A51";
                    foreground = "#17191E";
                } else if (isUtility) {
                    background = "#393D46";
                }

                button->setStyleSheet(
                    "QPushButton {"
                    " background-color: " + background + ";"
                    " color: " + foreground + ";"
                    " border: none;"
                    " border-radius: 12px;"
                    " padding: 8px;"
                    "}"
                    "QPushButton:hover {"
                    " background-color: #555B67;"
                    "}"
                    "QPushButton:pressed {"
                    " background-color: #747B89;"
                    "}"
                );

                connect(button, &QPushButton::clicked,
                        this, [this, text]() {
                    handleButton(text);
                });

                grid->addWidget(button, row, col);
            }
        }

        mainLayout->addLayout(grid);
        setCentralWidget(central);

        setStyleSheet(
            "QMainWindow, QWidget {"
            " background-color: #17191E;"
            " color: #FFFFFF;"
            "}"
            "QLabel { background: transparent; }"
        );

        central->setFocusPolicy(Qt::StrongFocus);
        central->setFocus();
    }

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        const int key = event->key();

        if (key >= Qt::Key_0 && key <= Qt::Key_9) {
            handleButton(
                QString::number(key - Qt::Key_0)
            );
        } else if (key >= Qt::Key_A && key <= Qt::Key_F) {
            QMainWindow::keyPressEvent(event);
            return;
        } else {
            switch (key) {
            case Qt::Key_Plus:
                handleButton("+");
                break;
            case Qt::Key_Minus:
                handleButton("−");
                break;
            case Qt::Key_Asterisk:
                handleButton("×");
                break;
            case Qt::Key_Slash:
                handleButton("÷");
                break;
            case Qt::Key_Percent:
                handleButton("%");
                break;
            case Qt::Key_Period:
            case Qt::Key_Comma:
                handleButton(".");
                break;
            case Qt::Key_Return:
            case Qt::Key_Enter:
            case Qt::Key_Equal:
                handleButton("=");
                break;
            case Qt::Key_Backspace:
                handleButton("⌫");
                break;
            case Qt::Key_Delete:
            case Qt::Key_Escape:
                handleButton("AC");
                break;
            default:
                QMainWindow::keyPressEvent(event);
                return;
            }
        }

        event->accept();
    }

private:
    QLabel *display = nullptr;
    QLabel *expressionDisplay = nullptr;

    QString expression;
    bool justEvaluated = false;
    bool hasError = false;

    QString formattedExpression() const
    {
        QString result = expression;
        result.replace("*", "×");
        result.replace("/", "÷");
        result.replace("-", "−");
        return result;
    }

    void updateDisplay()
    {
        expressionDisplay->setText(formattedExpression());

        if (expression.isEmpty()) {
            display->setText("0");
        } else {
            display->setText(formattedExpression());
        }
    }

    void clearAll()
    {
        expression.clear();
        justEvaluated = false;
        hasError = false;
        expressionDisplay->clear();
        display->setText("0");
    }

    bool isOperator(QChar character) const
    {
        return character == '+' || character == '-' ||
               character == '*' || character == '/';
    }

    void appendDigit(const QString &digit)
    {
        if (hasError || justEvaluated) {
            expression.clear();
            expressionDisplay->clear();
            justEvaluated = false;
            hasError = false;
        }

        if (expression.endsWith('%')) {
            expression += "*";
        }

        if (!expression.isEmpty()) {
            const QChar last = expression.back();

            if (last == ')') {
                expression += "*";
            }
        }

        expression += digit;
        updateDisplay();
    }

    void appendDecimal()
    {
        if (hasError || justEvaluated) {
            expression.clear();
            justEvaluated = false;
            hasError = false;
        }

        if (expression.endsWith('%') ||
            expression.endsWith(')')) {
            expression += "*0.";
            updateDisplay();
            return;
        }

        qsizetype start = expression.size();

        while (start > 0) {
            const QChar ch = expression[start - 1];

            if (ch.isDigit() || ch == '.') {
                --start;
            } else {
                break;
            }
        }

        if (expression.mid(start).contains('.')) {
            return;
        }

        if (expression.isEmpty() ||
            isOperator(expression.back()) ||
            expression.back() == '(') {
            expression += "0.";
        } else {
            expression += ".";
        }

        updateDisplay();
    }

    void appendOperator(QChar op)
    {
        if (hasError) {
            clearAll();
        }

        if (expression.isEmpty()) {
            if (op == '-') {
                expression = "-";
                updateDisplay();
            }
            return;
        }

        if (expression.endsWith('(')) {
            if (op == '-') {
                expression += "-";
                updateDisplay();
            }
            return;
        }

        if (expression.endsWith('%') ||
            expression.endsWith(')') ||
            expression.back().isDigit() ||
            expression.back() == '.') {
            expression += op;
        } else if (expression.size() == 1 &&
                   expression == "-") {
            return;
        } else if (expression.size() >= 2 &&
                   expression.back() == '-' &&
                   isOperator(expression[expression.size() - 2])) {
            if (op == '-') {
                return;
            }

            expression.chop(2);
            expression += op;
        } else if (isOperator(expression.back())) {
            expression.chop(1);
            expression += op;
        }

        justEvaluated = false;
        updateDisplay();
    }

    void toggleSign()
    {
        if (hasError || expression.isEmpty()) {
            expression = "-";
            hasError = false;
            justEvaluated = false;
            updateDisplay();
            return;
        }

        qsizetype start = expression.size();

        while (start > 0) {
            const QChar ch = expression[start - 1];

            if (ch.isDigit() || ch == '.') {
                --start;
            } else {
                break;
            }
        }

        if (start == expression.size()) {
            return;
        }

        if (start > 0 && expression[start - 1] == '-' &&
            (start == 1 ||
             isOperator(expression[start - 2]) ||
             expression[start - 2] == '(')) {
            expression.remove(start - 1, 1);
        } else {
            expression.insert(start, '-');
        }

        justEvaluated = false;
        updateDisplay();
    }

    void backspace()
    {
        if (hasError || justEvaluated) {
            clearAll();
            return;
        }

        if (!expression.isEmpty()) {
            expression.chop(1);
        }

        updateDisplay();
    }

    void calculate()
    {
        if (expression.isEmpty() || hasError) {
            return;
        }

        try {
            const double result =
                CalculatorEngine::evaluate(expression);

            expressionDisplay->setText(
                formattedExpression() + " ="
            );

            expression = QString::number(
                result, 'g', 15
            );

            display->setText(expression);
            justEvaluated = true;
        } catch (const std::exception &) {
            display->setText("Error");
            expressionDisplay->setText(
                "Invalid calculation"
            );
            expression.clear();
            justEvaluated = false;
            hasError = true;
        }
    }

    void handleButton(const QString &text)
    {
        if (text == "AC") {
            clearAll();
        } else if (text == "⌫") {
            backspace();
        } else if (text == "±") {
            toggleSign();
        } else if (text == "%") {
            if (!expression.isEmpty() &&
                (expression.back().isDigit() ||
                 expression.back() == ')')) {
                expression += '%';
                justEvaluated = false;
                updateDisplay();
            }
        } else if (text == ".") {
            appendDecimal();
        } else if (text == "=") {
            calculate();
        } else if (text == "+") {
            appendOperator('+');
        } else if (text == "−") {
            appendOperator('-');
        } else if (text == "×") {
            appendOperator('*');
        } else if (text == "÷") {
            appendOperator('/');
        } else {
            appendDigit(text);
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("byteCalc");
    QApplication::setApplicationVersion("1.0.0");

    CalculatorWindow window;
    window.show();

    return app.exec();
}
