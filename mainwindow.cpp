#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QScrollArea>
#include<QMessageBox>

using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //图像可抓取移动
    ui->graphicsView->setDragMode(QGraphicsView().ScrollHandDrag);
    // tableWidget
    ui->tableWidget_log->setSelectionBehavior(QAbstractItemView::SelectRows);       // 整行选中的方式
    ui->tableWidget_log->verticalHeader()->setVisible(false);                       // 隐藏行表头
    ui->tableWidget_log->resizeColumnsToContents();                                 // 列宽与内容匹配
    ui->tableWidget_log->resizeRowsToContents();                                    // 行高与内容匹配
    ui->tableWidget_log->resizeColumnToContents(1);                                 // 自动调整列宽
    ui->tableWidget_log->setColumnCount(7);                                         // 设置列数
    QHeaderView* widthlist = ui->tableWidget_log->horizontalHeader();               // 创建列宽list
    widthlist->setStyleSheet("QHeaderView::section{border:0px;background-color:#f2f1f7;color:#7f7f7f;}");//设置颜色
    widthlist->setSectionResizeMode(QHeaderView::Stretch);
    QStringList list;                                                        // 创建列表头list
    list << "序号" << "窗宽" << "窗位" << "放缩" << "旋转" << "锐化"<<"反转";
    ui->tableWidget_log->setHorizontalHeaderLabels(list);
    ui->tableWidget_log->show();
    ui->tableWidget_log->setEditTriggers(QAbstractItemView::NoEditTriggers); //不可编辑
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    ui->lineEdit_windowLevel->setText(QString::number(2048));
    ui->lineEdit_windowWidth->setText(QString::number(4096));
    ui->lineEdit_scaling->setText(QString::number((float)10/10,'f',1));
    ui->lineEdit_rotate->setText(QString::number(0));
    ui->lineEdit_enhance->setText(QString::number(0));
}

void MainWindow::showImage(QImage img)
{
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
}

void MainWindow::on_pushButton_Open_pressed()
{
    init();
    on_pushButton_Clear_log_pressed();
    if(forImageProcess->openRAW())
    {
        int windowLevel=ui->lineEdit_windowLevel->text().toInt();
        int windowWidth=ui->lineEdit_windowWidth->text().toInt();
        showImage(forImageProcess->getImageRAW(windowLevel,windowWidth));
    }
}

void MainWindow::on_horizontalSlider_windowLevel_valueChanged(int value)
{
    ui->lineEdit_windowLevel->setText(QString::number(value));
}

void MainWindow::on_horizontalSlider_windowWidth_valueChanged(int value)
{
    ui->lineEdit_windowWidth->setText(QString::number(value));
}

void MainWindow::on_horizontalSlider_scaling_valueChanged(int value)
{
    ui->lineEdit_scaling->setText(QString::number((float)value/10,'f',1));
}

void MainWindow::on_horizontalSlider_rotate_valueChanged(int value)
{
    ui->lineEdit_rotate->setText(QString::number(value));
}


void MainWindow::on_lineEdit_windowLevel_textChanged(const QString &arg1)
{
    int windowLevel=arg1.toInt();
    ui->horizontalSlider_windowLevel->setValue(windowLevel);
    if(!arg1.isEmpty())
    {
        changeImage();
    }
    else
    {
        //QMessageBox::information(this,tr("错误警告"),tr("参数不能为空！"));
        ui->graphicsView->showNormal();
        return;
    }
}

void MainWindow::on_lineEdit_windowWidth_textChanged(const QString &arg1)
{
    int windowWidth=arg1.toInt();
    ui->horizontalSlider_windowWidth->setValue(windowWidth);
    if(!arg1.isEmpty())
    {
        changeImage();
    }
    else{
        //QMessageBox::information(this,tr("错误警告"),tr("参数不能为空！"));
        ui->graphicsView->showNormal();
        return;
    }
}


void MainWindow::on_lineEdit_scaling_textChanged(const QString &arg1)
{
    float scaling =arg1.toFloat();
    ui->horizontalSlider_scaling->setValue(int(scaling*10));
    if(!arg1.isEmpty())
    {
        float rotate=ui->lineEdit_rotate->text().toFloat();
        forImageProcess->Scaling(scaling);
        forImageProcess->Rotate(rotate);
        showImage(forImageProcess->img_out);
    }
    else{
        //QMessageBox::information(this,tr("错误警告"),tr("参数不能为空！"));
        ui->graphicsView->showNormal();
        return;
    }
}

void MainWindow::on_lineEdit_rotate_textChanged(const QString &arg1)
{
    int rotate=arg1.toInt();
    ui->horizontalSlider_rotate->setValue(rotate);
    if(!arg1.isEmpty())
    {
        float scaling=ui->lineEdit_scaling->text().toFloat();
        forImageProcess->Scaling(scaling);
        forImageProcess->Rotate(rotate);
        showImage(forImageProcess->img_out);
    }
    else{
        //QMessageBox::information(this,tr("错误警告"),tr("参数不能为空！"));
        ui->graphicsView->showNormal();
        return;
    }
}

