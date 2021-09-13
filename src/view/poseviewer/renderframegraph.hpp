#ifndef RENDERFRAMEGRAPH_H
#define RENDERFRAMEGRAPH_H

#include <QObject>

#include <Qt3DRender/QViewport>

class RenderFramegraph : public Qt3DRender::QViewport {

    Q_OBJECT

public:
    RenderFramegraph();

};

#endif // RENDERFRAMEGRAPH_H
