/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include "h_data.h"

bool Script::loadXmlData(QByteArray & data)
{
	clear();

	if(!doc.setContent(data))
		return false;

	// now lets read strokes from dom
	QDomElement docElem = doc.documentElement();
	QDomNodeList strokeNodes = docElem.elementsByTagName("Stroke");

	// find width and height of the area to be plotted
	for(int i=0; i<strokeNodes.length(); i++){
		QDomNode strokeNode = strokeNodes.item(i);
		QDomElement strokeElem = strokeNode.toElement();
		if(strokeElem.isNull())
			continue;

		Stroke s;

		QDomAttr attr = strokeElem.attributeNode("orientation");
		if(attr.isNull())
			s.orientation = Stroke::Orientation::None;
		else {
			if(attr.value() == "leftToRight")
				s.orientation = Stroke::Orientation::LeftToRight;
			else if(attr.value() == "rightToLeft")
				s.orientation = Stroke::Orientation::RightToLeft;
			else if(attr.value() == "")
				s.orientation = Stroke::Orientation::None;
			else {
				LOG("Unknown orientation value: '%'", attr.value());
				return false;
			}
		}

		s.colour = strokeElem.attributeNode("colour").value();
		s.start_time = strokeElem.attributeNode("start_time").value().toDouble();
		s.end_time = strokeElem.attributeNode("end_time").value().toDouble();

		QDomNodeList pointNodes = strokeElem.elementsByTagName("Point");
		for(int j=0; j<pointNodes.length(); j++){
			QDomNode pointNode = pointNodes.item(j);
			QDomElement pointElem = pointNode.toElement();
			if(pointElem.isNull())
				continue;

			StrokePoint p;

			attr = pointElem.attributeNode("x");
			if(attr.isNull())
				continue;
			p.x = attr.value().toInt();

			attr = pointElem.attributeNode("y");
			if(attr.isNull())
				continue;
			p.y = attr.value().toInt();

			attr = pointElem.attributeNode("time");
			if(attr.isNull())
				continue;
			p.time = attr.value().toDouble();

			s.append(p);
		}

		append(s);
	}
	for(int i=0,j=removeNoise();i<5 && j>0;i++,j=removeNoise())
		LOG("%. iteration, % removed points",i,j);

	calculateHandednessStat();
	setWriterId(data);
	return true;
}

double Script::averageStrokeSize()
{
	double ret = 0.0;
	for(int i=0; i<size(); i++){
		Stroke s = at(i);
		ret += s.size();
	}
	if (size() != 0)
		ret = ret/size();

	return ret;
}

double Script::averageStrokeDiagonal()
{
	double ret = 0.0;
	for(int i=0; i<size(); i++){
		Stroke s = at(i);
		ret += s.diagonal();
	}
	if (size() != 0)
		ret = ret/size();

	return ret;
}

double Script::averageStrokeHeight()
{
	double ret = 0.0;
	for(int i=0; i<size(); i++){
		Stroke s = at(i);
		ret += s.height();
	}
	if (size() != 0)
		ret = ret/size();

	return ret;
}

double Script::averageStrokeWidth()
{
	double ret = 0.0;
	for(int i=0; i<size(); i++){
		Stroke s = at(i);
		ret += s.width();
	}
	if (size() != 0)
		ret = ret/size();

	return ret;
}

int Script::removeNoise()
{
	int cnt=0;

	double maxHeight = averageStrokeHeight();
	double maxWidth  = averageStrokeWidth();

	bool boolRemovedPoint;
	for(int i=0; i<size(); i++){
		Stroke s = at(i);
		boolRemovedPoint = false;
		for (int j=1;j<s.size();j++){
			StrokePoint p = s.at(j);
			StrokePoint pPrev = s.at(j-1);
			double pHeight = abs(pPrev.y-p.y);
			double pWidth  = abs(pPrev.x-p.x);
			if (pHeight+pWidth > 2*(maxHeight + maxWidth)) {
				LOG("\tRemove point %th from stroke %th (height: %, width: %)", j,i,pHeight,pWidth);
				s.removeAt(j);
				boolRemovedPoint = true;
				cnt++;
				j--;
			}
		}
		if ( boolRemovedPoint )
			replace(i,s);
	}

	return cnt;
}

