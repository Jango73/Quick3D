
#include "CShaderCollection.h"

//-------------------------------------------------------------------------------------------------

CShaderCollection::CShaderCollection()
{
}

//-------------------------------------------------------------------------------------------------

CShaderCollection::~CShaderCollection()
{
	foreach (QGLShaderProgram* pShader, m_vShaders.values())
	{
		delete pShader;
	}
}

//-------------------------------------------------------------------------------------------------

void CShaderCollection::clear()
{
	m_vShaders.clear();
}

//-------------------------------------------------------------------------------------------------

void CShaderCollection::addShader(QString sName, QGLShaderProgram* value)
{
	if (m_vShaders.contains(sName))
	{
		delete m_vShaders[sName];
	}

	m_vShaders[sName] = value;
}

//-------------------------------------------------------------------------------------------------

QGLShaderProgram* CShaderCollection::getShader(QString sName)
{
	if (m_vShaders.contains(sName))
	{
		return m_vShaders[sName];
	}

	return NULL;
}
