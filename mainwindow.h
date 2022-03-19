#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <vector>
#include <cmath>

#include <QMainWindow>
#include <QWidget>
#include <QTableWidget>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChartView>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFontDialog>
#include <QColorDialog>
#include <QGraphicsTextItem>

#include "download_file.h"
#include "read_file.h"
#include "magneticfluid.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:

    void on_plotDistribution_clicked();

    void on_additional_quantities_clicked();

    void on_clear_clicked();

    void on_download_clicked();

    void on_save_clicked();


    void on_lineEdit_C0_textChanged(const QString &arg1);

    void on_lineEdit_D0_textChanged(const QString &arg1);

    void on_lineEdit_eta_textChanged(const QString &arg1);

    void on_lineEdit_temp_textChanged(const QString &arg1);

    void on_ChartsSettingsButton_clicked();

    void on_Title_textChanged(const QString &arg1);

    void on_TitleFont_clicked();

    void on_ChartAxisName_textChanged(const QString &arg1);

    void on_FontAxis_clicked();

    void on_MajorFont_clicked();

    void on_MajorColor_clicked();

    void on_MinorColor_clicked();

    void on_SeriesColor_clicked();

    void on_SeriesName_textChanged(const QString &arg1);

    void on_LegendFont_clicked();

    void on_MarkerSize_valueChanged(int arg1);

    void on_LegendShow_stateChanged(int arg1);

    void on_Export_clicked();

   // void on_MajorCount_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;

    QTableWidgetItem *protoitem = new QTableWidgetItem();
    QGraphicsSimpleTextItem *m_coordX;

    bool hidden_frame = true;
    bool hidden_settings = true;

    bool act_cell = true;
    bool align = false;


    std::map<Qt::PenStyle, QString> line_styles = {
        {Qt::SolidLine, "SolidLine"},
        {Qt::DashLine, "DashLine"},
        {Qt::DotLine, "DotLine"},
        {Qt::DashDotLine, "DashDotLine"},
        {Qt::DashDotDotLine, "DashDotDotLine"},
        {Qt::NoPen, "NoPen"}
    };

    std::map<QScatterSeries::MarkerShape, QString> marker_shape = {
        {QScatterSeries::MarkerShapeTriangle, "Triangle"},
        {QScatterSeries::MarkerShapeCircle, "Circle"},
        {QScatterSeries::MarkerShapeStar, "Star"},
        {QScatterSeries::MarkerShapePentagon, "Pentagon"},
        {QScatterSeries::MarkerShapeRectangle, "Rectangle"},
        {QScatterSeries::MarkerShapeRotatedRectangle, "RotatedRectangle"}
    };

    std::map<QString, QChartView*> charts;
    std::map<QChartView*, QList<QScatterSeries*>> map_ScatterSeries;
    std::map<QChartView*, QList<QBarSeries*>> map_BarSeries;
    std::map<QChartView*, std::vector<int>> axis_count_ticks;

    void add_point(QScatterSeries *series, std::map<int, double>& map1,  std::map<int, double>& map2,
                   const int& row, const double& value1=0, const double& value2=0);

    void AutoScale(QChart *chart, QList<QScatterSeries*> &list_series);


    void allowed_addition(const double& item, const int& row, const int& col);

    void addition(const std::map<int, double>& fiel, const int& row, const int& col);

    void Histogram(const int& count = 0, const int& majorY=4, const int& minorY=4, const QColor& color=QColor(255, 0, 0, 127), const double& width=1);

    void CreateScatterChart(QLayout *layout, const QString& name, const QString& nameX, const QString &nameY,
                            const std::vector<QString>& names_series);

    void AxisSample(QValueAxis *axis, const QString& title, const int& major_count=12, const int& minor_count=4, const int& range=11);

    QString GetName(const int& index);

    void EnableSettingsMinor(bool state);

    void HistogramChangedIntervals(const int& arg1, const QString& type);

    void TicketsCountChanged(const int& index, const int& value, const QString& type);

    QString GetName();
};



#endif // MAINWINDOW_H
