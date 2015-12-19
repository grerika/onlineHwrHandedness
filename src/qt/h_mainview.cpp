/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <QtGui>

#include <config.h>
#include <h_macros.h>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>
#include <QDesktopWidget>

#include "h_mainview.h"

//SCC TidContext = "MainView";

SCC TidMainWindowTitle = QT_TRANSLATE_NOOP("MainView", "%1 - main view");
SCC TidMainWindowLoadButton = QT_TRANSLATE_NOOP("MainView", "Load file");
SCC TidMainWindowOpenDialog = QT_TRANSLATE_NOOP("MainView", "Open xml file");
SCC TidMainWindowSaveDialog = QT_TRANSLATE_NOOP("MainView", "Save xml file");
SCC TidMainWindowFileDialogFilter = QT_TRANSLATE_NOOP("MainView", "XML Files (*.xml)");
SCC TidMainWindowSaveButton = QT_TRANSLATE_NOOP("MainView", "Save file");
SCC TidMainWindowLeftToRight = QT_TRANSLATE_NOOP("MainView", "Left to right");
SCC TidMainWindowRightToLeft = QT_TRANSLATE_NOOP("MainView", "Right to left");
SCC TidMainWindowClearOrientation = QT_TRANSLATE_NOOP("MainView", "Clear orientation");
SCC TidMainWindowCalcHandedness = QT_TRANSLATE_NOOP("MainView", "Calculate handedness");
SCC TidMainWindowObjectId = QT_TRANSLATE_NOOP("MainView", "Stroke id: ");
SCC TidMainWindowOrientation = QT_TRANSLATE_NOOP("MainView", "Orientation: ");

