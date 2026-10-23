
#pragma once

// Qt
#include <QColor>
#include <QObject>
#include <QString>

#include <functional>

// Application
#include "quick3d_global.h"
#include "CVector3.h"

class QColorDialog;
class QWidget;

class QUICK3D_EXPORT CColorSelector : public QObject
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    explicit CColorSelector(QObject* pParent = nullptr);

    //!
    virtual ~CColorSelector() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //! Shows the single shared color dialog for \a sTitle, starting from \a tInitial, forwarding live changes to \a fOnColor
    void editColor(QWidget* pParent, const QString& sTitle, const QColor& tInitial, std::function<void(const QColor&)> fOnColor);

    //! Converts a normalized engine color to a Qt color
    static QColor vectorToColor(const Math::CVector3& vColor);

    //! Converts a Qt color to a normalized engine color
    static Math::CVector3 colorToVector(const QColor& tColor);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected slots:

    //!
    void onCurrentColorChanged(const QColor& tColor);

    //!
    void onDialogFinished(int iResult);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QColorDialog*                       m_pDialog;
    std::function<void(const QColor&)>  m_fOnColor;
};
