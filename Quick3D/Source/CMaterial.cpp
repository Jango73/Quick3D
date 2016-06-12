
// Application
#include "CRessourcesManager.h"
#include "C3DScene.h"
#include "CMaterial.h"
#include "CGLExtension.h"

#ifndef WIN32
#include <GL/glext.h>
#endif

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

double CMaterial::m_dTime = 0.0;

//-------------------------------------------------------------------------------------------------

CMaterial::CMaterial(C3DScene* pScene, QString sName)
    : m_pScene(pScene)
    , m_sName(sName)
    , m_dSelfIllumination(0.0)
    , m_dShininess(0.0)
    , m_dReflection(0.0)
    , m_dReflectionSteepness(0.0)
    , m_dSSSFactor(0.0)
    , m_dSSSRadius(0.02)
    , m_pShadowBuffer(NULL)
    , m_dIRFactor(0.8)
    , m_bUseSky(false)
    , m_bUseWaves(false)
{
    m_cAmbient = Vector4(0.03, 0.03, 0.03, 1.0);
    m_cDiffuse = Vector4(1.0, 1.0, 1.0, 1.0);
    m_cSpecular = Vector4(1.0, 1.0, 1.0, 1.0);
    m_cSubdermal = Vector4(1.0, 1.0, 1.0, 1.0);
}

//-------------------------------------------------------------------------------------------------

CMaterial::~CMaterial()
{
    clearTextures();

    if (m_pShadowBuffer)
    {
        delete m_pShadowBuffer;
    }
}

//-------------------------------------------------------------------------------------------------

Math::CVector2 CMaterial::getTexCoords(const CGeoloc& gPosition, int iLevel)
{
    return CVector2(
                fmod(gPosition.Latitude * 111000.0, 1000.0) * 0.05,
                fmod(gPosition.Longitude * 111000.0, 1000.0) * 0.05
                );
}

//-------------------------------------------------------------------------------------------------

void CMaterial::loadParameters(const QString& sBaseFile, CXMLNode xMaterial)
{
    // Lecture couleur ambiante
    CXMLNode xAmbient = xMaterial.getNodeByTagName(ParamName_Ambient);

    // Lecture couleur diffuse
    CXMLNode xDiffuse = xMaterial.getNodeByTagName(ParamName_Diffuse);

    // Lecture couleur spéculaire
    CXMLNode xSpecular = xMaterial.getNodeByTagName(ParamName_Specular);

    if (xDiffuse.isEmpty() == false)
    {
        m_cAmbient.X = xAmbient.attributes()[ParamName_r].toDouble();
        m_cAmbient.Y = xAmbient.attributes()[ParamName_g].toDouble();
        m_cAmbient.Z = xAmbient.attributes()[ParamName_b].toDouble();

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
        double dIntensity = xSpecular.attributes()[ParamName_Intensity].toDouble();
        m_cSpecular.X = xSpecular.attributes()[ParamName_r].toDouble() * dIntensity;
        m_cSpecular.Y = xSpecular.attributes()[ParamName_g].toDouble() * dIntensity;
        m_cSpecular.Z = xSpecular.attributes()[ParamName_b].toDouble() * dIntensity;
        m_dShininess = xSpecular.attributes()[ParamName_Hardness].toDouble();
    }
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
    QString sFinalResource = m_pScene->getRessourcesManager()->locateResource(sBaseFile, sResourceName);

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
    QString sFinalResource = m_pScene->getRessourcesManager()->locateResource(sBaseFile, sResourceName);

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

void CMaterial::createShadowTexture()
{
    if (m_pScene->getForDisplay())
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

    m_vDiffuseTextures.clear();
}

//-------------------------------------------------------------------------------------------------

void CMaterial::enableFrameBuffer()
{
    if (m_pShadowBuffer)
    {
        m_pScene->makeCurrentRenderingContext();
        m_pShadowBuffer->bind();
    }
}

//-------------------------------------------------------------------------------------------------

void CMaterial::disableFrameBuffer()
{
    if (m_pShadowBuffer)
    {
        m_pScene->makeCurrentRenderingContext();
        m_pShadowBuffer->release();
    }
}

//-------------------------------------------------------------------------------------------------

QGLShaderProgram* CMaterial::activate(CRenderContext* pContext)
{
    QGLShaderProgram* pProgram = pContext->scene()->getShaders()->getShader(SP_Standard);

    if (pProgram != NULL)
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
            pProgram->setUniformValue("u_material_reflection", (GLfloat) m_dReflection);
            pProgram->setUniformValue("u_material_reflection_steepness", (GLfloat) m_dReflectionSteepness);
            pProgram->setUniformValue("u_material_sss_factor", (GLfloat) m_dSSSFactor);
            pProgram->setUniformValue("u_material_sss_radius", (GLfloat) m_dSSSRadius);

            if (m_vDiffuseTextures.count() > 0)
            {
                for (int iIndex = 0; iIndex < m_vDiffuseTextures.count(); iIndex++)
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
                pProgram->setUniformValue("u_texture_diffuse_8", (GLint) 9);
                pProgram->setUniformValue("u_texture_diffuse_9", (GLint) 10);
            }
            else
            {
                pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 0);
            }

            // Environment setup
            pContext->scene()->setupEnvironment(pContext, pProgram, m_bUseSky);
        }
    }

    return pProgram;
}

//-------------------------------------------------------------------------------------------------

void CMaterial::activateShadow(CRenderContext* pContext, QGLShaderProgram* pProgram)
{
    if (m_pShadowBuffer)
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
