#include "imageprocess.h"
#include <QRgb>
#include<QColor>
#include<QFileDialog>
#include<QDebug>
#include <cmath>
#include<math.h>
#include<QCoreApplication>
#include<QDateTime>

using namespace std;
ImageProcess::ImageProcess(QWidget *parent):QWidget(parent)
{

}

bool ImageProcess::openRAW()
{
    OpenFile =QFileDialog::getOpenFileName(this,"打开RAW文件","","*.RAW");
    if(OpenFile.isEmpty())
    {
        return false;
    }
    else{
        QByteArray byteData=OpenFile.toLatin1();
        char * fileData=byteData.data();
        FILE *toRead =fopen(fileData,"rb");
        //依次读入width，height
        unsigned long *ptrPara=new (unsigned long);
        fread(ptrPara,1,4,toRead);
        Width=(int)*ptrPara;
        fread(ptrPara,1,4,toRead);
        Height=(int)*ptrPara;
        // 依次读入数据
        grayData=new ushort*[Width];
        grayData_new=new ushort*[Width];
        for(int i=0;i<Width;i++)
        {
            grayData[i]=new ushort[Height];
            grayData_new[i]=new ushort[Height];
        }
        for(int j=0;j<Height;j++)
        {
            for(int i=0;i<Width;i++)
            {
                fread(ptrPara,1,2,toRead);
                grayData[i][j]=(ushort)*ptrPara;
                grayData_new[i][j]=grayData[i][j];
            }
        }
        fclose(toRead);
        delete ptrPara;
        return true;
    }
    return false;
}

QImage ImageProcess::getImageRAW(int windowLevel, int windowWidth)
{
    QImage newImage=QImage((int)Width,(int)Height,QImage::Format_RGB32);
    for(int j=0;j<(int)Height;j++)
    {
        for(int i=0;i<(int)Width;i++)
        {
            int res=grayWindow(windowLevel,windowWidth,grayData_new[i][j]);;
            newImage.setPixelColor(i,j,QColor(res,res,res));
        }
    }
    img_origin=newImage;
    img_new=newImage;
    img_out=newImage;
    return newImage;
}

int ImageProcess::grayWindow(int windowLevel, int windowWidth, ushort grayValue)
{
    double fwindowLevel=(double)windowLevel;
    double fwindowWidth=(double)windowWidth;
    int minGray=(int)(fwindowLevel-(fwindowWidth)/2.0+0.5);
    int maxGray=(int)(fwindowLevel+(fwindowWidth)/2.0+0.5);
    if(grayValue<=minGray)
        return 0;
    if(grayValue>=maxGray)
        return 255;
    int result=(int)((grayValue-minGray)*255.0/(double)(windowWidth));
    return result;
}

void ImageProcess::Scaling(float scaling)
{
    float dx=scaling;
    float dy=scaling;
    // 传入了放大数值,确定了变化后的参数值
    int width=img_origin.width();
    int height=img_origin.height();
    int newWidth=(int)width*dx;
    int newHeight=(int)height*dy;
    QImage newImage=QImage(newWidth,newHeight,img_origin.format());
    // 双线性插值法确定新图像的对像素的值
    /* 现在开始做反向映射处理
     * 对 newX,newY 像素而言 反向映射后为 srcX,srcY ,srcX=i+u,srcY=j+v
     */
    int i,j;    //表示整数部分
    float u,v;  //表示小数部分
    for(int newX=0;newX<newWidth;newX++)
    {
        for(int newY=0;newY<newHeight;newY++)
        {
            // 对每一个点进行二维插值，可以处理彩色图像
            i=(int)newX/dx;u=(float)(newX/dx-i);
            j=(int)newY/dy;v=(float)(newY/dy-j);
            if(i<width-1&&j<height-1) //防止超界
            {
                  QColor temp00=img_origin.pixel(i,j);
                  QColor temp10=img_origin.pixel(i+1,j);
                  QColor temp01=img_origin.pixel(i,j+1);
                  QColor temp11=img_origin.pixel(i+1,j+1);
                  int rr,gg,bb;
                  rr=(int)((1-u)*(1-v)*temp00.red()+(u)*(1-v)*temp10.red()+(1-u)*(v)*temp01.red()+(u)*(v)*temp11.red());
                  gg=(int)((1-u)*(1-v)*temp00.green()+(u)*(1-v)*temp10.green()+(1-u)*(v)*temp01.green()+(u)*(v)*temp11.green());
                  bb=(int)((1-u)*(1-v)*temp00.blue()+(u)*(1-v)*temp10.blue()+(1-u)*(v)*temp01.blue()+(u)*(v)*temp11.blue());
                  if(rr>255){ rr=255; }
                  if(gg>255){ gg=255; }
                  if(bb>255){ bb=255; }
                  if(rr<0){ rr=0;}
                  if(gg<0){ gg=0;}
                  if(bb<0){ bb=0;}
                  newImage.setPixelColor(newX,newY,QColor(rr,gg,bb));
            }
        }
    }
    img_new=newImage;
    img_out=newImage;
}

