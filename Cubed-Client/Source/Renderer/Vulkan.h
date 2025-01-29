#pragma once

#include "vulkan/vulkan.h"

#include "backends/imgui_impl_vulkan.h"
#include <string>
#include <iostream>

namespace vkb
{
    const std::string to_string(VkResult result);
}

namespace Cubed
{
    ImGui_ImplVulkan_InitInfo* GetVulkanInfo();
    
}

#define VK_CHECK(x)                                                                    \
do                                                                                 \
{                                                                                  \
    VkResult err = x;                                                              \
    if (err)                                                                       \
    {                                                                              \
        std::cout << "Vulkan error: " << vkb::to_string(err) << std::endl;        \
    }                                                                              \
} while (0)                                                                    