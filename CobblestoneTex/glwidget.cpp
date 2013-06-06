#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent): QGLWidget(QGLFormat(QGL::DoubleBuffer), parent){
	dist = 0.2;
}

void GLWidget::initializeGL(){
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);

	program.addShaderFromSourceFile(QGLShader::Vertex, ":/shader.vert");
	program.addShaderFromSourceFile(QGLShader::Fragment, ":/shader.frag");
	program.link();
	program.bind();
}

void GLWidget::setTex(QImage &tex, QImage &norm){
	glActiveTexture(GL_TEXTURE1);
	deleteTexture(t1);
	t1 = bindTexture(tex);
	program.setUniformValue("colorMap", 1);

	glActiveTexture(GL_TEXTURE0);
	deleteTexture(t2);
	t2 = bindTexture(norm);
	program.setUniformValue("normalMap", 0);
}

void GLWidget::resizeGL(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	program.setUniformValue("screen", width(), height());
	program.setUniformValue("dist", dist);
}

void GLWidget::mouseMoveEvent(QMouseEvent *e){
	program.setUniformValue("light", e->posF().x(), height()-e->posF().y());
	updateGL();
}

void GLWidget::wheelEvent(QWheelEvent *e){
	dist += e->delta()/10000.0f;
	if (dist<0)
		dist = 0;
	program.setUniformValue("dist", dist);
	updateGL();
}

void GLWidget::paintGL(){
	glBegin(GL_QUADS);
		glVertex2f(0, 0); glTexCoord2f(0, 1);
		glVertex2f(width(), 0); glTexCoord2f(0, 0);
		glVertex2f(width(), height()); glTexCoord2f(1, 0);
		glVertex2f(0, height()); glTexCoord2f(1, 1);
	glEnd();
}
