/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <QtCore/qmath.h>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QDomDocument>

#include <h_macros.h>

class StrokePoint
{
public:
	StrokePoint():x(0),y(0),time(0){}
	double distance(const StrokePoint & p){ return DISTANCE(x, y, p.x, p.y); }
	operator bool(){return x != 0 || y != 0; }
	StrokePoint & operator=(const int & i) { x = i; y = i; return *this; }
	bool operator==(const int & i) { return x == i && y == i; }
	bool operator!=(const StrokePoint & p) { return x != p.x || y != p.y; }
	bool operator==(const StrokePoint & p) { return x == p.x && y == p.y; }
	int x;
	int y;
	double time;
};

class Stroke : public QList<StrokePoint>
{
public:
	enum Orientation {
		None		= 0,
		LeftToRight,
		RightToLeft
	};

	enum Orientation orientation;
	QString colour;
	double start_time;
	double end_time;
};

class Script : public QList<Stroke>
{
public:
	Script() : leftToRightNum(0), rightToLeftNum(0), uncertainty(30.0) {}

	bool loadXmlData(QByteArray & data);
	QByteArray getXmlData();

	void calculateHandednessStat();
	void calculateHandedness();
	void clearAllOrientation();

	int writerId;
	QString sampleId;
	QString handedness = QString("");
	int leftToRightNum;
	int rightToLeftNum;
	bool setHandedness(QByteArray & data);	
	double uncertainty; // maximum allowed wave like move of the hand during drawing straight line
	const int limit = 5; // minimal length for oriented strokes
private:
	int waveHeight(StrokePoint first, StrokePoint last, StrokePoint third);
	void setWriterId(QByteArray & data);
	void setSampleId(QByteArray & data);

	QDomDocument doc;
};

