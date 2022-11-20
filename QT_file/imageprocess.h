#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <QObject>
#include<QWidget>
#include<QImage>

class ImageProcess:public QWidget
{
    Q_OBJECT
public:
    explicit ImageProcess(QWidget *parent=nullptr);
    QString OpenFile;   //图像路径

    int Width;  //读入图像宽度
    int Height; //读入图像高度
    ushort **grayData;
    ushort **grayData_new;//变化数据

    QImage img_origin;
    QImage img_new;
    QImage img_out;

signals:
public slots:
    //打开RAW图片
    bool openRAW();

    //得到图像RAW
    QImage getImageRAW(int windowLevel,int windowWidth);
    int grayWindow(int windowLevel,int windowWidth,ushort grayValue);

    // 放缩
    void Scaling(float scaling);

    //旋转
    void Rotate(float angel);

    // 灰度反转
    void gray_inverse();

    // 翻转
    void flip_horizontal();
    void flip_vertical();
    void flip_diagonal();

    // 还原
    void origin();

    //保存图像
    void saveImage();

    // 图像强化
    void enhanceImage(int grade);
    ushort getLaplace(int i,int j);
};

#endif // IMAGEPROCESS_H
