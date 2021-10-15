#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define CFG_FILE "prboom+shell.cfg"

#include <QMainWindow>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct GAME{    // Структура конфигурации игры
    QString game_name;      // название
    QString default_flags;  // флаги по-умолчанию
    int default_complevel;  // совместимость по-умолчанию

    GAME():                 // конструктор по-умолчанию
        game_name("None"),
        default_flags(""),
        default_complevel(19)
    {}
    GAME(QString _game_name, QString _default_flags, int default_complevel): // стандартный конструктор
        game_name(_game_name),
        default_flags(_default_flags),
        default_complevel(default_complevel)
    {}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString prboom_cmd;     // команда prboom-plus
    QVector<GAME> games;    // контейнер с играми

public slots:
    void slotPlay();                    // слот кнопки "ИГРАТЬ"
    void slotGameChanged(int index);    // слот замены игры
    void slotParseConfig();             // слот считывания конфига
};
#endif // MAINWINDOW_H
