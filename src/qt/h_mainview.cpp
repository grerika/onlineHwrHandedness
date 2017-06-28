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
SCC TidMainWindowClearAllOrientation = QT_TRANSLATE_NOOP("MainView", "Clear all orientation");
SCC TidMainWindowCalcHandedness = QT_TRANSLATE_NOOP("MainView", "Calculate handedness");
SCC TidMainWindowStatLeftToRight = QT_TRANSLATE_NOOP("MainView", "Left to right: %1");
SCC TidMainWindowStatRightToLeft = QT_TRANSLATE_NOOP("MainView", "Right to left: %1");
SCC TidMainWindowStatObjectId = QT_TRANSLATE_NOOP("MainView", "Stroke id: %1");
SCC TidMainWindowStatOrientation = QT_TRANSLATE_NOOP("MainView", "Orientation: %1");
SCC TidMainWindowUncertainty = QT_TRANSLATE_NOOP("MainView", "Max SD of slope:");

SCC TidMainWindowWriter      = QT_TRANSLATE_NOOP("MainView", "WriterID: %1");
SCC TidMainWindowSample      = QT_TRANSLATE_NOOP("MainView", "SampleID: %1");
SCC TidMainWindowHandedness  = QT_TRANSLATE_NOOP("MainView", "Handedness: %1");
SCC TidMainWindowDecision    = QT_TRANSLATE_NOOP("MainView", "Decision: %1");
SCC TidMainWindowUndecided   = QT_TRANSLATE_NOOP("MainView", "undecided");
SCC TidMainWindowLeftHanded  = QT_TRANSLATE_NOOP("MainView", "left-handed");
SCC TidMainWindowRightHanded = QT_TRANSLATE_NOOP("MainView", "right-handed");
SCC TidMainWindowWriterLoadButton = QT_TRANSLATE_NOOP("MainView", "Load writer file");
SCC TidMainWindowWriterOpenDialog = QT_TRANSLATE_NOOP("MainView", "Open writer file");

