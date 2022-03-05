#include "mainwindow.h"
#include "ui_mainwindow.h"



MagneticFluid mf(1.53e-3, 135.32, 0.0123, 300.0);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Dielectric Properties Of Magnetic Fluid");

    ui->ChartsSettingsFrame->setHidden(hidden_settings);
    ui->frame->setHidden(hidden_frame);

    ui->lineEdit_C0->setText(QString::number(mf.c0_));
    ui->lineEdit_D0->setText(QString::number(mf.tan_d0_));
    ui->lineEdit_eta->setText(QString::number(mf.eta_));
    ui->lineEdit_temp->setText(QString::number(mf.T_));

    ///Setting Table
    ui->tableWidget->setRowCount(14);
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setHorizontalHeaderLabels(
                QStringList () << "frequency (kHz)" << "C (pF)" << "D"
                << "Re(epsilon)" << "Im(epsilon)" << "Radius (nm)" << "Сoncentration"
    );


    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    protoitem->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget->setItemPrototype(protoitem);
    ///

    ///Требуется построить график зависимости действительной и мнимой части диэлектрической проницаемости (epsilon) взависимости от частоты
    ///Этот график будет отображаться в первой вкладке TabWidgets (Это Важно!)
    CreateScatterChart(ui->epsilonLayout, 0, "frequency (kHz)", "epsilon", {"Re(epsilon)", "Im(epsilon)"});

    ///Во-второй вкладке отображается график зависимости мнимой части от действительной части диэлектрической проницаемости
    CreateScatterChart(ui->coulcoulLayout, 1, "Re(epsiloin)", "Im(epsilon)", {""});

    int current_index = ui->tabWidget->currentIndex();

    QObject::connect(ui->tabWidget, &QTabWidget::currentChanged, [&](int index){
        ui->MarkersView->setEnabled(true);

        QString axis = ui->ChoiseAxis->currentText();
        ui->Title->setText(charts[index]->chart()->title());

        ui->ChoiseAxis->currentTextChanged(axis);

        int count = 0;
        ui->ChoiseSeries->blockSignals(true);
        ui->ChoiseSeries->clear();
        ui->ChoiseSeries->blockSignals(false);

        for(const auto& item: charts[index]->chart()->series()) {
                if (item->name().isEmpty()) {
                    item->setName("series " + QString::number(count));
                }

                ui->ChoiseSeries->addItem(item->name());
                ++count;
        }

        ui->LegendShow->setChecked(charts[index]->chart()->legend()->isVisible());

    });


    QObject::connect(ui->ChoiseAxis, &QComboBox::currentTextChanged, [&](const QString &axis) {
        int index = ui->tabWidget->currentIndex();

        ui->MinorColor->setEnabled(true);
        ui->MinorCount->setEnabled(true);
        ui->MinorFont->setEnabled(true);
        ui->MinorStyle->setEnabled(true);

        if (axis == "X") {
            ui->ChartAxisName->setText(charts[index]->chart()->axisX()->titleText());
            ui->MajorStyle->setCurrentText(line_styles[charts[index]->chart()->axisX()->gridLinePen().style()]);
            ui->MinorStyle->setCurrentText(line_styles[charts[index]->chart()->axisX()->minorGridLinePen().style()]);

            if (charts[index]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar) {
                ui->MinorColor->setEnabled(false);
                ui->MinorCount->setEnabled(false);
                ui->MinorFont->setEnabled(false);
                ui->MinorStyle->setEnabled(false);
            }

        } else if (axis == "Y") {
            ui->ChartAxisName->setText(charts[index]->chart()->axisY()->titleText());
            ui->MajorStyle->setCurrentText(line_styles[charts[index]->chart()->axisY()->gridLinePen().style()]);
            ui->MinorStyle->setCurrentText(line_styles[charts[index]->chart()->axisY()->minorGridLinePen().style()]);
        }
    });


    QObject::connect(ui->MajorStyle, &QComboBox::currentTextChanged, [&](const QString &name){
        QPen pen;
        for (const auto& [style, name_]: line_styles) {
            if (name_ == name) {
                pen.setStyle(style);
            }
        }

        int index = ui->tabWidget->currentIndex();
        QString axis = ui->ChoiseAxis->currentText();
        if (axis == "X") {
            pen.setColor(charts[index]->chart()->axisX()->gridLineColor());
            charts[index]->chart()->axisX()->setGridLinePen(pen);
        } else if (axis == "Y") {
            pen.setColor(charts[index]->chart()->axisY()->gridLineColor());
            charts[index]->chart()->axisY()->setGridLinePen(pen);
        }
       });


    QObject::connect(ui->MinorStyle, &QComboBox::currentTextChanged, [&](const QString &name){
        QPen pen;
        for (const auto& [style, name_]: line_styles) {
            if (name_ == name) {
                pen.setStyle(style);
            }
        }

        int index = ui->tabWidget->currentIndex();
        QString axis = ui->ChoiseAxis->currentText();
        if (axis == "X") {
            pen.setColor(charts[index]->chart()->axisX()->minorGridLineColor());
            charts[index]->chart()->axisX()->setMinorGridLinePen(pen);
        } else if (axis == "Y") {
            pen.setColor(charts[index]->chart()->axisY()->minorGridLineColor());
            charts[index]->chart()->axisY()->setMinorGridLinePen(pen);
        }
       });


    QObject::connect(ui->ChoiseSeries, &QComboBox::currentTextChanged, [&](const QString& name) {
        ui->SeriesName->setText(name);

        int index = ui->tabWidget->currentIndex();

        if (charts[index]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeScatter) {

            QScatterSeries::MarkerShape shape;
            for (auto& item: map_ScatterSeries[charts[index]]) {
                if (item->name() == name) {
                    shape = item->markerShape();
                    ui->MarkerSize->setValue(item->markerSize());
                }
            }
            ui->MarkersView->setCurrentText(marker_shape[shape]);

        } else if (charts[index]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar) {
            for (auto& item: map_BarSeries[charts[index]]) {
                if (item->name() == name) {
                    ui->MarkerSize->setValue(item->barWidth() * 100);
                }
            }

            ui->MarkersView->setDisabled(true);

        }


    });


    QObject::connect(ui->MarkersView, &QComboBox::currentTextChanged, [&](const QString& name) {
        int index = ui->tabWidget->currentIndex();
        QString series_name = ui->ChoiseSeries->currentText();
        QScatterSeries::MarkerShape new_shape;

        for (const auto& [shape, name_]: marker_shape) {
            if (name_ == name) {
                new_shape = shape;
            }
        }

        for (auto& item: map_ScatterSeries[charts[index]]) {
            if (series_name == item->name()) {
                item->setMarkerShape(new_shape);
            }
        }

    });

    ui->tabWidget->currentChanged(current_index);

    for (auto& item: map_ScatterSeries) {
        for (auto& series: item.second) {

            QObject::connect(series, &QScatterSeries::pointAdded, [&](){
                AutoScale(item.first->chart(), item.second);
            });

            QObject::connect(series, &QScatterSeries::pointReplaced, [&](){
                AutoScale(item.first->chart(), item.second);
            });
        }
    }


    QObject::connect(ui->tableWidget, &QTableWidget::cellChanged, [&](int row, int col){

        double value =  ui->tableWidget->item(row, col)->text().toDouble();
        if (row >= ui->tableWidget->rowCount() - 2) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        }



        switch (col) {

        case 0:
            add_point(map_ScatterSeries[charts[0]][0], mf.frequency, mf.re_epsilon, row, value);
            add_point(map_ScatterSeries[charts[0]][1], mf.frequency, mf.im_epsilon, row, value);

            //add_point(series_re_e, mf.frequency, mf.re_epsilon, row, value);
            //add_point(series_im_e, mf.frequency, mf.im_epsilon, row, value);
            mf.frequency[row] = value;
            break;

        case 1:
            add_point(map_ScatterSeries[charts[0]][0], mf.re_epsilon, mf.frequency, row, 0, mf.ReEpsilon(value));
            //add_point(series_re_e, mf.re_epsilon, mf.frequency, row, 0, mf.ReEpsilon(value));

            if (mf.tan_d[row]) {
                add_point(map_ScatterSeries[charts[0]][1], mf.capacity, mf.frequency, row, 0, mf.ImEpsilon(value, mf.tan_d[row]));
                add_point(map_ScatterSeries[charts[1]][0], mf.re_epsilon, mf.tan_d, row, mf.ReEpsilon(value), mf.ImEpsilon(value, mf.tan_d[row]));

                //add_point(series_im_e, mf.capacity, mf.frequency, row, 0, mf.ImEpsilon(value, mf.tan_d[row]));
                //add_point(series_coul, mf.re_epsilon, mf.tan_d, row, mf.ReEpsilon(value), mf.ImEpsilon(value, mf.tan_d[row]));

                mf.im_epsilon[row] = mf.ImEpsilon(value, mf.tan_d[row]);
                allowed_addition(mf.im_epsilon[row], row, 4);
            }

            mf.capacity[row] = value;
            mf.re_epsilon[row] = mf.ReEpsilon(value);
            allowed_addition(mf.re_epsilon[row], row, 3);
            break;

        case 2:

            if (mf.capacity[row]) {

                add_point(map_ScatterSeries[charts[0]][1], mf.im_epsilon, mf.frequency, row, 0, mf.ImEpsilon(mf.capacity[row], value));
                add_point(map_ScatterSeries[charts[1]][0], mf.im_epsilon, mf.re_epsilon, row, 0, mf.ImEpsilon(mf.capacity[row], value));
                //add_point(series_im_e, mf.im_epsilon, mf.frequency, row, 0, mf.ImEpsilon(mf.capacity[row], value));
                //add_point(series_coul, mf.im_epsilon, mf.re_epsilon, row, 0, mf.ImEpsilon(mf.capacity[row], value));

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

    /*ui->Legend_h->setValue(chart_epsilon->legend()->x());
    ui->Legend_v->setValue(chart_epsilon->legend()->y());
    ui->Legend_he->setValue(chart_epsilon->legend()->geometry().height());
    ui->Legend_w->setValue(chart_epsilon->legend()->geometry().width());

    connect(chart_epsilon->legend(), &QLegend::attachedToChartChanged, [=](bool attachedToChart) {
        chart_epsilon->legend()->setBackgroundVisible(!attachedToChart);
    });*/


}


//void MainWindow::

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::AutoScale(QChart *chart, QList<QScatterSeries*> &list_series){
      double xMax = 0;
      double yMax = 0;

      for(auto& series: list_series) {
          for (auto p : series->points()){
               xMax = qMax(xMax, p.x());
               yMax = qMax(yMax, p.y());
          }
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

void MainWindow::AxisSample(QValueAxis *axis, const QString& title) {
    axis->setRange(0, 11);
    axis->setTickCount(12);
    axis->setMinorTickCount(4);

    QPen minor;
    minor.setColor("#A3D9FC");
    minor.setStyle(Qt::DotLine);

    QPen major;
    major.setColor("#6CC4FF");
    major.setStyle(Qt::DashLine);

    axis->setMinorGridLinePen(minor);
    axis->setGridLinePen(major);
    axis->setLabelFormat("%.2f");
    axis->setTitleText(title);
}


void MainWindow::CreateScatterChart(QLayout *layout, const int& index, const QString& nameX, const QString &nameY,
                                    const std::vector<QString>& names_series) {
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();

    AxisSample(axisX, nameX);
    AxisSample(axisY, nameY);

    QChart *chart = new QChart();

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    QList<QScatterSeries*> list_series;

    for (size_t i = 0; i < names_series.size(); ++i) {
        QScatterSeries *series = new QScatterSeries();
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        series->setMarkerSize(12);
        series->setName(names_series[i]);

        list_series.append(series);
    }

    chart->setAnimationOptions(QChart::AllAnimations);

    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);
    chart->legend()->setInteractive(true);

    QChartView *view = new QChartView();

    view->setChart(chart);
    view->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(view);

    map_ScatterSeries[view] = list_series;
    charts[index] = view;
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
        mf.radius_and_concentration.clear();
        mf.total_area = 0;

        for (auto& [chart, list]: map_ScatterSeries) {
            for (auto& series: list) {
                series->clear();
            }
            chart->chart()->update();
        }

        charts.erase(2);
        map_BarSeries.erase(0);

        ui->tabWidget->removeTab(2);
    }
}

void MainWindow::Histogram() {
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

    ///Создание новой страницы и вставка в нее слоя с виджетом для отображения графика
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    page->setLayout(layout);
    ui->tabWidget->insertTab(2, page, "Distribution");

    QBarSeries *series_dist = new QBarSeries();
    QChart *chart_dist = new QChart();
    QChartView *view_dist = new QChartView();

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();

    axisY->setTitleText("concentration");
    axisY->setRange(0, 1);
    axisY->setMinorTickCount(4);
    chart_dist->addAxis(axisY, Qt::AlignLeft);

    axisX->setTitleText("Radius (nm)");

    chart_dist->addSeries(series_dist);
    chart_dist->setTitle("Particle size distribution");
    view_dist->setChart(chart_dist);
    view_dist->setRenderHint(QPainter::Antialiasing);

    series_dist->append(set);
    series_dist->setBarWidth(1);

    chart_dist->setAnimationOptions(QChart::AllAnimations);
    chart_dist->legend()->setVisible(false);

    axisX->append(categories);
    chart_dist->addAxis(axisX, Qt::AlignBottom);
    series_dist->attachAxis(axisX);

    axisY->setRange(0, max_y);
    series_dist->attachAxis(axisY);

    layout->addWidget(view_dist);

    chart_dist->legend()->setInteractive(true);
    charts[2] = view_dist;
    map_BarSeries[view_dist] = {series_dist};

    ui->tabWidget->setCurrentIndex(2);
}

void MainWindow::on_plotDistribution_clicked() {

    if(mf.frequency.empty() && mf.im_epsilon.empty()) {
        return;
    }

    if (!mf.radius_and_concentration.empty()) {
        auto response = QMessageBox::information(this, "Rebuild Distribution", "Are you sure you want to rebuild distribution?",
                                 QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
       if (response == QMessageBox::StandardButton::Yes) {
        mf.radius_and_concentration.clear();
        mf.total_area = 0;
       }
    }

    Histogram();
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
            map_ScatterSeries[charts[1]][0]->replace(row, mf.re_epsilon[row], mf.im_epsilon[row]);
            map_ScatterSeries[charts[0]][1]->replace(row, mf.frequency[row], mf.im_epsilon[row]);
            allowed_addition(mf.im_epsilon[row], row, 4);
        }

        allowed_addition(mf.re_epsilon[row], row, 3);
        map_ScatterSeries[charts[0]][0]->replace(row, mf.frequency[row], mf.re_epsilon[row]);
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


void MainWindow::on_Export_clicked() {
    int index = ui->tabWidget->currentIndex();

    QString path = QFileDialog::getSaveFileName(this,  tr("Save file"), QDir::homePath(), tr("Images (*.png)"));

    if(path == ""){
        return void();
    }

    QPixmap p = charts[index]->grab();
    p.save(path, "PNG", 100);
}


void MainWindow::on_ChartsSettingsButton_clicked(){
    hidden_settings = hidden_settings? false : true;
    ui->ChartsSettingsFrame->setHidden(hidden_settings);
}


void MainWindow::on_additional_quantities_clicked() {
    hidden_frame = hidden_frame? false : true;
    ui->frame->setHidden(hidden_frame);
}


void MainWindow::on_Title_textChanged(const QString &arg1) {
    int index = ui->tabWidget->currentIndex();
    charts[index]->chart()->setTitle(arg1);
}


void MainWindow::on_TitleFont_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Embria"), this);

    if(ok) {
        int index = ui->tabWidget->currentIndex();
        charts[index]->chart()->setTitleFont(font);
    }
}


void MainWindow::on_ChartAxisName_textChanged(const QString &arg1) {
    int index = ui->tabWidget->currentIndex();
    QString axis = ui->ChoiseAxis->currentText();

    if (axis == "X") {
        charts[index]->chart()->axisX()->setTitleText(arg1);
    } else if (axis == "Y") {
        charts[index]->chart()->axisY()->setTitleText(arg1);
    }

}


void MainWindow::on_FontAxis_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Embria"), this);

    if(ok) {
        int index = ui->tabWidget->currentIndex();
        QString axis = ui->ChoiseAxis->currentText();
        if (axis == "X") {
            charts[index]->chart()->axisX()->setTitleFont(font);;
        } else if (axis == "Y") {
            charts[index]->chart()->axisY()->setTitleFont(font);;
        }
    }
}


void MainWindow::on_MajorFont_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Embria"), this);

    if(ok) {
        int index = ui->tabWidget->currentIndex();
        QString axis = ui->ChoiseAxis->currentText();
        if (axis == "X") {
            charts[index]->chart()->axisX()->setLabelsFont(font);
        } else if (axis == "Y") {
            charts[index]->chart()->axisY()->setLabelsFont(font);
        }
    }
}


