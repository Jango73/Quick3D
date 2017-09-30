
// Application
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CRenderContext.h"
#include "CMaterial.h"
#include "CGLExtension.h"

#ifndef WIN32
#include <GL/glext.h>
#endif

//-------------------------------------------------------------------------------------------------

using namespace Math;

IMPLEMENT_MEMORY_MONITORED(CMaterial, "CMaterial")

//-------------------------------------------------------------------------------------------------

double CMaterial::m_dTime = 0.0;

//-------------------------------------------------------------------------------------------------

CMaterial::CMaterial(C3DScene* pScene, QString sName)
    : CNamed(sName)
    , m_pScene(pScene)
    , m_dSelfIllumination(0.0)
    , m_dShininess(0.0)
    , m_dMetalness(0.0)
    , m_dSSSFactor(0.0)
    , m_dSSSRadius(0.02)
    , m_pShadowBuffer(nullptr)
    , m_dIRFactor(0.8)
    , m_bHasAlpha(false)
    , m_bUseSky(false)
    , m_bUseWaves(false)
    , m_bBillBoard(false)
    , m_bLines(false)
{
    m_cAmbient = CVector4(0.03, 0.03, 0.03, 1.0);
    m_cDiffuse = CVector4(1.0, 1.0, 1.0, 1.0);
    m_cSpecular = CVector4(1.0, 1.0, 1.0, 1.0);
    m_cSubdermal = CVector4(1.0, 1.0, 1.0, 1.0);
}

//-------------------------------------------------------------------------------------------------

CMaterial::~CMaterial()
{
    clearTextures();

    if (m_pShadowBuffer != nullptr)
    {
        delete m_pShadowBuffer;
    }
}

//-------------------------------------------------------------------------------------------------

Math::CVector2 CMaterial::texCoords(const CGeoloc& gPosition, int iLevel)
{
    return CVector2(
                fmod(gPosition.Latitude * 111000.0, 1000.0) * 0.05,
                fmod(gPosition.Longitude * 111000.0, 1000.0) * 0.05
                );
}

//-------------------------------------------------------------------------------------------------

bool CMaterial::hasAlpha() const
{
    return m_bHasAlpha;
}

//-------------------------------------------------------------------------------------------------

