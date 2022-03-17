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
    /// Явно задаем название страниц содержащих соответствующий график
    ui->tabWidget->setTabText(0, "Epsilon");
    ui->tabWidget->setTabText(1, "Coul-Coul");
    ///Требуется построить график зависимости действительной и мнимой части диэлектрической проницаемости (epsilon) взависимости от частоты
    ///Этот график будет отображаться в первой вкладке TabWidgets (Это Важно!)
    CreateScatterChart(ui->epsilonLayout, "Epsilon", "frequency (kHz)", "epsilon", {"Re(epsilon)", "Im(epsilon)"});

    ///Во-второй вкладке отображается график зависимости мнимой части от действительной части диэлектрической проницаемости
    CreateScatterChart(ui->coulcoulLayout, "Coul-Coul", "Re(epsiloin)", "Im(epsilon)", {""});

    int current_index = ui->tabWidget->currentIndex();

    QObject::connect(ui->tabWidget, &QTabWidget::currentChanged, [&](int index){
        ui->MarkersView->setEnabled(true);

        QString axis = ui->ChoiseAxis->currentText();
        ui->Title->setText(charts[GetName(index)]->chart()->title());

        ui->ChoiseAxis->currentTextChanged(axis);

        int count = 0;
        ui->ChoiseSeries->blockSignals(true);
        ui->ChoiseSeries->clear();
        ui->ChoiseSeries->blockSignals(false);

        for(const auto& item: charts[GetName(index)]->chart()->series()) {
                if (item->name().isEmpty()) {
                    item->setName("series " + QString::number(count));
                }

                ui->ChoiseSeries->addItem(item->name());
                ++count;
        }

        ui->LegendShow->setChecked(charts[GetName(index)]->chart()->legend()->isVisible());

    });


    QObject::connect(ui->ChoiseAxis, &QComboBox::currentTextChanged, [&](const QString &axis) {
        int index = ui->tabWidget->currentIndex();

        bool state = axis == "X" && charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar? 0 : 1;
        EnableSettingsMinor(state);

        QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
        object = axis == "Y"? charts[GetName(index)]->chart()->axisY() : object;

        ui->ChartAxisName->setText(object->titleText());
        ui->MajorStyle->setCurrentText(line_styles[object->gridLinePen().style()]);
        ui->MinorStyle->setCurrentText(line_styles[object->minorGridLinePen().style()]);

        int major_count = axis == "Y"? axis_count_ticks[charts[GetName(index)]][1] : axis_count_ticks[charts[GetName(index)]][0];
        int minor_count = axis == "Y"? axis_count_ticks[charts[GetName(index)]][3] : axis_count_ticks[charts[GetName(index)]][2];

        ui->MajorCount->blockSignals(true);
        ui->MajorCount->setValue(major_count);
        ui->MajorCount->blockSignals(false);

        ui->MinorCount->blockSignals(true);
        ui->MinorCount->setValue(minor_count);
        ui->MinorCount->blockSignals(false);

    });

    QObject::connect(ui->MajorCount, &QSpinBox::valueChanged, [&](const int& arg){
        int index = ui->tabWidget->currentIndex();

        QString axis_name = ui->ChoiseAxis->currentText();
        bool state = axis_name == "X" && charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar? 0 : 1;

        if (!state) {
            HistogramChangedIntervals(arg);

        } else {
            TicketsCountChanged(index, arg, "major");
        }

    });

    QObject::connect(ui->MinorCount, &QSpinBox::valueChanged, [&](const int& arg){
        int index = ui->tabWidget->currentIndex();

        QString axis_name = ui->ChoiseAxis->currentText();
        bool state = axis_name == "X" && charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar? 0 : 1;

        if (state) {
           TicketsCountChanged(index, arg, "minor");
        }

    });

    QObject::connect(ui->MajorStyle, &QComboBox::currentTextChanged, [&](const QString &name){

        QPen pen;
        auto it = std::find_if(line_styles.begin(), line_styles.end(), [&](const std::pair<Qt::PenStyle, QString> &item) {
            return item.second == name;
        });
        pen.setStyle(it->first);

        int index = ui->tabWidget->currentIndex();
        QString axis = ui->ChoiseAxis->currentText();

        QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
        object = axis == "Y"? charts[GetName(index)]->chart()->axisY() : object;

        pen.setColor(object->gridLineColor());
        object->setGridLinePen(pen);

       });


    QObject::connect(ui->MinorStyle, &QComboBox::currentTextChanged, [&](const QString &name){
        QPen pen;
        auto it = std::find_if(line_styles.begin(), line_styles.end(), [&](const std::pair<Qt::PenStyle, QString> &item) {
            return item.second == name;
        });
        pen.setStyle(it->first);

        int index = ui->tabWidget->currentIndex();
        QString axis = ui->ChoiseAxis->currentText();

        QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
        object = axis == "Y"? charts[GetName(index)]->chart()->axisY() : object;

        pen.setColor(object->minorGridLineColor());
        object->setMinorGridLinePen(pen);
    });

    
    QObject::connect(ui->ChoiseSeries, &QComboBox::currentTextChanged, [&](const QString& name) {
        ui->SeriesName->setText(name);

        int index = ui->tabWidget->currentIndex();

        if (charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeScatter) {

            auto it = std::find_if(map_ScatterSeries[charts[GetName(index)]].begin(), map_ScatterSeries[charts[GetName(index)]].end(), [&](const QScatterSeries* series){
                return series->name() == name;
            });

            QScatterSeries::MarkerShape shape((*it)->markerShape());
            ui->MarkerSize->setValue((*it)->markerSize());
            ui->MarkersView->setCurrentText(marker_shape[shape]);

        } else if (charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar) {
            auto it = std::find_if(map_BarSeries[charts[GetName(index)]].begin(), map_BarSeries[charts[GetName(index)]].end(), [&](const QBarSeries* series){
                return series->name() == name;
            });
            ui->MarkerSize->setValue((*it)->barWidth() * 100);
            ui->MarkersView->setDisabled(true);
        }

    });


    QObject::connect(ui->MarkersView, &QComboBox::currentTextChanged, [&](const QString& name) {
        int index = ui->tabWidget->currentIndex();
        QString series_name = ui->ChoiseSeries->currentText();

        auto it = std::find_if(marker_shape.begin(), marker_shape.end(), [&](const std::pair<QScatterSeries::MarkerShape, QString> &shape){
            return shape.second == name;
        });

        QScatterSeries::MarkerShape new_shape(it->first);
        auto it_2 = std::find_if(map_ScatterSeries[charts[GetName(index)]].begin(), map_ScatterSeries[charts[GetName(index)]].end(), [&](const QScatterSeries* series){
            return series->name() == series_name;
        });

        (*it_2)->setMarkerShape(new_shape);

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

            QObject::connect(series, &QScatterSeries::hovered, this, [&](const QPointF &point, bool state){

                int index = ui->tabWidget->currentIndex();
                QString axisX = charts[GetName(index)]->chart()->axisX()->titleText();
                QString axisY = charts[GetName(index)]->chart()->axisY()->titleText();

                QString message = axisX + ": " + QString::number(point.x()) + "\n"
                        + axisY + ": " + QString::number(point.y());

                charts[GetName(index)]->setToolTip(message);
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
            add_point(map_ScatterSeries[charts["Epsilon"]][0], mf.frequency, mf.re_epsilon, row, value);
            add_point(map_ScatterSeries[charts["Epsilon"]][1], mf.frequency, mf.im_epsilon, row, value);

            mf.frequency[row] = value;
            break;

        case 1:
            add_point(map_ScatterSeries[charts["Epsilon"]][0], mf.re_epsilon, mf.frequency, row, 0, mf.ReEpsilon(value));

            if (mf.tan_d[row]) {
                add_point(map_ScatterSeries[charts["Epsilon"]][1], mf.capacity, mf.frequency, row, 0, mf.ImEpsilon(value, mf.tan_d[row]));
                add_point(map_ScatterSeries[charts["Coul-Coul"]][0], mf.re_epsilon, mf.tan_d, row, mf.ReEpsilon(value), mf.ImEpsilon(value, mf.tan_d[row]));

                mf.im_epsilon[row] = mf.ImEpsilon(value, mf.tan_d[row]);
                allowed_addition(mf.im_epsilon[row], row, 4);
            }

            mf.capacity[row] = value;
            mf.re_epsilon[row] = mf.ReEpsilon(value);
            allowed_addition(mf.re_epsilon[row], row, 3);
            break;

        case 2:

            if (mf.capacity[row]) {

                add_point(map_ScatterSeries[charts["Epsilon"]][1], mf.im_epsilon, mf.frequency, row, 0, mf.ImEpsilon(mf.capacity[row], value));
                add_point(map_ScatterSeries[charts["Coul-Coul"]][0], mf.im_epsilon, mf.re_epsilon, row, 0, mf.ImEpsilon(mf.capacity[row], value));

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


void MainWindow::EnableSettingsMinor(bool state) {
    ui->MinorColor->setEnabled(state);
    ui->MinorCount->setEnabled(state);
    ui->MinorFont->setEnabled(state);
    ui->MinorStyle->setEnabled(state);
}


void MainWindow::AutoScale(QChart *chart, QList<QScatterSeries*> &list_series){
      double xMax = 0;
      double yMax = 0;

      int score = 0;
      for(auto& series: list_series) {
          for (auto p : series->points()){
              xMax = qMax(xMax, p.x());
              yMax = qMax(yMax, p.y());

              ++score;
          }
      }

      if (!score) {
          xMax = 10;
          yMax = 10;
      }
      chart->axisX()->setRange(0, xMax * 1.1);
      chart->axisY()->setRange(0, yMax * 1.1); 
}


void MainWindow::AutoScale(QChart *chart, QList<QBarSeries*> &list_series) {
    double yMax = 0;

    int score = 0;
    for(const auto& series: list_series) {
        for (const auto& barSet : series->barSets()){
            for (int i = 0; i < barSet->count(); ++i) {
                yMax = qMax(yMax, barSet->at(i));
                ++score;
            }
        }
    }

    if (!score) {
        yMax = 1;
    }
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

void MainWindow::AxisSample(QValueAxis *axis, const QString& title, const int& major_count, const int& minor_count, const int& range) {
    axis->setRange(0, range);
    axis->setTickCount(major_count);
    axis->setMinorTickCount(minor_count);

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


void MainWindow::TicketsCountChanged(const int& index, const int& value, const QString& type) {
    QString axis_name = ui->ChoiseAxis->currentText();
    QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
    object = axis_name == "Y"? charts[GetName(index)]->chart()->axisY() : object;

    //Save current settings of axis
    QPen major_style = object->gridLinePen();
    QFont major_font = object->labelsFont();
    QString axis_title = object->titleText();
    QFont axis_font =  object->titleFont();

    QPen minor_style = object->minorGridLinePen();

    //Create new axis
    QValueAxis *axis = new QValueAxis();

    //Delete old axis
    charts[GetName(index)]->chart()->removeAxis(object);

    int major_count = 12;
    int minor_count = 4;

    auto align = axis_name == "Y"? Qt::AlignLeft: Qt::AlignBottom;

    if(type == "major") {
        charts[GetName(index)]->chart()->addAxis(axis, align);
        minor_count = axis_count_ticks[charts[GetName(index)]][2];
        axis_count_ticks[charts[GetName(index)]][0] = value;
        major_count = value;

    } else if (type == "minor") {
        charts[GetName(index)]->chart()->addAxis(axis, align);
        major_count = axis_count_ticks[charts[GetName(index)]][0];
        axis_count_ticks[charts[GetName(index)]][2] = value;
        minor_count = value;
    }

    AxisSample(axis, axis_title, major_count, minor_count);

    if (charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeScatter) {
        for (auto& series: map_ScatterSeries[charts[GetName(index)]]) {
             series->attachAxis(axis);
        }

        AutoScale(charts[GetName(index)]->chart(), map_ScatterSeries[charts[GetName(index)]]);

    } else if (charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar) {
        double max_y = 1;
        for (auto& series: map_BarSeries[charts[GetName(index)]]) {
             series->attachAxis(axis);
              AutoScale(charts[GetName(index)]->chart(), map_BarSeries[charts[GetName(index)]]);
        }

    }

    //Give settings
    axis->setGridLinePen(major_style);
    axis->setLabelsFont(major_font);
    axis->setTitleFont(axis_font);
    axis->setMinorGridLinePen(minor_style);
}


void MainWindow::CreateScatterChart(QLayout *layout, const QString& name, const QString& nameX, const QString &nameY,
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

    m_coordX = new QGraphicsSimpleTextItem(chart);
    m_coordX->setPos(chart->size().width()/2 - 50, chart->size().height());

    map_ScatterSeries[view] = list_series;
    charts[name] = view;
    axis_count_ticks[view] = {12, 12, 4, 4};
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

        charts.erase("Distribution");
        map_BarSeries.erase(0);

        ui->tabWidget->removeTab(2);
    }
}

void MainWindow::Histogram(const int& count) {
    auto rows = mf.radius();
    auto interval = mf.intervals(count);

    double max_y = 0;
    QBarSet *set = new QBarSet("");

    QStringList categories;
    for (const auto& [r, n]: interval) {
        *set << n;

        max_y = n > max_y? n * 1.1 : max_y;
        categories << QString::number(roundoff(r, 2));
    }

    for (const auto& [radius, row]: rows) {
        allowed_addition(radius, row, 5);
        allowed_addition(mf.radius_and_concentration[radius], row, 6);
    }

    ///Создание новой страницы и вставка в нее слоя с виджетом для отображения графика
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

    chart_dist->legend()->setInteractive(true);
    charts["Distribution"] = view_dist;
    map_BarSeries[view_dist] = {series_dist};

    axis_count_ticks[view_dist] = {count, axisY->tickCount(), 0, 4};
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
       axis_count_ticks.erase(charts["Distribution"]);
       charts.erase("Distribution");
       map_BarSeries.erase(0);
       ui->tabWidget->removeTab(2);

    }

    Histogram();

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    page->setLayout(layout);
    ui->tabWidget->insertTab(2, page, "Distribution");

    layout->addWidget(charts["Distribution"]);

    ui->tabWidget->setCurrentIndex(2);

}

void MainWindow::HistogramChangedIntervals(const int& arg1) {
    mf.radius_and_concentration.clear();
    mf.total_area = 0;
    charts.erase("Distribution");
    map_BarSeries.erase(0);
    ui->tabWidget->removeTab(2);

    Histogram(arg1);

    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    page->setLayout(layout);
    ui->tabWidget->insertTab(2, page, "Distribution");

    layout->addWidget(charts["Distribution"]);

    ui->tabWidget->setCurrentIndex(2);
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
            map_ScatterSeries[charts["Coul-Coul"]][0]->replace(row, mf.re_epsilon[row], mf.im_epsilon[row]);
            map_ScatterSeries[charts["Epsilon"]][1]->replace(row, mf.frequency[row], mf.im_epsilon[row]);
            allowed_addition(mf.im_epsilon[row], row, 4);
        }

        allowed_addition(mf.re_epsilon[row], row, 3);
        map_ScatterSeries[charts["Epsilon"]][0]->replace(row, mf.frequency[row], mf.re_epsilon[row]);
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

    QPixmap p = charts[GetName(index)]->grab();
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
    charts[GetName(index)]->chart()->setTitle(arg1);
}


void MainWindow::on_TitleFont_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Embria"), this);

    if(ok) {
        int index = ui->tabWidget->currentIndex();
        charts[GetName(index)]->chart()->setTitleFont(font);
    }
}


void MainWindow::on_ChartAxisName_textChanged(const QString &arg1) {
    int index = ui->tabWidget->currentIndex();
    QString axis = ui->ChoiseAxis->currentText();

    QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
    object = axis == "Y"? charts[GetName(index)]->chart()->axisY(): object;
    object->setTitleText(arg1);
}


void MainWindow::on_FontAxis_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Embria"), this);

    if(ok) {
        int index = ui->tabWidget->currentIndex();
        QString axis = ui->ChoiseAxis->currentText();
        QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
        object = axis == "Y"? charts[GetName(index)]->chart()->axisY(): object;
        object->setTitleFont(font);
    }
}


void MainWindow::on_MajorFont_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Embria"), this);

    if(ok) {
        int index = ui->tabWidget->currentIndex();
        QString axis = ui->ChoiseAxis->currentText();
        QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
        object = axis == "Y"? charts[GetName(index)]->chart()->axisY(): object;
        object->setLabelsFont(font);
    }
}


void MainWindow::on_MajorColor_clicked() {
    QColor color = QColorDialog::getColor("#6CC4FF", this);

    if (color.isValid()) {
        int index = ui->tabWidget->currentIndex();
        QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
        QString axis = ui->ChoiseAxis->currentText();
        object = axis == "Y"? charts[GetName(index)]->chart()->axisY(): object;
        object->setGridLineColor(color);
    }
}

void MainWindow::on_MinorColor_clicked() {
    QColor color = QColorDialog::getColor("#6CC4FF", this);
    QString axis = ui->ChoiseAxis->currentText();

    if (color.isValid()) {
        int index = ui->tabWidget->currentIndex();
        QAbstractAxis* object = charts[GetName(index)]->chart()->axisX();
        object = axis == "Y"? charts[GetName(index)]->chart()->axisY(): object;
        object->setMinorGridLineColor(color);
    }
}


void MainWindow::on_SeriesColor_clicked() {
    QColor color = QColorDialog::getColor("#6CC4FF", this);
    QString series_name = ui->ChoiseSeries->currentText();

    if (color.isValid()) {
        int index = ui->tabWidget->currentIndex();

        if (charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeScatter) {

            auto it = std::find_if(map_ScatterSeries[charts[GetName(index)]].begin(), map_ScatterSeries[charts[GetName(index)]].end(), [&](const QScatterSeries * series){
                return series->name() == series_name;
            });

            (*it)->setColor(color);

        } else if (charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar) {

            auto it = std::find_if(map_BarSeries[charts[GetName(index)]].begin(), map_BarSeries[charts[GetName(index)]].end(), [&](const QBarSeries * series){
                return series->name() == series_name;
            });

            (*it)->barSets()[0]->setColor(color);

        }
    }
}


void MainWindow::on_SeriesName_textChanged(const QString &arg1) {
    int index = ui->tabWidget->currentIndex();
    int index_series = ui->ChoiseSeries->currentIndex();
    ui->ChoiseSeries->setItemText(index_series, arg1);
    charts[GetName(index)]->chart()->series()[index_series]->setName(arg1);
}


void MainWindow::on_LegendFont_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Embria"), this);

    if(ok) {
        int index = ui->tabWidget->currentIndex();
        charts[GetName(index)]->chart()->legend()->setFont(font);
    }
}


void MainWindow::on_MarkerSize_valueChanged(int arg1) {
    QString series_name = ui->ChoiseSeries->currentText();


    int index = ui->tabWidget->currentIndex();
    if (charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeScatter) {

        auto it = std::find_if(map_ScatterSeries[charts[GetName(index)]].begin(), map_ScatterSeries[charts[GetName(index)]].end(), [&](const QScatterSeries * series){
            return series->name() == series_name;
        });

        (*it)->setMarkerSize(arg1);

    } else if (charts[GetName(index)]->chart()->series()[0]->type() == QAbstractSeries::SeriesTypeBar) {

        auto it = std::find_if(map_BarSeries[charts[GetName(index)]].begin(), map_BarSeries[charts[GetName(index)]].end(), [&](const QBarSeries * series){
            return series->name() == series_name;
        });

        (*it)->setBarWidth(arg1 * 0.01);

    }
}


void MainWindow::on_LegendShow_stateChanged(int arg1) {
    int index = ui->tabWidget->currentIndex();
    charts[GetName(index)]->chart()->legend()->setVisible(arg1);
}

QString MainWindow::GetName(const int& index) {
    return ui->tabWidget->tabText(index);
}


