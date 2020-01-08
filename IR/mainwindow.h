#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "highlighter.h"
#include "dialog.h"
#include <bits/stdc++.h>
#include <QMainWindow>
#include <QMessageBox>
#include <QShowEvent>
#include <QVector>
#include <QBitArray>
#include <QPair>
#include <QTextStream>
#include <QFileDialog>
#include <QEvent>
#include <QDebug>
#include <QSettings>

typedef QPair<int,int> par;
typedef QPair<QString,int> par2;

namespace Ui {
class MainWindow;
}

struct Reuters{
    QString title;
    QString dir;
    QString text;
    Reuters(QString t="", QString d="", QString tx=""){
        title = t;
        dir = d;
        text = tx;
    }
    Reuters(const Reuters &r){
        title = r.title;
        dir = r.dir;
        text = r.text;
    }
    void operator=(const Reuters &r){
        title = r.title;
        dir = r.dir;
        text = r.text;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool isTerm(QString);
    QBitArray toQBitArray(QString);
    void match(QString);
    QString lookahead();
    QBitArray Query();
    QBitArray Expresion();
    QBitArray Termino();
    QBitArray Factor();
    QBitArray XTermino(QBitArray);
    QBitArray YFactor(QBitArray);
    QString process_line(QString, QVector<QString>);
    Reuters getTitle(int, QVector<QString>);
    QString getText(Reuters);
    QString getCSS(bool);
    void showReference();
    ~MainWindow();

private slots:
    void buscar();
    void about();
    void create_B_tree();
    void on_te_buscar_textChanged();
    void create_ind_invertido();
    void on_pb_pp_clicked();
    void on_pb_pa_clicked();
    void on_pb_ps_clicked();
    void on_pb_up_clicked();

private:
    Ui::MainWindow *ui;
    Dialog *dialog;

    Highlighter *highlighter;

    QVector<QString> entrada;
    bool flag;
    int index;

    int NDOC, NFILE;
    QString dir_reuters;

    QVector<QPair<par2, QVector<par> > > dicc;
    QVector<QString> stop_word;
    QVector<QString> black_list;

    int indInf, indSup;
    QVector<int> docsRelevantes;
    QBitArray resp;
};

#endif // MAINWINDOW_H
