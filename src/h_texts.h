/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_TEXTS_H
#define H_TEXTS_H

#include <QString>
#include <QVariant>

#include <h_conversions.h>

class Text : public QString
{
public:
		Text() : QString() {}
		explicit Text(const QVariant & v) : QString(v.toString()) {}
		explicit Text(const QString & t) : QString(t) {}
		Text(QString && t) : QString(move_cast(t)) {}
		explicit Text(const Text & t) : QString(t) {}
		Text(Text && t) : QString(move_cast(t)) {}
		explicit Text(const char * s) : QString(s) {}
		virtual ~Text() {}

		Text & operator=(const char * s) { QString::operator=(s); return *this; }
		Text & operator=(const Text & t) { copy(t); return *this; }

		bool isEqual(const QString & t) const { return QString::localeAwareCompare(*this, t) == 0; }
		bool isLess(const QString & t) const { return QString::localeAwareCompare(*this, t) < 0; }
		bool isMore(const QString & t) const { return 0 < QString::localeAwareCompare(*this, t); }

		void catf(const char * fmt) { append(fmt); }
		template<typename Arg> void catf(const char * fmt, const Arg & arg)
		{
			while(*fmt != 0){
				if(*fmt == '%'){
					fmt++; 
					if(*fmt != '%'){
						*this << arg;
						append(fmt);
						return;
					}
				}
				append(*fmt);
				fmt++;
			} 
			*this << arg;
		}
		template<typename Arg, typename... Args>
		void catf(const char * fmt, const Arg & arg, const Args & ... args)
		{
			while(*fmt != 0){
				if(*fmt == '%'){
					fmt++;
					if(*fmt != '%'){
						*this << arg;
						catf(fmt, args...);
						return;
					}
				}
				append(*fmt);
				fmt++;
			}
			//cat(args...);
		}

private:
		void copy(const QString & t) { QString::operator=(t); }
};

inline bool operator==(const Text & a, const QString & b) { return a.isEqual(b); }
inline bool operator==(const Text & a, const Text & b) { return a.isEqual(b); }
inline bool operator!=(const Text & a, const QString & b) { return !a.isEqual(b); }
inline bool operator!=(const QString & a, const Text & b) { return !b.isEqual(a); }
inline bool operator!=(const Text & a, const Text & b) { return !a.isEqual(b); }
inline bool operator<(const Text & a, const QString & b) { return a.isLess(b); }
inline bool operator<(const QString & a, const Text & b) { return b.isMore(a); }
inline bool operator<(const Text & a, const Text & b) { return a.isLess(b); }

inline Text & operator<<(Text & lhs, const bool & rhs) { lhs.append(rhs); return lhs; }
inline Text & operator<<(Text & lhs, const int & rhs) { lhs+=QString("%1").arg(rhs); return lhs; }
inline Text & operator<<(Text & lhs, const unsigned & rhs) {lhs.append(rhs); return lhs;}
inline Text & operator<<(Text & lhs, const double & rhs) { lhs+=QString("%1").arg(rhs); return lhs; }
inline Text & operator<<(Text & lhs, const QDateTime & rhs){lhs+=rhs.toString(); return lhs;}
inline Text & operator<<(Text & lhs, const QString & rhs) {lhs.append(rhs); return lhs;}

inline Text & operator<<=(Text & str, const char * rhs) { str = rhs; return str; }

enum class TextVariant {
	Long = 0,
	Medium,
	Short
};

QString translationVariant(const QString & text, TextVariant variant);

#endif
