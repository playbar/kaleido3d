//
//  CommandContext.hpp
//  RHI_Metal
//
//  Created by QinZhou on 15/9/2.
//
//
#pragma once
#ifndef __CommandContext_h__
#define __CommandContext_h__

#include "../Common.h"
#include <Interface/IRHI.h>

NS_K3D_METAL_BEGIN

class DeviceAdapter : public rhi::IDeviceAdapter
{
    friend class Device;
public:
    DeviceAdapter(id<MTLDevice> device)
    : m_Device(device), m_pRHIDevice(nullptr) {}
    
    id<MTLDevice> GetDevice() const { return m_Device; }
    
    rhi::DeviceRef GetDevice() override;
private:
    
    rhi::DeviceRef  m_pRHIDevice;
    id<MTLDevice>   m_Device;
};

class Device : public rhi::IDevice
{
public:
    id<MTLDevice> GetDevice();
    static void ReleaseObj(id object);
    
    Device();
    ~Device() override;
    
    Result                      Create(rhi::IDeviceAdapter *, bool withDebug) override;
    rhi::CommandContextRef      NewCommandContext(rhi::ECommandType) override;
    rhi::GpuResourceRef          NewGpuResource(rhi::ResourceDesc const&)override;
    rhi::ShaderResourceViewRef	NewShaderResourceView(rhi::GpuResourceRef, rhi::ResourceViewDesc const&)override;
    rhi::SamplerRef              NewSampler(const rhi::SamplerState&)override;
    rhi::PipelineStateObjectRef	NewPipelineState(rhi::PipelineDesc const&,rhi::PipelineLayoutRef,rhi::EPipelineType)override;
    rhi::PipelineLayoutRef		NewPipelineLayout(rhi::PipelineLayoutDesc const & table) override;
    rhi::SyncFenceRef            NewFence()override;
    rhi::IDescriptorPool*		NewDescriptorPool()override;
    rhi::RenderViewportRef		NewRenderViewport(void * winHandle, rhi::GfxSetting &)override;
    rhi::RenderTargetRef		NewRenderTarget(rhi::RenderTargetLayout const&)override;

    void Destroy();
    
private:
    Device(id<MTLDevice> device);
    friend class DeviceAdapter;
    
    id <MTLDevice>          m_Device;
    id <MTLCommandQueue>    m_CommandQueue;
    MTLFeatureSet           m_FeatureSet;
};

//// Create RHI MetalDevice /////
inline rhi::DeviceRef DeviceAdapter::GetDevice()
{
    if(!m_pRHIDevice)
    {
        auto pDevice = new Device(m_Device);
        pDevice->Create(this, false);
        m_pRHIDevice = rhi::DeviceRef(pDevice);
    }
    return m_pRHIDevice;
}
//////////////////////////////////

class PipelineState : public rhi::IPipelineStateObject
{
    friend class Device;
    friend class CommandContext;
public:
    PipelineState(id<MTLDevice> pDevice, rhi::PipelineDesc const & desc, rhi::EPipelineType const& type);
    ~PipelineState();

    rhi::EPipelineType GetType () override { return m_Type; }
    
    void SetLayout(rhi::PipelineLayoutRef) override;
    void SetShader(rhi::EShaderType, rhi::ShaderBundle const&) override;
    void SetRasterizerState(const rhi::RasterizerState&) override;
    void SetBlendState(const rhi::BlendState&) override;
    void SetDepthStencilState(const rhi::DepthStencilState&) override;
    void SetPrimitiveTopology(const rhi::EPrimitiveType) override;
    void SetVertexInputLayout(rhi::VertexDeclaration const*, uint32 Count) override;
    void SetRenderTargetFormat(const rhi::RenderTargetFormat &) override;
    void SetSampler(rhi::SamplerRef)override;

    void Finalize() override;
    
private:
    void InitPSO(rhi::PipelineDesc const & desc);
    void AssignShader(rhi::ShaderBundle const& shaderBundle);
    
    rhi::EPipelineType              m_Type;
    id<MTLDevice>                   m_Device;
    
    id<MTLComputePipelineState>     m_ComputePipelineState;
    MTLComputePipelineDescriptor*   m_ComputePipelineDesc;
    
    id<MTLRenderPipelineState>      m_RenderPipelineState;
    MTLRenderPipelineDescriptor*    m_RenderPipelineDesc;
    
    MTLDepthStencilDescriptor*      m_DepthStencilDesc;
    MTLCullMode                     m_CullMode;
    float                           m_DepthBias;
    float                           m_DepthBiasClamp;
    
    MTLPrimitiveType                m_PrimitiveType;
    
    MTLRenderPipelineReflection*    m_RenderReflection;
    MTLComputePipelineReflection*   m_ComputeReflection;
    
    id<MTLDepthStencilState>        m_DepthStencilState;
};

class DescriptorSet : public rhi::IDescriptor
{
public:
    DescriptorSet()
    {}
    
    void Update(uint32 bindSet, rhi::GpuResourceRef res) override;
    
private:
    
};

class PipelineLayout : public rhi::IPipelineLayout
{
public:
    PipelineLayout(rhi::PipelineLayoutDesc const&);
    ~PipelineLayout();
    
