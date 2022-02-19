#include "mainwindow.h"
#include "ui_mainwindow.h"



MagneticFluid mf(1.53e-3, 135.32, 0.0123, 300.0);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->SetingChartEpsilon->setHidden(hidden_setting_e);
    ui->frame->setHidden(hidden_frame);

    ui->lineEdit_C0->setText(QString::number(mf.c0_));
    ui->lineEdit_D0->setText(QString::number(mf.tan_d0_));
    ui->lineEdit_eta->setText(QString::number(mf.eta_));
    ui->lineEdit_temp->setText(QString::number(mf.T_));

    ui->tableWidget->setRowCount(14);
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setHorizontalHeaderLabels(
                QStringList () << "frequency (kHz)" << "C (pF)" << "D"
                << "Re(epsilon)" << "Im(epsilon)" << "Radius (nm)" << "Сoncentration"
    );


    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    protoitem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget->setItemPrototype(protoitem);

    axisY->setTitleText("concentration");
    axisY->setRange(0, 1);
    axisY->setMinorTickCount(4);
    chart_dist->addAxis(axisY, Qt::AlignLeft);
    axisX->setTitleText("Radius (nm)");

    chart_dist->setTitle("Particle size distribution");
    view_dist->setChart(chart_dist);
    view_dist->setRenderHint(QPainter::Antialiasing);
    ui->distributionLayout->addWidget(view_dist);

    series_re_e->setName("Re(epsilon)");
    series_im_e->setName("Im(epsilon)");

    CreateChart(ui->epsilonLayout, view_epsilon, chart_epsilon, {series_re_e, series_im_e}, "frequency (kHz)", "epsilon", x_max_e, y_max_e, true);
    CreateChart(ui->coulcoulLayout, view_coul, chart_coul, {series_coul}, "Re(epsiloin)", "Im(epsilon)", x_max_coul, y_max_coul);

    QObject::connect(series_re_e, &QScatterSeries::pointAdded, [=](){
        AutoScale(chart_epsilon, series_re_e, x_max_e, y_max_e);
    });

    QObject::connect(series_re_e, &QScatterSeries::pointReplaced, [=]() {
        AutoScale(chart_epsilon, series_re_e, x_max_e, y_max_e);
    });

    QObject::connect(series_coul, &QScatterSeries::pointReplaced, [=](){
       AutoScale(chart_coul, series_coul, x_max_coul, y_max_coul);
    });

    QObject::connect(series_coul, &QScatterSeries::pointAdded, [=](){
        AutoScale(chart_coul, series_coul, x_max_coul, y_max_coul);
    });

    QObject::connect(ui->tableWidget, &QTableWidget::cellChanged, [=](int row, int col){

        double value =  ui->tableWidget->item(row, col)->text().toDouble();
        if (row >= ui->tableWidget->rowCount() - 2) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        }

        switch (col) {

        case 0:
            add_point(series_re_e, mf.frequency, mf.re_epsilon, row, value);
            add_point(series_im_e, mf.frequency, mf.im_epsilon, row, value);
            mf.frequency[row] = value;
            break;

        case 1:
            add_point(series_re_e, mf.re_epsilon, mf.frequency, row, 0, mf.ReEpsilon(value));

            if (mf.tan_d[row]) {
                add_point(series_im_e, mf.capacity, mf.frequency, row, 0, mf.ImEpsilon(value, mf.tan_d[row]));
                add_point(series_coul, mf.re_epsilon, mf.tan_d, row, mf.ReEpsilon(value), mf.ImEpsilon(value, mf.tan_d[row]));

                mf.im_epsilon[row] = mf.ImEpsilon(value, mf.tan_d[row]);
                allowed_addition(mf.im_epsilon[row], row, 4);
            }

            mf.capacity[row] = value;
            mf.re_epsilon[row] = mf.ReEpsilon(value);
            allowed_addition(mf.re_epsilon[row], row, 3);
            break;

        case 2:

            if (mf.capacity[row]) {
                add_point(series_im_e, mf.im_epsilon, mf.frequency, row, 0, mf.ImEpsilon(mf.capacity[row], value));
                add_point(series_coul, mf.im_epsilon, mf.re_epsilon, row, 0, mf.ImEpsilon(mf.capacity[row], value));

                mf.im_epsilon[row] = mf.ImEpsilon(mf.capacity[row], value);
                allowed_addition(mf.im_epsilon[row], row, 4);
            }

            mf.tan_d[row] = value;
            break;

        default:
            if (act_cell) {
                QMessageBox::warning(this, "Warning", "Non-measurable quantities cannot be changed directly");
                act_cell = false;
                ui->tableWidget->setItem(row, col, new QTableWidgetItem(""));
            }
            act_cell = true;
            break;
       }

    });

}


