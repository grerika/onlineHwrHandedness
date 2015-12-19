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

	calculateHandednessStat();
	return true;
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
	return doc.toByteArray(2);
}

void Script::calculateHandednessStat()
{
	leftToRightNum = 0;
	rightToLeftNum = 0;
	for(int i=0; i<size(); i++){
		Stroke s = at(i);
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

void Script::calculateHandedness()
{
	double limit = 40.0; // kézmozgás bizonytalanságának maximuma; pontoknál, hullámosságnál, stb
	for(int i=0; i<size(); i++){
		Stroke & s = (*this)[i];
		if(s.orientation != Stroke::Orientation::None)
			continue;
		if(s.size() < 2)
			continue;

		StrokePoint & first = s[0];
		StrokePoint & last = s[s.size()-1];


		int signX = 0;
		StrokePoint upperArcX = s[0];
		StrokePoint lowerArcX = s[0];
		int signY = 0;
		StrokePoint upperArcY = s[0];
		StrokePoint lowerArcY = s[0];
		for(int j=1; j<s.size(); j++){
			StrokePoint & p = s[j];

			int dX = p.x - s[j-1].x;
			if(dX){
				int newSign = dX/qAbs(dX);
				if(signX < 0 && 0 < newSign){
					if((qAbs(lowerArcX.x-upperArcX.x) < qAbs(p.x-upperArcX.x))
							  || lowerArcX == s[0])
						lowerArcX = s[j-1];
				}
				if(newSign < 0 && 0 < signX){
					if((qAbs(lowerArcX.x-upperArcX.x) < qAbs(p.x-lowerArcX.x))
							  || upperArcX == s[0])
						upperArcX = s[j-1];
				}
				signX = newSign;
			}
			int dY = p.y - s[j-1].y;
			if(dY){
				int newSign = dY/qAbs(dY);
				if(signY < 0 && 0 < newSign){
					if(qAbs(lowerArcY.y-upperArcY.y) < qAbs(p.y-upperArcY.y)
							|| lowerArcY == s[0])
						lowerArcY = s[j-1];
				}
				if(newSign < 0 && 0 < signY){
					if(qAbs(lowerArcY.y-upperArcY.y) < qAbs(p.y-lowerArcY.y)
							|| upperArcY == s[0])
						upperArcY = s[j-1];
				}
				signY = newSign;
			}
		}
		// filter wave form on Y
		if(lowerArcY != s[0] && upperArcY != s[0] &&
				limit < qAbs( qAbs(lowerArcY.y-upperArcY.y)/* -qAbs(first.y-last.y)*/))
			continue;
		// filter wave form on X
		if(lowerArcX != s[0] && upperArcX != s[0] &&
				limit < qAbs( qAbs(lowerArcX.x-upperArcX.x)/* -qAbs(first.x-last.x)*/))
			continue;
		// filter 'U' form on Y
		if(qAbs(upperArcY.y - last.y) < qAbs(upperArcY.y - first.y)){
			if(qAbs(first.y-last.y) < qAbs(upperArcY.y - last.y))
				continue;
		} else {
			if(qAbs(first.y-last.y) < qAbs(upperArcY.y - first.y))
				continue;
		}
		// filter 'U' form on X
		if(qAbs(upperArcX.x - last.x) < qAbs(upperArcX.x - first.x)){
			if(qAbs(first.x-last.x) < qAbs(upperArcX.x - last.x))
				continue;
		} else {
			if(qAbs(first.x-last.x) < qAbs(upperArcX.x - first.x))
				continue;
		}


		// filter with too much slant
		double degree = 0;
		double rad = qAtan2(qAbs(last.y-first.y), qAbs(last.x-first.x));
		degree = rad / 3.14159265 * 180.0;
		if(40 < qAbs(degree))
			continue;

		// filter too short strokes (probably meant to be points)
		double d = DISTANCE(first.x, first.y, last.x, last.y);
		if(d < limit)
			continue;


		if(first.x < last.x)
			s.orientation = Stroke::Orientation::LeftToRight;
		else
			s.orientation = Stroke::Orientation::RightToLeft;
	}
	calculateHandednessStat();
}

