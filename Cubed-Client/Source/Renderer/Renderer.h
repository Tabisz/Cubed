#pragma once

#include <filesystem>

#include "Vulkan.h"
namespace  Cubed
{
    class Renderer
    {
    public:
        void Init();
        void Shutdown();
        
        void Render();

    private:
        VkShaderModule LoadShader(const std::filesystem::path& path);
        void InitPipeline();
    private:
        VkPipeline m_GraphicsPipeline = nullptr;
        VkPipelineLayout m_PipelineLayout = nullptr;
    };
}
