#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QLine>
#include <QTime>
#include <QRadioButton>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class GUI; }
QT_END_NAMESPACE

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    GUI(QWidget *parent = nullptr);
    ~GUI();

    bool swap(int idx1, int idx2);

    void delay(int n);

    void initBars(std::vector<int>* values);

    void initFittedBars(std::vector<int>* values);

    bool checkSorted();

    int getValue(int idx);

    void setValue(int idx, int val);

private slots:

    void shuffleRouter();

    void sortRouter();

signals:
    void wannaSort(QString algorithm);

    void shuffle(int count);

private:
    Ui::GUI *ui;
    QGraphicsScene* scene_;

    std::vector<QGraphicsRectItem*> bars_;
    std::vector<QRadioButton*> rbuttons_;

    QPen primPen_;
    QPen secPen_;
    QPen triPen_;
    QPen checkPen_;

    QBrush primBrush_;
    QBrush secBrush_;
    QBrush triBrush_;
    QBrush checkBrush_;

    int delay_;

    void setTheme();
};
#endif // GUI_H
