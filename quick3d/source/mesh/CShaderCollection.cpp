
#include "CShaderCollection.h"

//-------------------------------------------------------------------------------------------------

CShaderCollection::CShaderCollection()
{
}

//-------------------------------------------------------------------------------------------------

CShaderCollection::~CShaderCollection()
{
    foreach (QOpenGLShaderProgram* pShader, m_vShaders.values())
    {
        delete pShader;
    }
}

//-------------------------------------------------------------------------------------------------

void CShaderCollection::clear()
{
    foreach (QOpenGLShaderProgram* pShader, m_vShaders.values())
    {
        delete pShader;
    }

    m_vShaders.clear();
}

//-------------------------------------------------------------------------------------------------

void CShaderCollection::addShader(QString sName, QOpenGLShaderProgram* value)
{
    if (m_vShaders.contains(sName))
    {
        delete m_vShaders[sName];
    }

    m_vShaders[sName] = value;
}

//-------------------------------------------------------------------------------------------------

QOpenGLShaderProgram* CShaderCollection::getShader(QString sName)
{
    if (m_vShaders.contains(sName))
    {
        return m_vShaders[sName];
    }

    return nullptr;
}
