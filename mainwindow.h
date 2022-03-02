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

private:
    Ui::MainWindow *ui;

    QTableWidgetItem * protoitem = new QTableWidgetItem();

    //Charts of Epsilon
    QScatterSeries *series_re_e = new  QScatterSeries();
    QScatterSeries *series_im_e = new QScatterSeries();
    QChart *chart_epsilon = new QChart();
    QChartView *view_epsilon = new QChartView();

    //Charts Coul-Coul
    QScatterSeries *series_coul = new QScatterSeries();
    QChart *chart_coul = new QChart();
    QChartView *view_coul = new QChartView();


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


    std::map<int, QChart*> charts = {
        {0, chart_epsilon},
        {1, chart_coul},

    };

    std::map<QChart*, QList<QScatterSeries*>> map_ScatterSeries = {
        {chart_epsilon, {series_re_e, series_im_e}},
        {chart_coul, {series_coul}},

    };

    std::map<QChart*, QList<QBarSeries*>> map_BarSeries = {};


    void add_point(QScatterSeries *series, std::map<int, double>& map1,  std::map<int, double>& map2,
                   const int& row, const double& value1=0, const double& value2=0);

    void AutoScale(QChart *chart, QList<QScatterSeries*> &list_series) ;

    void CreateChart(QLayout *layout, QChartView *view, QChart *chart, std::vector<QScatterSeries*> series,
                     const QString& ax, const QString& ay, bool legend=false);

    void allowed_addition(const double& item, const int& row, const int& col);
    void addition(const std::map<int, double>& fiel, const int& row, const int& col);

    void Export(QChartView *view_chart);

};



#endif // MAINWINDOW_H
