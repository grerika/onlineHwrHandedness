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
SCC TidMainWindowSaveDialog = QT_TRANSLATE_NOOP("MainView", "Save xml file");
SCC TidMainWindowFileDialogFilter = QT_TRANSLATE_NOOP("MainView", "XML Files (*.xml)");
SCC TidMainWindowSaveButton = QT_TRANSLATE_NOOP("MainView", "Save file");

MainView::MainView(QWidget *parent) :
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
	connect(&plot, SIGNAL(surfaceCreated()), this, SLOT(redraw()));


	QHBoxLayout * openLayout = new QHBoxLayout;
	openLayout->addWidget(&loadDataBtn);
	openLayout->addWidget(&loadedFileNameEdit);

	QHBoxLayout * saveLayout = new QHBoxLayout;
	saveLayout->addWidget(&saveDataBtn);
	saveLayout->addWidget(&saveFileNameEdit);

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
	if(loadedFileName == ""){
		loadedFileName = QDir::currentPath();
		loadedFileNameEdit.setText(loadedFileName);
	}

	QString fileName = QFileDialog::getOpenFileName(this,
			tr(TidMainWindowOpenDialog),
			loadedFileName,//QDir::currentPath(),
			tr(TidMainWindowFileDialogFilter)
			);
	if(fileName == "")
		return;

	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)){
		QMessageBox::information(this, tr("Information"), tr("Failed to load file."));
		return;
	}
	auto data = file.readAll();
	data.replace("\r", "");
	if(!doc.setContent(data)){
		file.close();
		QMessageBox::information(this, tr("Information"), tr("Failed to parse xml file."));
		return;
	}
	file.close();

	loadedFileName = fileName;
	loadedFileNameEdit.setText(loadedFileName);

	redraw();
}

void MainView::saveDataSlot()
{
	if(saveFileName == "") {
		saveFileName = loadedFileName;
		saveFileName.replace("/Data/", "/ChangedData/");
		saveFileName.replace("\\Data\\", "\\ChangedData\\");
	}

	QString fileName = QFileDialog::getSaveFileName(this,
			tr(TidMainWindowSaveDialog),
			saveFileName,//QDir::currentPath(),
			tr(TidMainWindowFileDialogFilter)
			);
	if(fileName == "")
		return;
	LOG("filename: %", fileName);
	saveFileName = fileName;
	saveFileNameEdit.setText(fileName);

	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::information(this, tr("Information"), tr("Failed to save file."));
		return;
	}
	auto data = doc.toByteArray(2);
	if(file.write(data) == -1){
		file.close();
		QMessageBox::information(this, tr("Information"), tr("Failed to write xml file."));
		return;
	}
	file.close();
}

void MainView::redraw()
{
	QColor black(0, 0, 0);
	QColor white(255, 255, 255);
	QColor red(255, 0, 0);
	QColor green(0, 255, 0);
	QColor blue(0, 0, 255);

	double scale = 1.0;
	double maxX = 0;
	double maxY = 0;
	double minX = INT_MAX;
	double minY = INT_MAX;

	plot.clear();

	int objectId = 0;
	// now lets read strokes from dom
	QDomElement docElem = doc.documentElement();
	QDomNodeList strokeNodes = docElem.elementsByTagName("Stroke");

	// find width and height of the area to be plotted
	for(int i=0; i<strokeNodes.length(); i++){
		QDomNode strokeNode = strokeNodes.item(i);
		QDomElement strokeElem = strokeNode.toElement();
		if(strokeElem.isNull())
			continue;

		QDomNodeList pointNodes = strokeElem.elementsByTagName("Point");
		for(int j=0; j<pointNodes.length(); j++){
			QDomNode pointNode = pointNodes.item(j);
			QDomElement pointElem = pointNode.toElement();
			if(pointElem.isNull())
				continue;

			QDomAttr attrX = pointElem.attributeNode("x");
			if(attrX.isNull())
				continue;
			QDomAttr attrY = pointElem.attributeNode("y");
			if(attrY.isNull())
				continue;

			int x = attrX.value().toInt() * scale;
			int y = attrY.value().toInt() * scale;

			if(x < minX) minX = x;
			if(y < minY) minY = y;
			if(maxX < x) maxX = x;
			if(maxY < y) maxY = y;
		}
	}

	double width = maxX - minX;
	double height = maxY - minY;
	double scaleWidth = plot.width() / width;
	double scaleHeight = plot.height() / height;
	scale = scaleWidth < scaleHeight ? scaleWidth : scaleHeight;

	for(int i=0; i<strokeNodes.length(); i++){
		QDomNode strokeNode = strokeNodes.item(i);
		QDomElement strokeElem = strokeNode.toElement();
		if(strokeElem.isNull())
			continue;

		objectId = i+1;
		QColor color = black;
		if(plot.selectedObject == objectId)
			color = red;

		bool started = false;
		QDomNodeList pointNodes = strokeElem.elementsByTagName("Point");
		for(int j=0; j<pointNodes.length(); j++){
			QDomNode pointNode = pointNodes.item(j);
			QDomElement pointElem = pointNode.toElement();
			if(pointElem.isNull())
				continue;

			QDomAttr attrX = pointElem.attributeNode("x");
			if(attrX.isNull())
				continue;
			QDomAttr attrY = pointElem.attributeNode("y");
			if(attrY.isNull())
				continue;

			int x = (attrX.value().toInt() - minX) * scale;
			int y = (attrY.value().toInt() - minY) * scale;
			//LOG("id: %, x: %, y: %", objectId, x, y);
			//if(x == 0 && y == 0)
			//	continue;
			if(!started){
				plot.setCursor(x, y);
				started = true;
				continue;
			}
			plot.lineTo(objectId, color, x, y);
		}
	}

	plot.update();
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

