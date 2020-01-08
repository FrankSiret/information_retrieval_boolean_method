#include "funcion_thread.h"

QString file2[] = {"alt.atheism","comp.graphics","comp.os.ms-windows.misc",
                 "comp.sys.ibm.pc.hardware","comp.sys.mac.hardware",
                 "comp.windows.x","misc.forsale","rec.autos","rec.motorcycles",
                 "rec.sport.baseball","rec.sport.hockey","sci.crypt","sci.electronics",
                 "sci.med","sci.space","soc.religion.christian","talk.politics.guns",
                 "talk.politics.mideast","talk.politics.misc","talk.religion.misc"};

funcion_thread::funcion_thread(QObject *parent) : QObject(parent)
{
    mStop = false;
}

void funcion_thread::start(int NFILE, int NDOC, QString ruta_app, QString ruta_docs, terminos *BTree)
{
    accept_word = total_word = 0;
    mStop = false;
    int DocID;
    QString line;
    QFile file_docs;

    QString ruta_sw = ruta_app + "/stop_word.txt";
    QString ruta_bl = ruta_app + "/black_list.txt";

    QFile file_sw(ruta_sw);
    QFile file_bl(ruta_bl);

    if(mStop) return;

    if (!file_sw.open(QIODevice::ReadOnly | QIODevice::Text) || !file_bl.open(QIODevice::ReadOnly | QIODevice::Text)){
        mStop = true;
        emit resultError("stop_word, black_list");
        qDebug() << "ERROR: stop_word, black_list";
        return;
    }

    stop_word.clear();
    in.setDevice(&file_sw);
    while (!in.atEnd()) {
        line = in.readLine();
        stop_word.push_back(line);
    }

    black_list.clear();
    in.setDevice(&file_bl);
    while (!in.atEnd()) {
        line = in.readLine();
        black_list.push_back(line);
    }

    file_sw.close();
    file_bl.close();

    for(int i=0; i<NFILE; i++) {
        for(int j=1; j<=NDOC && !mStop; j++) {
            QString dir = file2[i] + "/" + QString::number(j) + ".txt";
            DocID = i*NDOC + j - 1;
            file_docs.setFileName(ruta_docs + "/" + dir);
            if(!file_docs.open(QIODevice::ReadOnly | QIODevice::Text)) {
                mStop = true;
                emit resultError("reuters: " + dir);
                qDebug() << "ERROR: reuters: " + dir;
                return;
            }
            in.setDevice(&file_docs);
            while(!in.atEnd() && !mStop){
                line = in.readLine();
                process_line(line, DocID, BTree);
            }
            file_docs.close();
            emit resultPartial(i, j);
        }
    }
    if(!mStop)
        emit resultReady(accept_word, total_word);
}

void funcion_thread::process_line(QString line, int id, terminos *BTree)
{
    QString split = line.section(":",0,0).toLower();
    QStringList list_split = QStringList() << "xref" << "path" << "newsgroups" << "date" << "lines" << "nntp-posting-user" <<
                                              "article-i.d." << "message-id" << "message-i.d." << "references" <<
                                              "nntp-posting-host" << "followup-to" << "sender" << "reply-to" <<
                                              "x-xxmessage-id" << "x-xxdate" << "x-useragent" << "in-reply-to" <<
                                              "x-newsreader" << "moderator" << "x-mailer" << "return-receipt-to" <<
                                              "originator" << "supersedes" << "approved" << "expires";
    if(list_split.contains(split))
        return;
    QString term;
    for(int i=0; i<line.size(); i++){
        if(!line[i].isLetter())
            continue;
        while(i<line.size() && line[i].isLetter())
            term+=line[i++].toLower();
        if(term.size()>=2)
            process_term(term, id, BTree);
        term.clear();
    }
}

void funcion_thread::process_term(QString term, int id, terminos *BTree)
{
    if(!stop_word.contains(term) && !black_list.contains(term)){
        accept_word ++;
        BTree->insert(term, id);
    }
    total_word ++;
}

void funcion_thread::stop()
{
    mStop = true;
}
