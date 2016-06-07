
#ifndef C3DSCENE_H
#define C3DSCENE_H

#include "quick3d_global.h"

// Qt
#include <QSharedPointer>
#include <QGLWidget>
#include <QImage>
#include <QTime>

// Application
#include "CVector3.h"
#include "CMatrix4.h"
#include "Interpolator.h"
#include "CDumpable.h"
#include "CGLExtension.h"
#include "CRessourcesManager.h"
#include "CBuildingGenerator.h"
#include "CTreeGenerator.h"
#include "CShaderCollection.h"
#include "CCamera.h"
#include "CLight.h"
#include "CFog.h"
#include "CRain.h"
#include "CRenderContext.h"
#include "CViewport.h"

//-------------------------------------------------------------------------------------------------

class CView;
class CController;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT C3DScene : public CDumpable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur
    C3DScene(bool bForDisplay = true);

    //! Destructeur
    virtual ~C3DScene();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setWorldOrigin(Math::CVector3 value);

    //!
    void setController(CController* pController);

    //!
    void setDefaultController(CController* pController);

    //!
    void setShaderQuality(double value);

    //!
    void setTime(double value);

    //!
    void setTimeOfDay(QTime value);

    //!
    void setFogLevel(double value);

    //!
    void setWindLevel(double value);

    //!
    void setRainVisible(bool bOnOff);

    //!
    void forceWideFOV(bool value);

    //!
    void forceSmallFOV(bool value);

    //!
    void forceIR(bool value);

    //!
    void setStreamView(bool value) { m_bStreamView = value; }

    //!
    void setDepthComputing(bool value) { m_bDepthComputing = value; }

    //!
    void setRenderingShadows(bool value) { m_bRenderingShadows = value; }

    //!
    void setEditMode(bool value) { m_bEditMode = value; }

    //!
    void setDebugMode(bool value) { m_DebugMode = value; }

    //!
    void setBoundsOnly(bool value) { m_bBoundsOnly = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    bool getForDisplay() const { return m_bForDisplay; }

    //!
    Math::CVector3 getWorldOrigin() const { return m_WorldOrigin; }

    //!
    QMap<int, CViewport*>& getViewports() { return m_pViewports; }

    //!
    const QMap<int, CViewport*>& getViewports() const { return m_pViewports; }

    //!
    double getShaderQuality() { return m_dShaderQuality; }

    //!
    CGLExtension* getGLExtension() { return m_pGLExtension; }

    //!
    CRessourcesManager* getRessourcesManager() { return m_pRessourcesManager; }

    //!
    CBuildingGenerator* getBuildingGenerator() { return m_pBuildingGenerator; }

    //!
    CTreeGenerator* getTreeGenerator() { return m_pTreeGenerator; }

    //!
    CShaderCollection* getShaders() { return m_vShaders; }

    //!
    CController* getController() { return m_pController; }

    //!
    QVector<CLight*> getLights();

    //!
    QVector<QSharedPointer<CComponent> >& getComponents() { return m_vComponents; }

    //!
    const QVector<QSharedPointer<CComponent> >& getComponents() const { return m_vComponents; }

    //!
    QVector<QSharedPointer<CComponent> > getComponentsByTag(const QString& sTag);

    //!
    QVector<CLight*> getLightsByTag(const QString& sTag);

    //!
    CFog& getFog() { return m_tFog; }

    //!
    double getTime() const { return m_dTime; }

    //!
    double getWindLevel() const { return m_dWindLevel; }

    //!
    bool isRenderingShadows() const { return m_bRenderingShadows; }

    //!
    bool getStreamView() const { return m_bStreamView; }

    //!
    bool getDepthComputing() const { return m_bDepthComputing; }

    //!
    const QImage& getFrameBuffer() const { return m_imgFrameBuffer; }

    //!
    bool getEditMode() { return m_bEditMode; }

    //!
    bool getDebugMode() { return m_DebugMode; }

    //!
    bool getBoundsOnly() { return m_bBoundsOnly; }

    //-------------------------------------------------------------------------------------------------
    // Méthodes de contrôle
    //-------------------------------------------------------------------------------------------------

    //!
    void clear();

    //! Détruit les composants (hors lumières)
    void clearComponents();

    //! Détruit les viewports
    void clearViewports();

    //! Initialise la scène
    virtual void init(QVector<CComponent*> vComponents);

    //! Initialise les shaders
    virtual void initShaders();

    //! Prépare l'environnement
    virtual void setupEnvironment(CRenderContext* pContext, QGLShaderProgram* pProgram, bool bBackgroundItem);

    //! Prépare les lumières
    virtual void setupLights(CRenderContext* pContext);

    //!
    virtual void makeCurrentRenderingContext();

    //!
    void autoResolveHeightFields();

    //!
    void updateScene(double dDeltaTime);

    //!
    void addComponent(QSharedPointer<CComponent> pComponent);

    //!
    void deleteComponentsByTag(const QString& sTag);

    //!
    static void Matrix4ToGLdouble(const Math::CMatrix4& tMatrix, GLdouble* dBuffer);

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 aRay) const;

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersectComponentHierarchy(CComponent* pComponent, Math::CRay3 aRay) const;

    //! Calcul d'intersection avec un rayon
    Math::RayTracingResult intersectRecurse(CComponent* pComponent, const Math::CRay3& aRay) const;

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // Méthodes protégées
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    static void getLightsRecurse(QVector<CLight*>& vLights, CComponent* pComponent);

    //!
    static void getLightsByTagRecurse(QVector<CLight*>& vLights, const QString &sTag, CComponent* pComponent);

    //-------------------------------------------------------------------------------------------------
    // Propriétés
    //-------------------------------------------------------------------------------------------------

public:

    int										m_iNumMeshesDrawn;
    int										m_iNumPolysDrawn;
    int										m_iNumChunksDrawn;
    CRain*									m_pRain;

protected:

    QMap<int, CViewport*>					m_pViewports;
    CGLExtension*							m_pGLExtension;
    CRessourcesManager*						m_pRessourcesManager;
    CBuildingGenerator*						m_pBuildingGenerator;
    CTreeGenerator*							m_pTreeGenerator;
    CShaderCollection*						m_vShaders;
    QVector<QSharedPointer<CComponent> >	m_vComponents;
    CController*							m_pController;
    CController*							m_pDefaultController;
    Math::CVector3							m_WorldOrigin;
    QTime									m_tTimeOfDay;
    CFog									m_tFog;
    Math::Interpolator<Math::Vector4>		m_vSunColor;
    bool									m_bForDisplay;
    bool									m_bEditMode;
    bool									m_DebugMode;
    bool									m_bBoundsOnly;
    double									m_dShaderQuality;
    double									m_dWindLevel;
    bool									m_bRenderingShadows;
    bool									m_bforceWideFOV;
    bool									m_bforceSmallFOV;
    bool									m_bForceIR;
    bool									m_bStreamView;
    bool									m_bDepthComputing;
    double									m_dTime;
    double									m_dSunIntensity;
    QImage									m_imgFrameBuffer;
};

#endif // C3DSCENE_H
