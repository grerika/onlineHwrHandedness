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
		if(s.size() < 2)
			continue;

		StrokePoint & first = s[0];
		StrokePoint & last = s[s.size()-1];

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

		// filter too short strokes (probably meant to be points)
		double d = DISTANCE(first.x, first.y, last.x, last.y);
		if(d < uncertainty)
			continue;


		if(first.x < last.x)
			s.orientation = Stroke::Orientation::LeftToRight;
		else
			s.orientation = Stroke::Orientation::RightToLeft;
	}
	calculateHandednessStat();
}

