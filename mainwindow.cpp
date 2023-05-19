#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::UpdateTimer);
    StartGame();
}

void MainWindow::easy(){
    elapsedTime = 0;
    timer->stop();
    ui->label_timer->setText("00:00");
    two = 0;
    victory = 12;
    difficulty = 0;
    Random_cards();
}

void MainWindow::medium(){
    elapsedTime = 0;
    timer->stop();
    ui->label_timer->setText("00:00");
    two = 0;
    victory = 18;
    difficulty = 1;
    Random_cards();
}

void MainWindow::hard(){
    elapsedTime = 0;
    timer->stop();
    ui->label_timer->setText("00:00");
    two = 0;
    victory = 24;
    difficulty = 2;
    Random_cards();
}

void MainWindow::Random_cards(){
    const int kNumCards = victory * 2;
    int cnt = 0;

    if(cards.size() > 0){
        cards.clear();
    }

    while (cards.size() < kNumCards) {
        cards << QString::number(cnt) + ".gif";
        cnt = (cnt + 1) % victory;
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));

    Display_cards();
}

void MainWindow::Display_cards() {
    const QSize kCardSize(100, 100);

    int row = 0, col = 0;

    QLayoutItem* item;
    while ((item = ui->grid_cards->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const QString& card : cards) {
        QPushButton* button = new QPushButton(this);
        button->setFixedSize(kCardSize);

        QString back_image_path = ":/back-cards/dos.png";
        QPixmap back_pixmap(back_image_path);

        QPixmap scaled_back_pixmap = back_pixmap.scaled(kCardSize, Qt::KeepAspectRatio);

        button->setIcon(QIcon(scaled_back_pixmap));
        button->setIconSize(kCardSize);

        ui->grid_cards->addWidget(button, row, col);

        connect(button, &QPushButton::clicked, [this, button, card]() { OnCardClicked(button, card); });

        col++;
        if (col >= 8) {
            col = 0;
            row++;
        }
    }
}

void MainWindow::OnCardClicked(QPushButton* button, const QString& card) {
    QPalette palette_active;
    QPalette palette_inactive;
    palette_active.setColor(QPalette::WindowText, Qt::black);
    palette_inactive.setColor(QPalette::WindowText, Qt::green);

    if(button != memory_but)
        two++;
    if(two == 1){
        memory_but = button;
        memory_str = card;
        button->blockSignals(true);
        timer->start(1000);
    }
    QString image_path = ":/pokemon-sprites/" + card;
    QPixmap pixmap(image_path);

    QString image_return = ":/back-cards/dos.png";
    QPixmap pixmap_return(image_return);

    const QSize kCardSize(100, 100);
    QPixmap scaled_pixmap = pixmap.scaled(kCardSize, Qt::KeepAspectRatio);

    QPixmap scaled_return = pixmap_return.scaled(kCardSize, Qt::KeepAspectRatio);

    button->setIcon(QIcon(scaled_pixmap));
    button->setIconSize(kCardSize);

    if(two >= 2){
        if(!memory_str.compare(card)){
            button->setDisabled(true);
            memory_but->setDisabled(true);
            current_user = !current_user;
            if(number_player != 1 && current_user == true){
                player1 += 1;
                ui->point_1->setText(QString::number(player1));
            } else if(number_player != 1 && current_user == false){
                player2++;
                ui->point_2->setText(QString::number(player2));
            }
            victory--;
        } else {
            QTimer::singleShot(1000, [this, button, scaled_return, card](){
                button->setIcon(QIcon(scaled_return));
                this->memory_but->setIcon(QIcon(scaled_return));
            });
        }
        if(number_player != 1)
            current_user = !current_user;
        if(current_user == true){
            ui->point_1->setPalette(palette_active);
            ui->player1->setPalette(palette_active);
            ui->point_2->setPalette(palette_inactive);
            ui->player2->setPalette(palette_inactive);
        } else {
            ui->point_1->setPalette(palette_inactive);
            ui->player1->setPalette(palette_inactive);
            ui->point_2->setPalette(palette_active);
            ui->player2->setPalette(palette_active);
        }
        memory_but->blockSignals(false);
        two = 0;
    }

    if(victory <= 0){
        timer->stop();
        EndGame();
    }
}

void MainWindow::StartGame(){
    QMessageBox msgBox;
    msgBox.setText("Vous désirez jouer à :");

    QPushButton* solo = msgBox.addButton(tr("1 joueur"), QMessageBox::ActionRole);
    QPushButton* duo = msgBox.addButton(tr("2 joueurs"), QMessageBox::ActionRole);


    msgBox.exec();

    if (msgBox.clickedButton() == solo){
        QLayoutItem* item;
        while ((item = ui->player1_grid->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        while ((item = ui->player2_grid->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        number_player = 1;
        easy();
    } else if(msgBox.clickedButton() == duo){
        number_player = 2;
        player1 = 0;
        player2 = 0;
        qDebug() << "player 1 : " << player1;
        current_user = true;
        easy();
    }
}

void MainWindow::EndGame() {
    if(number_player == 1){
        QMessageBox msgBox;
        msgBox.setText("Tu as gagné en " + timeStr + " !");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        QPushButton* restartButton = msgBox.addButton(tr("Recommencer"), QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == restartButton) {
            StartGame();
        }
    } else {
        QMessageBox msgBox;
        if(player1 > player2){
            msgBox.setText("Le joueur 1 a gagné !");
        } else if(player2 > player1){
            msgBox.setText("Le joueur 2 a gagné !");
        } else {
            msgBox.setText("Egalité parfaite !");
        }

        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        QPushButton* restartButton = msgBox.addButton(tr("Recommencer"), QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == restartButton) {
            StartGame();
        }
    }
}

void MainWindow::UpdateTimer() {
    elapsedTime++;
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    timeStr = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    ui->label_timer->setText(timeStr);
}

MainWindow::~MainWindow(){
    delete ui;
}
