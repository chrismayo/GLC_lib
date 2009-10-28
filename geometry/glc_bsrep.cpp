/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 1.2.0, packaged on September 2009.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/
//! \file glc_bsrep.cpp implementation for the GLC_BSRep class.

#include "glc_bsrep.h"
#include "../glc_fileformatexception.h"

// The binary rep suffix
const QString GLC_BSRep::m_Suffix("BSRep");

// The binary rep magic number
const QUuid GLC_BSRep::m_Uuid("{d6f97789-36a9-4c2e-b667-0e66c27f839f}");

// The binary rep version
const quint32 GLC_BSRep::m_Version= 100;


// Default constructor
GLC_BSRep::GLC_BSRep(const QString& fileName)
: m_FileInfo()
, m_pFile(NULL)
, m_DataStream()
{
	setAbsoluteFileName(fileName);
}

// Copy constructor
GLC_BSRep::GLC_BSRep(const GLC_BSRep& binaryRep)
: m_FileInfo(binaryRep.m_FileInfo)
, m_pFile(NULL)
, m_DataStream()
{

}

GLC_BSRep::~GLC_BSRep()
{

}

// Return true if the binary rep is up to date
bool GLC_BSRep::repIsUpToDate(const QDateTime& timeStamp)
{
	bool isUpToDate= false;
	if (open(QIODevice::ReadOnly))
	{
		if (headerIsOk())
		{
			isUpToDate= timeStampOk(timeStamp);
			isUpToDate= isUpToDate && close();
		}
		else
		{
			QString message(QString("GLC_BSRep::loadRep File not recognise ") + m_FileInfo.fileName());
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::WrongFileFormat);
			close();
			throw(fileFormatException);
		}
	}
	else
	{
		QString message(QString("GLC_BSRep::loadRep Enable to open the file ") + m_FileInfo.fileName());
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::FileNotFound);
		close();
		throw(fileFormatException);
	}

	return isUpToDate;
}

//////////////////////////////////////////////////////////////////////
// name Get Functions
//////////////////////////////////////////////////////////////////////
// Load the binary rep
GLC_3DRep GLC_BSRep::loadRep()
{

	GLC_3DRep loadedRep;

	if (open(QIODevice::ReadOnly))
	{
		if (headerIsOk())
		{
			timeStampOk(QDateTime());
			m_DataStream >> loadedRep;
			if (!close())
			{
				QString message(QString("GLC_BSRep::loadRep An error occur when loading file ") + m_FileInfo.fileName());
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::WrongFileFormat);
				throw(fileFormatException);
			}
		}
		else
		{
			QString message(QString("GLC_BSRep::loadRep File not supported ") + m_FileInfo.fileName());
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::FileNotSupported);
			close();
			throw(fileFormatException);
		}
	}
	else
	{
		QString message(QString("GLC_BSRep::loadRep Enable to open the file ") + m_FileInfo.fileName());
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::FileNotFound);
		close();
		throw(fileFormatException);
	}


	return loadedRep;
}

// Return bsrep suffix
QString GLC_BSRep::suffix()
{
	return m_Suffix;
}

//////////////////////////////////////////////////////////////////////
//name Set Functions
//////////////////////////////////////////////////////////////////////
// Set the binary representation file name
void GLC_BSRep::setAbsoluteFileName(const QString& fileName)
{
	m_FileInfo.setFile(fileName);
	if (m_FileInfo.suffix() != m_Suffix)
	{
		m_FileInfo.setFile(fileName + '.' + m_Suffix);
	}

}

// Save the GLC_3DRep in serialised binary
bool GLC_BSRep::save(const GLC_3DRep& rep)
{
	//! Check if the currentFileInfo is valid and writable
	bool saveOk= open(QIODevice::WriteOnly);
	if (saveOk)
	{
		writeHeader(rep.lastModified());

		// Binary representation geometry
		// Add the rep
		m_DataStream << rep;

		// Close the file
		saveOk= close();
	}
	return saveOk;
}


// Open the file
bool GLC_BSRep::open(QIODevice::OpenMode mode)
{
	bool openOk= m_FileInfo.exists();
	if (openOk || (mode == QIODevice::WriteOnly))
	{
		m_DataStream.setDevice(NULL);
		delete m_pFile;
		m_pFile= new QFile(m_FileInfo.filePath());
		openOk= m_pFile->open(mode);
		if (openOk)
		{
			m_DataStream.setDevice(m_pFile);
		}
	}
	else
	{
		qDebug() << "File info " << m_FileInfo.filePath() << " do not exists";
	}
	return openOk;
}

// Close the file
bool GLC_BSRep::close()
{
	Q_ASSERT(m_pFile != NULL);
	Q_ASSERT(m_DataStream.device() != NULL);
	bool closeOk= m_DataStream.status() == QDataStream::Ok;
	m_DataStream.setDevice(NULL);
	m_pFile->close();
	delete m_pFile;
	m_pFile= NULL;

	return closeOk;
}

// Write the header
void GLC_BSRep::writeHeader(const QDateTime& dateTime)
{
	Q_ASSERT(m_pFile != NULL);
	Q_ASSERT(m_DataStream.device() != NULL);
	Q_ASSERT(m_pFile->openMode() == QIODevice::WriteOnly);

	// Binary representation Header
	// Add the magic number
	m_DataStream << m_Uuid;
	// Add the version
	m_DataStream << m_Version;
	// Add the time stamp
	m_DataStream << dateTime;
}

// Check the header
bool GLC_BSRep::headerIsOk()
{
	Q_ASSERT(m_pFile != NULL);
	Q_ASSERT(m_DataStream.device() != NULL);
	Q_ASSERT(m_pFile->openMode() == QIODevice::ReadOnly);

	QUuid uuid;
	quint32 version;
	m_DataStream >> uuid;
	m_DataStream >> version;

	bool headerOk= (uuid == m_Uuid);
	headerOk= headerOk && (version == m_Version);

	return headerOk;
}

// Check the time Stamp
bool GLC_BSRep::timeStampOk(const QDateTime& timeStamp)
{
	Q_ASSERT(m_pFile != NULL);
	Q_ASSERT(m_DataStream.device() != NULL);
	Q_ASSERT(m_pFile->openMode() == QIODevice::ReadOnly);

	QDateTime dateTime;
	m_DataStream >> dateTime;

	bool timeStampOk= (dateTime <= timeStamp);

	return timeStampOk;
}

