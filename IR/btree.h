#ifndef BTREE_H
#define BTREE_H

#include <bits/stdc++.h>
#include <QString>
#include <QChar>

using namespace std;

// clase nodo de la lista diccionario
class nodod {
private:
  int doc_id;
  int frequency_doc;
  nodod* next = NULL;

public:
  nodod(int, int);
  nodod(const nodod &);
  operator=(const nodod &);
  int get_doc_id() {return doc_id;}
  int get_freq_doc() {return frequency_doc;}
  nodod* get_next() {return next;}
  ~nodod();
  friend class diccionario;
};

// clase diccionario
class diccionario {
private:
  nodod* begin = NULL;

public:
  diccionario(){}
  void insert(int);
  nodod* get_begin() {return begin;}
  ~diccionario(){}
};

// clase nodo del arbol-B terminos
class nodot {
private:
  QChar letter;
  QString term;
  int frequency_term;
  nodot* child = NULL;
  nodot* next = NULL;
  diccionario first;

public:
  nodot(QChar, QString, int);
  nodot(const nodot &);
  operator=(const nodot &);
  QChar get_letter() {return letter;}
  QString get_term() {return term;}
  int get_freq_term() {return frequency_term;}
  nodot* get_child() {return child;}
  nodot* get_next() {return next;}
  diccionario get_first() {return first;}
  ~nodot();
  friend class terminos;
};

// clase terminos
class terminos {
private:
  nodot* root = NULL;
public:
  terminos();
  void insert(QString, int);
  nodot* get_root() {return root;}
  ~terminos(){}
};

#endif // BTREE_H
