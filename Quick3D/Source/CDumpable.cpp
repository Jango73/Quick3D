
// Application
#include "CDumpable.h"

//-------------------------------------------------------------------------------------------------

CDumpable::CDumpable()
{
}

//-------------------------------------------------------------------------------------------------

CDumpable::~CDumpable()
{
}

//-------------------------------------------------------------------------------------------------

void CDumpable::dump(QTextStream& stream, int iIdent)
{
}

//-------------------------------------------------------------------------------------------------

void CDumpable::dumpOpenBlock(QTextStream& stream, int iIdent)
{
	dumpIdent(stream, iIdent, "{");
}

//-------------------------------------------------------------------------------------------------

void CDumpable::dumpCloseBlock(QTextStream& stream, int iIdent)
{
	dumpIdent(stream, iIdent, "}");
}

//-------------------------------------------------------------------------------------------------

void CDumpable::dumpIdent(QTextStream& stream, int iIdent, QString sText)
{
	for (int i = 0; i < iIdent * 4; i++)
	{
		stream << " ";
	}

	stream << sText;
	stream << "\r\n";
}
