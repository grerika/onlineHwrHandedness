/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <QByteArray>
#include <QString>
#include <QList>
#include <QDomDocument>

class StrokePoint
{
public:
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
	Script() : leftToRightNum(0), rightToLeftNum(0) {}

	bool loadXmlData(QByteArray & data);
	QByteArray getXmlData();
	void calculateHandednessStat();
	void calculateHandedness();

	int leftToRightNum;
	int rightToLeftNum;
private:
	QDomDocument doc;
};

