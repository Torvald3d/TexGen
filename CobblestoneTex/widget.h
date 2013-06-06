#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QTime>
#include <math.h>
#include <QDateTime>
#include "glwidget.h"
#include "proceduraltexture.h"

struct Point{
	int x, y;
};

struct Color{
	unsigned int b: 8;
	unsigned int g: 8;
	unsigned int r: 8;
	unsigned int a: 8;
};

namespace Ui {
class Widget;
}

class Widget : public QWidget{
Q_OBJECT
public:
	explicit Widget(QWidget *parent = 0);
	~Widget();

	float realRand();
	float distance(int x1, int y1, Point &b); //quad
	float getPoint(float x, float y, float cellSize);
	float mod(float a, float b);
	float getRnd(int x, int y);
	void celluarTex();
	void perlinNoise();
	void generate3();
	void bumpMap();

	QImage img1;
	QImage img2;
	QImage img3;
	QImage img4;
	int seed;
	QTime time;

	ProceduralTexture texGen;

	GLWidget *gl;

private slots:
	void paintEvent(QPaintEvent *);
	void on_pushButton_clicked();
	void on_horizontalSlider_valueChanged(int value);
	void on_pushButton_2_clicked();
	void on_pushButton_3_clicked();
	void on_pushButton_4_clicked();
	void on_pushButton_5_clicked();

	void cobblestone();

	void on_spinBox_4_valueChanged(int arg1);
	void on_spinBox_5_valueChanged(int arg1);
	void on_spinBox_6_valueChanged(int arg1);
	void on_spinBox_7_valueChanged(int arg1);
	void on_spinBox_8_valueChanged(int arg1);
	void on_spinBox_9_valueChanged(int arg1);
	void on_doubleSpinBox_2_valueChanged(double arg1);
	void on_spinBox_10_valueChanged(int arg1);
	void on_doubleSpinBox_3_valueChanged(double arg1);
	void on_spinBox_11_valueChanged(int arg1);
	void on_doubleSpinBox_4_valueChanged(double arg1);
	void on_doubleSpinBox_5_valueChanged(double arg1);
	void on_spinBox_12_valueChanged(int arg1);

	void on_doubleSpinBox_6_valueChanged(double arg1);

private:
	Ui::Widget *ui;
};

#endif // WIDGET_H
