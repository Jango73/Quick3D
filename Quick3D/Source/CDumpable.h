
#ifndef CDUMPABLE_H
#define CDUMPABLE_H

#include "quick3d_global.h"

// Qt
#include <QTextStream>
#include <QString>

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CDumpable
{
public:

    //! Constructeur
    CDumpable();

    //! Destructor
    virtual ~CDumpable();

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //! Ouvre un block dans le fichier de sortie
    void dumpOpenBlock(QTextStream& stream, int iIdent);

    //! Ferme un block dans le fichier de sortie
    void dumpCloseBlock(QTextStream& stream, int iIdent);

    //! Ecrit un texte indenté dans le fichier de sortie
    void dumpIdent(QTextStream& stream, int iIdent, QString sText);
};

#endif // CDUMPABLE_H
