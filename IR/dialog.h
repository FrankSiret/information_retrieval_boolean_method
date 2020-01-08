#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QShowEvent>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QtConcurrent>
#include <QFileDialog>
#include "funcion_thread.h"
#include <QSettings>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *);
    ~Dialog();

public slots:
    void handleReady(int, int);
    void handlePartial(int, int);
    void handleError(QString);
    void save_in_file();
    void pre_orden(nodot *);
    void on_pb_start_clicked();

signals:
    void end_process();
    void on_stop();

private slots:
    void on_pb_cargar_directorio_clicked();

private:
    Ui::Dialog *ui;
    QWidget *p;
    bool Thtml;
    funcion_thread fun_thread;
    terminos *BTree;
    bool complete;
    int NFILE, NDOC;
    int accept_word, total_word, total_term;
    QString ruta_docs, ruta_app;
    QTextStream out;
    int tiempo_inicial, tiempo_final, tiempo;
};

#endif // DIALOG_H
