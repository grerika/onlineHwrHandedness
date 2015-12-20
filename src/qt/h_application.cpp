/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

//#define DEBUG

#include <QtGui>
#include <QMessageBox>

#include <config.h>

#include "h_application.h"

Application::Application(int & argc, char *argv[]) :
	QApplication(argc, argv),
	mainViewPtr(0)
{
	QCoreApplication::setOrganizationName(ORGANIZATION);
	QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN_NAME);
	QCoreApplication::setApplicationName(PKGNAME);
}

Application::~Application()
{
	delete mainViewPtr;
}

MainView & Application::mainView()
{
	if(!mainViewPtr)
		mainViewPtr = new MainView();
	return *mainViewPtr;
}

void Application::loadTranslation(QString lang)
{
	if(lang == ""){
		QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		lang = env.value("LANG", "en").left(2);
		if(lang != "hu")
			lang = "en";
	}

	if(!lang.length())
		lang = "en" ; //locale.name();
	lang.truncate(2);
	QString trFile(Path::translation(C_STR(lang)));
	LOG("Translation file to load: %", trFile);

	QTranslator* translator(new QTranslator);
	if(!translator->load(trFile)){
		delete translator;
		FATAL("Can not load language file %", trFile);
	}
	qApp->installTranslator(translator);
}

bool Application::notify(QObject * receiver, QEvent * event)
{
	//DBG("receiver: % event: %", receiver, event);
	QString info;
	static bool altPressed = false;

	try {
		switch(event->type()){
		case QEvent::KeyPress:
			{
				QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

				if(keyEvent->matches(QKeySequence::Quit)){
					if(mainViewPtr)
						mainViewPtr->close();
					else
						quit();
					break;
				}

				if(keyEvent->key() == Qt::Key_Alt && altPressed == true)
					return true;//repeated alt press QT BUG return as if handled
				else
					altPressed = true;
				/*DBG("Key press % for %:%",
						keyEvent,
						receiver->metaObject()->className(), receiver);*/
			}
			break;
		case QEvent::KeyRelease:
			{
				QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
				if(keyEvent->key() == Qt::Key_Alt)
					altPressed = false;
				/*DBG("Key release % for %:%",
						keyEvent->key(),
						receiver->metaObject()->className(), receiver);*/
			}
			break;
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
#ifdef DEBUG
/*			{
				QMouseEvent* const mouseEvent = static_cast<QMouseEvent*>(event);
				QPoint pos = mouseEvent->globalPos();
				DBG("Mouse event at %, % for %:%",
						pos.x(), pos.y(),
						receiver->metaObject()->className(), receiver);
			break;
			}*/
#endif
		case QEvent::MouseMove:
		default:
			break;
		}
		/*DBG("Event %:% for %:%",
				event->type(), event,
				receiver->metaObject()->className(), receiver);*/
		bool res = QApplication::notify(receiver, event);
		//DBG("Res ", (res) ? "true" : "false");
		return res;
	} catch(std::exception& e) {
		info = QString::fromUtf8(e.what());
	}
	if(info.size()){
		if(1024 < info.size()){
			info.resize(1024);
			info.append(" ...");
		}

		QMessageBox(	QMessageBox::Warning, "Exception thrown", info,
				QMessageBox::Ok, 0, Qt::Dialog).exec();
		return true;
	}

	return false; /* event was not handled */
}
