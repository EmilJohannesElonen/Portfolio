#ifndef LOGICENGINE_H
#define LOGICENGINE_H

#include <QObject>
#include "gui.h"
#include <algorithm>

class LogicEngine : public QObject
{
    Q_OBJECT

public:
    LogicEngine();

    void setUi(GUI* ui);

    void setValues();

    void bubbleSort();

    void mergeSort();

    void quickSort(int min, int max);

    void radixSort();

    void swapValues(int idx1, int idx2);

    /**
     * @brief getDigit gets the n:th digit !starting from the last!
     * @param num = the number to get the digit from
     * @param n = index of the digit (last is first (n=0) etc.)
     * @return the value of the digit
     * @details if num doesn't have n digits, zero is returned
     */
    int getDigit(int num, int n);

public slots:
    void beginSorting(QString algorithm);

    void shuffleBars(int count);

private:
    GUI* ui_;
    std::vector<int> values_;

    void initValues(int amount);
};

#endif // LOGICENGINE_H
