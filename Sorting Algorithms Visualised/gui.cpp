#include "gui.h"
#include "ui_gui.h"

int BORDER_UP = 10;
int BORDER_LEFT = 10;
int VIEW_WIDTH = 601;
int VIEW_HEIGHT = 400;

GUI::GUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GUI)
    , bars_({})
    , primPen_(QPen(Qt::white))
    , secPen_(QPen(Qt::blue))
    , triPen_(QPen(Qt::red))
    , checkPen_(QPen(Qt::green))
    , primBrush_(QBrush(Qt::white))
    , secBrush_(QBrush(Qt::blue))
    , triBrush_(QBrush(Qt::red))
    , checkBrush_(QBrush(Qt::green))
    , delay_(10)
{
    ui->setupUi(this);
    ui->graphicsView->setGeometry(BORDER_UP,BORDER_LEFT,VIEW_WIDTH,VIEW_HEIGHT);
    scene_ = new QGraphicsScene(this);
    scene_->setSceneRect(0, 0, VIEW_WIDTH-2,VIEW_HEIGHT-2);
    ui->graphicsView->setScene(scene_);
    setTheme();

    ui->mergeSort->setChecked(true);
    ui->barCount->setMaximum(300);
    ui->barCount->setMinimum(2);
    ui->barCount->setValue(300);
    rbuttons_ = {ui->bubbleSort, ui->mergeSort, ui->quickSort, ui->radixSort};

    connect(ui->sortButton, &QPushButton::pressed, this, &GUI::sortRouter);
    connect(ui->shuffleButton, &QPushButton::pressed, this, &GUI::shuffleRouter);
}

GUI::~GUI()
{
    delete ui;
}

bool GUI::swap(int idx1, int idx2)
{
    if(idx1 >= bars_.size() || idx2 >= bars_.size()) {
        return false;
    }

    int val1 = getValue(idx1);
    int val2 = getValue(idx2);

    setValue(idx1, val2);
    setValue(idx2, val1);

    return true;
}

void GUI::delay(int n)
{
    QTime dieTime= QTime::currentTime().addMSecs(n);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void GUI::initBars(std::vector<int>* values)
{
    if(ui->fitCheckBox->isChecked()) {
        initFittedBars(values);
        return;
    }

    scene_->clear();
    bars_.clear();

    int amount = values->size();
    int pixelsPer = 600/amount;
    int leftOver = 600%amount;

    for(int i = 0; i < amount; i++) {
        int val = values->at(i);
        bars_.push_back(scene_->addRect(0,0,pixelsPer-2,val,primPen_,primBrush_));
        bars_.back()->moveBy(i*pixelsPer+(leftOver/2),397-val);
    }
}

void GUI::initFittedBars(std::vector<int> *values)
{
    scene_->clear();
    bars_.clear();

    int amount = values->size();
    int pixelsPer = 600/amount;
    int leftOver = 600%amount;
    float extraPixPerBar = static_cast<float>(leftOver)/static_cast<float>(amount);
    float everyNthBar = 0;
    if(leftOver != 0) {
        everyNthBar = 1/extraPixPerBar;
    }
    float nextSpace = everyNthBar;
    int a = 1;
    int extra;
    int extrasInserted = 0;
    for(int i = 0; i < amount; i++) {
        int val = values->at(i);
        if(a == static_cast<int>(nextSpace)) {
            extra = -1;
            nextSpace += everyNthBar;
        } else {
            extra = -2;
        }
        bars_.push_back(scene_->addRect(0,0,pixelsPer-2,val,primPen_,primBrush_));
        bars_.back()->moveBy(i*pixelsPer+extrasInserted,397-val);
        a++;
        if(extra == -1) {
            extrasInserted++;
        }
    }
}

bool GUI::checkSorted()
{
    bars_.at(0)->setPen(checkPen_);
    bars_.at(0)->setBrush(checkBrush_);
    for(std::vector<QGraphicsRectItem*>::size_type i = 1;
        i < bars_.size(); i++) {
        if(getValue(i) > getValue(i-1)) {
            bars_.at(i)->setPen(checkPen_);
            bars_.at(i)->setBrush(checkBrush_);
        } else {
            bars_.at(i)->setPen(triPen_);
            bars_.at(i)->setBrush(triBrush_);
            return false;
        }
        delay(delay_);
    }
    ui->shuffleButton->setEnabled(true);
    ui->sortButton->setEnabled(true);
    ui->barCount->setEnabled(true);
    for(auto rbutton : rbuttons_) {
        rbutton->setEnabled(true);
    }
    return true;
}

int GUI::getValue(int idx)
{
    return 397-bars_.at(idx)->y();
}

void GUI::setValue(int idx, int val)
{
    QGraphicsRectItem* bar = bars_.at(idx);
    int width = bar->rect().width();
    int x = bar->x();
    scene_->removeItem(bar);
    bar = scene_->addRect(0,0,width,val,primPen_, primBrush_);
    bar->moveBy(x,397-val);
    bars_.at(idx) = bar;
    bars_.at(idx)->setPen(secPen_);
    bars_.at(idx)->setBrush(secBrush_);
    delay(delay_);
    bars_.at(idx)->setPen(primPen_);
    bars_.at(idx)->setBrush(primBrush_);
}

void GUI::shuffleRouter()
{
    emit shuffle(ui->barCount->value());
}

void GUI::sortRouter()
{
    for(auto bar : bars_) {
        bar->setPen(primPen_);
        bar->setBrush(primBrush_);
    }

    ui->shuffleButton->setEnabled(false);
    ui->sortButton->setEnabled(false);
    ui->barCount->setEnabled(false);
    QString text;
    for(auto rbutton : rbuttons_) {
        if(rbutton->isChecked()) {
           text = rbutton->text();
        }
        rbutton->setEnabled(false);
    }

    emit wannaSort(text);
}

void GUI::setTheme()
{
    scene_->setBackgroundBrush(QColor(67,70,75));
}
