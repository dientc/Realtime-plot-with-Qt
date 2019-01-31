#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_GyroPlot( 0 ),
    m_AcclPlot( 0 ),
    m_TempPlot( 0 ),
    m_Serial(0),
    timeInterval( 50 ),// replot after every 0.1 milisecond; sample rate MPU 6050: 1000Hz
    m_Counter(0),
    m_NumOfSamples(200)
{
    ui->setupUi(this);

    // Allocate memory for the plot widget objects
    m_GyroPlot = new QCustomPlot( this );
    m_GyroPlot->setFixedSize(1000, 400);
    initGyroPlot();

    m_AcclPlot = new QCustomPlot( this );
    m_AcclPlot->setFixedSize(1000, 400);
    initAcclPlot();

    m_TempPlot = new QCustomPlot( this );
    m_TempPlot->setFixedSize(1000, 200);
    initTempPlot();


    // Allocate memory for serial reader
    m_Serial = new QSerialPort(this);
    initSerial();

    // Layout the window
    // the plot widget to our application's main window
    QWidget *window = new QWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_GyroPlot);
    layout->addWidget(m_AcclPlot);
    layout->addWidget(m_TempPlot);

    window->setLayout(layout);
    setCentralWidget(window);

    // setup a timer and start it.
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL( timeout() ), this, SLOT( updatePlot() ) );
    timer->start( timeInterval );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSerial()
{
    // Setting for serial read
    m_Serial->setPortName("ttyACM1");
    if(!m_Serial->setBaudRate(QSerialPort::Baud115200))
        qDebug() << m_Serial->errorString();
    if(!m_Serial->setDataBits(QSerialPort::Data8))
        qDebug() << m_Serial->errorString();
    if(!m_Serial->setParity(QSerialPort::NoParity))
        qDebug() << m_Serial->errorString();
    if(!m_Serial->setFlowControl(QSerialPort::NoFlowControl))
        qDebug() << m_Serial->errorString();
    if(!m_Serial->setStopBits(QSerialPort::OneStop))
        qDebug() << m_Serial->errorString();

    if(!m_Serial->open(QIODevice::ReadOnly))
        qDebug() << m_Serial->errorString();
    qDebug() << m_Serial->bytesAvailable();
}

void MainWindow::initGyroPlot()
{
    m_GyroPlot->addGraph();
    m_GyroPlot->graph( 0 )->setPen( QColor(Qt::red) );
    // Add Gyro Y graph
    m_GyroPlot->addGraph();
    m_GyroPlot->graph( 1 )->setPen( QColor(Qt::green) );
    // Add Gyro Z graph
    m_GyroPlot->addGraph();
    m_GyroPlot->graph( 2 )->setPen( QColor(Qt::black) );

    // should set this in the constructor
    m_GyroPlot->xAxis->setRange(0, m_NumOfSamples);
    m_GyroPlot->yAxis->setRange(-2000, 2000);

    // setting label
    m_GyroPlot->xAxis->setLabel("sample");
    m_GyroPlot->yAxis->setLabel("gyro");
}

void MainWindow::initAcclPlot()
{
    m_AcclPlot->addGraph();
    m_AcclPlot->graph( 0 )->setPen( QColor(Qt::red) );
    // Add Accel Y graph
    m_AcclPlot->addGraph();
    m_AcclPlot->graph( 1 )->setPen( QColor(Qt::green) );
    // Add Accel Z graph
    m_AcclPlot->addGraph();
    m_AcclPlot->graph( 2 )->setPen( QColor(Qt::black) );


    // should set this in the constructor
    m_AcclPlot->xAxis->setRange(0, m_NumOfSamples);
    m_AcclPlot->yAxis->setRange(-16, 16);

    // setting label
    m_AcclPlot->xAxis->setLabel("sample");
    m_AcclPlot->yAxis->setLabel("accl");
}

void MainWindow::initTempPlot()
{
    m_TempPlot->addGraph();
    m_TempPlot->graph( 0 )->setPen( QColor(Qt::red) );

    // should set this in the constructor
    m_TempPlot->xAxis->setRange(0, m_NumOfSamples);
    m_TempPlot->yAxis->setRange(0, 50);

    // setting label
    m_TempPlot->xAxis->setLabel("sample");
    m_TempPlot->yAxis->setLabel("temp");
}