void MainWindow::on_MajorColor_clicked() {
    QColor color = QColorDialog::getColor("#6CC4FF", this);
    QString axis = ui->ChoiseAxis->currentText();
    if (color.isValid()) {
        int index = ui->tabWidget->currentIndex();
        if (axis == "X") {
            charts[index]->chart()->axisX()->setGridLineColor(color);
        } else if (axis == "Y") {
            charts[index]->chart()->axisY()->setGridLineColor(color);
        }
    }
}

void MainWindow::on_MinorColor_clicked() {
    QColor color = QColorDialog::getColor("#6CC4FF", this);
    QString axis = ui->ChoiseAxis->currentText();
    if (color.isValid()) {
        int index = ui->tabWidget->currentIndex();
        if (axis == "X") {
            charts[index]->chart()->axisX()->setMinorGridLineColor(color);
        } else if (axis == "Y") {
            charts[index]->chart()->axisY()->setMinorGridLineColor(color);
        }
    }
}


void MainWindow::on_SeriesColor_clicked() {
    QColor color = QColorDialog::getColor("#6CC4FF", this);
    QString series_name = ui->ChoiseSeries->currentText();

    if (color.isValid()) {
        int index = ui->tabWidget->currentIndex();

        if (charts[index]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeScatter) {
            for (auto& series: map_ScatterSeries[charts[index]]) {
                if (series->name() == series_name) {
                    series->setColor(color);
                }
            }
        } else if (charts[index]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar) {
            for (auto& series: map_BarSeries[charts[index]]) {
                if (series->name() == series_name) {
                    series->barSets()[0]->setColor(color);
                }
            }
        }

    }
}


