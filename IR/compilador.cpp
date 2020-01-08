#include "compilador.h"

compilador::compilador(QVector<QString> x, int nd, int nf)
{
    query = x;
    flag  = 0;
    index = 0;
    NDOC  = nd;
    NFILE = nf;
}

bool compilador::isTerm(QString x){
    if(x!="and"&&x!="or"&&x!="not"&&x!="("&&x!=")"){
        for(int i=0; i<x.size(); i++){
            if(!x[i].isLetterOrNumber())
                return 0;
        }
        return 1;
    }
    else return 0;
}

QBitArray compilador::toQBitArray(QString x){
    QBitArray res(DLEN);
    // --------- codigo aqui ----------

    // --------------------------------
    return res;
}

QString compilador::lookahead(){
    return entrada[index];
}

void compilador::match(QString a){
    if(lookahead() != a){
        flag = 1;
    }
    index ++;
}

QBitArray compilador::Query(){
    if(flag)
        return QBitArray();
    if(isTerm(lookahead()) || lookahead() == "(" || lookahead() == "not")
    {
        QBitArray Ecod = Expresion();
        match("fin_query");
        return Ecod;
    }
    flag = 1;
    return QBitArray();
}

QBitArray compilador::Expresion(){
    if(flag)
        return QBitArray();
    if(isTerm(lookahead()) || lookahead()=="(" || lookahead() == "not"){
        QBitArray Tcode = Termino();
        QBitArray Ecode = XTermino(Tcode);
        return Ecode;
    }
    flag = 1;
    return QBitArray();
}

QBitArray compilador::Termino(){
    if(flag)
        return QBitArray();
    if(isTerm(lookahead()) || lookahead()=="(" || lookahead()=="not"){
        QBitArray Fcode = Factor();
        QBitArray Tcode = YFactor(Fcode);
        return Tcode;
    }
    flag = 1;
    return QBitArray();
}

QBitArray compilador::Factor(){
    if(flag)
        return QBitArray();
    if(isTerm(lookahead())){
        QBitArray Fcode = toQBitArray(lookahead());
        match(lookahead());
        return Fcode;
    }
    if(lookahead()=="("){
        match("(");
        QBitArray Fcode = Expresion();
        match(")");
        return Fcode;
    }
    if(lookahead()=="not"){
        match("not");
        QBitArray Fcode = ~Factor();
        return Fcode;
    }
    flag = 1;
    return QBitArray();
}

QBitArray compilador::XTermino(QBitArray Xher){
    if(flag)
        return QBitArray();
    if(lookahead()=="or"){
        match(lookahead());
        QBitArray Tcode = Termino();
        QBitArray X1code = XTermino(Xher | Tcode);
        return X1code;
    }
    if(lookahead()=="fin_query" || lookahead()==")"){
        return Xher;
    }
    flag = 1;
    return QBitArray();
}

QBitArray compilador::YFactor(QBitArray Yher){
    if(flag)
        return QBitArray();
    if(lookahead()=="and"){
        match(lookahead());
        QBitArray Fcode = Factor();
        QBitArray Y1code = YFactor(Yher & Fcode);
        return Y1code;
    }
    if(lookahead()=="fin_query" || lookahead()=="or" || lookahead()==")"){
        return Yher;
    }
    flag = 1;
    return QBitArray();
}