    rhi::DescriptorRef GetDescriptorSet() const override;
    
private:
    
};

class CommandContext : public rhi::ICommandContext
{
public:
    CommandContext(rhi::ECommandType const & cmdType, id<MTLCommandBuffer> cmdBuf);
    
    ~CommandContext() override;
    
    void Detach(rhi::IDevice *) override;
    
    void CopyTexture(const rhi::TextureCopyLocation& Dest, const rhi::TextureCopyLocation& Src) override;
    void CopyBuffer(rhi::IGpuResource& Dest, rhi::IGpuResource& Src, rhi::CopyBufferRegion const & Region) override;
    void TransitionResourceBarrier(rhi::GpuResourceRef resource, /*EPipelineStage stage,*/ rhi::EResourceState dstState) override {}
    void Execute(bool Wait) override;
    void Reset() override;
    
    void Begin() override;
    
    void ClearColorBuffer(rhi::GpuResourceRef, kMath::Vec4f const&) override {}
    void ClearDepthBuffer(rhi::IDepthBuffer*) override {}
    
    void BeginRendering() override;
    void SetRenderTargets(uint32 NumColorBuffer, rhi::IColorBuffer*, rhi::IDepthBuffer*, bool ReadOnlyDepth = false) override {}
    void SetRenderTarget(rhi::RenderTargetRef) override;
    void SetPipelineState(uint32 HashCode, rhi::PipelineStateObjectRef) override;
    void SetPipelineLayout(rhi::PipelineLayoutRef) override {}
    void SetScissorRects(uint32, const rhi::Rect*) override {}
    void SetViewport(const rhi::ViewportDesc &) override;
    void SetPrimitiveType(rhi::EPrimitiveType) override;
    void SetIndexBuffer(const rhi::IndexBufferView& IBView) override;
    void SetVertexBuffer(uint32 Slot, const rhi::VertexBufferView& VBView) override;
    void DrawInstanced(rhi::DrawInstancedParam) override;
    void DrawIndexedInstanced(rhi::DrawIndexedInstancedParam) override;
    void EndRendering() override;
    void PresentInViewport(rhi::RenderViewportRef rvp) override;
    
    void Dispatch(uint32 X = 1, uint32 Y =1, uint32 Z = 1) override;
    
    void ExecuteBundle(rhi::ICommandContext*) override {}
    
    void End() override;
    
protected:
    
private:
    MTLRenderPassDescriptor*        m_RenderpassDesc;
    rhi::ECommandType               m_CommandType;
    MTLPrimitiveType                m_CurPrimType;
    id<MTLComputeCommandEncoder>    m_ComputeEncoder;
    id<MTLRenderCommandEncoder>     m_RenderEncoder;
    id <MTLParallelRenderCommandEncoder> m_ParallelRenderEncoder;
    id<MTLCommandBuffer>            m_CmdBuffer;
    
    id<MTLBuffer>                   m_TmpIndexBuffer;
};

class RenderViewport : public rhi::IRenderViewport
{
public:
    RenderViewport(CAMetalLayer * mtlLayer = nil);
    ~RenderViewport() override;
    
    bool                    InitViewport(void *windowHandle, rhi::IDevice * pDevice, rhi::GfxSetting &) override;
    void                    PrepareNextFrame() override;
    bool                    Present(bool vSync) override { return false; }
    
    rhi::RenderTargetRef    GetRenderTarget(uint32 index) override { return nullptr; }
    
    rhi::RenderTargetRef    GetCurrentBackRenderTarget() override;
    uint32                  GetSwapChainCount()override { return 0; }
    uint32                  GetSwapChainIndex()override { return 0; }
    
    uint32                  GetWidth() const override { return m_Width; }
    uint32                  GetHeight() const override { return m_Height; }
    
    friend class            Device;
    friend class            CommandContext;
private:
    
    CAMetalLayer *          m_Layer;
    id<CAMetalDrawable>     m_CurrentDrawable;
    MTLRenderPassDescriptor*m_RenderPassDescriptor;
    id<MTLTexture>          m_DepthTex;
    uint32                  m_Width;
    uint32                  m_Height;
};

class RenderTarget : public rhi::IRenderTarget
{
public:
    RenderTarget() {}
    RenderTarget(MTLRenderPassDescriptor* rpd, id<MTLTexture> color);
    ~RenderTarget() override;
    
    void                SetClearColor(kMath::Vec4f clrColor) override;
    void                SetClearDepthStencil(float depth, uint32 stencil) override;
    rhi::GpuResourceRef	GetBackBuffer() override;
    
    friend class        CommandContext;
private:
    MTLRenderPassDescriptor*m_RenderPassDescriptor = nil;
    id<MTLTexture>          m_ColorTexture;
};

class Sampler : public rhi::ISampler
{
public:
    explicit Sampler(rhi::SamplerState const & state);
    ~Sampler() override;
    
    rhi::SamplerState       GetSamplerDesc() const override;
    
private:
    id <MTLSamplerState>    m_SampleState;
};

NS_K3D_METAL_END

#endif /* CommandContext_hpp */
