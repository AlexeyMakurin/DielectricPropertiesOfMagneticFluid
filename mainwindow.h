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

    void on_setting_ChartEpsilon_clicked();

    void on_additional_quantities_clicked();

    void on_clear_clicked();

    void on_download_clicked();

    void on_save_clicked();

    void on_export_dist_clicked();

    void on_export_coul_clicked();

    void on_export_epsilon_clicked();

    void on_lineEdit_C0_textChanged(const QString &arg1);

    void on_lineEdit_D0_textChanged(const QString &arg1);

    void on_lineEdit_eta_textChanged(const QString &arg1);

    void on_lineEdit_temp_textChanged(const QString &arg1);

    void on_ChartEpsilonTittle_textChanged(const QString &arg1);

    void on_ChartEpsilonAxisName_textChanged(const QString &arg1);


    void on_EpsilonTitleFont_clicked();

    void on_EpsilonFontAxis_clicked();

    void on_MajorEpsilonCount_valueChanged(int arg1);

    void on_EpsilonMajorColor_clicked();


    void on_EpsilonMajorStyle_activated(int index);

    void on_EpsilonMinorColor_clicked();

    void on_MinorEpsilonCount_valueChanged(int arg1);

    void on_EpsilonMinorStyle_activated(int index);

    void on_EpsilonseriesColor_clicked();

    void on_EpsilonMarkerSize_valueChanged(int arg1);

    void on_EpsilonMarkers_activated(int index);

    void on_EpsilonSeries_activated(int index);

    void on_EpsilonLegendShow_stateChanged(int arg1);

    void on_EpsilonLegendFont_clicked();

    void on_SettingsChartCoul_clicked();

    void on_TitleChartCoul_textChanged(const QString &arg1);

    void on_FontTitleChartCoul_clicked();

    void on_SettingsHist_clicked();

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

    //Distribution
    QBarSeries *series_dist = new QBarSeries();
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    QValueAxis *axisY = new QValueAxis();
    QChart *chart_dist = new QChart();
    QChartView *view_dist = new QChartView();

    //Other values
    qreal x_max_e = 11;
    qreal y_max_e = 11;
    qreal x_max_coul = 11;
    qreal y_max_coul = 11;

    bool hidden_frame = true;
    bool hidden_setting_e = true;
    bool hidden_setting_coul = true;
    bool hidden_settings_hist = true;
    bool act_cell = true;
    bool align = false;

    void add_point(QScatterSeries *series, std::map<int, double>& map1,  std::map<int, double>& map2,
                   const int& row, const double& value1=0, const double& value2=0);

    void AutoScale(QChart *chart, QScatterSeries *series, double& xMax, double& yMax);

    void CreateChart(QLayout *layout, QChartView *view, QChart *chart, std::vector<QScatterSeries*> series,
                     const QString& ax, const QString& ay, qreal& x_max, qreal& y_max, bool legend=false);

    void allowed_addition(const double& item, const int& row, const int& col);
    void addition(const std::map<int, double>& fiel, const int& row, const int& col);

    void Export(QChartView *view_chart);

};



#endif // MAINWINDOW_H
