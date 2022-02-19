#ifndef DOWNLOAD_FILE_H
#define DOWNLOAD_FILE_H

#include <QDialog>
#include <QTableWidget>
#include <QMessageBox>

#include <map>

namespace Ui {
class download_file;
}

class download_file : public QDialog
{
    Q_OBJECT

public:
    explicit download_file(QWidget *parent = nullptr);
    ~download_file();

    void add_data(std::vector<QStringList> data);
    std::map<int, double> GetFreq() const;
    std::map<int, double> GetC() const;
    std::map<int, double> GetD() const;

private slots:
    void on_download_freq_clicked();

    void on_download_c_clicked();

    void on_dowload_d_clicked();

private:
    Ui::download_file *ui;
    QTableWidgetItem *protoitem = new QTableWidgetItem();

    void read_column(std::map<int, double>& map, const int& col);

    std::map<int, double> frequency;
    std::map<int, double> capacity;
    std::map<int, double> tan_d;
};

#endif // DOWNLOAD_FILE_H
