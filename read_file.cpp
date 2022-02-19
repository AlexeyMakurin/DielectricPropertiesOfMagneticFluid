#include "read_file.h"

std::vector<QStringList> read_csv(const QString& path) {

    QFile file(path);
    //QTextStream in(&file);

    std::vector<QStringList> data;
    if(file.open(QIODevice::ReadOnly | QFile::Text)){
        while(!file.atEnd()) {
            QString line = file.readLine();
            QStringList list_line = line.split(",");
            data.push_back(list_line);
        }
    }
    file.close();

    return data;
}


std::vector<QStringList> read_excel(QMainWindow *mainwindow,const QString& path) {
    QString file(path);

    QAxObject* excel = new QAxObject("Excel.Application", mainwindow);

    QAxObject* workbooks = excel->querySubObject("Workbooks");
    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", file);
    excel->dynamicCall("SetVisible(bool)", false);

    QAxObject* worksheet = workbook->querySubObject("WorkSheets(int)", 1);

    QAxObject* usedrange = worksheet->querySubObject("UsedRange");
    QAxObject* rows = usedrange->querySubObject("Rows");
    QAxObject* columns = usedrange->querySubObject("Columns");

    int intCols = columns->property("Count").toInt();
    int intRows = rows->property("Count").toInt();

    std::vector<QStringList> result;

    for (int row=0; row < intRows; ++row) {
        QStringList line;
        for (int col = 0; col < intCols; ++col) {
            QAxObject* cell = worksheet->querySubObject("Cells(int, int)", row + 1, col + 1);
            QVariant value = cell->dynamicCall("Value()");
            line.append(value.toString());
        }
        result.push_back(line);
    }

    workbook->dynamicCall("Close");
    excel->dynamicCall("Quit()");

    return result;
}
