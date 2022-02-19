#include "download_file.h"
#include "ui_download_file.h"


download_file::download_file(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::download_file)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->setColumnCount(1);

    //ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectColumns);

    protoitem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget->setItemPrototype(protoitem);
}

download_file::~download_file()
{
    delete ui;
}


void download_file::add_data(std::vector<QStringList> data) {

    for (int row = 0; row < data.size(); ++row) {
        if (row >= ui->tableWidget->rowCount()) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        }

        for (int col = 0; col < data[row].size(); ++col) {

            if (col >= ui->tableWidget->columnCount()) {
                ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
            }

            QTableWidgetItem *item = protoitem->clone();
            item->setText(data[row].at(col));
            ui->tableWidget->setItem(row, col, item);
        }
    }
}

void download_file::on_download_freq_clicked() {
    int col = ui->tableWidget->currentColumn();

    if (!frequency.empty()) {
        auto response = QMessageBox::information(this, "replace", "Do you want to replace the current data",
                                                 QMessageBox::Yes, QMessageBox::No);

        if (response == QMessageBox::StandardButton::Yes) {
            read_column(frequency, col);
        }

    } else {
        read_column(frequency, col);
    }

}


void download_file::on_download_c_clicked() {
    int col = ui->tableWidget->currentColumn();

    if (!capacity.empty()) {
        auto response = QMessageBox::information(this, "replace", "Do you want to replace the current data",
                                                 QMessageBox::Yes, QMessageBox::No);

        if (response == QMessageBox::StandardButton::Yes) {
            read_column(capacity, col);
        }

    } else {
        read_column(capacity, col);
    }
}


void download_file::on_dowload_d_clicked() {
    int col = ui->tableWidget->currentColumn();

    if (!tan_d.empty()) {
        auto response = QMessageBox::information(this, "replace", "Do you want to replace the current data",
                                                 QMessageBox::Yes, QMessageBox::No);

        if (response == QMessageBox::StandardButton::Yes) {
            read_column(tan_d, col);
        }

    } else {
        read_column(tan_d, col);
    }
}


void download_file::read_column(std::map<int, double>& map, const int& col) {
    if (col < 0) {
        QMessageBox::information(this, "...", "Choose a column!");
    } else {
        int row = 0;
        for (int index = 0; index < ui->tableWidget->rowCount(); ++index) {
            QString line = ui->tableWidget->item(index, col)->text();
            bool num;
            double value = line.toDouble(&num);

            if (num) {
                map[row] = value;
                ++row;
            } else if (line == "") {
                ++row;
            }
        }
    }
}

std::map<int, double> download_file::GetFreq() const {
    return frequency;
}

std::map<int, double> download_file::GetC() const {
    return capacity;
}

std::map<int, double> download_file::GetD() const {
    return tan_d;
}







