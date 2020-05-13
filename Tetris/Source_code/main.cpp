/* Tetris Game
 * --------------
 * A Tetris game using a GUI following standard Tetris rules.
 * The main function only initializes the mainwindow where the game mechanics are located.
 *
 * Emil Elonen, emil.elonen@tuni.fi
 * */

#include "mainwindow.hh"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
