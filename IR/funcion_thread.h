#ifndef FUNCION_THREAD_H
#define FUNCION_THREAD_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QVector>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "btree.h"

class funcion_thread : public QObject
{
    Q_OBJECT
public:
    explicit funcion_thread(QObject *parent = 0);
    void start(int, int, QString, QString, terminos *);
    void process_line(QString, int, terminos *);
    void process_term(QString, int, terminos *);

signals:
    void resultReady(int, int);
    void resultPartial(int, int);
    void resultError(QString);

public slots:
    void stop();

private:
    bool mStop;
    QTextStream in, out;
    int accept_word, total_word;
    QVector<QString> stop_word, black_list;
};

#endif // FUNCION_THREAD_H
