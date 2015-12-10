/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_CONFIG_H
#define H_CONFIG_H

#include <QLocale>
#include <QString>

#include <h_macros.h>

namespace Path {
	/* Absolute path and name of the binary expected,
	 * ending as prefix/bin/binary_name. */
	void initRootPath(const char * args0);

	void initConfigFileName();

	const QString translation(const char * lang);
	const QString icon(const char * fileName);
}

#endif
