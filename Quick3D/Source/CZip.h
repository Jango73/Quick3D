
#pragma once

// Qt
#include <QFile>
#include <QFileInfo>

// std
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// Fondations
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CZip
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Default constructor
    CZip(const QString& sFileName);

    //! Destructor
    virtual ~CZip();

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //!
    int zipFiles(QStringList sFiles);

    //!
    QStringList getZipFileList();

    //! Get file content
    QByteArray getZipFileContent(QString& sFile);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QString m_sFileName;
};

bool QUICK3D_EXPORT gzipCompress(QByteArray input, QByteArray &output, int level = -1);
bool QUICK3D_EXPORT gzipDecompress(QByteArray input, QByteArray &output);
