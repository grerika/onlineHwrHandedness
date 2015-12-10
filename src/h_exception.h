/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2015 Csaszar, Peter
 */

#ifndef H_EXCEPTION_H
#define H_EXCEPTION_H

#include <stdarg.h>

#include <stdexcept>

class PrimeException : public std::exception
{
public:
	/* FIXME
	 * Shit versions of gcc require copy construction on throwing, but
	 * I cheet and I implement moving instead, which however might 
	 * result in some unexpected situation. We will see. */
	explicit PrimeException(const PrimeException & orig) :
		std::exception(),
		id(orig.id)
	{
		/* FIXME: hack because I hate deep copies. */
		((PrimeException&)orig).id = 0;
	}
	const PrimeException & operator=(const PrimeException &) = delete;

	PrimeException(PrimeException && temp) :
		std::exception(),
		id(temp.id)
	{
		temp.id = 0;
	}

	const PrimeException & operator=(PrimeException && temp)
	{
		id = temp.id;
		temp.id = 0;
		return *this;
	}

	virtual ~PrimeException() throw()
	{
	}

	virtual const char * what() const throw()
	{
		return "No explanation given.";
	}

protected:
	explicit PrimeException() :
		std::exception(),
		id(nextId())
	{
	}
	explicit PrimeException(const std::exception & e) :
		std::exception(),
		id(nextId())
	{
		notePrintf("Causing exception was received as std:exception. What: %s", e.what());
	}

protected:
	unsigned id;
};

#endif

