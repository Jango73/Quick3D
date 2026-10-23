

// qt-plus
#include "CLogger.h"
#include "CTimeSampler.h"

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

    // Read ambient color
    CXMLNode xAmbient = xMaterial.getNodeByTagName(ParamName_Ambient);

    // Read diffuse color
    CXMLNode xDiffuse = xMaterial.getNodeByTagName(ParamName_Diffuse);

    // Read specular color
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
    else
    {
        LOG_WARNING(QString("CMaterial::addDiffuseTexture() : texture not found : %1").arg(sResourceName));
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
    else
    {
        LOG_WARNING(QString("CMaterial::addDynamicDiffuseTexture() : texture not found : %1").arg(sResourceName));
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
    else
    {
        LOG_WARNING(QString("CMaterial::addNormalTexture() : texture not found : %1").arg(sResourceName));
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
    if (m_pScene->forDisplay() && m_pShadowBuffer == nullptr)
    {
        m_pScene->makeCurrentRenderingContext();

        // No OpenGL context exists yet while the scene loads (widget not shown):
        // creation is deferred to the first render pass (see enableFrameBuffer)
        if (QOpenGLContext::currentContext() != nullptr)
        {
            m_pShadowBuffer = new QOpenGLFramebufferObject(2048, 2048, QOpenGLFramebufferObject::Depth, GL_TEXTURE_2D, GL_RED);
        }
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
    if (m_pShadowBuffer == nullptr)
    {
        createShadowTexture();
    }

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

/*!
    Selects the shader program for the given context. \br\br
    pContext must be non-null: a null context returns null.
*/
QOpenGLShaderProgram* CMaterial::programForContext(CRenderContext* pContext)
{
    if (pContext == nullptr)
    {
        return nullptr;
    }

    if (m_bBillBoard)
    {
        return pContext->scene()->shaders()->getShader(SP_Standard_Billboard);
    }

    if (m_bLines)
    {
        return pContext->scene()->shaders()->getShader(SP_Special_Lines);
    }

    return pContext->scene()->shaders()->getShader(SP_Standard_Mesh);
}

//-------------------------------------------------------------------------------------------------

/*!
    Binds the given program to the given context. \br\br
    pContext must be non-null: a null context is a no-op. The context scene
    is the single scene source here, as in programForContext().
*/
void CMaterial::bindProgram(CRenderContext* pContext, QOpenGLShaderProgram* pProgram)
{
    if (pContext == nullptr)
    {
        return;
    }

    pContext->scene()->makeCurrentRenderingContext();

    pProgram->bind();
}

//-------------------------------------------------------------------------------------------------

/*!
    Applies the pass-invariant state: uniforms, diffuse maps and environment.
    Runs only when the material changes.
*/
void CMaterial::applyStaticState(CRenderContext* pContext, QOpenGLShaderProgram* pProgram)
{
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

    bindDiffuseMaps(pContext, pProgram);

    // Bump map setup

    pProgram->setUniformValue("u_texture_bump_enable", (GLint) 0);
    pProgram->setUniformValue("u_texture_bump", (GLint) 1);

    // Environment setup
    pContext->scene()->setupEnvironment(pContext, pProgram, m_bUseSky);
}

//-------------------------------------------------------------------------------------------------

/*!
    Binds the diffuse texture maps and selects the diffuse shader path.
*/
void CMaterial::bindDiffuseMaps(CRenderContext* pContext, QOpenGLShaderProgram* pProgram)
{
    Q_UNUSED(pContext);

    if (m_vDiffuseTextures.count() > 0)
    {
        int iSlotCount = m_vDiffuseTextures.count();

        if (iSlotCount > DiffuseTextureSlotCount)
        {
            iSlotCount = DiffuseTextureSlotCount;
        }

        for (int iIndex = 0; iIndex < iSlotCount; iIndex++)
        {
            m_vDiffuseTextures[iIndex]->activate(iIndex);
        }

        // Fallback : textures created before the first frame have no GPU
        // counterpart yet (no OpenGL context while the scene loads). If the
        // upload failed, stay on the untextured path instead of sampling
        // unbound units (pure black output)
        bool bTexturesReady = true;

        for (int iIndex = 0; iIndex < iSlotCount; iIndex++)
        {
            if (m_vDiffuseTextures[iIndex]->glTexture() == 0)
            {
                bTexturesReady = false;
                break;
            }
        }

        if (bTexturesReady)
        {
            pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 1);

            for (int iIndex = 0; iIndex < iSlotCount; iIndex++)
            {
                pProgram->setUniformValue(QString("u_texture_diffuse_%1").arg(iIndex).toLatin1().constData(), (GLint) (DiffuseTextureUnitBase + iIndex));
            }
        }
        else
        {
            pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 0);
        }
    }
    else
    {
        pProgram->setUniformValue("u_texture_diffuse_enable", (GLint) 0);
    }
}

//-------------------------------------------------------------------------------------------------

/*!
    Applies the per-draw state. The base implementation does nothing.
*/
void CMaterial::applyDynamicState(CRenderContext* pContext, QOpenGLShaderProgram* pProgram)
{
    Q_UNUSED(pContext);
    Q_UNUSED(pProgram);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Activates this material for rendering: selects the shader program for
    the context, applies the pass-invariant state on material change only,
    then applies the per-draw state on every call. \br\br
    pContext must be non-null: a null context returns null. Returns the
    bound program, null when no program matches the context.
*/
/***************************************************************************/
QOpenGLShaderProgram* CMaterial::activate(CRenderContext* pContext)
{
    if (pContext == nullptr)
    {
        return nullptr;
    }

    // Timed through the render context statistics and flushed to CTimeSampler once per
    // layer pass: a scoped sample here would run hundreds of mutex-protected map updates
    // per frame and collapse the frame rate it is supposed to measure.
    bool bSample = CTimeSampler::getInstance()->isEnabled();
    QElapsedTimer tTimer;

    if (bSample == true)
    {
        tTimer.start();
    }

    QOpenGLShaderProgram* pProgram = programForContext(pContext);

    if (pProgram != nullptr)
    {
        if (pContext->pActiveMaterial != this)
        {
            pContext->pActiveMaterial = this;

            bindProgram(pContext, pProgram);

            applyStaticState(pContext, pProgram);
        }

        applyDynamicState(pContext, pProgram);
    }

    if (bSample == true)
    {
        pContext->tStatistics.m_iMaterialActivateNs += tTimer.nsecsElapsed();
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

/***************************************************************************/
/*!
    Selects the tile for the given geolocation and level before a terrain
    paint. \br\br
    The base implementation does nothing: only tiled materials track a
    current tile.
*/
/***************************************************************************/
void CMaterial::prepareForTerrainPaint(const CGeoloc& gPosition, int iLevel)
{
    Q_UNUSED(gPosition);
    Q_UNUSED(iLevel);
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this material to \a stream using the indentation value in \a iIdent.
*/
void CMaterial::dump(QTextStream& stream, int iIdent)
{
    dumpIndented(stream, iIdent, QString("[CMaterial]"));
    dumpIndented(stream, iIdent, QString("Name : %1").arg(m_sName));
    dumpIndented(stream, iIdent, QString("Ambient color : %1").arg(m_cAmbient.toString()));
    dumpIndented(stream, iIdent, QString("Diffuse color : %1").arg(m_cDiffuse.toString()));
    dumpIndented(stream, iIdent, QString("Specular color : %1").arg(m_cSpecular.toString()));
    dumpIndented(stream, iIdent, QString("Subdermal color : %1").arg(m_cSubdermal.toString()));
    dumpIndented(stream, iIdent, QString("Self illumination : %1").arg(m_dSelfIllumination));
    dumpIndented(stream, iIdent, QString("Shininess : %1").arg(m_dShininess));
    dumpIndented(stream, iIdent, QString("Metalness : %1").arg(m_dMetalness));
    dumpIndented(stream, iIdent, QString("SSS Factor : %1").arg(m_dSSSFactor));
    dumpIndented(stream, iIdent, QString("SSS Radius : %1").arg(m_dSSSRadius));
    dumpIndented(stream, iIdent, QString("IR Factor : %1").arg(m_dIRFactor));
    dumpIndented(stream, iIdent, QString("Has alpha : %1").arg(m_bHasAlpha));
    dumpIndented(stream, iIdent, QString("Use sky : %1").arg(m_bUseSky));
    dumpIndented(stream, iIdent, QString("Use waves : %1").arg(m_bUseWaves));
    dumpIndented(stream, iIdent, QString("Bill board : %1").arg(m_bBillBoard));
    dumpIndented(stream, iIdent, QString("Lines : %1").arg(m_bLines));
}
