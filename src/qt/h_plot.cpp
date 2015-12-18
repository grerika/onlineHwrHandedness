/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <QtGui>

#include <config.h>
#include <h_macros.h>

#include "h_plot.h"

Plot::Plot(QWidget *parent) : QWidget(parent),
	selectedObject(0),
	cursorX(0),
	cursorY(0),
	rgbPlot(0),
	idPlot(0),
	size(0),
	selectionDistance(10),
	image(0)
{
}

Plot::~Plot()
{
	if(image)
		delete image;
	if(idPlot)
		delete[] idPlot;
}

void plotImageCleanupHandler(void *info)
{
	delete[] (QRgb*)(info);
}

void Plot::changeEvent(QEvent * event)
{
	QWidget::changeEvent(event);
}

void Plot::resizeEvent(QResizeEvent * event)
{
	if(layout() && (event->size() == event->oldSize()))
		return;

	if(idPlot)
		delete[] idPlot;
	size = width() * height();
	//LOG("Resize event: Width: %, Height: %, Size: %", width(), height(), size);
	idPlot = new int[size];
	rgbPlot = new QRgb[size];
	image = new QImage((const uchar *)(rgbPlot),
			width(), height(),
			QImage::Format_ARGB32, plotImageCleanupHandler, rgbPlot);

	surfaceCreated();
}

QSize Plot::sizeHint() const
{
	return QSize(600, 600);
}

QSize Plot::minimumSizeHint() const
{
	return QSize(400, 400);
}

void Plot::paintEvent(QPaintEvent * event)
{
	(void)event;

	QPainter widgetPainter(this);
	widgetPainter.drawImage(0, 0, *image);
}

void Plot::mousePressEvent(QMouseEvent * event)
{
	if(!rgbPlot || !idPlot)
		return;
	//LOG("line drawing: Width: %, Height: %, Size: %", width(), height(), size);
	int width = this->width();
	int height = this->height();
	if(size != width * height){
		LOG("Widht and or height changed since plot area allocated.");
		return;
	}

	int x = event->x();
	int y = event->y();

	int prevSelectedObject = selectedObject;
	selectedObject = 0;
	for(int d = 0; d <= selectionDistance; d++){
		selectedObject = idPlot[qAbs(y - d) * width + x]; if(selectedObject) break; // up
		selectedObject = idPlot[y * width + qAbs(x - d)]; if(selectedObject) break; // left
		selectedObject = idPlot[(y + d) * width + x]; if(selectedObject) break; // down
		selectedObject = idPlot[y * width + x + d]; if(selectedObject) break; // right
	}
	if(prevSelectedObject != selectedObject){
		LOG("Found line selectedObject: %", selectedObject);
		surfaceCreated();
	}
}

void Plot::setCursor(int x, int y)
{
	cursorX = x;
	cursorY = y;
}

void Plot::line(int id, QColor col, int x1, int y1, int x2, int y2)
{
	if(!rgbPlot || !idPlot)
		return;
	//LOG("line drawing: Width: %, Height: %, Size: %", width(), height(), size);
	int width = this->width();
	int height = this->height();
	if(size != width * height){
		LOG("Widht and or height changed since plot area allocated.");
		return;
	}

	//int length = qSqrt(qPow(x2 - x1, 2) + qPow(y2 - y1, 2));
	double m = double(x2 - x1) / double(y2 - y1);

	int xD = x2 - x1;
	int xL = qAbs(xD);
	//LOG("xL: %", xL);
	int s = (xL == 0) ? 1 : xL/xD;
	//LOG("xL sign: %", s);
	for(int x = 0; x < xL; x++){
		int xx = x1 + s * x;
		int yy = s*x/m + y1;
		if(width <= xx) break;
		if(height <= yy) break;
		if(xx < 0) break;
		if(yy < 0) break;
		int idx = width * yy + xx;
		//LOG("Drawing on x, idx is ", idx);
		rgbPlot[idx] = col.rgb();
		idPlot [idx] = id;
	}

	int yD = y2 - y1;
	int yL = qAbs(yD);
	//LOG("yL: %", yL);
	s = (yL == 0) ? 1 : yL/yD;
	//LOG("yL sign: %", s);
	for(int y = 0; y < yL; y++){
		int xx = s*y*m + x1;
		int yy = y1 + s * y;
		if(width <= xx) break;
		if(height <= yy) break;
		if(xx < 0) break;
		if(yy < 0) break;
		int idx = width * yy + xx;
		//LOG("Drawing on y, idx is ", idx);
		rgbPlot[idx] = col.rgb();
		idPlot [idx] = id;
	}
}

void Plot::lineTo(int id, QColor col, int x2, int y2)
{
	line(id, col, cursorX, cursorY, x2, y2);
	setCursor(x2, y2);
}

void Plot::clear()
{
	int width = this->width();
	int height = this->height();

	if(size != width * height){
		LOG("Widht and or height changed since plot area allocated.");
		return;
	}

	QColor transparent(0, 0, 0, 0);
	for(int i=0; i<size; i++){
		rgbPlot[i] = transparent.rgba();
		idPlot[i] = 0;
	}
}