//void MainWindow::

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::AutoScale(QChart *chart, QScatterSeries *series, double& xMax, double& yMax){
      xMax = 0;
      yMax = 0;

      for (auto p : series->points()){
           xMax = qMax(xMax, p.x());
           yMax = qMax(yMax, p.y());
      }

      chart->axisX()->setRange(0, xMax * 1.1);
      chart->axisY()->setRange(0, yMax * 1.1);
}


void MainWindow::allowed_addition(const double& item, const int& row, const int& col) {
    act_cell = false;
    QTableWidgetItem *newitem = protoitem->clone();
    newitem->setText(QString::number(item));
    newitem->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(row, col, newitem);
}


void MainWindow::add_point(QScatterSeries *series, std::map<int, double>& map1, std::map<int, double>& map2,
                           const int& row, const double& value1, const double& value2) {
    if (!map2[row]) {
        return;
    }

    if (value1 && !value2 && map1[row]) {
        series->replace(row, value1, map2[row]);

    } else if (value2 && !value1 && map1[row]) {
        series->replace(row, map2[row], value2);

    } else if (value1 && value2 && map1[row]) {
        series->replace(row, value1, value2);

    } else if (value1 && !value2 && !map1[row]) {
        series->append(value1, map2[row]);

    } else if (value2 && !value1 && !map1[row]) {
        series->append(map2[row], value2);

    }  else if (value1 && value2 && !map1[row]) {
        series->append(value1, value2);

    } else {
        return;
    }
}

void MainWindow::addition(const std::map<int, double>& field, const int& row, const int& col) {
    if (col > 2) {
        return;
    }
    if (field.count(row)) {
        QTableWidgetItem *newitem = protoitem->clone();
        newitem->setText(QString::number(field.at(row)));
        ui->tableWidget->setItem(row, col, newitem);
    }
}


void MainWindow::CreateChart(QLayout *layout, QChartView *view, QChart *chart, std::vector<QScatterSeries*> series,
                             const QString& ax, const QString &ay, qreal& x_max, qreal& y_max, bool legend) {

    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;

    axisX->setRange(0, x_max);
    axisX->setTickCount(12);
    axisX->setMinorTickCount(4);

    QPen pen_minor;
    pen_minor.setColor("#A3D9FC");
    pen_minor.setStyle(Qt::DotLine);

    QPen pen;
    pen.setColor("#6CC4FF");
    pen.setStyle(Qt::DashLine);

    axisX->setMinorGridLinePen(pen_minor);
    axisX->setGridLinePen(pen);
    axisX->setLabelFormat("%.2f");
    axisX->setTitleText(ax);

    axisY->setRange(0, y_max);
    axisY->setTickCount(12);
    axisY->setMinorTickCount(4);
    axisY->setMinorGridLinePen(pen_minor);
    axisY->setGridLinePen(pen);
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText(ay);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for (int i = 0; i < series.size(); ++i) {
        chart->addSeries(series[i]);
        series[i]->attachAxis(axisX);
        series[i]->attachAxis(axisY);
        series[i]->setMarkerSize(8);
        series[i]->setBorderColor("#808B96");
/*
        if (i % 2 == 0) {
            series[i]->setMarkerShape(QScatterSeries::MarkerShapeCircle);
            series[i]->setMarkerSize(8);
            series[i]->setColor("#A63CFF");
            series[i]->setBorderColor("#152CD5");
        } else {
            series[i]->setMarkerShape(QScatterSeries::MarkerShapeTriangle);
            //series[i]->setBestFitLineColor("#AF0540");
            series[i]->setMarkerSize(10);
            series[i]->setBorderColor("#8C033B");
            series[i]->setColor("#FF3C68");
        }*/
    }

    //chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setAnimationOptions(QChart::AllAnimations);

    chart->legend()->setVisible(legend);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeCircle);

    chart->legend()->setAlignment(Qt::AlignRight);

    //QChartView *view = new QChartView(chart);
    view->setChart(chart);
    view->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(view);

}


