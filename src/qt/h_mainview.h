/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_MAINVIEW_H
#define H_MAINVIEW_H

#include <config.h>
#include <h_macros.h>
#include <h_config.h>
#include "h_plot.h"
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDomDocument>

class MainView : public QWidget
{
private:
	Q_OBJECT
	MY_Q_OBJECT

public:
	MainView(QWidget *parent = 0);
	virtual ~MainView();

private:
	virtual void showEvent(QShowEvent *event);
	virtual void closeEvent(QCloseEvent *event);
	virtual void changeEvent(QEvent * event);

	void retranslate();

private slots:
	void loadDataSlot();
	void saveDataSlot();
	void redraw();

private:
	QString loadedFileName;
	QString saveFileName;
	QDomDocument doc;
	//Script script;

	Plot plot;
	QPushButton loadDataBtn;
	QLineEdit loadedFileNameEdit;
	QPushButton saveDataBtn;
	QLineEdit saveFileNameEdit;
};

#endif
