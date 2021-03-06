#ifndef RENDERERWIDGET_H
#define RENDERERWIDGET_H

#include <QWidget>
#include <Interface/IRHI.h>
#include <RHI/Vulkan/Public/IVkRHI.h>

using namespace rhi;

class RendererWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RendererWidget(QWidget *parent = 0);

    void init();
signals:

public slots:

private:
  k3d::SharedPtr<k3d::IVkRHI> RHI;
  rhi::DeviceRef Device;
  rhi::RenderViewportRef RenderVp;
};

#endif // RENDERERWIDGET_H