QByteArray Script::getXmlData()
{
	QDomElement docElem = doc.documentElement();
	QDomNode strokeSetNode = docElem.elementsByTagName("StrokeSet").item(0);
	if(strokeSetNode.isNull()){
		strokeSetNode = doc.createElement("StrokeSet");
		docElem.appendChild(strokeSetNode);
	}
	QDomNodeList strokeNodes = docElem.elementsByTagName("Stroke");
	for(int i=strokeNodes.length(); 0<=i; i--){
		QDomNode strokeNode = strokeNodes.item(i);
		strokeSetNode.removeChild(strokeNode);
	}
	for(int i=0; i<size(); i++){
		Stroke s = at(i);
		QDomElement stroke = doc.createElement("Stroke");
		stroke.setAttribute("colour", s.colour);
		stroke.setAttribute("start_time", s.start_time);
		stroke.setAttribute("end_time", s.end_time);
		if ( s.length() >= limit )
			switch(s.orientation){
				case Stroke::Orientation::LeftToRight :
					stroke.setAttribute("orientation", "leftToRight");
					break;
				case Stroke::Orientation::RightToLeft :
					stroke.setAttribute("orientation", "rightToLeft");
					break;
				default: break;
			}
		for(int j=0; j<s.size(); j++){
			StrokePoint & p = s[j];
			QDomElement point = doc.createElement("Point");
			point.setAttribute("x", p.x);
			point.setAttribute("y", p.y);
			point.setAttribute("time", p.time);
			stroke.appendChild(point);
		}
		strokeSetNode.appendChild(stroke);
	}
	for(int i=0,j=removeNoise();i<5 && j>0;i++,j=removeNoise())
		LOG("%. iteration, % removed points",i,j);

	// set writerId and sampleId
	QDomNodeList formNodes = docElem.elementsByTagName("Form");
	QDomAttr attr = formNodes.item(0).toElement().attributeNode("writerID");
	if(attr.isNull())
		writerId = -1;
	else
		writerId = attr.value().toInt();
	attr = formNodes.item(0).toElement().attributeNode("id");
	if(attr.isNull())
		sampleId = "Missing writer id";
	else
		sampleId = attr.value();
	LOG("SampleId '%'", sampleId);
	return doc.toByteArray(2);
}

void Script::setWriterId(QByteArray & data)
{
	writerId = 0;
	if(!doc.setContent(data)){
		writerId = -1;
	}else{	// determine writerid
		QDomElement docElem      = doc.documentElement();
		QDomNodeList strokeNodes = docElem.elementsByTagName("Form");
		QDomAttr attr = strokeNodes.item(0).toElement().attributeNode("writerID");
		if(attr.isNull())
			writerId = -2;
		else
			writerId = attr.value().toInt();
	}
	setSampleId(data);
	LOG("writerId '%'", writerId);
	LOG("SampleId '%'", sampleId);
}

void Script::setSampleId(QByteArray & data)
{
	sampleId = "0";
	if(!doc.setContent(data)){
		sampleId = "Invalid data file";
	}else{	// determine sampleId
		QDomElement docElem      = doc.documentElement();
		QDomNodeList strokeNodes = docElem.elementsByTagName("Form");
		QDomAttr attr = strokeNodes.item(0).toElement().attributeNode("id");
		if(attr.isNull())
			sampleId = "Form element's id attribute is empty";
		else
			sampleId = attr.value();
	}
}

bool Script::setHandedness(QByteArray & data)
{
	handedness = "Missing";
	if(!doc.setContent(data)){
		handedness = "Invalid data file";
	}else{
		QDomElement docElem      = doc.documentElement();
		QDomNodeList writerNodes = docElem.elementsByTagName("Writer");
		for(int i=0; i<writerNodes.length(); i++){
			QDomElement writerElem = writerNodes.item(i).toElement();
			if(writerElem.isNull())
				continue;

			int attrWriterID = writerElem.attributeNode("name").value().toInt();
			if (attrWriterID == writerId){
				QDomAttr attr = writerElem.attributeNode("WritingType");
				if(attr.isNull()){
					handedness = "Missing WritingType data";
				}else{
					handedness = attr.value().toLower();
					LOG("handedness: '%'",handedness);
					return true;
				}
			}
		}
	}
	return false;
}

void Script::calculateHandednessStat()
{
	leftToRightNum = 0;
	rightToLeftNum = 0;
	for(int i=0; i<size(); i++){
		Stroke s = at(i);
		if ( s.length() < limit )
			continue;
		switch(s.orientation){
			case Stroke::Orientation::LeftToRight :
				leftToRightNum++;
				break;
			case Stroke::Orientation::RightToLeft :
				rightToLeftNum++;
				break;
			default: break;
		}
	}
}