void MainWindow::on_download_clicked() {
    QString filter = "All File (*.*) ;; CSV (*.csv) ;; Excel (*.xlsx)";
    QString path = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath(), filter);

    if(path == ""){
        return void();
    }

    std::vector<QStringList> data;
    try {
        if (path.right(5) == ".xlsx" || path.right(4) == ".xls") {
            data = read_excel(this, path);
        } else if (path.right(4) == ".csv") {
            data = read_csv(path);
        } else {
            throw std::invalid_argument("Invalid file format!");
        }

    }  catch (std::exception& err) {
         QMessageBox::warning(this, "warning", "The file was not read due to " + QString(err.what()));
         return void();
    }

    download_file file_ui;
    file_ui.setModal(true);
    file_ui.add_data(data);
    file_ui.exec();

    if (file_ui.result() == QDialog::DialogCode::Accepted) {
        auto freq = file_ui.GetFreq();
        auto cap = file_ui.GetC();
        auto tan = file_ui.GetD();

        int count = std::max({freq.size(), cap.size(), tan.size()});

        for (int row = 0; row < count; ++row) {

            if (row >= ui->tableWidget->rowCount()) {
                ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            }

            addition(freq, row, 0);
            addition(cap, row, 1);
            addition(tan, row, 2);
        }
    }
}


void MainWindow::on_clear_clicked() {
    auto response = QMessageBox::information(this, "Clear All", "Are you sure you want to delete all data?",
                             QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);

    if (response == QMessageBox::StandardButton::Yes) {
        ui->tableWidget->clearContents();
        mf.frequency.clear();
        mf.capacity.clear();
        mf.tan_d.clear();
        mf.re_epsilon.clear();
        mf.im_epsilon.clear();

        series_re_e->clear();
        series_im_e->clear();
        chart_epsilon->update();

        series_coul->clear();
        series_coul->clear();
        chart_coul->update();

        series_dist->clear();
        mf.radius_and_concentration.clear();
        axisX->clear();
        chart_dist->update();

        mf.total_area = 0;
    }
}


void MainWindow::on_plotDistribution_clicked() {

    if(mf.frequency.empty() && mf.im_epsilon.empty()) {
        return;
    }

    if (mf.radius_and_concentration.empty()) {
        auto response = mf.intervals_of_radius();
        double max_y = 0;
        QBarSet *set = new QBarSet("");

        QStringList categories;
        for (const auto& [r, n]: response.first) {
            *set << n;
            max_y = n > max_y? n * 1.1 : max_y;
            categories << QString::number(roundoff(r, 2));
        }

        for (const auto& [radius, row]: response.second) {
            allowed_addition(radius, row, 5);
            allowed_addition(mf.radius_and_concentration[radius], row, 6);
        }

        series_dist->append(set);
        series_dist->setBarWidth(1);

        chart_dist->setAnimationOptions(QChart::AllAnimations);
        chart_dist->legend()->setVisible(false);

        axisX->append(categories);
        chart_dist->addAxis(axisX, Qt::AlignBottom);
        series_dist->attachAxis(axisX);
        chart_dist->addSeries(series_dist);

        axisY->setRange(0, max_y);
        series_dist->attachAxis(axisY);

        chart_dist->update();

    } else {
        QMessageBox::information(this, "...", "The distribution already built.");
    }
}


