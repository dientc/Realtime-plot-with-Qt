#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include <qcustomplot.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void initSerial();
    void updatePlot();
    void readSensor();

    void initGyroPlot();
    void initAcclPlot();
    void initTempPlot();

    void replotGyro();
    void replotAccl();
    void replotTemp();

private:
    Ui::MainWindow *ui;
    // Our custom plot widget
    QCustomPlot *m_GyroPlot, *m_AcclPlot, *m_TempPlot;

    // Serial Port reader
    QSerialPort *m_Serial;

    // The time between each update, this
    // will be  used by the timer to call "updatePlot" slot
    qreal timeInterval;

    // Counting on every replotting
    uint m_Counter;

    // Define number of samples plotted
    int m_NumOfSamples;

    // Data buffers
    QVector<qreal> m_OxGyro, m_OxAccl, m_OxTemp;
    QVector<qreal> m_OyGyroX, m_OyGyroY, m_OyGyroZ;
    QVector<qreal> m_OyAcclX, m_OyAcclY, m_OyAcclZ;
    QVector<qreal> m_OyTemp;
};

#endif // MAINWINDOW_H
