#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    complete = false;
    NFILE = NDOC = 10;
    ui->sb_file->setValue(10);
    ui->sb_doc->setValue(10);

    ruta_app = qApp->applicationDirPath();

    ui->sb_doc->setEnabled(false);
    ui->sb_file->setEnabled(false);
    ui->pb_start->setEnabled(false);
}

void Dialog::closeEvent(QCloseEvent *)
{
    if(complete){
        emit on_stop();
        QMessageBox::information(this, "Finalizado", "Nuevo índice de búsqueda creado correctamente."
                                                     "<br><b>total term:</b> " + QString::number(total_term) +
                                                     "<br><b>accepted word:</b> " + QString::number(accept_word) +
                                                     "<br><b>denied word:</b> " + QString::number(total_word - accept_word) +
                                                     "<br><b>execution time (ms):</b> " + QString::number(tiempo));
        emit end_process();
    }
    else {
        emit on_stop();
        QMessageBox::critical(this, "Error", "Error, indice no creado");
    }
}

void Dialog::showEvent(QShowEvent *)
{

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::handleReady(int acc, int tot)
{
    tiempo_final = QTime::currentTime().msec()
                 + QTime::currentTime().second()*1000
                 + QTime::currentTime().minute()*60000
                 + QTime::currentTime().hour()*3600000;
    accept_word = acc;
    total_word = tot;
    total_term = 0;
    tiempo = tiempo_final - tiempo_inicial;
    complete = true;

    save_in_file();
    ui->pb_main->setValue(ui->pb_main->value()+1);

    close();
}

void Dialog::handlePartial(int ifile, int jdoc)
{
    ui->l_staus->setText(((ifile+1<10)?"0":"") + QString::number(ifile+1) + "/" + QString::number(NFILE) + " Cargando...");
    ui->pb_main->setValue(ui->pb_main->value()+1);
}

void Dialog::handleError(QString error)
{
    QMessageBox::critical(this, "ERROR", error);
    close();
}

void Dialog::save_in_file()
{
    QString ruta_ii = ruta_app + "/indice_invertido.txt";
    QFile file_out(ruta_ii);

    if(!file_out.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "ERROR: indice_invertido";
        return;
    }

    out.setDevice(&file_out);
    out << NFILE << "\n";
    out << NDOC << "\n";
    pre_orden(BTree->get_root()->get_child());

    file_out.close();
}

void Dialog::pre_orden(nodot *it)
{
    if(it==NULL)return;
    if(it->get_letter()=='$') {
        out << it->get_freq_term() << "\n";
        out << it->get_term() << "\n";
        total_term ++;
        for(nodod* jt = it->get_first().get_begin(); jt != NULL; jt=jt->get_next()){
            out << jt->get_freq_doc() << "\n";
            out << jt->get_doc_id() << "\n";
        }
    }
    else if(it->get_child()!=NULL)
        pre_orden(it->get_child());
    if(it->get_next()!=NULL)
        pre_orden(it->get_next());
}

void Dialog::on_pb_start_clicked()
{
    tiempo_inicial = QTime::currentTime().msec()
                   + QTime::currentTime().second()*1000
                   + QTime::currentTime().minute()*60000
                   + QTime::currentTime().hour()*3600000;
    ui->sb_doc->setEnabled(false);
    ui->sb_file->setEnabled(false);
    ui->pb_start->setEnabled(false);
    ui->pb_cargar_directorio->setEnabled(false);

    NFILE = ui->sb_file->value();
    NDOC = ui->sb_doc->value();
    ui->pb_main->setMaximum(NFILE*NDOC+1);

    connect(&fun_thread, &funcion_thread::resultReady, this, &Dialog::handleReady);
    connect(&fun_thread, &funcion_thread::resultPartial, this, &Dialog::handlePartial);
    connect(&fun_thread, &funcion_thread::resultError, this, &Dialog::handleError);
    connect(this, &Dialog::on_stop, &fun_thread, &funcion_thread::stop);

    BTree = new terminos;
    QFuture<void> procesar = QtConcurrent::run(&this->fun_thread, &funcion_thread::start, NFILE, NDOC, ruta_app, ruta_docs, BTree);
}

void Dialog::on_pb_cargar_directorio_clicked()
{
    QSettings regedit("HKEY_CURRENT_USER\\Software\\IR", QSettings::NativeFormat);
    ruta_docs  = QFileDialog::getExistingDirectory(this, "Seleccionar Directorio",
                    regedit.value("dir_reuters", qApp->applicationDirPath()).toString(),
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(ruta_docs.isEmpty()){
        QMessageBox::critical(this, "ERROR", "No se ha selecionado un directorio");
        return;
    }
    else {
        regedit.setValue("dir_reuters", ruta_docs);
        ui->sb_doc->setEnabled(true);
        ui->sb_file->setEnabled(true);
        ui->pb_start->setEnabled(true);
    }

    QString folder = ruta_docs.section("/",-1);
    qDebug() << folder;
    ui->sb_file->setMaximum(20);
    ui->sb_doc->setMaximum(1000);
    Thtml = false;
    ui->l_dir->setText(folder);
}