void ImageProcess::Rotate(float angel)
{
    // 旋转角度 angel、旋转中心(xx,yy)
    // 角度的单位转化，转弧度,计算 sin,cos 值
    double fRotateAngel=angel*3.1415926535/180.0;
    float fsin,fcos;
    fsin=(float)sin((double)fRotateAngel);
    fcos=(float)cos((double)fRotateAngel);

    // 确定原图像的几何中心（矩阵中心）
    // 以几何中心为旋转中心时
    int width=img_new.width();
    int height=img_new.height();
    float a=(float)(width/2);
    float b=(float)(height/2);

    /*~~~下面开始确定 newImage 的大小和几何中心~~~*/
    // 确定一下 newImage 的中心
    float x1,y1,x2,y2,x3,y3,x4,y4;
    x1=(float)(0-a);y1=(float)(0-b);
    x2=(float)(width-a);y2=(float)(0-b);
    x3=(float)(width-a);y3=(float)(height-b);
    x4=(float)(0-a);y4=(float)(height-b);
    //旋转后四个角的坐标
    float xt1,yt1,xt2,yt2,xt3,yt3,xt4,yt4;
    xt1=fcos*x1-fsin*y1;yt1=fsin*x1+fcos*y1;
    xt2=fcos*x2-fsin*y2;yt2=fsin*x2+fcos*y2;
    xt3=fcos*x3-fsin*y3;yt3=fsin*x3+fcos*y3;
    xt4=fcos*x4-fsin*y4;yt4=fsin*x4+fcos*y4;
    // 计算旋转后的图像真实宽度
    int newWidth,newHeight;
    if(abs(xt3-xt1)>abs(xt4-xt2))
        newWidth=(int)(abs(xt3-xt1));
    else
        newWidth=(int)(abs(xt4-xt2));
    // 计算旋转后的图像真实高度
    if(abs(yt3-yt1)>abs(yt4-yt2))
        newHeight=(int)(abs(yt3-yt1));
    else
        newHeight=(int)(abs(yt4-yt2));

    // 很好~ 这就定出 c、d
    float c=(float)(newWidth/2);
    float d=(float)(newHeight/2);

    // 可以开始确定 QImage 了
    QImage newImage=QImage(newWidth,newHeight,img_new.format());

    /*~~~ newImage 的画布已经准备就绪，现在开始绘制 ~~~*/
    // 逆向映射，判断、并确定像素值
    int i,j;    // 整数部分
    float u,v;  // 小数部分
    for(int newX=0;newX<newWidth;newX++)
    {
        for(int newY=0;newY<newHeight;newY++)
        {
            i=(int)(newX*(fcos)+newY*(fsin)+(-c*fcos-d*fsin+a));
            j=(int)(newX*(-fsin)+newY*(fcos)+(c*fsin-d*fcos+b));
            u=(float)(newX*(fcos)+newY*(fsin)+(-c*fcos-d*fsin+a)-i);
            v=(float)(newX*(-fsin)+newY*(fcos)+(c*fsin-d*fcos+b)-j);

            // 判断很关键
            if(i>=0&&i<width-2&&j>=0&&j<height-2)
            {
                // 双线性插值
                QColor temp00=img_new.pixel(i,j);
                QColor temp10=img_new.pixel(i+1,j);
                QColor temp01=img_new.pixel(i,j+1);
                QColor temp11=img_new.pixel(i+1,j+1);
                int rr,gg,bb;
                rr=(int)((1-u)*(1-v)*temp00.red()+(u)*(1-v)*temp10.red()+(1-u)*(v)*temp01.red()+(u)*(v)*temp11.red());
                gg=(int)((1-u)*(1-v)*temp00.green()+(u)*(1-v)*temp10.green()+(1-u)*(v)*temp01.green()+(u)*(v)*temp11.green());
                bb=(int)((1-u)*(1-v)*temp00.blue()+(u)*(1-v)*temp10.blue()+(1-u)*(v)*temp01.blue()+(u)*(v)*temp11.blue());
                if(rr>255){ rr=255; }
                if(gg>255){ gg=255; }
                if(bb>255){ bb=255; }
                if(rr<0){ rr=0;}
                if(gg<0){ gg=0;}
                if(bb<0){ bb=0;}
                newImage.setPixelColor(newX,newY,QColor(rr,gg,bb));
            }
            else{
                // 用白色来填充好了
                newImage.setPixelColor(newX,newY,QColor(255,255,255));
            }
        }
    }
    img_out=newImage;
}