MainView::MainView(QWidget *parent) :
	loadWriterDataBtn(tr(TidMainWindowWriterLoadButton), this),
	loadDataBtn(tr(TidMainWindowLoadButton), this),
	saveDataBtn(tr(TidMainWindowSaveButton), this),
	leftToRightSct(QKeySequence(Qt::Key_J), this),
	rightToLeftSct(QKeySequence(Qt::Key_B), this),
	clearOrientationSct(QKeySequence(Qt::Key_C), this),
	calcHandednessBtn(this)
{
	(void)parent;
	setWindowIcon(QIcon(Path::icon("handedness.png")));
	setAttribute(Qt::WA_QuitOnClose, true);

	loadWriterDataBtn.setAutoDefault(false);
	loadDataBtn.setAutoDefault(false);
	saveDataBtn.setAutoDefault(false);
	leftToRightBtn.setText(tr(TidMainWindowLeftToRight));
	rightToLeftBtn.setText(tr(TidMainWindowRightToLeft));
	clearOrientationBtn.setText(tr(TidMainWindowClearOrientation));
	clearAllOrientationBtn.setText(tr(TidMainWindowClearAllOrientation));

	leftToRightSum.setText(tr(TidMainWindowStatLeftToRight).arg(0));
	rightToLeftSum.setText(tr(TidMainWindowStatRightToLeft).arg(0));
	objectIdLabel.setText(tr(TidMainWindowStatObjectId).arg(0));
	orientationLabel.setText(tr(TidMainWindowStatOrientation).arg(0));

	writerLabel.setText(tr(TidMainWindowWriter).arg(0));
	sampleLabel.setText(tr(TidMainWindowSample).arg(0));	
	handednessLabel.setText(tr(TidMainWindowHandedness).arg(0));
	decisionLabel.setText(tr(TidMainWindowDecision).arg(0));

	calcHandednessBtn.setText(tr(TidMainWindowCalcHandedness));
	uncertaintyLabel.setText(tr(TidMainWindowUncertainty).arg(0));
	// FIXME who deletes the validator?
	uncertaintyEdit.setValidator(new QDoubleValidator(0, 100, 2, this));
	uncertaintyEdit.setText(QString::number(script.uncertainty));

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
	connect(&loadWriterDataBtn, SIGNAL(clicked()), this, SLOT(loadWriterDataSlot()));
	connect(&loadDataBtn, SIGNAL(clicked()), this, SLOT(loadDataSlot()));
	connect(&saveDataBtn, SIGNAL(clicked()), this, SLOT(saveDataSlot()));
	connect(&plot, SIGNAL(surfaceCreated()), this, SLOT(redraw()));
	connect(&plot, SIGNAL(objectSelected()), this, SLOT(updateCursor()));

	connect(&leftToRightBtn, SIGNAL(clicked()), this, SLOT(leftToRightSlot()));
	connect(&leftToRightSct, SIGNAL(activated()), this, SLOT(leftToRightSlot()));
	connect(&rightToLeftBtn, SIGNAL(clicked()), this, SLOT(rightToLeftSlot()));
	connect(&rightToLeftSct, SIGNAL(activated()), this, SLOT(rightToLeftSlot()));
	connect(&clearOrientationBtn, SIGNAL(clicked()), this, SLOT(clearOrientationSlot()));
	connect(&clearAllOrientationBtn, SIGNAL(clicked()), this, SLOT(clearAllOrientationSlot()));
	connect(&clearOrientationSct, SIGNAL(activated()), this, SLOT(clearOrientationSlot()));

	connect(&calcHandednessBtn, SIGNAL(clicked()), this, SLOT(calcHandednessSlot()));

	//leftToRightSum.setStyleSheet("QLabel { color : rgb(135, 90, 45); }"); // brown
	leftToRightSum.setStyleSheet("QLabel { color : rgb(120, 80, 40); }"); // brown
	//rightToLeftSum.setStyleSheet("QLabel { color : rgb(255, 255, 85); }"); // yellow
	rightToLeftSum.setStyleSheet("QLabel { color : rgb(0, 0, 255); }");

	QHBoxLayout * openWriterLayout = new QHBoxLayout;
	openWriterLayout->addWidget(&loadWriterDataBtn);
	openWriterLayout->addWidget(&loadedWriterFileNameEdit);
	
	QHBoxLayout * openLayout = new QHBoxLayout;
	openLayout->addWidget(&loadDataBtn);
	openLayout->addWidget(&loadedFileNameEdit);

	QHBoxLayout * saveLayout = new QHBoxLayout;
	saveLayout->addWidget(&saveDataBtn);
	saveLayout->addWidget(&saveFileNameEdit);

	QHBoxLayout * toolLayout = new QHBoxLayout;
	toolLayout->addStretch(1);
	toolLayout->addWidget(&leftToRightBtn);
	toolLayout->addWidget(&clearOrientationBtn);
	toolLayout->addWidget(&rightToLeftBtn);
	toolLayout->addStretch(1);

	QVBoxLayout * statLayout = new QVBoxLayout;

	statLayout->addWidget(&writerLabel);
	statLayout->addWidget(&sampleLabel);
	statLayout->addWidget(&handednessLabel);
	statLayout->addStretch(1);
	statLayout->addWidget(&leftToRightSum);
	statLayout->addWidget(&rightToLeftSum);
	statLayout->addWidget(&decisionLabel);
	statLayout->addStretch(1);	
	
	statLayout->addWidget(&calcHandednessBtn);
	statLayout->addWidget(&uncertaintyLabel);
	statLayout->addWidget(&uncertaintyEdit);
	statLayout->addWidget(&clearAllOrientationBtn);
	statLayout->addStretch(1);
	statLayout->addWidget(&objectIdLabel);
	statLayout->addWidget(&orientationLabel);
	statLayout->addStretch(1);	

	QHBoxLayout * plotLayout = new QHBoxLayout;
	plotLayout->addWidget(&plot);
	plotLayout->addLayout(statLayout);

	QVBoxLayout * mainLayout = new QVBoxLayout;
	mainLayout->addLayout(openWriterLayout);	
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



void MainView::loadWriterDataSlot()
{
	if(loadedWriterFileName == ""){
		loadedWriterFileName = QDir::currentPath();
		loadedWriterFileNameEdit.setText(loadedWriterFileName);
	}

	QString fileName = QFileDialog::getOpenFileName(this,
			tr(TidMainWindowWriterOpenDialog),
			loadedWriterFileName,//QDir::currentPath(),
			tr(TidMainWindowFileDialogFilter)
			);
	if(fileName == "")
		return;

	if ( loadedFileName != "" ){
		QFile file(fileName);
		if(!file.open(QIODevice::ReadOnly)){
			QMessageBox::information(this, tr("Information"), tr("Failed to load writer file."));
			return;
		}	
		auto data = file.readAll();
		data.replace("\r", "");

		if(!script.setHandedness(data)){
			file.close();
			QMessageBox::information(this, tr("Information"), tr("Failed to parse xml writer file."));
			return;
		}
		file.close();
		
		handednessLabel.setText(tr(TidMainWindowHandedness).arg(script.handedness));
		if (script.handedness == "left-handed")
			handednessLabel.setStyleSheet("QLabel { color : rgb(0, 0, 255); }"); // LEFT
		else
			handednessLabel.setStyleSheet("QLabel { color : rgb(120, 80, 40); }"); // RIGHT
	}
	loadedWriterFileName = fileName;
	loadedWriterFileNameEdit.setText(loadedWriterFileName);
	
	redraw();
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
	
	writerLabel.setText(tr(TidMainWindowWriter).arg(script.writerId));
	sampleLabel.setText(tr(TidMainWindowSample).arg(script.sampleId));	
	
	if ( loadedWriterFileName != "" ){
		file.setFileName(loadedWriterFileName);
		if(!file.open(QIODevice::ReadOnly)){
			QMessageBox::information(this, tr("Information"), tr("Failed to load writer file."));
			return;
		}	
		auto data2 = file.readAll();
		data2.replace("\r", "");

		if(!script.setHandedness(data2)){
			file.close();
			QMessageBox::information(this, tr("Information"), tr("Failed to parse xml writer file."));
			return;
		}
		file.close();
		
		handednessLabel.setText(tr(TidMainWindowHandedness).arg(script.handedness));
		if (script.handedness == "left-handed")
			handednessLabel.setStyleSheet("QLabel { color : rgb(0, 0, 255); }"); // LEFT
		else
			handednessLabel.setStyleSheet("QLabel { color : rgb(120, 80, 40); }"); // RIGHT		
	}

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
	writerLabel.setText(tr(TidMainWindowWriter).arg(script.writerId));	
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
	QColor gray(127, 127, 127);
	QColor red(255, 0, 0);
	QColor green(0, 255, 0);
	QColor blue(0, 0, 255);
	QColor brown(210, 140, 70);
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

		color = black;
		if(s.orientation == Stroke::Orientation::LeftToRight){
			color = brown;
			lineWidth = 2;
		}
		if(s.orientation == Stroke::Orientation::RightToLeft){
			color = blue;
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
				plot.bigPoint(objectId, black, x, y, 4); // mark the start
				plot.setCursor(x, y);
				started = true;
				continue;
			}
			plot.lineTo(objectId, color, x, y, lineWidth);
			if ( j == s.size()-1 ) // mark the end
				plot.bigPoint(objectId, gray, x, y, 4);
		}
	}

	plot.update();
}

