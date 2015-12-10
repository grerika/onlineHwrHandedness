/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_CONVERSIONS_H
#define H_CONVERSIONS_H

#include <QString>
#include <QDateTime>
#include <QVariant>

template<typename Type>
Type && move_cast(Type & t)
{
	return (Type &&)t;
}

inline QVariant & operator<<=(QVariant & lhs, const bool & rhs) { lhs.setValue(rhs); return lhs; }
inline QVariant & operator<<=(QVariant & lhs, const int & rhs) { lhs.setValue(rhs); return lhs; }
inline QVariant & operator<<=(QVariant & lhs, const unsigned & rhs) {lhs.setValue(rhs); return lhs;}
inline QVariant & operator<<=(QVariant & lhs, const double & rhs) { lhs.setValue(rhs); return lhs; }
inline QVariant & operator<<=(QVariant & lhs, const QDateTime & rhs){lhs.setValue(rhs); return lhs;}
inline QVariant & operator<<=(QVariant & lhs, const QString & rhs) {lhs.setValue(rhs); return lhs;}


inline bool & operator<<=(bool & lhs, const QVariant & rhs) { lhs = rhs.toBool(); return lhs; }
inline int & operator<<=(int & lhs, const QVariant & rhs) { lhs = rhs.toInt(); return lhs; }
inline unsigned & operator<<=(unsigned & lhs, const QVariant & rhs){lhs = rhs.toUInt(); return lhs;}
inline double & operator<<=(double & lhs, const QVariant & rhs){ lhs = rhs.toDouble(); return lhs; }
inline QDateTime & operator<<=(QDateTime & lhs, const QVariant & v){lhs=v.toDateTime(); return lhs;}
inline QString & operator<<=(QString & lhs, const QVariant & rhs){lhs = rhs.toString(); return lhs;}

#endif
