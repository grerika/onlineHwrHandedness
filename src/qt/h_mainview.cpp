/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <QtGui>

#include <config.h>
#include <h_macros.h>

#include "h_mainview.h"

//SCC TidContext = "MainView";

SCC TidMainWindowTitle = QT_TRANSLATE_NOOP("MainView", "%1 - main view");

MainView::MainView(QWidget *parent)
{
	(void)parent;
	setWindowIcon(QIcon(Path::icon("handedness.png")));
	setAttribute(Qt::WA_QuitOnClose, true);

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
}

MainView::~MainView()
{
}

void MainView::retranslate()
{
	setWindowTitle(tr(TidMainWindowTitle).arg(PKGNAME));
}

void MainView::changeEvent(QEvent * event)
{
	QWidget::changeEvent(event);
	if(event->type() == QEvent::LanguageChange)
		retranslate();
}

void MainView::closeEvent(QCloseEvent *event)
{
	(void)event;
	QTimer::singleShot(0, qApp, SLOT(quit()));
}

