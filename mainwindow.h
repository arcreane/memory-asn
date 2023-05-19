#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <algorithm>
#include <random>
#include <chrono>
#include <QTimer>
#include <QLabel>
#include <QtWidgets>
#include <QtCore>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void Random_cards(int diff);
    void Display_cards();
    void OnCardClicked(QPushButton* button, const QString& card);
    void easy();
    void medium();
    void hard();
    void StartGame();
    void EndGame(bool win);
    void UpdateTimer();

private:
    Ui::MainWindow *ui;
    QStringList cards;
    QList<QPushButton*> buttons;
    QPushButton* memory_but;
    QString memory_str;
    int two;
    int victory;
    int difficulty;
    QTimer* timer;
    int elapsedTime;
};
#endif // MAINWINDOW_H
