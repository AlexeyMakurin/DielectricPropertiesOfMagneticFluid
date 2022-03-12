#include "mychart.h"

MyChart::MyChart() {

}

MyChart::MyChart(const QString& nameX, const QString& nameY, const QStringList& names_series) {
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();

    AxisSample(axisX, nameX);
    AxisSample(axisY, nameY);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    for (uint i = 0; i < names_series.size(); ++i) {
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

    view->setChart(chart);
    view->setRenderHint(QPainter::Antialiasing);
}


void MyChart::AxisSample(QValueAxis *axis, const QString& title) {
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


QChart* MyChart::GetChart() {
    return chart;
}


QList<QScatterSeries*> MyChart::GetSeries() {
    return list_series;
}


QChartView* MyChart::GetView() {
    return view;
}

