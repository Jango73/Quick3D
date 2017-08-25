
#pragma once

// Qt
#include <QTextStream>
#include <QString>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CDumpable
{
public:

    //! Constructeur
    CDumpable();

    //! Destructor
    virtual ~CDumpable();

    //! Dumps contents to a stream
    virtual void dump(QTextStream& stream, int iIdent);

    //! Ouvre un block dans le fichier de sortie
    void dumpOpenBlock(QTextStream& stream, int iIdent);

    //! Ferme un block dans le fichier de sortie
    void dumpCloseBlock(QTextStream& stream, int iIdent);

    //! Ecrit un texte indenté dans le fichier de sortie
    void dumpIdent(QTextStream& stream, int iIdent, QString sText);
};
