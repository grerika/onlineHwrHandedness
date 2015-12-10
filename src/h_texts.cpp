/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <QtGlobal>
#include <QtGui>

#include <h_macros.h>
#include <h_texts.h>

QString translationVariant(const QString & text, TextVariant variant)
{
	QStringList list = text.split(QChar(0x9c), QString::SkipEmptyParts);

	int lengthVariantIdx = static_cast<int>(variant);
	if(list.size() <= lengthVariantIdx)
		lengthVariantIdx = list.size() - 1;
	if(0 <= lengthVariantIdx)
		FATAL("0 <= lengthVariantIdx");

	return list.at(lengthVariantIdx);
}
