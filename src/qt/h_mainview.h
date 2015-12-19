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
#include "h_data.h"
#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include <QShortcut>
#include <QLabel>

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
	void updateStat();
	void updateCursor();
	void leftToRightSlot();
	void rightToLeftSlot();
	void clearOrientationSlot();

	void calcHandednessSlot();

private:
	QString loadedFileName;
	QString saveFileName;
	Script script;

	Plot plot;
	QPushButton loadDataBtn;
	QLineEdit loadedFileNameEdit;
	QPushButton saveDataBtn;
	QLineEdit saveFileNameEdit;

	QToolButton leftToRightBtn;
	QToolButton rightToLeftBtn;
	QShortcut leftToRightSct;
	QShortcut rightToLeftSct;
	QToolButton clearOrientationBtn;
	QShortcut clearOrientationSct;

	QToolButton calcHandednessBtn;

	QLabel leftToRightSum;
	QLabel rightToLeftSum;
	QLabel objectIdLabel;
	QLabel orientationLabel;
};

#endif
