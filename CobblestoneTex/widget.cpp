#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget){
	ui->setupUi(this);
	gl = new GLWidget(ui->frame_5);
	ui->label_3->setText(QString::number((float)ui->horizontalSlider->value()/(float)(ui->horizontalSlider->maximum()/2)));
}

Widget::~Widget(){
	delete ui;
}

void Widget::paintEvent(QPaintEvent *){
	QPainter p(this);
	p.drawImage(ui->frame->pos(), img1);
	p.drawImage(ui->frame_3->pos(), img2);
	p.drawImage(ui->frame_4->pos(), img3);
	p.drawImage(ui->frame_5->pos(), img4);
	p.end();
}

inline float Widget::realRand(){
	return (float)qrand()/INT_MAX;
}

inline float Widget::getRnd(int x, int y){
	return (float)((0x6C078965*(seed^((x*2971902361)^(y*3572953751))))&0x7FFFFFFF)/(float)0x7FFFFFFF;
}

inline float Widget::distance(int x1, int y1, Point &b){
	return (x1-b.x)*(x1-b.x)+(y1-b.y)*(y1-b.y);
	//return sqrt((x1-b.x)*(x1-b.x)+(y1-b.y)*(y1-b.y));
}

inline float Widget::mod(float a, float b){
	return a-(float)((int)(a/b)*(int)b);
}

inline float Widget::getPoint(float x, float y, float cellSize){
	float dx = mod(x, cellSize);
	float dy = mod(y, cellSize);
	float z1 = getRnd(x-dx, y-dy);						// z3 --- z4
	float z2 = getRnd(x-dx+cellSize, y-dy);				//  |     |
	float z3 = getRnd(x-dx, y-dy+cellSize);				//  + z   +
	float z4 = getRnd(x-dx+cellSize, y-dy+cellSize);	// z1 --- z2
	float yNorm = dy/cellSize;
	float ft = yNorm*M_PI;
	float f = (1.0f-cosf(ft))*0.5f;
	float z1z3 = z1*(1.0f-f)+z3*f;
	float z2z4 = z2*(1.0f-f)+z4*f;
	float xNorm = dx/cellSize;
	ft = xNorm*M_PI;
	f = (1.0f-cosf(ft))*0.5f;
	float z = z1z3*(1.0f-f)+z2z4*f;
	return z;
}

void Widget::celluarTex(){
	//сложить с шумом перлина для создания неровных граней
	int w = ui->frame->width();
	int h = ui->frame->height();
	int side = ui->spinBox->value();
	float contrast = (float)ui->horizontalSlider->value()/(float)(ui->horizontalSlider->maximum()/2);

	int cellX = w/side+2;
	int cellY = h/side+2;
	Point points[cellX][cellY];
	for (int i=0; i<cellX; i++)
		for (int j=0; j<cellY; j++){
			points[i][j].x = i*side+qrand()%((int)(side*0.7))+side*0.15-side;
			points[i][j].y = j*side+qrand()%((int)(side*0.7))+side*0.15-side;
		}

	int distBuff[w*h];
	int maxDist = INT_MIN;
	time.start();

	for (int i=0; i<w*h; i++){
		int x = i%w;
		int y = i/w;
		int min = INT_MAX;
		int min2 = INT_MAX;
		int startX = x/side;
		int finishX = startX+3;
		for (int cp, point=0; startX<finishX; startX++){
			int startY = y/side;
			int finishY = startY+3;
			for (; startY<finishY; startY++, point++){
				if (startX<0 || startX>=cellX || startY<0 || startY>=cellY)
					continue;
				int d = distance(x, y, points[startX][startY]);
				if (d<min){
					cp = point;
					min2 = min;
					min = d;
				}
				if (d<min2 && cp!=point)
					min2 = d;
			}
		}
		distBuff[i] = min2-min;
		if (maxDist<distBuff[i])
			maxDist = distBuff[i];
	}

	img1 = QImage(w, h, QImage::Format_ARGB32);
	img1.fill(QColor::fromRgb(0, 0, 0, 0));
	Color *pix = (Color*)img1.bits();

	int avbr = 0;
	for (int i=0; i<w*h; i++){
		pix[i].r = (distBuff[i]*255)/maxDist;
		avbr += pix[i].r;
	}
	avbr /= w*h;
	for (int i=0; i<w*h; i++){
		int res = contrast*(pix[i].r-avbr)+avbr;
		if (res<0)
			res = 0;
		if (res>255)
			res = 255;
		pix[i].r = res;
		pix[i].g = pix[i].b = pix[i].r;
		pix[i].a = 255;
	}

	ui->label_4->setText(QString::number(time.elapsed())+" ms");

	repaint();
}