QString MainView::orientationToText(enum Stroke::Orientation orient)
{
	switch(orient){
		case Stroke::Orientation::LeftToRight :
			return tr(TidMainWindowLeftToRight);
			break;
		case Stroke::Orientation::RightToLeft :
			return tr(TidMainWindowRightToLeft);
			break;
		default:
			return "";
			break;
	}
}

void MainView::updateStat()
{
	leftToRightSum.setText(tr(TidMainWindowStatLeftToRight).arg(script.leftToRightNum));
	rightToLeftSum.setText(tr(TidMainWindowStatRightToLeft).arg(script.rightToLeftNum));
	if(plot.selectedObject && plot.selectedObject-1 < script.size())
		orientationLabel.setText(tr(TidMainWindowStatOrientation).arg(
				  orientationToText(script[plot.selectedObject-1].orientation)));
	else
		orientationLabel.setText(tr(TidMainWindowStatOrientation).arg(""));

	if (script.rightToLeftNum + script.leftToRightNum == 0 ){
		decisionLabel.setText(tr(TidMainWindowDecision).arg(tr(TidMainWindowUndecided)));
		decisionLabel.setStyleSheet("QLabel { color : rgb(0, 100, 0); }"); // undec - gren		
	}else{
		if (script.rightToLeftNum > 0){
			decisionLabel.setText(tr(TidMainWindowDecision).arg(tr(TidMainWindowLeftHanded)));
			decisionLabel.setStyleSheet("QLabel { color : rgb(0, 0, 255); }"); // LEFT
		}else{
			decisionLabel.setText(tr(TidMainWindowDecision).arg(tr(TidMainWindowRightHanded)));
			decisionLabel.setStyleSheet("QLabel { color : rgb(120, 80, 40); }"); // RIGHT
		}
	}
}

void MainView::updateCursor()
{
	objectIdLabel.setText(tr(TidMainWindowStatObjectId).arg(plot.selectedObject));
	if(plot.selectedObject && plot.selectedObject-1 < script.size())
		orientationLabel.setText(tr(TidMainWindowStatOrientation).arg(
				  orientationToText(script[plot.selectedObject-1].orientation)));
	else
		orientationLabel.setText(tr(TidMainWindowStatOrientation).arg(""));

	if (script.rightToLeftNum + script.leftToRightNum == 0 ){
		decisionLabel.setText(tr(TidMainWindowDecision).arg(tr(TidMainWindowUndecided)));
		decisionLabel.setStyleSheet("QLabel { color : rgb(0, 100, 0); }"); // undec - gren		
	}else{
		if (script.rightToLeftNum > 0){
			decisionLabel.setText(tr(TidMainWindowDecision).arg(tr(TidMainWindowLeftHanded)));
			decisionLabel.setStyleSheet("QLabel { color : rgb(0, 0, 255); }"); // LEFT
		}else{
			decisionLabel.setText(tr(TidMainWindowDecision).arg(tr(TidMainWindowRightHanded)));
			decisionLabel.setStyleSheet("QLabel { color : rgb(120, 80, 40); }"); // RIGHT
		}
	}
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

void MainView::clearAllOrientationSlot()
{
	script.clearAllOrientation();
	script.calculateHandednessStat();
	updateStat();
	redraw();
}

void MainView::calcHandednessSlot()
{
	double uncertainty = uncertaintyEdit.text().toDouble();
	clearAllOrientationSlot(); // clear orientations before calculation
	if(0 <= uncertainty)
		script.uncertainty = uncertainty;
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

