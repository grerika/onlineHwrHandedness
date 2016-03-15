/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#include <stdio.h>
#include <QtGui>

Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)

#include "h_application.h"
#include "h_mainview.h"

int main(int argc, char *args[])
{
	Application app(argc, args);

	int argi = 1;

	if(1 <= argc - argi && (
			  !strcmp(args[argi], "--help") ||
			  !strcmp(args[argi], "-h"))){
		printf(	"Usage: %s [--verbose|-v]\n"
			"       %s [--help|-h]\n"
			"\n", args[0], args[0]);
		return 0;
	}

	while(argi < argc){
		if(1 <= argc - argi && (
				  !strcmp(args[argi], "--verbose") ||
				  !strcmp(args[argi], "-v"))){
			argi++;
			continue;
		}

		if(!strcmp(args[argi], "")) /* The rests are Qt arguments. */
			break;

		fprintf(stderr, "Bad argument given: '%s'\n", args[argi]);
		LOG("Bad argument given: '%'", args[argi]);
		return 1;
	}

	try {
		//QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		//LOG("Environment variables:\n%", env.toStringList().join("\n"));

		Path::initRootPath(args[0]);

		app.setWindowIcon(QIcon(Path::icon("handedness.png")));

		app.loadTranslation();

		/* Show main view and run the app */
		app.mainView().show();
		bool ret = app.exec();
		return ret;
	} catch (std::exception & e) {
		fprintf(stderr, "Failed main view construction or other unexpected exception: %s",
				e.what());
		return -1;
	}
}
