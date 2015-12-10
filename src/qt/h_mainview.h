/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_MAINVIEW_H
#define H_MAINVIEW_H

#include <config.h>
#include <h_macros.h>
#include <h_config.h>
#include <QWidget>

class MainView : public QWidget
{
private:
	Q_OBJECT
	MY_Q_OBJECT

public:
	MainView(QWidget *parent = 0);
	virtual ~MainView();

private:
	virtual void closeEvent(QCloseEvent *event);

	void retranslate();
	void applyLayout();

	virtual void changeEvent(QEvent * event);

private slots:

};

#endif