MainView::MainView(QWidget *parent) :
	loadDataBtn(TidMainWindowLoadButton, this),
	saveDataBtn(TidMainWindowSaveButton, this),
	leftToRightBtn(this),
	rightToLeftBtn(this),
	leftToRightSct(QKeySequence(Qt::Key_J), this),
	rightToLeftSct(QKeySequence(Qt::Key_B), this),
	clearOrientationBtn(this),
	clearOrientationSct(QKeySequence(Qt::Key_C), this),
	calcHandednessBtn(this)
{
	(void)parent;
	setWindowIcon(QIcon(Path::icon("handedness.png")));
	setAttribute(Qt::WA_QuitOnClose, true);

	loadDataBtn.setAutoDefault(false);
	saveDataBtn.setAutoDefault(false);
	leftToRightBtn.setText(TidMainWindowLeftToRight);
	rightToLeftBtn.setText(TidMainWindowRightToLeft);
	clearOrientationBtn.setText(TidMainWindowClearOrientation);
	objectIdLabel.setText(TidMainWindowObjectId);
	orientationLabel.setText(TidMainWindowOrientation);

	calcHandednessBtn.setText(TidMainWindowCalcHandedness);

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
	connect(&loadDataBtn, SIGNAL(clicked()), this, SLOT(loadDataSlot()));
	connect(&saveDataBtn, SIGNAL(clicked()), this, SLOT(saveDataSlot()));
	connect(&plot, SIGNAL(surfaceCreated()), this, SLOT(redraw()));
	connect(&plot, SIGNAL(objectSelected()), this, SLOT(updateCursor()));

	connect(&leftToRightBtn, SIGNAL(clicked()), this, SLOT(leftToRightSlot()));
	connect(&leftToRightSct, SIGNAL(activated()), this, SLOT(leftToRightSlot()));
	connect(&rightToLeftBtn, SIGNAL(clicked()), this, SLOT(rightToLeftSlot()));
	connect(&rightToLeftSct, SIGNAL(activated()), this, SLOT(rightToLeftSlot()));
	connect(&clearOrientationBtn, SIGNAL(clicked()), this, SLOT(clearOrientationSlot()));
	connect(&clearOrientationSct, SIGNAL(activated()), this, SLOT(clearOrientationSlot()));

	connect(&calcHandednessBtn, SIGNAL(clicked()), this, SLOT(calcHandednessSlot()));

	QHBoxLayout * openLayout = new QHBoxLayout;
	openLayout->addWidget(&loadDataBtn);
	openLayout->addWidget(&loadedFileNameEdit);

	QHBoxLayout * saveLayout = new QHBoxLayout;
	saveLayout->addWidget(&saveDataBtn);
	saveLayout->addWidget(&saveFileNameEdit);

	QHBoxLayout * toolLayout = new QHBoxLayout;
	toolLayout->addWidget(&leftToRightBtn);
	toolLayout->addWidget(&clearOrientationBtn);
	toolLayout->addWidget(&rightToLeftBtn);

	QVBoxLayout * statLayout = new QVBoxLayout;
	statLayout->addWidget(&calcHandednessBtn);
	statLayout->addStretch(1);
	statLayout->addWidget(&leftToRightSum);
	statLayout->addWidget(&rightToLeftSum);
	statLayout->addStretch(1);
	statLayout->addWidget(&objectIdLabel);
	statLayout->addWidget(&orientationLabel);
	statLayout->addStretch(1);

	QHBoxLayout * plotLayout = new QHBoxLayout;
	plotLayout->addWidget(&plot);
	plotLayout->addLayout(statLayout);

	QVBoxLayout * mainLayout = new QVBoxLayout;
	mainLayout->addLayout(openLayout);
	mainLayout->addLayout(saveLayout);
	mainLayout->addLayout(toolLayout);
	mainLayout->addLayout(plotLayout);

	setLayout(mainLayout);
	//QApplication::aboutQt();

	// align window to the center of the desktop
	this->setGeometry(
			QStyle::alignedRect(
			Qt::LeftToRight,
			Qt::AlignCenter,
			this->size(),
			qApp->desktop()->availableGeometry()
			)
		);

	updateStat();
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
	if(!script.loadXmlData(data)){
		file.close();
		QMessageBox::information(this, tr("Information"), tr("Failed to parse xml file."));
		return;
	}
	file.close();

	loadedFileName = fileName;
	loadedFileNameEdit.setText(loadedFileName);

	plot.selectedObject = 0;
	updateStat();

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
	auto data = script.getXmlData();
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
	QColor brown(135, 90, 45);
	QColor yellow(255, 255, 85);
	QColor purple(200, 0, 200);

	double maxX = 0;
	double maxY = 0;
	double minX = INT_MAX;
	double minY = INT_MAX;

	plot.clear();
	bool started;

	int objectId = 0;
	// find width and height of the area to be plotted
	for(int i=0; i<script.size(); i++){
		Stroke & s = script[i];
		for(int j=0; j<s.size(); j++){
			StrokePoint & p = s[j];

			if(p.x < minX) minX = p.x;
			if(p.y < minY) minY = p.y;
			if(maxX < p.x) maxX = p.x;
			if(maxY < p.y) maxY = p.y;
		}
	}
	double width = maxX - minX;
	double height = maxY - minY;
	double scaleWidth = plot.width() / width;
	double scaleHeight = plot.height() / height;
	double scale = scaleWidth < scaleHeight ? scaleWidth : scaleHeight;

	for(int i=0; i<script.size(); i++){
		Stroke & s = script[i];
		objectId = i+1;
		QColor color;
		int lineWidth = 1;

		color = blue;
		if(s.orientation == Stroke::Orientation::LeftToRight){
			color = brown;
			lineWidth = 2;
		}
		if(s.orientation == Stroke::Orientation::RightToLeft){
			color = yellow;
			lineWidth = 2;
		}

		if(plot.selectedObject == objectId){
			started = false;
			for(int j=0; j<s.size(); j++){
				StrokePoint & p = s[j];

				int x = (p.x - minX) * scale;
				int y = (p.y - minY) * scale;
				if(!started){
					plot.setCursor(x, y);
					started = true;
					continue;
				}
				plot.lineTo(objectId, red, x, y, lineWidth+2);
			}
		}

		started = false;
		for(int j=0; j<s.size(); j++){
			StrokePoint & p = s[j];

			int x = (p.x - minX) * scale;
			int y = (p.y - minY) * scale;
			if(!started){
				plot.setCursor(x, y);
				started = true;
				continue;
			}
			plot.lineTo(objectId, color, x, y, lineWidth);
		}
	}

	plot.update();
}

void MainView::updateStat()
{
	leftToRightSum.setText(QString("Left to right strokes: ")+
				QString::number(script.leftToRightNum));
	rightToLeftSum.setText(QString("Right to left strokes: ")+
				QString::number(script.rightToLeftNum));
}

void MainView::updateCursor()
{
	objectIdLabel.setText(QString("Stroke id: ")+QString::number(plot.selectedObject));
	if(plot.selectedObject && plot.selectedObject-1 < script.size())
		orientationLabel.setText(QString("Orientation: ")+
				QString::number(script[plot.selectedObject-1].orientation));
	else
		orientationLabel.setText(QString("Orientation: 0"));
	redraw();
}

void MainView::leftToRightSlot()
{
	if(!plot.selectedObject)
		return;
	script[plot.selectedObject-1].orientation = Stroke::Orientation::LeftToRight;
	script.calculateHandednessStat();
	updateStat();
	redraw();
}

void MainView::rightToLeftSlot()
{
	if(!plot.selectedObject)
		return;
	script[plot.selectedObject-1].orientation = Stroke::Orientation::RightToLeft;
	script.calculateHandednessStat();
	updateStat();
	redraw();
}

void MainView::clearOrientationSlot()
{
	if(!plot.selectedObject)
		return;
	script[plot.selectedObject-1].orientation = Stroke::Orientation::None;
	script.calculateHandednessStat();
	updateStat();
	redraw();
}

void MainView::calcHandednessSlot()
{
	script.calculateHandedness();
	updateStat();
	redraw();
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

