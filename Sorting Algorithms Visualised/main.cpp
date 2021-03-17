#include "gui.h"
#include "logicengine.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUI w;
    LogicEngine l;
    l.setUi(&w);
    w.show();
    return a.exec();
}
