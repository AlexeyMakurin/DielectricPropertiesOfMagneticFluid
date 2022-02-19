#ifndef READ_FILE_H
#define READ_FILE_H

#include <map>
#include <vector>
#include <string>

//#include <qaxobject.h>

//#include "mainwindow.h"
#include <QAxObject>
#include <QMainWindow>
#include <QFile>

std::vector<QStringList> read_csv(const QString& path);
std::vector<QStringList> read_excel(QMainWindow *mainwindow, const QString& path);
#endif // READ_FILE_H
