#include "mainwindow.h"
#include "ui_mainwindow.h"

QString file[] = {"alt.atheism","comp.graphics","comp.os.ms-windows.misc",
                 "comp.sys.ibm.pc.hardware","comp.sys.mac.hardware",
                 "comp.windows.x","misc.forsale","rec.autos","rec.motorcycles",
                 "rec.sport.baseball","rec.sport.hockey","sci.crypt","sci.electronics",
                 "sci.med","sci.space","soc.religion.christian","talk.politics.guns",
                 "talk.politics.mideast","talk.politics.misc","talk.religion.misc"};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont font;
    font.setFamily("Titillium Web");
    font.setFixedPitch(true);
    font.setPointSize(14);

    ui->te_buscar->setFont(font);
    highlighter = new Highlighter(ui->te_buscar->document());

    ui->l_error->setVisible(0);
    ui->te_buscar->setFocus();

    ui->pb_pp->setEnabled(0);
    ui->pb_pa->setEnabled(0);
    ui->pb_ps->setEnabled(0);
    ui->pb_up->setEnabled(0);

    connect(ui->pb_buscar, SIGNAL(clicked()), this, SLOT(buscar()));
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionAcerca_de, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionBuscar, SIGNAL(triggered(bool)), this, SLOT(buscar()));
    connect(ui->action_Nueva_Busqueda, SIGNAL(triggered(bool)), this, SLOT(create_B_tree()));

    ui->actionClose->setShortcut(QKeySequence::Close);
    ui->actionAcerca_de->setShortcut(QKeySequence::HelpContents);
    ui->actionBuscar->setShortcut(QKeySequence::FindNext);
    ui->action_Nueva_Busqueda->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_N);

    NDOC=10;
    NFILE=10;

    QSettings regedit("HKEY_CURRENT_USER\\Software\\IR", QSettings::NativeFormat);
    dir_reuters = regedit.value("dir_reuters").toString();

    create_ind_invertido();
}

void MainWindow::about(){
    QMessageBox::about(this, "About Boolean Retrieval", "<h3 style='color:red'>Hello World</h3>");
}

QBitArray MainWindow::toQBitArray(QString t){

    if(stop_word.contains(t)){
        QBitArray res(NDOC*NFILE, 1);
        return res;
    }
    QBitArray res(NDOC*NFILE);

    t+="$";
    int inicio=0;
    int fin=dicc.size();
    int medio=(inicio+fin)/2;
    int position = -1;

    while(inicio<fin-1){
        if(dicc[medio].first.first == t){
            position  = medio;
            break;
        }
        else if(dicc[medio].first.first < t){
            inicio=medio;
            medio=(inicio+fin)/2;
        }
        else{
            fin=medio;
            medio=(inicio+fin)/2;
        }
    }
    if(!dicc.isEmpty() && dicc[inicio].first.first == t)
        position  = inicio;

    if(position != -1)
        for(int i=0; i<dicc[position].second.size(); i++)
            res[dicc[position].second[i].first] = 1;

    return res;
}

bool MainWindow::isTerm(QString x){
    if(x!="and"&&x!="or"&&x!="not"&&x!="("&&x!=")"){
        for(int i=0; i<x.size(); i++){
            if(!x[i].isLetterOrNumber())
                return 0;
        }
        return 1;
    }
    else return 0;
}

QString MainWindow::lookahead(){
    return entrada[index];
}

void MainWindow::match(QString a){
    if(lookahead() != a){
        flag = 1;
    }
    index ++;
}