void Script::clearAllOrientation()
{
	for(int i=0; i<size(); i++){
		Stroke & s = (*this)[i];
		s.orientation = Stroke::Orientation::None;
	}
}

/* Looking for the distance of the third point and the line defined by first and last points. */
int Script::waveHeight(StrokePoint first, StrokePoint last, StrokePoint third)
{
	//https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
	return qAbs((last.y-first.y)*third.x-(last.x-first.x)*third.y+last.x*first.y-last.y*first.x) /
		DISTANCE(first.x, first.y, last.x, last.y);
}

void Script::calculateHandedness()
{
	for(int i=0; i<size(); i++){
		Stroke & s = (*this)[i];
		if(s.orientation != Stroke::Orientation::None)
			continue;

		double avgDiagonal = averageStrokeDiagonal();
		// filter too small strokes (probably meant to be points)
		if(s.diagonal() < 0.25*avgDiagonal)
			continue;

		// filter too big variance in slope
		double sdSlope = s.sdSlope();
		// 0.3 (radian) default uncertainty ~ 17.2 degree
		if(uncertainty < sdSlope)
			continue;
		// filter too much slant
		double avgSlope = s.avgSlope();
		if(1.5*uncertainty < avgSlope)
			continue;

		StrokePoint & first = s[0];
		StrokePoint & last = s[s.size()-1];

/*
		int maxDistance = 0;
		for(int j=1; j<s.size(); j++){
			StrokePoint & p = s[j];
			int d = waveHeight(first, last, p);
			if(maxDistance < d)
				maxDistance = d;
		}

		// filter big wave heights (strokes probably not meant to be linear)
		if(uncertainty < maxDistance)
			continue;

		// filter with too much slant
		double degree = 0;
		double rad = qAtan2(qAbs(last.y-first.y), qAbs(last.x-first.x));
		degree = rad / 3.14159265 * 180.0;
		if(40 < qAbs(degree))
			continue;
*/
		if(first.x < last.x)
			s.orientation = Stroke::Orientation::LeftToRight;
		else
			s.orientation = Stroke::Orientation::RightToLeft;
	}
	calculateHandednessStat();
}


double Stroke::minX()
{
	double ret = at(0).x;
	for(int i=0;i<size();i++){
		StrokePoint p = at(i);
		if ( ret > p.x )
			ret = p.x;
	}
	return ret;
}

double Stroke::minY()
{
	double ret = at(0).y;
	for(int i=0;i<size();i++){
		StrokePoint p = at(i);
		if ( ret > p.y )
			ret = p.y;
	}
	return ret;
}

double Stroke::maxX()
{
	double ret = 0;
	for(int i=0;i<size();i++){
		StrokePoint p = at(i);
		if ( ret < p.x )
			ret = p.x;
	}
	return ret;
}

double Stroke::maxY()
{
	double ret = 0;
	for(int i=0;i<size();i++){
		StrokePoint p = at(i);
		if ( ret < p.y )
			ret = p.y;
	}
	return ret;
}


double Stroke::height()
{
	return maxY()-minY();
}

double Stroke::width()
{
	return  maxX()-minX();
}



double Stroke::avgSlope()
{
	double ret = 0.0;
	int cnt = 0;
	for(int i=1;i<size();i++){
		if ( abs(at(i).x-at(i-1).x)>0.0001){
			ret += (at(i).y-at(i-1).y)/(at(i).x-at(i-1).x);
			cnt++;
		}
	}
	ret /= cnt;
	return  ret;
}

double Stroke::sdSlope()
{
	double avg = avgSlope();

	return  sqrt(avg2Slope()+pow(avg,2));
}

double Stroke::avg2Slope()
{
	double ret = 0.0;
	int cnt = 0;
	for(int i=1;i<size();i++){
		if ( abs(at(i).x-at(i-1).x)>0.0001){
			double slope = (at(i).y-at(i-1).y)/(at(i).x-at(i-1).x);
			ret += pow(slope,2);
			cnt++;
		}
	}
	ret /= cnt;
	return  ret;
}

double Stroke::diagonal()
{
	return DISTANCE(minX(),minY(),maxX(),maxY());
}

