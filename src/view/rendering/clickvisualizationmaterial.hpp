#ifndef CLICKVISUALIZATIONMATERIAL_H
#define CLICKVISUALIZATIONMATERIAL_H

#include <QMaterial>
#include <QObject>

class ClickVisualizationMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
public:
    ClickVisualizationMaterial();
};

#endif // CLICKVISUALIZATIONMATERIAL_H
