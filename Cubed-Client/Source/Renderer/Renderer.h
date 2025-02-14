#pragma once

#include <filesystem>

#include "Vulkan.h"
namespace  Cubed
{
    struct Buffer
    {
        VkBuffer Handle;
        VkDeviceMemory Memory;
        VkDeviceSize Size = 0;
        VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    };
    
    class Renderer
    {
    public:
        void Init();
        void Shutdown();
        
        void Render();


    private:
        void InitPipeline();
        void InitBuffers();
        void CreateOrResizeBuffer(Buffer& buffer, uint64_t newSize);
        VkShaderModule LoadShader(const std::filesystem::path& path);
    private:
        VkPipeline m_GraphicsPipeline = nullptr;
        VkPipelineLayout m_PipelineLayout = nullptr;

        Buffer m_VertexBuffer, m_IndexBuffer;
    };
}