void ImageProcess::gray_inverse()
{
    for(int j=0;j<(int)Height;j++)
    {
        for(int i=0;i<(int)Width;i++)
        {
            grayData_new[i][j]=4095-grayData_new[i][j];
        }
    }
}

void ImageProcess::flip_horizontal()
{
    ushort temp;
    for(int j=0;j<(int)Height;j++)
    {
        for(int i=0;i<(int)Width/2;i++)
        {
            temp=grayData_new[i][j];
            grayData_new[i][j]=grayData_new[Width-i-1][j];
            grayData_new[Width-i-1][j]=temp;
        }
    }
}

void ImageProcess::flip_vertical()
{
    ushort temp;
    for(int i=0;i<(int)Width;i++)
    {
        for(int j=0;j<(int)Height/2;j++)
        {
            temp=grayData_new[i][j];
            grayData_new[i][j]=grayData_new[i][Height-1-j];
            grayData_new[i][Height-1-j]=temp;
        }
    }
}

void ImageProcess::flip_diagonal()
{
    flip_horizontal();
    flip_vertical();
}

void ImageProcess::origin()
{
    for(int j=0;j<Height;j++)
    {
        for(int i=0;i<Width;i++)
        {
            grayData_new[i][j]=grayData[i][j];
        }
    }
}

void ImageProcess::saveImage()
{
    QString fileName;
    QPixmap pix;
    QString strDir = QCoreApplication::applicationDirPath()+"\\output\\";
    QDir dir(strDir);

    if(!dir.exists())
    {
        dir.mkdir(strDir);
    }
    fileName = strDir + QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")  + ".bmp";

    pix = QPixmap::fromImage(img_out);
    QString strFile = QFileDialog::getSaveFileName(this,"保存图片",fileName,"PNG(*.png);;BMP(*.bmp);;JPG(*.jpg)");

    if(!strFile.isNull())
    {
        pix.save(strFile);
    }
}

void ImageProcess::enhanceImage(int grade)
{
    for(int j=0;j<(int)Height;j++)
    {
        for(int i=0;i<(int)Width;i++)
        {
            if(i==0||i==int(Width)-1||j==0||j==int(Height)-1)
            {
                grayData_new[i][j]=grayData[i][j];
            }
            else
            {
                ushort enhance=grade*getLaplace(i,j);
                if(enhance<5*grade*grayData[i][j])
                {
                    grayData_new[i][j]=grayData[i][j]+enhance;
                }
                else
                {
                    grayData_new[i][j]=grayData[i][j];
                }
                //grayData_new[i][j]=grayData[i][j]+grade*enhance;
            }
        }
    }
}

ushort ImageProcess::getLaplace(int i, int j)
{
    ushort SuanZi;
    SuanZi=(-grayData[i-1][j-1]-grayData[i-1][j]-grayData[i-1][j+1]
            -grayData[i][j-1]+8*grayData[i][j]-grayData[i][j+1]
            -grayData[i+1][j-1]-grayData[i+1][j]-grayData[i+1][j+1]);
    return SuanZi;
}


