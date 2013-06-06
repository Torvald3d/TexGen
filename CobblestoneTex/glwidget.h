#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMouseEvent>

class GLWidget: public QGLWidget{
public:
	GLWidget(QWidget *parent = 0);
	QGLShaderProgram program;

	void setTex(QImage &tex, QImage &norm);
	int t1, t2;
	float dist;

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void mouseMoveEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
};

#endif // GLWIDGET_H
