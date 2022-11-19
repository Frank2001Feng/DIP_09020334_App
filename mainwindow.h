#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<imageprocess.h>
#include<QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ImageProcess *forImageProcess=new ImageProcess();
    int flag_inverse=0;
    ~MainWindow();

private slots:
    //初始化
    void init();

    //显示图片
    void showImage(QImage img);

    //点击打开
    void on_pushButton_Open_pressed();

    // 保存图像
    void on_pushButton_Save_pressed();

    // 输入发生变化时
    void on_horizontalSlider_windowLevel_valueChanged(int value);
    void on_horizontalSlider_windowWidth_valueChanged(int value);
    void on_horizontalSlider_scaling_valueChanged(int value);
    void on_horizontalSlider_rotate_valueChanged(int value);

    void on_lineEdit_windowLevel_textChanged(const QString &arg1);
    void on_lineEdit_windowWidth_textChanged(const QString &arg1);
    void on_lineEdit_scaling_textChanged(const QString &arg1);
    void on_lineEdit_rotate_textChanged(const QString &arg1);

    //重置
    void on_pushButton_reset_wL_pressed();
    void on_pushButton_reset_wW_pressed();
    void on_pushButton_reset_S_pressed();
    void on_pushButton_reset_R_pressed();
    void on_pushButton_reset_Flip_pressed();

    //  根据参数 改变图像
    void changeImage();

    //灰度反转
    void on_pushButton_Gray_inverse_pressed();

    //翻转，水平、竖直、对角
    void on_pushButton_Flip_horizontal_pressed();
    void on_pushButton_Flip_vertical_pressed();
    void on_pushButton_Flip_diagonal_pressed();

    //复原原图
    void on_pushButton_Origin_pressed();

    // log
    void on_pushButton_Current_pressed();
    void on_tableWidget_log_itemDoubleClicked(QTableWidgetItem *item);
    void on_pushButton_Clear_log_pressed();

    //图像强化
    void on_pushButton_Enhance_pressed();
    void on_horizontalSlider_enhance_valueChanged(int value);
    void on_lineEdit_enhance_textChanged(const QString &arg1);
    void on_pushButton_rest_enhance_pressed();



private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
