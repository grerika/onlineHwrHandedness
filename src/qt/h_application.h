/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_APPLICATION_H
#define H_APPLICATION_H

#include <QApplication>

#include "h_mainview.h"

/* Forwards: */
class QSessionManager;

class Application : public QApplication
{
private:
	Q_OBJECT
	MY_Q_OBJECT
public:
	Application(int & argc, char *argv[]);
	virtual ~Application();

	void loadTranslation(QString lang = QString());

	MainView & mainView();

	virtual bool notify(QObject * receiver, QEvent * event);

private:
	MainView * mainViewPtr;
};

#endif