void MainWindow::on_lineEdit_C0_textChanged(const QString &arg1) {
    mf.c0_ = arg1.toDouble();

    if(mf.re_epsilon.empty()) {
        return;
    }

    for (auto& [row, value]: mf.re_epsilon) {
        mf.re_epsilon[row] = mf.ReEpsilon(mf.capacity[row]);

        if (mf.im_epsilon[row]) {
            mf.im_epsilon[row] = mf.ImEpsilon(mf.capacity[row], mf.tan_d[row]);
            series_coul->replace(row, mf.re_epsilon[row], mf.im_epsilon[row]);
            series_im_e->replace(row, mf.frequency[row], mf.im_epsilon[row]);
            allowed_addition(mf.im_epsilon[row], row, 4);
        }

        allowed_addition(mf.re_epsilon[row], row, 3);
        series_re_e->replace(row, mf.frequency[row], mf.re_epsilon[row]);
    }

}


void MainWindow::on_lineEdit_D0_textChanged(const QString &arg1) {
    mf.tan_d0_ = arg1.toDouble();
}


void MainWindow::on_lineEdit_eta_textChanged(const QString &arg1) {
    mf.eta_ = arg1.toDouble();
}


void MainWindow::on_lineEdit_temp_textChanged(const QString &arg1) {
    mf.T_ = arg1.toDouble();
}


void MainWindow::on_save_clicked() {
    QString filter = "CSV (*.csv)";
    QString path = QFileDialog::getSaveFileName(this, "Open a file", QDir::homePath(), filter);

    if(path == ""){
        return void();
    }

    QFile file(path);

    if(file.open(QFile::WriteOnly)){
        QTextStream out(&file);

        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            out << ui->tableWidget->horizontalHeaderItem(col)->text();
            if (col + 1 != ui->tableWidget->columnCount()) {
                out << ",";
            }
        }
        out << "\n";
        int count_row = std::max({mf.frequency.size(), mf.capacity.size(), mf.tan_d.size()});
        for(int row = 0; row < count_row; ++row) {
            for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {

                try {
                    out << ui->tableWidget->item(row, col)->text() << ",";
                } catch(std::exception& err) {
                    out << "" << ",";
                }

               if (col + 1 != ui->tableWidget->columnCount()) {
                   out << ",";
               }
            }
            out << "\n";
        }
        out.flush();
    }

    file.close();
}




void MainWindow::on_export_dist_clicked() {
     Export(view_dist);
}


void MainWindow::on_export_epsilon_clicked() {
     Export(view_epsilon);
}



void MainWindow::on_export_coul_clicked() {
     Export(view_coul);
}


void MainWindow::Export(QChartView *view_chart) {
    QString path = QFileDialog::getSaveFileName(this,  tr("Save file"), QDir::homePath(), tr("Images (*.png)"));

    if(path == ""){
        return void();
    }

    QPixmap p = view_chart->grab();
    p.save(path, "PNG", 100);
}


void MainWindow::on_setting_ChartEpsilon_clicked() {
    hidden_setting_e = hidden_setting_e? false : true;
    ui->SetingChartEpsilon->setHidden(hidden_setting_e);
}


void MainWindow::on_additional_quantities_clicked() {
    hidden_frame = hidden_frame? false : true;
    ui->frame->setHidden(hidden_frame);
}





void MainWindow::on_ChartEpsilonTittle_textChanged() {

}


void MainWindow::on_ChartEpsilonTittle_textChanged(const QString &arg1) {
    chart_epsilon->setTitle(arg1);
}


void MainWindow::on_ChartEpsilonAxisXname_textChanged(const QString &arg1) {
    series_re_e->setName(arg1);
}


void MainWindow::on_ChartEpsilonAxisYname_textChanged(const QString &arg1) {
    series_im_e->setName(arg1);
}