void MainWindow::updatePlot()
{
    // Read MPU6050 to get data
    readSensor();

    // Update all plots
    replotGyro();
    replotAccl();
    replotTemp();
}

void MainWindow::replotGyro()
{
    if(m_OxGyro.size() > m_NumOfSamples)
    {
        m_OxGyro.remove(0);
        m_OyGyroX.remove(0);
        m_OyGyroY.remove(0);
        m_OyGyroZ.remove(0);

        m_GyroPlot->xAxis->setRange(m_Counter - m_NumOfSamples, m_Counter);
    }

    m_GyroPlot->graph(0)->setData(m_OxGyro, m_OyGyroX);
    m_GyroPlot->graph(1)->setData(m_OxGyro, m_OyGyroY);
    m_GyroPlot->graph(2)->setData(m_OxGyro, m_OyGyroZ);

    m_GyroPlot->replot();
}

void MainWindow::replotAccl()
{
    if(m_OxAccl.size() > m_NumOfSamples)
    {
        m_OxAccl.remove(0);
        m_OyAcclX.remove(0);
        m_OyAcclY.remove(0);
        m_OyAcclZ.remove(0);

        m_AcclPlot->xAxis->setRange(m_Counter - m_NumOfSamples, m_Counter);
    }

    m_AcclPlot->graph(0)->setData(m_OxAccl, m_OyAcclX);
    m_AcclPlot->graph(1)->setData(m_OxAccl, m_OyAcclY);
    m_AcclPlot->graph(2)->setData(m_OxAccl, m_OyAcclZ);

    m_AcclPlot->replot();
}

void MainWindow::replotTemp()
{
    if(m_OxTemp.size() > m_NumOfSamples)
    {
        m_OxTemp.remove(0);
        m_OyTemp.remove(0);

        m_TempPlot->xAxis->setRange(m_Counter - m_NumOfSamples, m_Counter);
    }

    m_TempPlot->graph(0)->setData(m_OxTemp, m_OyTemp);

    m_TempPlot->replot();
}

void MainWindow::readSensor()
{
    if(m_Serial->isOpen())
    {
        if(!m_Serial->waitForReadyRead(-1)) //block until new data arrives
            qDebug() << "error: " << m_Serial->errorString();
        else
        {
            //qDebug() << "New data available: " << m_Serial->bytesAvailable();
            if (m_Serial->bytesAvailable() >= 14)
            {
                QByteArray datas = m_Serial->readAll();
                //qDebug() << datas;
                int16_t gx = (int16_t)(datas[8]<<8) | (int16_t)(datas[9]);
                float fgx = (float)gx/(32768.0f/2000.0f);
                int16_t gy = (int16_t)(datas[10]<<8) | (int16_t)(datas[11]);
                float fgy = (float)gy/(32768.0f/2000.0f);
                int16_t gz = (int16_t)(datas[12]<<8) | (int16_t)(datas[13]);
                float fgz = (float)gz/(32768.0f/2000.0f);
                //qDebug() << "gyro: " << fgx << fgy << fgz;
                m_OyGyroX.append(fgx);
                m_OyGyroY.append(fgy);
                m_OyGyroZ.append(fgz);

                int16_t ax = (int16_t)(datas[0]<<8) | (int16_t)(datas[1]);
                float fax = (float)ax/2048.0f;
                int16_t ay = (int16_t)(datas[2]<<8) | (int16_t)(datas[3]);
                float fay = (float)ay/2048.0f;
                int16_t az = (int16_t)(datas[4]<<8) | (int16_t)(datas[5]);
                float faz = (float)az/2048.0f;
                //qDebug() << "accl: " << fax << fay << faz;
                m_OyAcclX.append(fax);
                m_OyAcclY.append(fay);
                m_OyAcclZ.append(faz);

                int16_t temp = (int16_t)(datas[6]<<8) | (int16_t)(datas[7]);
                float ftemp = (float)temp/340.0f+36.53f;
                //qDebug() << "temp: "<< ftemp;
                m_OyTemp.append(ftemp);

                // Increase sample counter!!!
                m_OxGyro.append(m_Counter);
                m_OxAccl.append(m_Counter);
                m_OxTemp.append(m_Counter);
                ++m_Counter;
            }
        }
    } else {
        qDebug() << "device close!!!";
    }
}
