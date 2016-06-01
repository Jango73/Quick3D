
// Application
#include "CLogManager.h"
#include "CConnectedStream.h"

//-------------------------------------------------------------------------------------------------

CConnectedStream::CConnectedStream()
: m_iMinBytesForReadyRead(0)
{
}

//-------------------------------------------------------------------------------------------------

CConnectedStream::~CConnectedStream()
{
}

//-------------------------------------------------------------------------------------------------

void CConnectedStream::setMinBytesForReadyRead(int iMinBytes)
{
	m_iMinBytesForReadyRead = iMinBytes;
}