void MainWindow::changeImage()
{
    int windowLevel=ui->lineEdit_windowLevel->text().toInt();
    int windowWidth=ui->lineEdit_windowWidth->text().toInt();
    float scaling=ui->lineEdit_scaling->text().toFloat();
    float rotate=ui->lineEdit_rotate->text().toFloat();
    forImageProcess->getImageRAW(windowLevel,windowWidth);
    forImageProcess->Scaling(scaling);
    forImageProcess->Rotate(rotate);
    showImage(forImageProcess->img_out);
}

void MainWindow::on_pushButton_Gray_inverse_pressed()
{
    ui->lineEdit_windowLevel->setText(QString::number(4096-ui->lineEdit_windowLevel->text().toInt()));
    forImageProcess->gray_inverse();
    changeImage();
    flag_inverse=(flag_inverse+1)%2;
}

void MainWindow::on_pushButton_reset_wL_pressed()
{
    ui->lineEdit_windowLevel->setText(QString::number(2048));
}

void MainWindow::on_pushButton_reset_wW_pressed()
{
    ui->lineEdit_windowWidth->setText(QString::number(4096));
}

void MainWindow::on_pushButton_reset_S_pressed()
{
    ui->lineEdit_scaling->setText(QString::number((float)10/10,'f',1));
}

void MainWindow::on_pushButton_reset_R_pressed()
{
    ui->lineEdit_rotate->setText(QString::number(0));
}

void MainWindow::on_pushButton_Flip_horizontal_pressed()
{
    forImageProcess->flip_horizontal();
    changeImage();
}

void MainWindow::on_pushButton_Flip_vertical_pressed()
{
    forImageProcess->flip_vertical();
    changeImage();
}

void MainWindow::on_pushButton_Flip_diagonal_pressed()
{
    forImageProcess->flip_diagonal();
    changeImage();
}

void MainWindow::on_pushButton_Origin_pressed()
{
    init();//初始化
    forImageProcess->origin();
    changeImage();
}

void MainWindow::on_pushButton_Current_pressed()
{
    int rowcount=ui->tableWidget_log->rowCount();
    ui->tableWidget_log->insertRow(rowcount);     //新增行
    ui->tableWidget_log->setItem(rowcount,0,new QTableWidgetItem(QString::number(rowcount)));
    ui->tableWidget_log->setItem(rowcount,1,new QTableWidgetItem(ui->lineEdit_windowLevel->text()));
    ui->tableWidget_log->setItem(rowcount,2,new QTableWidgetItem(ui->lineEdit_windowWidth->text()));
    ui->tableWidget_log->setItem(rowcount,3,new QTableWidgetItem(ui->lineEdit_scaling->text()));
    ui->tableWidget_log->setItem(rowcount,4,new QTableWidgetItem(ui->lineEdit_rotate->text()));
    ui->tableWidget_log->setItem(rowcount,5,new QTableWidgetItem(ui->lineEdit_enhance->text()));
    ui->tableWidget_log->setItem(rowcount,6,new QTableWidgetItem(QString::number(flag_inverse)));
}

void MainWindow::on_tableWidget_log_itemDoubleClicked(QTableWidgetItem *item)
{
    int row=item->row();

    flag_inverse=ui->tableWidget_log->item(row,6)->text().toInt();

    ui->lineEdit_windowLevel->setText(ui->tableWidget_log->item(row,1)->text());
    ui->lineEdit_windowWidth->setText(ui->tableWidget_log->item(row,2)->text());
    ui->lineEdit_scaling->setText(ui->tableWidget_log->item(row,3)->text());
    ui->lineEdit_rotate->setText(ui->tableWidget_log->item(row,4)->text());
    ui->lineEdit_enhance->setText(ui->tableWidget_log->item(row,5)->text());
    on_pushButton_Enhance_pressed();
}

void MainWindow::on_pushButton_Save_pressed()
{
    forImageProcess->saveImage();
}

void MainWindow::on_pushButton_Clear_log_pressed()
{
    int row_all=ui->tableWidget_log->rowCount();
    for(int i=0;i<=row_all;++i){
        ui->tableWidget_log->removeRow(0);//每次都把第一行删除
    }
}

void MainWindow::on_horizontalSlider_enhance_valueChanged(int value)
{
    ui->lineEdit_enhance->setText(QString::number(value));
}

void MainWindow::on_lineEdit_enhance_textChanged(const QString &arg1)
{
    int grade=arg1.toInt();
    ui->horizontalSlider_enhance->setValue(grade);
}

void MainWindow::on_pushButton_rest_enhance_pressed()
{
    ui->lineEdit_enhance->setText(QString::number(0));
    forImageProcess->enhanceImage(ui->lineEdit_enhance->text().toInt());
    changeImage();
}
void MainWindow::on_pushButton_Enhance_pressed()
{
    if(flag_inverse==0){
        forImageProcess->enhanceImage(ui->lineEdit_enhance->text().toInt());
    }
    else{
        forImageProcess->enhanceImage(ui->lineEdit_enhance->text().toInt());
        forImageProcess->gray_inverse();
    }
    changeImage();
}

void MainWindow::on_pushButton_reset_Flip_pressed()
{
    forImageProcess->origin();
    forImageProcess->enhanceImage(ui->lineEdit_enhance->text().toInt());
    changeImage();
}
