
// Qt
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

// Application
#include "CColorEdit.h"

//-------------------------------------------------------------------------------------------------

const int COLOR_COMPONENT_MINIMUM = 0;
const int COLOR_COMPONENT_MAXIMUM = 255;
const int COLOR_PREVIEW_WIDTH = 40;
const int COLOR_PREVIEW_HEIGHT = 20;

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Builds an inline color editor with preview and RGB components.
*/
/***************************************************************************/
CColorEdit::CColorEdit(QWidget* pParent)
    : QWidget(pParent)
    , m_tColor(Qt::black)
    , m_pPreview(nullptr)
    , m_pRed(nullptr)
    , m_pGreen(nullptr)
    , m_pBlue(nullptr)
{
    QHBoxLayout* pLayout = new QHBoxLayout(this);
    pLayout->setContentsMargins(0, 0, 0, 0);

    m_pPreview = new QLabel(this);
    m_pPreview->setFixedSize(COLOR_PREVIEW_WIDTH, COLOR_PREVIEW_HEIGHT);
    m_pPreview->setAutoFillBackground(true);
    pLayout->addWidget(m_pPreview);

    m_pRed = new QSpinBox(this);
    m_pRed->setRange(COLOR_COMPONENT_MINIMUM, COLOR_COMPONENT_MAXIMUM);
    m_pRed->setPrefix("R ");
    pLayout->addWidget(m_pRed);

    m_pGreen = new QSpinBox(this);
    m_pGreen->setRange(COLOR_COMPONENT_MINIMUM, COLOR_COMPONENT_MAXIMUM);
    m_pGreen->setPrefix("G ");
    pLayout->addWidget(m_pGreen);

    m_pBlue = new QSpinBox(this);
    m_pBlue->setRange(COLOR_COMPONENT_MINIMUM, COLOR_COMPONENT_MAXIMUM);
    m_pBlue->setPrefix("B ");
    pLayout->addWidget(m_pBlue);

    connect(m_pRed, SIGNAL(valueChanged(int)), this, SLOT(onComponentChanged()));
    connect(m_pGreen, SIGNAL(valueChanged(int)), this, SLOT(onComponentChanged()));
    connect(m_pBlue, SIGNAL(valueChanged(int)), this, SLOT(onComponentChanged()));

    setColor(m_tColor);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Releases the inline color editor.
*/
/***************************************************************************/
CColorEdit::~CColorEdit()
{
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Sets the edited color and refreshes the preview and components.
*/
/***************************************************************************/
void CColorEdit::setColor(const QColor& tColor)
{
    if (tColor.isValid() == false)
    {
        return;
    }

    m_tColor = tColor;

    m_pRed->blockSignals(true);
    m_pGreen->blockSignals(true);
    m_pBlue->blockSignals(true);

    m_pRed->setValue(m_tColor.red());
    m_pGreen->setValue(m_tColor.green());
    m_pBlue->setValue(m_tColor.blue());

    m_pRed->blockSignals(false);
    m_pGreen->blockSignals(false);
    m_pBlue->blockSignals(false);

    QPalette tPalette = m_pPreview->palette();
    tPalette.setColor(QPalette::Window, m_tColor);
    m_pPreview->setPalette(tPalette);
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Gets the edited color.
*/
/***************************************************************************/
QColor CColorEdit::getColor() const
{
    return m_tColor;
}

//-------------------------------------------------------------------------------------------------

/***************************************************************************/
/*!
    Rebuilds the color from the RGB components and notifies listeners.
*/
/***************************************************************************/
void CColorEdit::onComponentChanged()
{
    QColor tColor(m_pRed->value(), m_pGreen->value(), m_pBlue->value());

    if (tColor != m_tColor)
    {
        setColor(tColor);

        emit colorChanged(m_tColor);
    }
}
