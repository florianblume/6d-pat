#include "outlinematerial.hpp"

OutlineMaterial::OutlineMaterial(Qt3DRender::QTexture2D *stencilTexture,
                                 const QColor &outlineColor,
                                 int outlineSize)
    : Qt3DRender::QMaterial()
    , m_outlineColor(outlineColor)
    , m_outlineSize(outlineSize) {

}

QColor OutlineMaterial::outlineColor() const {
    return m_outlineColor;
}

void OutlineMaterial::setOutlineColor(const QColor &outlineColor) {
    m_outlineColor = outlineColor;
}

int OutlineMaterial::outlineSize() const {
    return m_outlineSize;
}

void OutlineMaterial::setOutlineSize(int outlineSize) {
    m_outlineSize = outlineSize;
}
