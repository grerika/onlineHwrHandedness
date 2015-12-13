/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <QtGui>

#include <config.h>
#include <h_macros.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include "h_mainview.h"

//SCC TidContext = "MainView";

SCC TidMainWindowTitle = QT_TRANSLATE_NOOP("MainView", "%1 - main view");
SCC TidMainWindowLoadButton = QT_TRANSLATE_NOOP("MainView", "Load file");
SCC TidMainWindowOpenDialog = QT_TRANSLATE_NOOP("MainView", "Open xml file");
SCC TidMainWindowOpenDialogPattarn = QT_TRANSLATE_NOOP("MainView", "XML Files (*.xml)");
SCC TidMainWindowSaveButton = QT_TRANSLATE_NOOP("MainView", "Save file");

MainView::MainView(QWidget *parent) :
	plot(doc),
	loadDataBtn(TidMainWindowLoadButton, this),
	saveDataBtn(TidMainWindowSaveButton, this)
{
	(void)parent;
	setWindowIcon(QIcon(Path::icon("handedness.png")));
	setAttribute(Qt::WA_QuitOnClose, true);

	loadDataBtn.setAutoDefault(false);
	saveDataBtn.setAutoDefault(false);


	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
	connect(&loadDataBtn, SIGNAL(clicked()), this, SLOT(loadDataSlot()));
	connect(&saveDataBtn, SIGNAL(clicked()), this, SLOT(saveDataSlot()));


	QHBoxLayout * openLayout = new QHBoxLayout;
	openLayout->addWidget(&loadDataBtn);
	openLayout->addWidget(&loadedFileNameEdit);

	QHBoxLayout * saveLayout = new QHBoxLayout;
	saveLayout->addWidget(&saveDataBtn);
	saveLayout->addWidget(&savedFileNameEdit);

	QHBoxLayout * plotLayout = new QHBoxLayout;
	plotLayout->addWidget(&plot);
	//layout->addStretch(0);

	QVBoxLayout * mainLayout = new QVBoxLayout;
	mainLayout->addLayout(openLayout);
	mainLayout->addLayout(saveLayout);
	mainLayout->addLayout(plotLayout);

	setLayout(mainLayout);
}

MainView::~MainView()
{
}

void MainView::loadDataSlot()
{
	loadedFileName = "";
	loadedFileNameEdit.setText(loadedFileName);

	QString fileName = QFileDialog::getOpenFileName(this,
			tr(TidMainWindowOpenDialog),
			QDir::currentPath(),
			tr(TidMainWindowOpenDialogPattarn)
			);

	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)){
		QMessageBox::information(this, tr("Information"), tr("Failed to load file."));
		return;
	}
	if(!doc.setContent(&file)){
		file.close();
		QMessageBox::information(this, tr("Information"), tr("Failed to parse xml file."));
		return;
	}
	file.close();

	loadedFileName = fileName;
	loadedFileNameEdit.setText(loadedFileName);

	plot.update();
}

void MainView::saveDataSlot()
{
}

void MainView::showEvent(QShowEvent *event)
{
	(void)event;
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

