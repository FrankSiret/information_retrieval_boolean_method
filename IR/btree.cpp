#include "btree.h"

nodod::nodod(int t=0, int f=0)
{
  doc_id = t;
  frequency_doc = f;
  next = NULL;
}

nodod::nodod(const nodod &x)
{
  doc_id = x.doc_id;
  frequency_doc = x.frequency_doc;
  next = x.next;
}

nodod::operator=(const nodod &x)
{
  doc_id = x.doc_id;
  frequency_doc = x.frequency_doc;
  next = x.next;
}

nodod::~nodod()
{
  delete next;
}

void diccionario::insert(int d)
{
  if(begin == NULL) {
    nodod* a=new nodod(d,1);
    begin=a;
  }
  else {
    nodod* p=begin;
    if(p->doc_id == d)
      p->frequency_doc++;
    else{
      for(; p->next!=NULL; p=p->next)
        if(p->next->doc_id==d){
          p->next->frequency_doc++;
          break;
        }
      if(p->next==NULL)
        p->next = new nodod(d, 1);
    }
  }
}

nodot::nodot(QChar l = '$', QString t="", int f=0)
{
  letter = l;
  term = t;
  frequency_term = f;
  child = NULL;
  next = NULL;
}

nodot::nodot(const nodot &x)
{
  letter = x.letter;
  term = x.term;
  frequency_term = x.frequency_term;
  child = x.child;
  next = x.next;
}

nodot::operator=(const nodot &x)
{
  letter = x.letter;
  term = x.term;
  frequency_term = x.frequency_term;
  child = x.child;
  next = x.next;
}

nodot::~nodot()
{
  delete child;
  delete next;
}

terminos::terminos()
{
  root = new nodot('@',"root",0);
}

void terminos::insert(QString t, int d)
{
  t+="$";
  nodot* p=root;
  nodot* q;

  for(int ind=0; ind<t.size(); ind++){
    if(p->child==NULL) {
      p->child = new nodot(t[ind], t.section("", 1, ind+1), 1);
      if(ind==t.size()-1) p->child->first.insert(d);
      p=p->child;
    }
    else {
      q=p->child;
      if(q->letter==t[ind]){
        q->frequency_term++;
        if(ind==t.size()-1) q->first.insert(d);
        p=q;
      }
      else if(q->letter>t[ind]){
        nodot* a=new nodot(t[ind],t.section("", 1, ind+1), 1);
        if(ind==t.size()-1) a->first.insert(d);
        a->next=q;
        p->child=a;
        p=a;
      }
      else {
        if(q->next==NULL){
          nodot* a=new nodot(t[ind],t.section("", 1, ind+1), 1);
          if(ind==t.size()-1) a->first.insert(d);
          q->next=a;
          p=a;
        }
        else {
          bool flag=0;
          for(; q->next!=NULL && q->next->letter<=t[ind]; q=q->next)
            if(q->next->letter==t[ind]){
              q=q->next;
              q->frequency_term++;
              if(ind==t.size()-1) q->first.insert(d);
              p=q;
              flag=1;
              break;
            }
          if(!flag){
            nodot* a=new nodot(t[ind],t.section("", 1, ind+1), 1);
            if(ind==t.size()-1) a->first.insert(d);
            a->next=q->next;
            q->next=a;
            p=a;
          }
        }
      }
    }
  }
}
