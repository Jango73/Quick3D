
// Qt
#include <QColorDialog>
#include <QWidget>

// Application
#include "CColorSelector.h"

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Builds a selector owning the single shared color dialog.
*/
/***************************************************************************/
CColorSelector::CColorSelector(QObject* pParent)
    : QObject(pParent)
    , m_pDialog(nullptr)
    , m_fOnColor(nullptr)
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Releases the selector and its shared dialog.
*/
/***************************************************************************/
CColorSelector::~CColorSelector()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Shows the single shared color dialog for \a sTitle, starting from
    \a tInitial, forwarding live changes to \a fOnColor. Reuses the same
    non-native dialog every time so all callers share one source of truth.
*/
/***************************************************************************/
void CColorSelector::editColor(QWidget* pParent, const QString& sTitle, const QColor& tInitial, std::function<void(const QColor&)> fOnColor)
{
    if (m_pDialog == nullptr)
    {
        m_pDialog = new QColorDialog(pParent);
        m_pDialog->setOption(QColorDialog::DontUseNativeDialog, true);

        connect(m_pDialog, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(onCurrentColorChanged(const QColor&)));
        connect(m_pDialog, SIGNAL(finished(int)), this, SLOT(onDialogFinished(int)));
    }
    else
    {
        m_pDialog->setParent(pParent);
    }

    m_fOnColor = fOnColor;

    m_pDialog->setWindowTitle(sTitle);
    m_pDialog->setCurrentColor(tInitial);
    m_pDialog->show();
    m_pDialog->raise();
    m_pDialog->activateWindow();
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Converts a normalized engine color to a Qt color.
*/
/***************************************************************************/
QColor CColorSelector::vectorToColor(const Math::CVector3& vColor)
{
    return QColor(int(vColor.X * 255.0), int(vColor.Y * 255.0), int(vColor.Z * 255.0));
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Converts a Qt color to a normalized engine color.
*/
/***************************************************************************/
Math::CVector3 CColorSelector::colorToVector(const QColor& tColor)
{
    return Math::CVector3(double(tColor.red()) / 255.0, double(tColor.green()) / 255.0, double(tColor.blue()) / 255.0);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Forwards the live dialog color to the active caller.
*/
/***************************************************************************/
void CColorSelector::onCurrentColorChanged(const QColor& tColor)
{
    if (m_fOnColor != nullptr && tColor.isValid() == true)
    {
        m_fOnColor(tColor);
    }
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Forgets the active caller when the dialog closes.
*/
/***************************************************************************/
void CColorSelector::onDialogFinished(int iResult)
{
    Q_UNUSED(iResult);

    m_fOnColor = nullptr;
}
