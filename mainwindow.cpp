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
    Random_cards(12);
}

void MainWindow::medium(){
    elapsedTime = 0;
    timer->stop();
    ui->label_timer->setText("00:00");
    two = 0;
    victory = 18;
    difficulty = 1;
    Random_cards(18);
}

void MainWindow::hard(){
    elapsedTime = 0;
    timer->stop();
    ui->label_timer->setText("00:00");
    two = 0;
    victory = 24;
    difficulty = 2;
    Random_cards(24);
}

void MainWindow::Random_cards(int _difficulty){
    const int kNumCards = _difficulty * 2;
    const int kCardsPerColor = _difficulty;
    int cnt = 0;

    if(cards.size() > 0){
        cards.clear();
    }

    while (cards.size() < kNumCards) {
        cards << QString::number(cnt) + ".gif";
        cnt = (cnt + 1) % kCardsPerColor;
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
            victory--;
        } else {
            QTimer::singleShot(1000, [this, button, scaled_return, card](){
                button->setIcon(QIcon(scaled_return));
                this->memory_but->setIcon(QIcon(scaled_return));
            });
        }
        memory_but->blockSignals(false);
        two = 0;
    }

    if(victory <= 0){
        EndGame(true);
    }
}

void MainWindow::StartGame(){
    QMessageBox msgBox;
    msgBox.setText("Vous désirez jouer à :");

    QPushButton* solo = msgBox.addButton(tr("1 joueur"), QMessageBox::ActionRole);
    QPushButton* duo = msgBox.addButton(tr("2 joueurs"), QMessageBox::ActionRole);

    if (msgBox.clickedButton() == solo){
        easy();
    } else if(msgBox.clickedButton() == duo){
        easy();
    }

    msgBox.exec();
}

void MainWindow::EndGame(bool win) {
    if(win == false){
        QMessageBox msgBox;
        msgBox.setText("Tu as perdu !");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        QPushButton* restartButton = msgBox.addButton(tr("Recommencer"), QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == restartButton) {
            if(difficulty == 0)
                easy();
            else if(difficulty == 1)
                medium();
            else
                hard();
        }
    } else {
        QMessageBox msgBox;
        msgBox.setText("Tu as gagné !");
            msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        QPushButton* restartButton = msgBox.addButton(tr("Recommencer"), QMessageBox::ActionRole);

        msgBox.exec();

        if (msgBox.clickedButton() == restartButton) {
            if(difficulty == 0)
                easy();
            else if(difficulty == 1)
                medium();
            else
                hard();
        }
    }
}

void MainWindow::UpdateTimer() {
    elapsedTime++;
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    QString timeStr = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    ui->label_timer->setText(timeStr);
}

MainWindow::~MainWindow(){
    delete ui;
}
