/** 
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_MACROS_H
#define H_MACROS_H

#ifdef NO_COLOR
#define VT_GRAPHICS_MODE(code) ""
#else
#define VT_GRAPHICS_MODE(code) "\e\["#code"m"
#endif

#define VT_NORMAL		VT_GRAPHICS_MODE(0)
#define VT_TA_BOLD		VT_GRAPHICS_MODE(1)
#define VT_TA_UNDERSCORE	VT_GRAPHICS_MODE(4)
#define VT_TA_BLINK		VT_GRAPHICS_MODE(5)
#define VT_TA_REVERSE		VT_GRAPHICS_MODE(7)
#define VT_TA_CONCEALED		VT_GRAPHICS_MODE(8)

#define VT_BLACK	VT_GRAPHICS_MODE(30)
#define VT_RED		VT_GRAPHICS_MODE(31)
#define VT_GREEN	VT_GRAPHICS_MODE(32)
#define VT_YELLOW	VT_GRAPHICS_MODE(33)
#define VT_BLUE		VT_GRAPHICS_MODE(34)
#define VT_MAGENTA	VT_GRAPHICS_MODE(35)
#define VT_CYAN		VT_GRAPHICS_MODE(36)
#define VT_WHITE	VT_GRAPHICS_MODE(37)

#define VT_BG_BLACK	VT_GRAPHICS_MODE(40)
#define VT_BG_RED	VT_GRAPHICS_MODE(41)
#define VT_BG_GREEN	VT_GRAPHICS_MODE(42)
#define VT_BG_YELLOW	VT_GRAPHICS_MODE(43)
#define VT_BG_BLUE	VT_GRAPHICS_MODE(44)
#define VT_BG_MAGENTA	VT_GRAPHICS_MODE(45)
#define VT_BG_CYAN	VT_GRAPHICS_MODE(46)
#define VT_BG_WHITE	VT_GRAPHICS_MODE(47)

#include <QCoreApplication>
#include <h_texts.h>
#include <stdio.h>
#include <stdlib.h>

#define C_STR(QStr) QStr.toUtf8().constData()

#define NO_FCLOG __attribute__ ((no_instrument_function))

#define MY_Q_OBJECT \
	QString trShort(const char * sourceText, const char * context = 0, \
			const char * disambiguation = 0, int n = -1) { \
		return translationVariant(QCoreApplication::translate( \
				  context ? context : this->metaObject()->className(), \
				  sourceText, disambiguation, n ), TextVariant::Short); \
	} \
	QString trMed(const char * sourceText, const char * context = 0, \
			const char * disambiguation = 0, int n = -1) { \
		return translationVariant(QCoreApplication::translate( \
				  context ? context : this->metaObject()->className(), \
				  sourceText, disambiguation, n ), TextVariant::Medium); \
	} \
	QString trLong(const char * sourceText, const char * context = 0, \
			const char * disambiguation = 0, int n = -1) \
	{ \
		return translationVariant(QCoreApplication::translate( \
				  context ? context : this->metaObject()->className(), \
				  sourceText, disambiguation, n ), TextVariant::Long); \
	}

#define SCC static const char*


#ifdef DEBUG
#define DBG(...){ \
                        Text text; \
                        text.catf(VT_MAGENTA VT_TA_BOLD "DEBUG    " VT_NORMAL \
                                        VT_BLUE, __PRETTY_FUNCTION__, VT_NORMAL); \
                        text.catf(__VA_ARGS__); \
                        puts(C_STR(text)); \
                }
#else
#define DBG(...) {;}
#endif

#define LOG(...){ \
                        Text text; \
                        text.catf(VT_CYAN VT_TA_BOLD "LOG      " VT_NORMAL); \
                        text.catf(__VA_ARGS__); \
                        puts(C_STR(text)); \
                }

#define FATAL(...){ \
                        Text text; \
                        text.append(VT_RED VT_TA_UNDERSCORE VT_TA_BOLD "FATAL    " VT_NORMAL); \
                        text.catf(__VA_ARGS__); \
                        fflush(stdout); \
                        puts(C_STR(text)); \
                        fflush(stderr); \
                        exit(-1); \
			abort(); \
                }

#endif