void Widget::perlinNoise(){
	int w = ui->frame->width();
	int h = ui->frame->height();
	int it = ui->spinBox_2->value();
	int n = ui->spinBox_3->value();
	float contrast = ui->doubleSpinBox->value();

	img2 = QImage(w, h, QImage::Format_ARGB32);
	img2.fill(QColor::fromRgb(0, 0, 0, 0));
	Color *pix = (Color*)img2.bits();

	seed = 0;//qrand();

	time.start();
	for (int i=0; i<w*h; i++){
		float s = 0;
		for (int j=1; j<=n; j++){
			int f = pow(2, n-j);
			float a = pow(0.5, j);
			s += getPoint(i%w, i/w, f)*a;
		}
		float ds = s*(float)it-(float)((int)(s*it));
		ds = contrast*(ds-0.5f)+0.5f;
		if (ds>1)
			ds = 1;
		if (ds<0)
			ds = 0;
		s = ((float)((int)(s*(float)it))+ds)/(float)it;
		pix[i].r = s*255;
		pix[i].g = pix[i].b = pix[i].r;
		pix[i].a = 255;
	}
	ui->label_5->setText(QString::number(time.elapsed())+" ms");

	repaint();
}

void Widget::generate3(){
	int w = ui->frame->width();
	int h = ui->frame->height();

	img3 = QImage(w, h, QImage::Format_ARGB32);
	img3.fill(QColor::fromRgb(0, 0, 0, 0));
	Color *pix = (Color*)img3.bits();
	Color *pix1 = (Color*)img1.bits();
	Color *pix2 = (Color*)img2.bits();

	time.start();
	int v = 50;
	float op = 0.6;
	for (int i=0; i<w*h; i++){
		int r = (1-op)*255+op*pix2[i].r;
		int b = pix1[i].r;
		if (b<=v){
			b = (float)b/v*255.0f;
			r = r-(255-b)*0.7f;
		}
		if (r<0)
			r = 0;
		pix[i].r = r;
		pix[i].g = pix[i].b = pix[i].r;
		pix[i].a = 255;
	}
	ui->label_6->setText(QString::number(time.elapsed())+" ms");

	repaint();
}

void Widget::bumpMap(){
	int w = ui->frame->width();
	int h = ui->frame->height();

	img4 = QImage(w, h, QImage::Format_ARGB32);
	img4.fill(QColor::fromRgb(0, 0, 0, 0));
	Color *pix = (Color*)img4.bits();
	Color *pix2 = (Color*)img3.bits();

	int maxR = 0;
	int maxG = 0;
	for (int i=0; i<w*h; i++){
		int x = i%w;
		int y = i/w;
		if (x>0)
			if ((int)pix2[y*w+x-1].r-(int)pix2[y*w+x].r>maxR)
				maxR = (int)pix2[y*w+x-1].r-(int)pix2[y*w+x].r;
		if (y>0)
			if ((int)pix2[(y-1)*w+x].r-(int)pix2[y*w+x].r>maxG)
				maxG = (int)pix2[(y-1)*w+x].r-(int)pix2[y*w+x].r;
	}
	maxR *= 2;
	maxG *= 2;
	for (int i=0; i<w*h; i++){
		int x = i%w;
		int y = i/w;
		if (x>0)
			pix[y*w+x].r = 127+((int)pix2[y*w+x-1].r-(int)pix2[y*w+x].r)*255/maxR;
		if (y>0)
			pix[y*w+x].g = 127+((int)pix2[(y-1)*w+x].r-pix2[y*w+x].r)*255/maxG;
		pix[i].a = pix[i].b = 255;
	}

	repaint();
}

