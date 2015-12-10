/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <QtGui>

#include "config.h"
#include "h_config.h"

#include <h_texts.h>

static QString rootDir;

namespace Path {

const QString translation(const char * lang)
{
	QString path;
#ifdef RELATIVE_PATH
	path += rootDir;
#endif
	path.append(TRANSLATIONS_PATH);
	LOG("Translation path: ", path);
	path += lang;
	path += ".qm";
	return path;
}

const QString icon(const char * fileName)
{
	QString iconPath;
#ifdef RELATIVE_PATH
	iconPath += rootDir;
#endif
	iconPath.append(ICONS_PATH);
	iconPath.append(fileName);
	LOG("Icon path: ", iconPath);
	return iconPath;
}

void initRootPath(const char * args0)
{
	LOG("args0: ", args0);
	QString cmd(QDir::fromNativeSeparators(args0));

	QString prefix(PREFIX);
	LOG("Compile time prefix: ", prefix);

	QString current(C_STR(QDir::currentPath()));
	LOG("Current path: %", current);

	QString path;
	if(cmd.startsWith("/"))
		path += cmd;
	else if(!cmd.count("/")){
		path += "/"; path += prefix; path += "bin/"; path += cmd;
	}else{
		path += current; path += "/"; path += cmd;
	}
	path = QDir::cleanPath(path);
	int pos;
	pos = path.lastIndexOf("/");
	path.chop(path.length() - pos);
	LOG("Computed binary path: %", path);

	QString root(path);
	pos = root.lastIndexOf("/");
	root.chop(root.length() - pos);
	pos = root.lastIndexOf("/");
	root.chop(root.length() - pos);
	root += "/";

	rootDir = root;
/*
	int rootDepth = path.count("/");

	for(int i = 0; i < rootDepth; i++)
		rootDir.append("../");
	rootDir.append(path.str);
	rootDir.append("/");
*/
	LOG("rootDir: %", rootDir);
	LOG("QDir::homePath(): %", QDir::homePath());
}

void initConfigFileName()
{
	QDir dir(QDir::homePath());

	if(!dir.exists(".handedness"))
		dir.mkdir(".handedneww");
}

}
