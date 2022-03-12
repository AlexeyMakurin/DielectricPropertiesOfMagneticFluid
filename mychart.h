#ifndef MYCHART_H
#define MYCHART_H

#include <QtCharts/QScatterSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QBarCategoryAxis>


class MyChart {
public:
    MyChart();
    MyChart(const QString& nameX, const QString& nameY, const QStringList& names_series);

    QList<QScatterSeries*> GetSeries();
    QChartView* GetView();
    QChart* GetChart();

private:
   void AxisSample(QValueAxis *axis, const QString& title);

   QList<QScatterSeries*> list_series;
   QChart *chart = new QChart();
   QChartView *view = new QChartView();

};

#endif // MYCHART_H
