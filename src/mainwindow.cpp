#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , prboom_cmd("prboom-plus") // команда prboom-plus по-умолчанию
{
    ui->setupUi(this);

    connect(ui->play_but,SIGNAL(clicked()),SLOT(slotPlay()));
    connect(ui->parse_but,SIGNAL(clicked()),SLOT(slotParseConfig()));
    connect(ui->game,SIGNAL(currentIndexChanged(int)),SLOT(slotGameChanged(int)));

    slotParseConfig();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotParseConfig(){
    games.clear();              // очищаем вектор игр
    games.push_back(GAME());    // добавляем пустую игру

    QFile cfg_file(CFG_FILE);
    if(!cfg_file.open(QIODevice::ReadOnly | QIODevice::Text)){  // открываем файл конфигурации
        QMessageBox msgBox;
        msgBox.setText("Файл конфигурации не найден!");         // если не открыли - выводим сообщение об ошибке
        msgBox.exec();
    }
    else{
        QTextStream in(&cfg_file);
        QString line;
        bool got_line = false;  // флаг того, что строка уже считана
        while (!in.atEnd() || got_line) {   // пока не достигнут конец файла или строка уже считана
            if(!got_line)       // если строка
                line = in.readLine();   // читаем строку
            got_line = false;           // сразу чистим флаг
            int tab_idx = line.indexOf('\t');   // ищем символ табуляции
            if(tab_idx == -1)                   // не нашли - продолжаем цикл
                continue;

            QString par_name = line.left(tab_idx);
            if(par_name == "prboom-plus"){  // если параметр - команда prboom-plus
                prboom_cmd = line.right(line.size()-tab_idx-1); // записываем её
                continue;
            }
            else if(par_name != "game_name")    // если параметр - не название игры...
                continue;                       // ...продолжаем цикл

            QString game_name;
            QString default_flags;
            int default_complevel;

            game_name = line.right(line.size()-tab_idx-1);  // записываем название игры

            line = in.readLine();                           // читаем новую строку
            tab_idx = line.indexOf('\t');                   // ищем символ табуляции
            if(tab_idx == -1)                               // не нашли - продолжаем цикл
                continue;
            par_name = line.left(tab_idx);
            if(par_name != "default_flags"){                // если параметр - НЕ default_flags
                got_line = true;                            // отмечаем флаг считанной строки...
                continue;                                   // ...и продолжаем цикл
            }
            default_flags = line.right(line.size()-tab_idx-1);  // записываем default_flags

            line = in.readLine();                           // читаем новую строку
            tab_idx = line.indexOf('\t');                   // ищем символ табуляции
            if(tab_idx == -1)                               // не нашли - продолжаем цикл
                continue;
            par_name = line.left(tab_idx);
            if(par_name != "default_complevel"){            // если параметр - НЕ default_complevel
                got_line = true;                            // отмечаем флаг считанной строки...
                continue;                                   // ...и продолжаем цикл
            }
            bool _ok = false;
            default_complevel = line.right(line.size()-tab_idx-1).toInt(&_ok);  // записываем default_complevel
            if(!_ok || default_complevel > 17 || default_complevel < -1)        // неверный complevel...
                default_complevel = 19;                                         // ...пишем, что нет флага complevel
            else if(default_complevel == -1)                                    // complevel = -1...
                default_complevel = 18;                                         // ...меняем на 18 в соответствии с ComboBox

            games.push_back(GAME(game_name,default_flags,default_complevel));   // записываем игру в контейнер
        }
    }
    cfg_file.close();                       // закрываем файл
    ui->game->clear();                      // очищаем список игр
    for(auto& elem : games)
        ui->game->addItem(elem.game_name);  // заполняем список из контейнера игр
}

void MainWindow::slotPlay(){
    QString _command = prboom_cmd;  // в финальную командную строку добавляем команду prboom-plus

    if(!games[ui->game->currentIndex()].default_flags.isEmpty())
        _command += " " + games[ui->game->currentIndex()].default_flags;    // добавляем флаги по-умолчанию

    if(ui->fastmonsters->isChecked())
        _command += " -fast";           // добавляем быстрых монстров, если требуется

    int complev = ui->complevel->currentIndex();        // добавляем complevel, если требуется
    if(complev == 18)
        _command += " -complevel -1";
    else if(complev < 18)
        _command += " -complevel " + QString::number(complev);

    if(!ui->extraflags->text().isEmpty())               // добавляем дополнительные флаги
        _command += " " + ui->extraflags->text();

    QProcess *myProcess = new QProcess(this);           // вызываем команду в новом процессе
    myProcess->startDetached(_command);
    //myProcess->waitForFinished(-1);                     // если надо дождаться завершения игры
}

void MainWindow::slotGameChanged(int index){
    if(index >= 0)
        ui->complevel->setCurrentIndex(games[index].default_complevel);     // если изменилась игра - изменить текущий complevel на дефолтный
}