QBitArray MainWindow::Query(){
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

QBitArray MainWindow::Expresion(){
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

QBitArray MainWindow::Termino(){
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

QBitArray MainWindow::Factor(){
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

QBitArray MainWindow::XTermino(QBitArray Xher){
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

QBitArray MainWindow::YFactor(QBitArray Yher){
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

QString MainWindow::process_line(QString tx, QVector<QString> vt)
{
    QString res, textResp;
    QString term, term2;
    bool band = 0;
    for(int i=0; i<tx.size(); ){
        if(!tx[i].isLetter()){
            res+=tx[i++];
            continue;
        }
        while(i<tx.size() && tx[i].isLetter())
            term+=tx[i++];

        if(term.size()>=2 && isTerm(term.toLower()) && vt.contains(term.toLower())){
            textResp += (res + "<b>" + term + "</b>");
            band = 1;
            res.clear();
        }
        else res += term;
        term.clear();
    }
    if(band) textResp += res;
    return textResp;
}

Reuters MainWindow::getTitle(int id, QVector<QString> vt)
{
    QStringList list_split = QStringList() << "xref" << "path" << "newsgroups" << "date" << "lines" << "nntp-posting-user" <<
                                              "article-i.d." << "message-id" << "message-i.d." << "references" <<
                                              "nntp-posting-host" << "followup-to" << "sender" << "reply-to" <<
                                              "x-xxmessage-id" << "x-xxdate" << "x-useragent" << "in-reply-to" <<
                                              "x-newsreader" << "moderator" << "x-mailer" << "return-receipt-to" <<
                                              "originator" << "supersedes" << "approved" << "expires";
    QString title= "Not subject";
    QString text = "...";
    QString pLine;
    QString ruta_file = dir_reuters + "/" + file[id/NDOC]+"/"+QString::number(id%NDOC+1)+".txt";
    //qDebug() << ruta_file;
    QFile file(ruta_file);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return Reuters(title,ruta_file,text);
    QTextStream in(&file);

    bool band = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if(!band && line.section(":",0,0).toLower()=="subject"){
            title = line.section("",10);
            band = 1;
        }
        if(!list_split.contains(line.section(":",0,0).toLower())){
            pLine = process_line(line, vt);
            if(!pLine.isEmpty())
                text += " " + pLine + " ...";
        }
    }
    return Reuters(title,ruta_file,text);
}

QString MainWindow::getText(Reuters reut)
{
    return "<html><head/><body><a href='file:///"+ reut.dir + "'>"
           "<span style='font-weight:600; color:#0000ff;'>" + reut.title + "</span></a>"
           "<br><span style='color:#21be2b;'>/"+reut.dir.section("/",-3) + "</span><br>"
           "<span>"+reut.text+"</span></body></html>";
}

QString MainWindow::getCSS(bool x)
{
    if(x)
        return "*{padding:5px}*::hover{background:#EEE}";
    return "*{padding:5px}*::hover{background:none}";
}

void MainWindow::showReference()
{
    int t = indInf;
    if(indSup >= t){
        ui->item1->setText(getText(getTitle(docsRelevantes[t++],entrada)));
        ui->item1->setStyleSheet(getCSS(1));
    }
    else {
        ui->item1->setText("");
        ui->item1->setStyleSheet(getCSS(0));
    }
    if(indSup >= t){
        ui->item2->setText(getText(getTitle(docsRelevantes[t++],entrada)));
        ui->item2->setStyleSheet(getCSS(1));
    }
    else {
        ui->item2->setText("");
        ui->item2->setStyleSheet(getCSS(0));
    }
    if(indSup >= t){
        ui->item3->setText(getText(getTitle(docsRelevantes[t++],entrada)));
        ui->item3->setStyleSheet(getCSS(1));
    }
    else {
        ui->item3->setText("");
        ui->item3->setStyleSheet(getCSS(0));
    }
    if(indSup >= t){
        ui->item4->setText(getText(getTitle(docsRelevantes[t++],entrada)));
        ui->item4->setStyleSheet(getCSS(1));
    }
    else {
        ui->item4->setText("");
        ui->item4->setStyleSheet(getCSS(0));
    }
    if(indSup >= t){
        ui->item5->setText(getText(getTitle(docsRelevantes[t++],entrada)));
        ui->item5->setStyleSheet(getCSS(1));
    }
    else {
        ui->item5->setText("");
        ui->item5->setStyleSheet(getCSS(0));
    }
    ui->l_status->setText("PAGE SHOWN " + QString::number(indInf/5 + 1) + " OF " + QString::number(indSup/5 + 1) +
                          " -- AMOUNT OF DOCUMENTS RECOVERED " + QString::number(indSup+1));
    if(indInf < 5){
        ui->pb_pp->setEnabled(0);
        ui->pb_pa->setEnabled(0);
    }
    else {
        ui->pb_pp->setEnabled(1);
        ui->pb_pa->setEnabled(1);
    }
    if(indSup - indInf < 5){
        ui->pb_ps->setEnabled(0);
        ui->pb_up->setEnabled(0);
    }
    else {
        ui->pb_ps->setEnabled(1);
        ui->pb_up->setEnabled(1);
    }
}

void MainWindow::buscar(){
    int tiempo_inicial = QTime::currentTime().msec()
                       + QTime::currentTime().second()*1000
                       + QTime::currentTime().minute()*60000
                       + QTime::currentTime().hour()*3600000;
    QString query = ui->te_buscar->toPlainText();
    if(query.isEmpty())
        return;
    QString res;
    entrada.clear();
    int i=0;
    // parser
    bool bandnext = 0, bandprev=0, banderror=0;
    while(i<query.size()){
        if(query[i]==' '||query[i]=='\t'||query[i]=='\n'){
            i++;
            continue;
        }
        else if(query[i]=='('||query[i]==')')
            res=query[i++].toLower();
        else while(i<query.size() && query[i]!=' ' && query[i]!='\t' && query[i]!='\n' && query[i]!='(' && query[i]!=')')
            res+=query[i++].toLower();

        if(bandnext){
            if(res=="and" || res=="or"){
                entrada.push_back(res);
                bandprev=1;
                bandnext=0;
            }
            else if(!entrada.isEmpty() && res==")" && entrada.last()=="("){
                entrada.removeLast();
                if(!entrada.isEmpty() && entrada.last()=="not")
                    entrada.removeLast();
                if(!entrada.isEmpty() && (entrada.last()=="and"||entrada.last()=="or"))
                    entrada.removeLast();
            }
            else {
                banderror=1;
                entrada.push_back(res);
            }
        }
        else if(bandprev){
            if((res=="not" || res=="(") || (isTerm(res) && !stop_word.contains(res))){
                entrada.removeLast();
                entrada.push_back(res);
                bandprev=0;
            }
            else if(isTerm(res) && stop_word.contains(res)){
                entrada.removeLast();
                bandprev=0;
                bandnext=1;
            }
            else {
                banderror=1;
                entrada.push_back(res);
            }
        }
        else if(isTerm(res) && stop_word.contains(res)){
            if(!entrada.isEmpty() && (entrada.last()=="and"||entrada.last()=="or"))
                entrada.removeLast();
            else {
                if(!entrada.isEmpty() && entrada.last()=="not")
                    entrada.removeLast();
                if(!entrada.isEmpty() && (entrada.last()=="and"||entrada.last()=="or"))
                    entrada.removeLast();
                if(entrada.isEmpty() || entrada.last()=="(")
                    bandnext=1;
            }
        }
        else entrada.push_back(res);
        res.clear();
    }
    entrada.push_back("fin_query");

    flag  = 0;
    index = 0;
    ui->l_status->clear();
    ui->l_time->clear();
    indInf = indSup = 0;
    docsRelevantes.clear();

    for(int i=0; i<entrada.size(); i++){
        cout<<entrada[i].toStdString()<<' ';
        //ui->l_status->setText(ui->l_status->text()+" "+entrada[i]);
    }
    cout<<endl;

    if(entrada.size()>1){
        resp = Query();
        if(flag || banderror) {
            cout<<"ERROR"<<endl;
            ui->l_status->setText("ERROR");
            ui->l_error->setVisible(1);
        }
        else {
            for(int i=0; i<resp.size(); i++)
                if(resp[i]) {
                    cout<<i<<' ';
                    docsRelevantes.push_back(i);
                }
            cout<<endl;
            indSup = docsRelevantes.size() - 1;
            showReference();
        }
    }
    else ui->l_status->setText("NO PROCESS:"+ui->l_status->text());
    int tiempo_final = QTime::currentTime().msec()
                     + QTime::currentTime().second()*1000
                     + QTime::currentTime().minute()*60000
                     + QTime::currentTime().hour()*3600000;
    ui->l_time->setText("TIME IN MS: " + QString::number(tiempo_final-tiempo_inicial) + " - ");
}

void MainWindow::create_ind_invertido(){
    int i=0, cont=0;

    QString url = qApp->applicationDirPath();
    QString ruta_ii = url + "/indice_invertido.txt";
    QString ruta_sw = url + "/stop_word.txt";
    QString ruta_bl = url + "/black_list.txt";

    QFile file_ii(ruta_ii);
    QFile file_sw(ruta_sw);
    QFile file_bl(ruta_bl);

    if (!file_ii.open(QIODevice::ReadOnly | QIODevice::Text) || !file_sw.open(QIODevice::ReadOnly | QIODevice::Text)
            || !file_bl.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString line, term, f_term, f_doc, doc_id;
    QTextStream in(&file_ii);

    NFILE = in.readLine().toInt();
    NDOC = in.readLine().toInt();

    dicc.clear();

    while (!in.atEnd()) {
        cont = 0;
        f_term = in.readLine();
        term = in.readLine();
        dicc.push_back(qMakePair(par2(term, f_term.toInt()), QVector<par>()));
        while(1){
            f_doc = in.readLine();
            doc_id = in.readLine();
            dicc[i].second.push_back(par(doc_id.toInt(), f_doc.toInt()));
            cont+=f_doc.toInt();
            if(cont>=f_term.toInt())
                break;
        }
        i++;
    }

    stop_word.clear();
    QTextStream in2(&file_sw);
    while (!in2.atEnd()) {
        line = in2.readLine();
        stop_word.push_back(line);
    }

    black_list.clear();
    QTextStream in3(&file_bl);
    while (!in3.atEnd()) {
        line = in3.readLine();
        black_list.push_back(line);
    }

    file_ii.close();
    file_sw.close();
    file_bl.close();
}

void MainWindow::create_B_tree(){
    dialog = new Dialog(this);
    connect(dialog, &Dialog::end_process, this, &MainWindow::create_ind_invertido);
    dialog->show();
}

QString editor2;
void MainWindow::on_te_buscar_textChanged()
{
    ui->l_error->setVisible(0);
    QString editor = ui->te_buscar->toPlainText();
    if(editor2 != editor && editor.size()>0 && editor.contains(QChar('\n'))){
        editor2 = editor.remove(QChar('\n'), Qt::CaseInsensitive);
        ui->te_buscar->setPlainText(editor2);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_pp_clicked()
{
    indInf=0;
    showReference();
}

void MainWindow::on_pb_pa_clicked()
{
    indInf-=5;
    showReference();
}

void MainWindow::on_pb_ps_clicked()
{
    indInf+=5;
    showReference();
}

void MainWindow::on_pb_up_clicked()
{
    indInf = (indSup/5)*5;
    showReference();
}
