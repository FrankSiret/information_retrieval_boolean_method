#ifndef COMPILADOR_H
#define COMPILADOR_H
#include <QString>
#include <QBitArray>

class compilador
{
private:
    QVector<QString> consulta;
    bool flag;
    int index;
    int NDOC, NFILE;

public:
    compilador(QVector<QString>, int, int);

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
};

#endif // COMPILADOR_H
