/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_PLOT_H
#define H_PLOT_H

#include <config.h>
#include <h_macros.h>
#include <h_config.h>
#include <QWidget>
#include <QPen>
#include <QDomDocument>

/*
class StrokePoint
{
	int x;
	int y;
	double time;
};
typedef QList<StrokePoint> Stroke;
typedef QList<Stroke> Script;
*/

class Plot : public QWidget
{
private:
	Q_OBJECT
	MY_Q_OBJECT

public:
	Plot(QWidget *parent = 0);
	virtual ~Plot();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

	void setCursor(int x, int y);
	void line(int id, QColor col, int x1, int y1, int x2, int y2);
	void lineTo(int id, QColor col, int x2, int y2);
	void clear();

signals:
	void surfaceCreated();

protected:
	virtual void changeEvent(QEvent * event);
	virtual void resizeEvent(QResizeEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public:
	int selectedObject;
private:
	int cursorX;
	int cursorY;
	//const Script & script;
	QRgb * rgbPlot;
	int * idPlot;
	int size;
	int selectionDistance;
	QImage * image;
};

#endif
