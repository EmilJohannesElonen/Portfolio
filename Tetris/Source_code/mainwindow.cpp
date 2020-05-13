/*
 * The mainwindow class executes all the functioanlity of the game.
 * */

#include "mainwindow.hh"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    game_on_(false),
    game_paused_(false),
    current_score_(0),
    current_level_(10),
    active_shape_(0),
    next_shape_(0),
    hold_shape_(0),
    is_progressive_(false),
    deploy_disabled_turns_(0),
    hold_disabled_turns_(0),
    blocks_placed_(0),
    lines_removed_(0),
    elapsed_time_(0),
    username_("GUEST")
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw the grid and rectangles...
    scene_ = new QGraphicsScene(this);
    // ... and to draw the upcoming tetromino...
    next_tetr_scene_ = new QGraphicsScene(this);
    // ... and to draw the "on hold" tetromino.
    hold_scene_ = new QGraphicsScene(this);

    timer_ = new QTimer(this);
    game_time_ = new QTimer(this);

    // Frequently used coordinate changes
    step_move_ = {static_cast<qreal>(STEP),
                  static_cast<qreal>(STEP),
                  static_cast<qreal>(STEP),
                  static_cast<qreal>(STEP)};
    neg_step_move_ = {-static_cast<qreal>(STEP),
                      -static_cast<qreal>(STEP),
                      -static_cast<qreal>(STEP),
                      -static_cast<qreal>(STEP)};
    zero_move_ = {0,0,0,0};

    // The graphicsView object is placed on the window
    // at the following coordinates, but if you want
    // different placement, you can change their values.
    int left_margin = 100; // x coordinate
    int top_margin = 150; // y coordinate

    // The width of the graphicsView is BORDER_RIGHT added by 3,
    // since the borders take one pixel on each side and the extra
    // pixel helps to print the blocks' border more nicely on the
    // right edge. The height of the graphicsView is BORDER_DOWN
    // added by 2. Same goes for the other GraphicsViews' dimensions.
    ui->graphicsView->setGeometry(left_margin, top_margin,
                                  BORDER_RIGHT + 3, BORDER_DOWN + 2);
    ui->nextGraphicsView->setGeometry(left_margin, BUTTON_GAP,
                                       5*STEP + 2, 3*STEP + 2);
    ui->holdGraphicsView->setGeometry(left_margin + 5*STEP + 2,
                                      BUTTON_GAP,5*STEP + 2,
                                      3*STEP + 2);
    ui->graphicsView->setScene(scene_);
    ui->nextGraphicsView->setScene(next_tetr_scene_);
    ui->holdGraphicsView->setScene(hold_scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a tetromino is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);
    next_tetr_scene_->setSceneRect(0, 0, 5*STEP, 3*STEP);
    hold_scene_->setSceneRect(0, 0, 5*STEP, 3*STEP);

    // Drawing the light gray grid to improve visuals.
    for (int x=STEP; x<=BORDER_RIGHT - STEP; x+=STEP)
        scene_->addLine(x,0,x,BORDER_DOWN, QPen(QColor(240,240,240)));
    for (int y=STEP-1; y<=BORDER_DOWN - STEP; y+=STEP)
        scene_->addLine(0,y,BORDER_RIGHT,y, QPen(QColor(240,240,240)));

    // The "game lost"-screen is a transparent rect which is drawn on top
    // of the main scene and its opacity is alteterd to make it visible.
    game_lost_screen_ = scene_->addRect(0,0,BORDER_RIGHT+1,BORDER_DOWN,
                                          Qt::NoPen,QBrush(Qt::white));
    game_lost_screen_->setOpacity(0.0);
    game_lost_screen_->setZValue(1);


    // Setting geometries for other widgets in the GUI
    int left_margin_buttons = left_margin + BORDER_RIGHT + 2 +
            2*BUTTON_GAP;
    ui->startPushButton->setGeometry(left_margin_buttons, top_margin,
                                     BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->pausePushButton->setGeometry(left_margin_buttons, top_margin +
                                     BUTTON_HEIGHT + BUTTON_GAP, BUTTON_WIDTH,
                                     BUTTON_HEIGHT);
    ui->quitPushButton->setGeometry(left_margin_buttons, top_margin +
                                    2*BUTTON_HEIGHT + 2*BUTTON_GAP, BUTTON_WIDTH,
                                    BUTTON_HEIGHT);
    ui->difficultyLabel->setGeometry(left_margin_buttons, top_margin +
                                     3*BUTTON_HEIGHT + 4*BUTTON_GAP, BUTTON_WIDTH,
                                     BUTTON_HEIGHT);
    ui->easyRadioButton->setGeometry(left_margin_buttons, top_margin +
                                     4*BUTTON_HEIGHT + 4*BUTTON_GAP, BUTTON_WIDTH,
                                     BUTTON_HEIGHT);
    ui->mediumRadioButton->setGeometry(left_margin_buttons, top_margin +
                                     5*BUTTON_HEIGHT + 4*BUTTON_GAP, BUTTON_WIDTH,
                                     BUTTON_HEIGHT);
    ui->hardRadioButton->setGeometry(left_margin_buttons, top_margin +
                                     6*BUTTON_HEIGHT + 4*BUTTON_GAP, BUTTON_WIDTH,
                                     BUTTON_HEIGHT);
    ui->progressiveRadioButton->setGeometry(left_margin_buttons, top_margin +
                                     7*BUTTON_HEIGHT + 4*BUTTON_GAP, BUTTON_WIDTH,
                                     BUTTON_HEIGHT);
    ui->currentScoreLabel->setGeometry(left_margin_buttons, 10,
                                       BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->scoreLcdNumber->setGeometry(left_margin_buttons + BUTTON_WIDTH,10,
                                    BUTTON_WIDTH + BUTTON_GAP, BUTTON_HEIGHT);
    ui->currentLevelLabel->setGeometry(left_margin_buttons,
                                       10 + BUTTON_HEIGHT,
                                       BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->levelLcdNumber->setGeometry(left_margin_buttons + BUTTON_WIDTH,
                                    10 +  BUTTON_HEIGHT, BUTTON_WIDTH
                                    + BUTTON_GAP, BUTTON_HEIGHT);
    ui->timeLabel->setGeometry(left_margin_buttons, 10 + 2*BUTTON_HEIGHT,
                               BUTTON_WIDTH, BUTTON_HEIGHT/2);
    ui->timeInfoLabel->setGeometry(left_margin_buttons, 10 + 2.5*BUTTON_HEIGHT,
                                   BUTTON_WIDTH, BUTTON_HEIGHT/2);
    ui->timeLcdNumber->setGeometry(left_margin_buttons + BUTTON_WIDTH,
                                   10 + 2*BUTTON_HEIGHT, BUTTON_WIDTH + BUTTON_GAP,
                                   BUTTON_HEIGHT);
    ui->usernameLabel->setGeometry(left_margin_buttons + BUTTON_WIDTH + 10,
                                   top_margin,BUTTON_WIDTH + BUTTON_GAP,
                                   BUTTON_HEIGHT/2);
    ui->leaveBlankLabel->setGeometry(left_margin_buttons + BUTTON_WIDTH + 10,
                                     top_margin + BUTTON_HEIGHT/2,
                                     BUTTON_WIDTH + BUTTON_GAP, BUTTON_HEIGHT/2);
    ui->leaveBlankLabel2->setGeometry(left_margin_buttons + BUTTON_WIDTH + 10,
                                      top_margin + BUTTON_HEIGHT,
                                      BUTTON_WIDTH + BUTTON_GAP, BUTTON_HEIGHT/2);
    ui->usernameLineEdit->setGeometry(left_margin_buttons + BUTTON_WIDTH + 10,
                                      top_margin + BUTTON_HEIGHT + BUTTON_GAP,
                                      BUTTON_WIDTH + BUTTON_GAP, BUTTON_HEIGHT);
    ui->leaderboardLabel->setGeometry(left_margin_buttons + BUTTON_WIDTH + 10,
                                      top_margin + 3*BUTTON_HEIGHT + 4*BUTTON_GAP + 10,
                                      BUTTON_WIDTH + BUTTON_GAP, 4*BUTTON_HEIGHT);
    ui->upcomingLabel->setGeometry(left_margin, 0,
                                   BORDER_RIGHT + 2, BUTTON_GAP);
    ui->holdLabel->setGeometry(left_margin + 5*STEP + 2, 0,
                               BORDER_RIGHT + 2, BUTTON_GAP);
    ui->statsLabel->setGeometry(left_margin + 5, top_margin + 150,
                                BORDER_RIGHT, BORDER_DOWN - 100);
    ui->resultLabel->setGeometry(left_margin + BORDER_RIGHT - 100,
                                 top_margin + 150, 100, BORDER_DOWN - 100);
    ui->statsLabel->setAlignment(Qt::AlignLeft);
    ui->resultLabel->setAlignment(Qt::AlignRight);
    ui->leaderboardLabel->setAlignment(Qt::AlignTop);
    ui->usernameLineEdit->setMaxLength(5);

    // To prevent the space-key from focusing on the button.
    ui->pausePushButton->setFocusPolicy(Qt::NoFocus);

    // Initial value for difficulty radio buttons
    ui->easyRadioButton->setChecked(true);

    ui->scoreLcdNumber->setDigitCount(8);
    ui->levelLcdNumber->setDigitCount(8);
    ui->timeLcdNumber->setDigitCount(8);
    ui->levelLcdNumber->display(11-current_level_);
    ui->timeLcdNumber->display("00 00");

    // Setting random engine ready for the first real call.
    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng); // Wiping out the first random number (which is almost always 0)
    // After the above settings, you can use randomEng to draw the next falling
    // tetromino by calling: distr(randomEng) in a suitable method.

    // Add more initial settings and connect calls, when needed.
    ui->pausePushButton->setDisabled(true);

    connect(ui->quitPushButton, &QPushButton::clicked, this, &MainWindow::close);
    connect(timer_, &QTimer::timeout, this, &MainWindow::move_tetromino_down);
    connect(game_time_, &QTimer::timeout, this, &MainWindow::display_time);

    init_occ_cords();
    init_placed_blocks();
    init_leaderboard();

    active_tetromino_ = {};
    next_tetromino_ = {};

    // The origin located objects makes creating new tetrominoes easier.
    origin_block_ = new square(0,0,SQUARE_SIDE,SQUARE_SIDE);
    for( int i = 0; i < 4; i++ ) {
        origin_tetromino_.push_back(origin_block_);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer_;
    delete origin_block_;
    for ( auto const& score : highscores_ ) {
        delete score.second;
    }
}


void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if( !game_on_ || active_tetromino_.size() == 0) {
        return;
    }

    // Moving left
    if(event->key() == Qt::Key_A) { 

        if( is_move_possible(neg_step_move_,zero_move_,true) ) {
            for( auto block : active_tetromino_ ) {
                block->moveBy(-1*STEP,0);
            }
        }
    }

    // Moving right
    if(event->key() == Qt::Key_D) {

        if( is_move_possible(step_move_,zero_move_,true) ) {
            for( auto block : active_tetromino_ ) {
                block->moveBy(STEP,0);
            }
        }
    }

    // Moving down
    if(event->key() == Qt::Key_S) {

        if( is_move_possible(zero_move_,step_move_,true) ) {
            for( auto block : active_tetromino_ ) {
                block->moveBy(0,STEP);
            }
        }
    }

    // Rotating right
    if( event->key() == Qt::Key_W ) {
        rotate_right();
    }

    // Rotating left
    if( event->key() == Qt::Key_Q ) {
        rotate_left();
    }

    // Hold/Deploy
    if( event->key() == Qt::Key_Space ) {
        if( hold_tetromino_.size() != 0) {
            if( deploy_disabled_turns_ != 0) {
                return;
            }
            deploy();
        } else {
            if( hold_disabled_turns_ != 0 ) {
                return;
            }
            hold();
        }
    }
}


int MainWindow::get_difficulty()
{
    if( ui->progressiveRadioButton->isChecked() ) {
        is_progressive_ = true;
    } else {
        is_progressive_ = false;
    }

    if( ui->easyRadioButton->isChecked() ||
            ui->progressiveRadioButton->isChecked() ) {
        return 10;
    } else if( ui->mediumRadioButton->isChecked() ) {
        return 5;
    } else {
        return 1;
    }
}

void MainWindow::clear_game()
{
    current_score_ = 0;
    blocks_placed_ = 0;
    lines_removed_ = 0;
    elapsed_time_ = 0;

    clear_mainscreen();
    clear_tetromino(next_tetromino_, next_tetr_scene_);
    clear_tetromino(hold_tetromino_, hold_scene_);

    game_lost_screen_->setOpacity(0.0);
    ui->timeLcdNumber->display("00 00");
    ui->statsLabel->clear();
    ui->resultLabel->clear();
    active_tetromino_.clear();
    next_tetromino_.clear();
    hold_tetromino_.clear();
    init_occ_cords();
    init_placed_blocks();
    delay(1000); // For smoother transition
}

void MainWindow::clear_mainscreen()
{
    for( qreal y = -4*static_cast<qreal>(STEP) ; y < static_cast<qreal>(20*STEP) ;
         y += static_cast<qreal>(STEP) ) {
        for( int x = 0; x < 10; x++) {
            if( placed_blocks_.at(y).at(x) != nullptr ) {
                scene_->removeItem(placed_blocks_.at(y).at(x));
                delete placed_blocks_.at(y).at(x);
            }
        }
    }
}

void MainWindow::clear_tetromino(tetromino& tetrom, QGraphicsScene* scene)
{
    for( auto block : tetrom ) {
        scene->removeItem(block);
        delete block;
    }
}


void MainWindow::new_tetromino()
{
    active_shape_ = next_shape_;
    active_tetromino_.clear();

    std::vector<qreal> delta_x;
    std::vector<qreal> delta_y;
    for(int i = 0; i < 4; i++) {
        delta_x.push_back(RECIPES.at(active_shape_).at(i).at(0));
        delta_y.push_back(RECIPES.at(active_shape_).at(i).at(1));
    }

    if( !is_move_possible(delta_x,delta_y,false) ) {
        game_lost();
        return;
    }

    next_shape_ = distr(randomEng);
    clear_tetromino(next_tetromino_, next_tetr_scene_);
    next_tetromino_.clear();

    // To get all tetrominoes to print centered we need to add
    // different coordinate corrections for some tetrominoes.
    std::vector<int> corrections;
    coord_corrections(corrections, next_shape_);

    draw_tetromino(next_tetr_scene_, next_tetromino_, next_shape_,
                   -2*STEP + corrections.at(1), STEP + corrections.at(0));

    draw_tetromino(scene_, active_tetromino_, active_shape_,
                   0, -1);
}

bool MainWindow::is_move_possible(std::vector<qreal>& dx, std::vector<qreal>& dy,
                                  bool use_active )
{
    bool possible = true;
    std::vector<Coords> new_coords;
    if( use_active ) {
        for(int i = 0; i < 4; i++) {
            Coords x_n_y;
            x_n_y.x = active_tetromino_.at(i)->x() + dx.at(i);
            x_n_y.y = active_tetromino_.at(i)->y() + dy.at(i);
            new_coords.push_back(x_n_y);
        }
    } else {
        for(int i = 0; i < 4; i++) {
            Coords x_n_y;
            x_n_y.x = origin_tetromino_.at(i)->x() + dx.at(i);
            x_n_y.y = origin_tetromino_.at(i)->y() + dy.at(i);
            new_coords.push_back(x_n_y);
        }
    }

    for( auto coord : new_coords ) {
            if( 0 > coord.x || coord.x >= BORDER_RIGHT ) {
                possible = false;
            } else if( coord.y > BORDER_DOWN - STEP ) {
                possible = false;
            } else if( is_in(occupied_coordinates_.at(coord.y),
                                        coord.x) ) {
                possible = false;
            }
        }

    new_coords.clear();

    return possible;
}

bool MainWindow::is_in(std::vector<qreal>& vector, qreal x)
{
    std::vector<qreal>::iterator it = std::find(vector.begin(),
                                                vector.end(), x);

    if( it != vector.end() ) {
        return true;
    }

    return false;
}


void MainWindow::remove_verticals(std::vector<qreal> y_coords)
{
    // This execution, while being more complicated than necessary,
    // creates a animation for the eliminated rows that is inspired
    // by the classic NES Tetris.

    int rows = static_cast<int>(y_coords.size());
    current_score_ += (10+(rows-1)*20);
    if( rows == 4 ) {
        current_score_ += 10;
    }
    lines_removed_ += static_cast<int>(y_coords.size());

    for( qreal x = 0; x < 5; x++ ) {
        for( auto y : y_coords ) {
            square* block1 = placed_blocks_.at(y).at(4-x);
            square* block2 = placed_blocks_.at(y).at(5+x);
            scene_->removeItem(block1);
            scene_->removeItem(block2);
            placed_blocks_.at(y).at(4-x) = nullptr;
            placed_blocks_.at(y).at(5+x) = nullptr;
            delete block1;
            delete block2;
        }
        delay(50 + current_level_*5);
    }

    delay(50 + current_level_*5);

    int steps = 0;

    for( qreal y = y_coords.at(y_coords.size()-1);
         y > -4*static_cast<qreal>(STEP);
         y -= static_cast<qreal>(STEP) ) {

        if(std::find(y_coords.begin(),y_coords.end(),y) !=
                y_coords.end()) {
            steps += 1;
        }

        for( int x = 0; x < 10; x++) {
            if( placed_blocks_.at(y).at(x) != nullptr ) {
                square* block = placed_blocks_.at(y).at(x);
                placed_blocks_.at(y).at(x) = nullptr;
                block->moveBy(0,steps*STEP);
                placed_blocks_.at(block->y()).at(block->x()/STEP) = block;
            }
        }
    }

    for(qreal y = BORDER_DOWN - STEP;
        y > -4*static_cast<qreal>(STEP);
        y -= static_cast<qreal>(STEP) ) {

        occupied_coordinates_.at(y).clear();
        for( auto block : placed_blocks_.at(y) ) {
            if( block != nullptr ) {
                occupied_coordinates_.at(y).push_back(block->x());
            }
        }
    }
    ui->scoreLcdNumber->display(current_score_);
}


bool MainWindow::check_game_state()
{
    timer_->stop();
    game_on_ = false; // Temporary state change to disable key input.

    for( auto block : active_tetromino_ ) {
        placed_blocks_.at(block->y()).at(block->x()/STEP) = block;
        occupied_coordinates_.at(block->y()).push_back(block->x());
    }

    std::vector<qreal> full_rows;

    for( auto coords : occupied_coordinates_) {
        if( coords.second.size() ==
                static_cast<std::vector<qreal>::size_type>(COLUMNS)) {
            full_rows.push_back(coords.first);
        }
    }

    if( full_rows.size() > 0 ) {
        remove_verticals(full_rows);
    }

    if( occupied_coordinates_.at(-1*static_cast<qreal>(STEP)).size() != 0 ) {
        game_lost();
        return false;
    }

    timer_->start(current_level_*100);
    game_on_ = true;
    return true;
}


void MainWindow::game_lost()
{
    game_on_ = false;
    timer_->stop();
    game_time_->stop();
    ui->pausePushButton->setDisabled(true);
    ui->quitPushButton->setDisabled(false);
    ui->startPushButton->setDisabled(false);
    ui->easyRadioButton->setDisabled(false);
    ui->mediumRadioButton->setDisabled(false);
    ui->hardRadioButton->setDisabled(false);
    ui->progressiveRadioButton->setDisabled(false);
    ui->usernameLineEdit->setDisabled(false);

    game_lost_screen_->setOpacity(0.7);
    ui->statsLabel->setText("\tGAME OVER!\n\nFinal score:"
                            "\nTotal tetrominos placed:"
                            "\nTotal lines eliminated: "
                            "\nFinal time:");
    ui->resultLabel->setText("\n\n" + QString::number(current_score_) +
                             "\n" + QString::number(blocks_placed_) +
                             "\n" + QString::number(lines_removed_) +
                             "\n" + QString::number(elapsed_time_/60) +
                             " min " + QString::number(elapsed_time_%60) +
                             " s");
    if( current_score_ > highscores_.at(5)->score ) {
        QString text = ui->statsLabel->text();
        text.append("\n\nNEW TOP 5 HIGHSCORE!");
        ui->statsLabel->setText(text);

        highscores_.at(5)->score = current_score_;
        highscores_.at(5)->username = username_.toStdString();
        sort_highscores();
        save_highscores();
        display_leaderboard();
    }

}

void MainWindow::delay(int n)
{
    QTime dieTime= QTime::currentTime().addMSecs(n);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::rotate_right()
{
    if( active_shape_ == 3 ) { // square
        return;
    } else {
        qreal maxx = max_x();
        qreal maxy = max_y();

        int x_corrector;
        if( maxx - min_x() < maxy - min_y() ) {
            x_corrector = 2;
        } else {
            x_corrector = 1;
        }

        std::vector<qreal> delta_x;
        std::vector<qreal> delta_y;
        qreal dx;
        qreal dy;
        for( auto block : active_tetromino_ ) {
            dx = block->x() - maxx;
            dy = block->y() - maxy;
            delta_x.push_back(-(dx+dy)-x_corrector*STEP);
            delta_y.push_back(dx-dy);
        }

        if( is_move_possible(delta_x,delta_y,true) ) {
            for( auto block : active_tetromino_ ) {
                dx = block->x() - maxx;
                dy = block->y() - maxy;
                block->moveBy(-(dx+dy)-x_corrector*STEP,dx-dy);
            }
        }
    }
}

void MainWindow::rotate_left()
{
    if( active_shape_ == 3 ) { // square
        return;
    } else {
        qreal minx = min_x();
        qreal maxy = max_y();

        int x_corrector;
        if( max_x() - minx < maxy - min_y() ) {
            x_corrector = 1;
        } else {
            x_corrector = 2;
        }

        std::vector<qreal> delta_x;
        std::vector<qreal> delta_y;
        qreal dx;
        qreal dy;
        for( auto block : active_tetromino_ ) {
            dx = block->x() - minx;
            dy = block->y() - maxy;
            delta_x.push_back((dy-dx)+x_corrector*STEP);
            delta_y.push_back(-(dx+dy));
        }

        if( is_move_possible(delta_x,delta_y,true) ) {
            for( auto block : active_tetromino_ ) {
                dx = block->x() - minx;
                dy = block->y() - maxy;
                block->moveBy((dy-dx)+x_corrector*STEP,-(dx+dy));
            }
        }
    }
}

qreal MainWindow::max_y()
{
    // Initializing with lowest possible value
    qreal max_y_coord = static_cast<qreal>(-4*STEP);

    for( auto block : active_tetromino_ ) {
        if( block->y() > max_y_coord ) {
            max_y_coord = block->y();
        }
    }
    return max_y_coord;
}

qreal MainWindow::max_x()
{
    // Initializing with lowest possible value
    qreal max_x_coord = 0;

    for( auto block : active_tetromino_ ) {
        if( block->x() > max_x_coord ) {
            max_x_coord = block->x();
        }
    }
    return max_x_coord;
}

qreal MainWindow::min_y()
{
    // Initializing with highest possible value
    qreal min_y_coord = static_cast<qreal>(19*STEP);

    for( auto block : active_tetromino_ ) {
        if( block->y() < min_y_coord ) {
            min_y_coord = block->y();
        }
    }
    return min_y_coord;
}

qreal MainWindow::min_x()
{
    // Initializing with highest possible value
    qreal min_x_coord = static_cast<qreal>(9*STEP);

    for( auto block : active_tetromino_ ) {
        if( block->x() < min_x_coord ) {
            min_x_coord = block->x();
        }
    }
    return min_x_coord;
}

void MainWindow::hold()
{
    hold_shape_ = active_shape_;

    // To get all tetrominoes to print centered we need to add
    // different coordinate corrections for some tetrominoes.
    std::vector<int> corrections;
    coord_corrections(corrections, hold_shape_);

    draw_tetromino(hold_scene_,hold_tetromino_,hold_shape_,
                   -2*STEP + corrections.at(1),STEP + corrections.at(0));

    clear_tetromino(active_tetromino_, scene_);

    // Resetting timer to make transition clearer.
    timer_->stop();
    timer_->start(current_level_*100);


    new_tetromino();
}

void MainWindow::deploy()
{
    next_shape_ = hold_shape_;

    clear_tetromino(next_tetromino_, next_tetr_scene_);

    next_tetromino_.clear();

    clear_tetromino(hold_tetromino_, hold_scene_);

    hold_tetromino_.clear();

    // To get all tetrominoes to print centered we need to add
    // different coordinate corrections for some tetrominoes.
    std::vector<int> corrections;
    coord_corrections(corrections, next_shape_);

    draw_tetromino(next_tetr_scene_,next_tetromino_,next_shape_,
                   -2*STEP + corrections.at(1),STEP + corrections.at(0));

    hold_disabled_turns_ = 2;

}

void MainWindow::draw_tetromino(QGraphicsScene* scene,tetromino& tetrom,
                                int shape, int starting_x, int starting_y)
{
    for(int i = 0; i < 4; i++) {
        square* block;
        QBrush brush(COLORS.at(shape));
        block = scene->addRect(starting_x,starting_y,
                               STEP,STEP,QPen(),brush);
        block->moveBy(RECIPES.at(shape).at(i).at(0),
                      RECIPES.at(shape).at(i).at(1));
        tetrom.push_back(block);
    }
}

void MainWindow::coord_corrections(std::vector<int>& corrections, int shape)
{
    int x_correction;
    int y_correction;
    if(shape == 0 ) { // Horizontal
        x_correction = -STEP/2;
        y_correction = 0;
    } else if( shape == 3 ) { // Square
        x_correction = -STEP/2;
        y_correction = STEP/2;
    } else {
        x_correction = 0;
        y_correction = STEP/2;
    }
    corrections.push_back(y_correction);
    corrections.push_back(x_correction);
}

void MainWindow::display_time()
{
    elapsed_time_ += 1;
    QString minutes = QString::number(elapsed_time_/60);
    QString seconds = QString::number(elapsed_time_%60);

    if(elapsed_time_%60 < 10) {
        seconds = QString::number(0)+seconds;
    }
    if(elapsed_time_/60 < 10) {
        minutes = QString::number(0)+minutes;
    }

    QString text = minutes + " " + seconds;
    ui->timeLcdNumber->display(text);
}


void MainWindow::save_highscores()
{
    remove("temporary.txt");
    std::ofstream file("temporary.txt");

    for( int i = 1; i < 6; i++ ) {
        QString username = QString::fromStdString(highscores_.at(i)->username).trimmed();
        file << i << ";" << username.toStdString();
        file << ";" << highscores_.at(i)->score << std::endl;
    }

    file.close();

    remove("leaderboard.txt");

    rename("temporary.txt", "leaderboard.txt");
}

void MainWindow::sort_highscores()
{
    std::map<int,std::vector<std::string>> sorting_map;
    int scores_to_print = 0;

    for( int i = 1; i < 6; i++ ) {
        if( highscores_.at(i)->score == 0 ) {
            ;
        }
        else if(sorting_map.find(highscores_.at(i)->score)
                != sorting_map.end() ) {
            sorting_map.at(highscores_.at(i)->score).push_back(highscores_.at(i)->username);
            scores_to_print++;
        }
        else {
            sorting_map.insert( {highscores_.at(i)->score,
                                 {highscores_.at(i)->username}});
            scores_to_print++;
        }
    }

    for( int j = 5; j > scores_to_print; j-- ) {
        highscores_.at(j)->username = "";
        highscores_.at(j)->score = 0;
    }

    std::map<int,std::vector<std::string>>::iterator it;
    for( it = sorting_map.begin(); it != sorting_map.end(); it++ ) {
        std::sort(it->second.begin(), it->second.end());
        for( int a = static_cast<int>(it->second.size()); a > 0; a--) {
            highscores_.at(scores_to_print)->score = it->first;
            highscores_.at(scores_to_print)->username = it->second.at(a-1);
            scores_to_print--;
        }
    }
}

void MainWindow::display_leaderboard()
{
    QString text = "Leaderboard:\n\n";
    for( int i = 1; i < 6; i++ ) {
        text.append(QString::number(i) + ": ");
        QString username = QString::fromStdString(highscores_.at(i)->username);
        if( username.trimmed().isEmpty() ) {
            text.append("\n");
        }
        else {
            for( int i = 5-username.size(); i > 0; i--) {
                username.push_back(' ');
            }
            text.append(username + "      \t" +
                        QString::number(highscores_.at(i)->score) +
                        "\n");
        }
    }
    ui->leaderboardLabel->setText(text);
}



void MainWindow::on_startPushButton_clicked()
{
    game_on_ = true;

    ui->startPushButton->setDisabled(true);
    ui->quitPushButton->setDisabled(true);
    ui->pausePushButton->setDisabled(false);
    ui->easyRadioButton->setDisabled(true);
    ui->mediumRadioButton->setDisabled(true);
    ui->hardRadioButton->setDisabled(true);
    ui->progressiveRadioButton->setDisabled(true);
    ui->usernameLineEdit->setDisabled(true);

    clear_game();
    username_ = ui->usernameLineEdit->text();
    if(username_.trimmed().isEmpty()) {
        username_ = "GUEST";
    }
    current_level_ = get_difficulty();
    next_shape_ = distr(randomEng);
    ui->levelLcdNumber->display(11-current_level_);
    ui->scoreLcdNumber->display(current_score_);
    new_tetromino();
    timer_->start(current_level_*100);
    game_time_->start(1000);

}

void MainWindow::move_tetromino_down()
{
    if( !is_move_possible(zero_move_,step_move_,true) ) {

        if( !check_game_state() ) {
            return;
        }

        blocks_placed_ += 1;

        if( hold_disabled_turns_ > 0) {
            hold_disabled_turns_ -= 1;
        }
        if( deploy_disabled_turns_ > 0) {
            deploy_disabled_turns_ -= 1;
        }

        if( is_progressive_ ) {
            if( blocks_placed_%10 == 0 && current_level_ > 1) {
                current_level_ -= 1;
                ui->levelLcdNumber->display(11-current_level_);
                timer_->setInterval(current_level_*100);
            }
        }
        new_tetromino();

    } else {
        // do the move
        for( auto block : active_tetromino_ ) {
            block->moveBy(0,STEP);
        }
    }
}

bool operator ==(const Coords& left, const Coords& right)
{
    if( left.x == right.x && left.y == right.y ) {
        return true;
    }
    return false;
}

void MainWindow::on_pausePushButton_clicked()
{
    if( !game_paused_ ) {
        game_on_ = false;
        game_paused_ = true;
        ui->quitPushButton->setDisabled(false);
        timer_->stop();
        game_time_->stop();
        ui->pausePushButton->setText("Return");
    } else {
        game_on_ = true;
        game_paused_ = false;
        ui->quitPushButton->setDisabled(true);
        timer_->start(current_level_*100);
        game_time_->start(1000);
        ui->pausePushButton->setText("Pause");
    }
}

void MainWindow::init_occ_cords()
{
    occupied_coordinates_ = {};
    // The 4 negative rows (above the GraphicsView) may be needed in
    // some cases, since the tetrominos fall from outside the picture.
    for( qreal i = -4*static_cast<qreal>(STEP) ; i < static_cast<qreal>(20*STEP) ;
         i += static_cast<qreal>(STEP) ) {
        occupied_coordinates_.insert({ i,{} });
    }
}

void MainWindow::init_placed_blocks()
{
    placed_blocks_ = {};
    // The 4 negative rows (above the GraphicsView) may be needed in
    // some cases, since the tetrominos fall from outside the picture.
    for( qreal i = -4*static_cast<qreal>(STEP) ; i < static_cast<qreal>(20*STEP) ;
         i += static_cast<qreal>(STEP) ) {
        placed_blocks_.insert({ i,{} });
        for( int a = 0; a < 10; a++ ) {
            placed_blocks_.at(i).push_back(nullptr);
        }
    }
}

void MainWindow::init_leaderboard()
{
    highscores_ = {};

    for( int i = 1; i < 6; i++ ) {
        Score* blank_score = new Score;
        blank_score->username = "";
        blank_score->score = 0;
        highscores_.insert({i, blank_score});
    }

    std::ifstream file("leaderboard.txt");

    if( !file ) {
        save_highscores();
    }
    else {
        std::string row;
        while( std::getline(file,row) ) {
            QStringList data = QString::fromStdString(row).split(';');
            if( data.size() == 3 ) {
                QString index = data.at(0).trimmed();
                QString username = data.at(1).trimmed();
                QString score = data.at(2).trimmed();

                if( index.toInt() < 6 && index.toInt() > 0 ) {
                    bool numeric = false;
                    score.toInt(&numeric);
                    if ( numeric ) {

                        if( username.size() > 5 ) {
                            username = data.at(1).mid(0,5);
                        }
                        if( username.isEmpty() ) {
                            score = "0";
                        }
                        if( score.toInt() == 0 ) {
                            username = "";
                        }
                        highscores_.at(index.toInt())->username = username.toStdString();
                        highscores_.at(index.toInt())->score = score.toInt();
                    }
                }
            }
        }
        file.close();
        sort_highscores();
    }
    display_leaderboard();
}
