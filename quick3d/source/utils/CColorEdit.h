
#pragma once

// Qt
#include <QColor>
#include <QWidget>

// Application
#include "quick3d_global.h"

class QLabel;
class QSpinBox;

class QUICK3D_EXPORT CColorEdit : public QWidget
{
    Q_OBJECT

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    explicit CColorEdit(QWidget* pParent = nullptr);

    //!
    virtual ~CColorEdit() Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //! Sets the edited color and refreshes the preview and components
    void setColor(const QColor& tColor);

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //! Gets the edited color
    QColor getColor() const;

//-------------------------------------------------------------------------------------------------
// Signals
//-------------------------------------------------------------------------------------------------

signals:

    //!
    void colorChanged(const QColor& tColor);

    //-------------------------------------------------------------------------------------------------
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected slots:

    //!
    void onComponentChanged();

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    QColor      m_tColor;
    QLabel*     m_pPreview;
    QSpinBox*   m_pRed;
    QSpinBox*   m_pGreen;
    QSpinBox*   m_pBlue;
};