void MainWindow::on_SeriesName_textChanged(const QString &arg1) {
    int index = ui->tabWidget->currentIndex();
    int index_series = ui->ChoiseSeries->currentIndex();
    ui->ChoiseSeries->setItemText(index_series, arg1);
    charts[index]->chart()->series()[index_series]->setName(arg1);
}


void MainWindow::on_LegendFont_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Embria"), this);

    if(ok) {
        int index = ui->tabWidget->currentIndex();
        charts[index]->chart()->legend()->setFont(font);
    }
}


void MainWindow::on_MarkerSize_valueChanged(int arg1) {
    QString series_name = ui->ChoiseSeries->currentText();


    int index = ui->tabWidget->currentIndex();
    if (charts[index]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeScatter) {
        for (auto& series: map_ScatterSeries[charts[index]]) {
            if (series->name() == series_name) {
                series->setMarkerSize(arg1);
            }
        }
    } else if (charts[index]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar) {
        for (auto& series: map_BarSeries[charts[index]]) {
            if (series->name() == series_name) {
                series->setBarWidth(arg1 * 0.01);
            }
        }
    }
}


void MainWindow::on_LegendShow_stateChanged(int arg1) {
    int index = ui->tabWidget->currentIndex();
    charts[index]->chart()->legend()->setVisible(arg1);
}