void CMaterial::loadParameters(const QString& sBaseFile, const CXMLNode& xMaterial)
{
    m_sName = xMaterial.attributes()[ParamName_Name];

    // Lecture couleur ambiante
    CXMLNode xAmbient = xMaterial.getNodeByTagName(ParamName_Ambient);

    // Lecture couleur diffuse
    CXMLNode xDiffuse = xMaterial.getNodeByTagName(ParamName_Diffuse);

    // Lecture couleur spéculaire
    CXMLNode xSpecular = xMaterial.getNodeByTagName(ParamName_Specular);

    // Read normal color
    CXMLNode xNormal = xMaterial.getNodeByTagName(ParamName_Normal);

    if (xAmbient.isEmpty() == false)
    {
        m_cAmbient.X = xAmbient.attributes()[ParamName_r].toDouble();
        m_cAmbient.Y = xAmbient.attributes()[ParamName_g].toDouble();
        m_cAmbient.Z = xAmbient.attributes()[ParamName_b].toDouble();
    }

    if (xDiffuse.isEmpty() == false)
    {
        m_cDiffuse.X = xDiffuse.attributes()[ParamName_r].toDouble();
        m_cDiffuse.Y = xDiffuse.attributes()[ParamName_g].toDouble();
        m_cDiffuse.Z = xDiffuse.attributes()[ParamName_b].toDouble();

        QString sTextureName = xDiffuse.attributes()[ParamName_Map];

        if (sTextureName.isEmpty() == false)
        {
            QString sFileName = QFileInfo(sTextureName).fileName();

            if (sFileName.startsWith(ParamName_DynTex))
            {
                addDynamicDiffuseTexture(sBaseFile, sTextureName);
            }
            else
            {
                addDiffuseTexture(sBaseFile, sTextureName);
            }
        }
    }

    if (xSpecular.isEmpty() == false)
    {
        m_cSpecular.X = xSpecular.attributes()[ParamName_r].toDouble();
        m_cSpecular.Y = xSpecular.attributes()[ParamName_g].toDouble();
        m_cSpecular.Z = xSpecular.attributes()[ParamName_b].toDouble();
        m_dShininess = xSpecular.attributes()[ParamName_Hardness].toDouble();
        m_dMetalness = xSpecular.attributes()[ParamName_Intensity].toDouble();
    }

    if (xNormal.isEmpty() == false)
    {
        QString sTextureName = xNormal.attributes()[ParamName_Map];

        if (sTextureName.isEmpty() == false)
        {
            addNormalTexture(sBaseFile, sTextureName);
        }
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::solveLinks(C3DScene* pScene)
{
}

//-------------------------------------------------------------------------------------------------

void CMaterial::clearLinks(C3DScene* pScene)
{
}

//-------------------------------------------------------------------------------------------------

void CMaterial::update(double dDeltaTime)
{
    for (int iIndex = 0; iIndex < m_vDiffuseTextures.count(); iIndex++)
    {
        m_vDiffuseTextures[iIndex]->update(dDeltaTime);
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::addDiffuseTexture(const QString& sBaseFile, const QString& sResourceName)
{
    QString sFinalResource = m_pScene->ressourcesManager()->locateResource(sBaseFile, sResourceName);

    if (sFinalResource.isEmpty() == false)
    {
        QImage imgTexture = QImage(sFinalResource);
        addDiffuseTexture(sFinalResource, imgTexture);
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::addDiffuseTexture(const QString& sName, const QImage& imgTexture)
{
    if (imgTexture.width() > 0 && imgTexture.height() > 0)
    {
        m_vDiffuseTextures.append(new CTexture(m_pScene, sName, imgTexture, imgTexture.size(), m_vDiffuseTextures.count(), false));
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::addDynamicDiffuseTexture(const QString& sBaseFile, const QString& sResourceName)
{
    QString sFinalResource = m_pScene->ressourcesManager()->locateResource(sBaseFile, sResourceName);

    if (sFinalResource.isEmpty() == false)
    {
        QImage imgTexture = QImage(sFinalResource);
        addDynamicDiffuseTexture(sFinalResource, imgTexture);
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::addDynamicDiffuseTexture(const QString& sName, const QImage& imgTexture)
{
    m_pScene->makeCurrentRenderingContext();

    if (imgTexture.width() > 0 && imgTexture.height() > 0)
    {
        m_vDiffuseTextures.append(new CTexture(m_pScene, sName, imgTexture, imgTexture.size(), m_vDiffuseTextures.count(), true));
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::addNormalTexture(const QString& sBaseFile, const QString& sResourceName)
{
    QString sFinalResource = m_pScene->ressourcesManager()->locateResource(sBaseFile, sResourceName);

    if (sFinalResource.isEmpty() == false)
    {
        QImage imgTexture = QImage(sFinalResource);
        addNormalTexture(sFinalResource, imgTexture);
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::addNormalTexture(const QString& sName, const QImage& imgTexture)
{
    if (imgTexture.width() > 0 && imgTexture.height() > 0)
    {
        m_vNormalTextures.append(new CTexture(m_pScene, sName, imgTexture, imgTexture.size(), m_vNormalTextures.count(), false));
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::createShadowTexture()
{
    if (m_pScene->forDisplay())
    {
        m_pScene->makeCurrentRenderingContext();

        m_pShadowBuffer = new QGLFramebufferObject(2048, 2048, QGLFramebufferObject::Depth, GL_TEXTURE_2D, GL_RED);
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::clearTextures()
{
    foreach (CTexture* pTexture, m_vDiffuseTextures)
    {
        delete pTexture;
    }

    foreach (CTexture* pTexture, m_vNormalTextures)
    {
        delete pTexture;
    }

    m_vDiffuseTextures.clear();
}

//-------------------------------------------------------------------------------------------------

void CMaterial::enableFrameBuffer()
{
    if (m_pShadowBuffer != nullptr)
    {
        m_pScene->makeCurrentRenderingContext();
        m_pShadowBuffer->bind();
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::disableFrameBuffer()
{
    if (m_pShadowBuffer != nullptr)
    {
        m_pScene->makeCurrentRenderingContext();
        m_pShadowBuffer->release();
    }
}

//-------------------------------------------------------------------------------------------------

QGLShaderProgram* CMaterial::activate(CRenderContext* pContext)
{
    QGLShaderProgram* pProgram = nullptr;

    if (m_bBillBoard)
    {
        pProgram = pContext->scene()->shaders()->getShader(SP_Standard_Billboard);
    }
    else if (m_bLines)
    {
        pProgram = pContext->scene()->shaders()->getShader(SP_Special_Lines);
    }
    else
    {
        pProgram = pContext->scene()->shaders()->getShader(SP_Standard_Mesh);
    }

    if (pProgram != nullptr)
    {
        if (pContext->pActiveMaterial != this)
        {
            pContext->pActiveMaterial = this;

            m_pScene->makeCurrentRenderingContext();

            pProgram->bind();

            // IR setup
            if (pContext->bUseIR)
            {
                pProgram->setUniformValue("u_IR_enable", (GLint) 1);
                pProgram->setUniformValue("u_IR_factor", (GLfloat) m_dIRFactor);
            }
            else
            {
                pProgram->setUniformValue("u_IR_enable", (GLint) 0);
                pProgram->setUniformValue("u_IR_factor", (GLfloat) 1.0);
            }

            // Inverse polarity setup
            if (pContext->bUseInversePolarity)
            {
                pProgram->setUniformValue("u_inverse_polarity_enable", (GLint) 1);
            }
            else
            {
                pProgram->setUniformValue("u_inverse_polarity_enable", (GLint) 0);
            }

            // Sky setup
            if (m_bUseSky)
            {
                pProgram->setUniformValue("u_sky_enable", (GLint) 1);
            }
            else
            {
                pProgram->setUniformValue("u_sky_enable", (GLint) 0);
            }

            // Wave setup
            pProgram->setUniformValue("u_wave_enable", (GLint) 0);

            // Standard setup
            pProgram->setUniformValue("u_material_ambient", QVector4D(m_cAmbient.X, m_cAmbient.Y, m_cAmbient.Z, m_cAmbient.W));
            pProgram->setUniformValue("u_material_diffuse", QVector4D(m_cDiffuse.X, m_cDiffuse.Y, m_cDiffuse.Z, m_cDiffuse.W));
            pProgram->setUniformValue("u_material_specular", QVector4D(m_cSpecular.X, m_cSpecular.Y, m_cSpecular.Z, m_cSpecular.W));
            pProgram->setUniformValue("u_material_subdermal", QVector4D(m_cSubdermal.X, m_cSubdermal.Y, m_cSubdermal.Z, m_cSubdermal.W));
            pProgram->setUniformValue("u_material_self_illum", (GLfloat) m_dSelfIllumination);
            pProgram->setUniformValue("u_material_shininess", (GLfloat) m_dShininess);
            pProgram->setUniformValue("u_material_metalness", (GLfloat) m_dMetalness);
            pProgram->setUniformValue("u_material_sss_factor", (GLfloat) m_dSSSFactor);
            pProgram->setUniformValue("u_material_sss_radius", (GLfloat) m_dSSSRadius);

            if (m_vDiffuseTextures.count() > 0)
            {
                for (int iIndex = 0; iIndex < m_vDiffuseTextures.count() && iIndex < 8; iIndex++)
                {
                    m_vDiffuseTextures[iIndex]->activate(iIndex);
                }

                pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 1);

                pProgram->setUniformValue("u_texture_diffuse_0", (GLint) 1);
                pProgram->setUniformValue("u_texture_diffuse_1", (GLint) 2);
                pProgram->setUniformValue("u_texture_diffuse_2", (GLint) 3);
                pProgram->setUniformValue("u_texture_diffuse_3", (GLint) 4);
                pProgram->setUniformValue("u_texture_diffuse_4", (GLint) 5);
                pProgram->setUniformValue("u_texture_diffuse_5", (GLint) 6);
                pProgram->setUniformValue("u_texture_diffuse_6", (GLint) 7);
                pProgram->setUniformValue("u_texture_diffuse_7", (GLint) 8);
            }
            else
            {
                pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 0);
            }

            // Bump map setup

            pProgram->setUniformValue("u_texture_bump_enable", (GLint) 0);
            pProgram->setUniformValue("u_texture_bump", (GLint) 1);

            // Environment setup
            pContext->scene()->setupEnvironment(pContext, pProgram, m_bUseSky);
        }
    }

    return pProgram;
}

//-------------------------------------------------------------------------------------------------

void CMaterial::activateShadow(CRenderContext* pContext)
{
    if (m_pShadowBuffer != nullptr)
    {
        GL_glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_pShadowBuffer->texture());

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
}

//-------------------------------------------------------------------------------------------------

CGeoloc CMaterial::transformGeoloc(const CGeoloc& gPosition)
{
    return gPosition;
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this material to \a stream using the indentation value in \a iIdent.
*/
void CMaterial::dump(QTextStream& stream, int iIdent)
{
    dumpIdent(stream, iIdent, QString("[CMaterial]"));
    dumpIdent(stream, iIdent, QString("Name : %1").arg(m_sName));
    dumpIdent(stream, iIdent, QString("Ambient color : %1").arg(m_cAmbient.toString()));
    dumpIdent(stream, iIdent, QString("Diffuse color : %1").arg(m_cDiffuse.toString()));
    dumpIdent(stream, iIdent, QString("Specular color : %1").arg(m_cSpecular.toString()));
    dumpIdent(stream, iIdent, QString("Subdermal color : %1").arg(m_cSubdermal.toString()));
    dumpIdent(stream, iIdent, QString("Self illumination : %1").arg(m_dSelfIllumination));
    dumpIdent(stream, iIdent, QString("Shininess : %1").arg(m_dShininess));
    dumpIdent(stream, iIdent, QString("Metalness : %1").arg(m_dMetalness));
    dumpIdent(stream, iIdent, QString("SSS Factor : %1").arg(m_dSSSFactor));
    dumpIdent(stream, iIdent, QString("SSS Radius : %1").arg(m_dSSSRadius));
    dumpIdent(stream, iIdent, QString("IR Factor : %1").arg(m_dIRFactor));
    dumpIdent(stream, iIdent, QString("Has alpha : %1").arg(m_bHasAlpha));
    dumpIdent(stream, iIdent, QString("Use sky : %1").arg(m_bUseSky));
    dumpIdent(stream, iIdent, QString("Use waves : %1").arg(m_bUseWaves));
    dumpIdent(stream, iIdent, QString("Bill board : %1").arg(m_bBillBoard));
    dumpIdent(stream, iIdent, QString("Lines : %1").arg(m_bLines));
}
