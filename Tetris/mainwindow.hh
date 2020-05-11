/* Class: MainWindow
  * -----------------
  *
  * Header file for the MainWindow class in which the game is played.
  * */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <random>
#include <QTimer>
#include <QTime>
#include <QKeyEvent>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>

using square = QGraphicsRectItem;
using starting_coords = std::vector<std::vector<int>>;
using tetromino = std::vector<square*>;

struct Coords
{
    qreal x;
    qreal y;
};

struct Score
{
    std::string username;
    int score;
};

bool operator ==(const Coords&, const Coords&);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void on_startPushButton_clicked();

    void on_pausePushButton_clicked();

private:
    Ui::MainWindow *ui;

    QGraphicsScene* next_tetr_scene_;

    QGraphicsScene* hold_scene_;

    QGraphicsScene* scene_;

    QTimer* game_time_;

    QTimer* timer_;

    /**
     * @brief initializes the data structure containing all
     * occupied coordinates
     */
    void init_occ_cords();

    /**
     * @brief initializes the data structure containing all
     * placed blocks
     */
    void init_placed_blocks();

    /**
     * @brief initializes the leaderboard
     */
    void init_leaderboard();

    /**
     * @brief move_tetromino_down (by one step)
     */
    void move_tetromino_down();

    /**
     * @brief get_difficulty
     * @return selected difficulty level (1-10)
     */
    int get_difficulty();

    /**
     * @brief clear_game
     */
    void clear_game();

    /**
     * @brief clear_mainscreen
     */
    void clear_mainscreen();

    /**
     * @brief removes a tetromino from a specified scene.
     *
     * params: reference to the tetromino, pointer to the scene
     */
    void clear_tetromino(tetromino&, QGraphicsScene*);

    /**
     * @brief creates a new tetromino
     */
    void new_tetromino();

    /**
     * @brief is_move_possible
     * @return true if move is possible, false otherwise
     *
     * params: vectors containing the new x and y coordinates
     */
    bool is_move_possible(std::vector<qreal>&, std::vector<qreal>&,
                          bool );

    /**
     * @brief checks if a qreal element is in a specific vector
     * @return true if element is found, false otherwise
     *
     * params: the vector and the element
     */
    bool is_in(std::vector<qreal>&,qreal);

    /**
     * @brief removes all full vertical rows
     *
     * params: a vector containing the y coordinates of full rows
     */
    void remove_verticals(std::vector<qreal>);


    /**
     * @brief checks weather game is lost or can be continued
     * @return false if game is lost, true otherwise
     */
    bool check_game_state();

    /**
     * @brief does actions required in a game over-situation
     */
    void game_lost();

    /**
     * @brief delay
     * params: delay time in ms
     */
    void delay(int);

    /**
     * @brief rotate_right
     */
    void rotate_right();

    /**
     * @brief rotate_left
     */
    void rotate_left();

    /**
     * @brief max_y
     * @return max y coordinate
     */
    qreal max_y();

    /**
     * @brief max_x
     * @return max x coordinate
     */
    qreal max_x();

    /**
     * @brief min_y
     * @return min y coordinate
     */
    qreal min_y();

    /**
     * @brief min_x
     * @return min x coordinate
     */
    qreal min_x();

    /**
     * @brief takes active tetromino into hold
     */
    void hold();

    /**
     * @brief deploys the tetromino in hold
     */
    void deploy();

    /**
     * @brief draw_tetromino
     *
     * params: The scene to be drawn into, the tetromino to be drawn
     * the shape of the tetromino, x and y coordinates
     */
    void draw_tetromino(QGraphicsScene*,tetromino&,
                        int,int,int);

    /**
     * @brief calculates coordinate corrections for moving a tetromino
     *
     * params: reference to the tetromino, shape of the tetromino
     */
    void coord_corrections(std::vector<int>&, int);

    /**
     * @brief display_time
     */
    void display_time();

    /**
     * @brief saves highscores into the correct file
     */
    void save_highscores();

    /**
     * @brief sort_highscores
     */
    void sort_highscores();

    /**
     * @brief display_leaderboard
     */
    void display_leaderboard();

    // Constants describing scene coordinates
    // Copied from moving circle example and modified a bit
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 480; // 260; (in moving circle)
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 240; // 680; (in moving circle)

    // Other widget sizes
    const int BUTTON_WIDTH = 120;
    const int BUTTON_HEIGHT = 40;
    const int BUTTON_GAP = 30;

    // Size of a tetromino component
    const int SQUARE_SIDE = 24;
    // Square side and step size should be equal by default but they
    // can be adjusted individually.
    const int STEP = 24;
    // Number of horizontal cells (places for tetromino components)
    const int COLUMNS = BORDER_RIGHT / SQUARE_SIDE;
    // Number of vertical cells (places for tetromino components)
    const int ROWS = BORDER_DOWN / SQUARE_SIDE;

    // Constants for different tetrominos and the number of them
    enum Tetromino_kind {HORIZONTAL,
                         LEFT_CORNER,
                         RIGHT_CORNER,
                         SQUARE,
                         STEP_UP_RIGHT,
                         PYRAMID,
                         STEP_UP_LEFT,
                         NUMBER_OF_TETROMINOS};
    // From the enum values above, only the last one is needed in this template.
    // Recall from enum type that the value of the first enumerated value is 0,
    // the second is 1, and so on.
    // Therefore the value of the last one is 7 at the moment.
    // Remove those tetromino kinds above that you do not implement,
    // whereupon the value of NUMBER_OF_TETROMINOS changes, too.
    // You can also remove all the other values, if you do not need them,
    // but most probably you need a constant value for NUMBER_OF_TETROMINOS.

    // The recipes for constructing the differently shaped tetrominos and
    // moving them to the correct starting coordinates. The recipes are in
    // the same order as in the Teromino_kind enum.
    std::vector<starting_coords> RECIPES {
        {{3*STEP,0},    {4*STEP,0},    {5*STEP,0},    {6*STEP,0}},
        {{3*STEP,0},    {3*STEP,-STEP},{4*STEP,0},    {5*STEP,0}},
        {{3*STEP,0},    {4*STEP,0},    {5*STEP,0},    {5*STEP,-STEP}},
        {{4*STEP,0},    {4*STEP,-STEP},{5*STEP,0},    {5*STEP,-STEP}},
        {{3*STEP,0},    {4*STEP,0},    {4*STEP,-STEP},{5*STEP,-STEP}},
        {{3*STEP,0},    {4*STEP,0},    {5*STEP,0},    {4*STEP,-STEP}},
        {{3*STEP,-STEP},{4*STEP,-STEP},{4*STEP,0},    {5*STEP,0}} };

    // The colors for all the different tetrominos. The colors are at the
    // the same index as the corresponding tetromino in the Teromino_kind
    // enum.
    // Orange needs to bee defined via RGB since there is no preset.
    std::vector<QColor> COLORS {
        Qt::cyan, Qt::blue, QColor(255,128,0),
        Qt::yellow, Qt::green, Qt::darkMagenta, Qt::red};


    // For randomly selecting the next dropping tetromino
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    // More constants, attibutes, and methods
    bool game_on_;
    bool game_paused_;
    int current_score_;
    int current_level_;
    int active_shape_;
    int next_shape_;
    int hold_shape_;
    bool is_progressive_;
    int deploy_disabled_turns_;
    int hold_disabled_turns_;
    int blocks_placed_;
    int lines_removed_;
    int elapsed_time_;
    QString username_;

    std::map<int,Score*> highscores_;
    std::map<qreal,tetromino> placed_blocks_;

    // Frequently used coordinate changes
    std::vector<qreal> step_move_;
    std::vector<qreal> zero_move_;
    std::vector<qreal> neg_step_move_;

    square* game_lost_screen_;
    square* origin_block_;
    tetromino origin_tetromino_;
    tetromino active_tetromino_;
    tetromino next_tetromino_;
    tetromino hold_tetromino_;

    // <y-coord, <x-coords>>
    std::map<qreal,std::vector<qreal>> occupied_coordinates_;

};

#endif // MAINWINDOW_HH