void Widget::on_pushButton_clicked(){
	celluarTex();
}

void Widget::on_pushButton_2_clicked(){
	perlinNoise();
}

void Widget::on_pushButton_3_clicked(){
	generate3();
}

void Widget::on_horizontalSlider_valueChanged(int value){
	ui->label_3->setText(QString::number((float)value/(float)(ui->horizontalSlider->maximum()/2)));
}

void Widget::on_pushButton_4_clicked(){
	bumpMap();
}

void Widget::cobblestone(){
	texGen.setSeed(ui->spinBox_12->value());
	texGen.setSize(ui->frame->width(), ui->frame->height());

	ColorHSV c;
	c.h = ui->spinBox_6->value();
	c.s = ui->spinBox_7->value();
	c.v = ui->spinBox_8->value();
	;
	time.start();
	ColorRGB *t1 = texGen.cobblestone(ui->spinBox_4->value(),
									  ui->spinBox_5->value(),
									  c,
									  ui->spinBox_9->value(),
									  ui->doubleSpinBox_2->value(),
									  ui->spinBox_10->value(),
									  ui->doubleSpinBox_3->value(),
									  ui->doubleSpinBox_6->value(),
									  ui->spinBox_11->value(),
									  ui->doubleSpinBox_4->value(),
									  ui->doubleSpinBox_5->value(),
									  true);
	ui->label_12->setText(QString::number(time.elapsed())+" ms");
	time.start();
	ColorRG *t2 = texGen.getNormal();
	ui->label_12->setText(ui->label_12->text()+" "+QString::number(time.elapsed())+" ms");

	img1 = QImage(texGen.width(), texGen.height(), QImage::Format_ARGB32);
	img1.fill(QColor::fromRgb(0, 0, 0, 0));
	img2 = QImage(texGen.width(), texGen.height(), QImage::Format_ARGB32);
	img2.fill(QColor::fromRgb(0, 0, 0, 0));
	Color *pix1 = (Color*)img1.bits();
	Color *pix2 = (Color*)img2.bits();
	int n = texGen.width()*texGen.height();
	for (int i=0; i<n; i++){
		pix1[i].r = t1[i].r;
		pix1[i].g = t1[i].g;
		pix1[i].b = t1[i].b;
		pix1[i].a = 255;
		pix2[i].r = t2[i].r;
		pix2[i].g = t2[i].g;
		pix2[i].b = 255;
		pix2[i].a = 255;
	}
	delete[] t1;
	delete[] t2;

	gl->setGeometry(0, 0, ui->frame->width(), ui->frame->height());
	gl->setTex(img1, img2);

	repaint();
}

void Widget::on_pushButton_5_clicked(){
	srandom(QDateTime::currentMSecsSinceEpoch()%INT_MAX);
	ui->spinBox_12->setValue(rand()%ui->spinBox_12->maximum());
}

void Widget::on_spinBox_4_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_spinBox_5_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_spinBox_6_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_spinBox_7_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_spinBox_8_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_spinBox_9_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_doubleSpinBox_2_valueChanged(double arg1){
	cobblestone();
}

void Widget::on_spinBox_10_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_doubleSpinBox_3_valueChanged(double arg1){
	cobblestone();
}

void Widget::on_spinBox_11_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_doubleSpinBox_4_valueChanged(double arg1){
	cobblestone();
}

void Widget::on_doubleSpinBox_5_valueChanged(double arg1){
	cobblestone();
}

void Widget::on_spinBox_12_valueChanged(int arg1){
	cobblestone();
}

void Widget::on_doubleSpinBox_6_valueChanged(double arg1){
	cobblestone();
}
