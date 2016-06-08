
#pragma once

#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

// Qt
#include <QFile>
#include <QFileInfo>

// Fondations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CZip
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur par défaut
    CZip(const QString& sFileName);

    //! Destructeur
    virtual ~CZip();

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    int zipFiles(QStringList sFiles);

    //!
    QStringList getZipFileList();

    //! Get file content
    QByteArray getZipFileContent(QString& sFile);

protected:
    QString m_sFileName;
};

bool QUICK3D_EXPORT gzipCompress(QByteArray input, QByteArray &output, int level = -1);
bool QUICK3D_EXPORT gzipDecompress(QByteArray input, QByteArray &output);
