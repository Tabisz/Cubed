#include "Renderer.h"

#include <array>
#include <fstream>
#include <glm/vec3.hpp>

#include "Walnut/Application.h"

namespace Cubed
{

	static uint32_t ImGui_ImplVulkan_MemoryType(VkMemoryPropertyFlags properties, uint32_t type_bits)
	{
		VkPhysicalDevice physicalDevice = Cubed::GetVulkanInfo()->PhysicalDevice;
		VkPhysicalDeviceMemoryProperties prop;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &prop);
		for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
			if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
				return i;
		return 0xFFFFFFFF; // Unable to find memoryType
	}
	
    void Renderer::Init()
    {
    	InitPipeline();
		InitBuffers();
    }

    void Renderer::Shutdown()
    {
    	
    }

	void Renderer::Render()
    {
    	VkCommandBuffer commandBuffer = Walnut::Application::GetActiveCommandBuffer();
    	auto wd =  Walnut::Application::GetMainWindowData();
    	// Bind the graphics pipeline.
    	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

    	VkViewport vp{};
    	vp.y = static_cast<float>(wd->Height);
    	vp.width    = static_cast<float>(wd->Width);
    	vp.height   = -static_cast<float>(wd->Height);
    	vp.minDepth = 0.0f;
    	vp.maxDepth = 1.0f;
    	// Set viewport dynamically
    	vkCmdSetViewport(commandBuffer, 0, 1, &vp);

    	VkRect2D scissor{};
    	scissor.extent.width  = static_cast<uint32_t>(wd->Width);
    	scissor.extent.height = static_cast<uint32_t>(wd->Height);
    	// Set scissor dynamically
    	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    	// Draw three vertices with one instance.
    	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    }
1:31:00
	
    void Renderer::InitPipeline()
    {
    	VkDevice device = GetVulkanInfo()->Device;
		VkRenderPass renderPass = Walnut::Application::GetMainWindowData()->RenderPass;
    	
        	// Create a blank pipeline layout.
	// We are not binding any resources to the pipeline in this first sample.
	VkPipelineLayoutCreateInfo layout_info{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
	VK_CHECK(vkCreatePipelineLayout(device, &layout_info, nullptr, &m_PipelineLayout));

	VkPipelineVertexInputStateCreateInfo vertex_input{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

	// Specify we will use triangle lists to draw geometry.
	VkPipelineInputAssemblyStateCreateInfo input_assembly{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	// Specify rasterization state.
	VkPipelineRasterizationStateCreateInfo raster{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
	raster.cullMode  = VK_CULL_MODE_BACK_BIT;
	raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
	raster.lineWidth = 1.0f;

	// Our attachment will write to all color channels, but no blending is enabled.
	VkPipelineColorBlendAttachmentState blend_attachment{};
	blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo blend{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
	blend.attachmentCount = 1;
	blend.pAttachments    = &blend_attachment;

	// We will have one viewport and scissor box.
	VkPipelineViewportStateCreateInfo viewport{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
	viewport.viewportCount = 1;
	viewport.scissorCount  = 1;

	// Disable all depth testing.
	VkPipelineDepthStencilStateCreateInfo depth_stencil{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};

	// No multisampling.
	VkPipelineMultisampleStateCreateInfo multisample{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Specify that these states will be dynamic, i.e. not part of pipeline state object.
	std::array<VkDynamicState, 2> dynamics{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

	VkPipelineDynamicStateCreateInfo dynamic{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
	dynamic.pDynamicStates    = dynamics.data();
	dynamic.dynamicStateCount =(uint32_t)dynamics.size();

	// Load our SPIR-V shaders.
	std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};
	//
	// Vertex stage of the pipeline
	shader_stages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
	shader_stages[0].module = LoadShader("Assets/Shaders/bin/basic.vert.spirv");
	shader_stages[0].pName  = "main";
	
	// Fragment stage of the pipeline
	shader_stages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	shader_stages[1].module = LoadShader("Assets/Shaders/bin/basic.frag.spirv");
	shader_stages[1].pName  = "main";

	VkGraphicsPipelineCreateInfo pipe{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
	pipe.stageCount          = (uint32_t)shader_stages.size();
	pipe.pStages             = shader_stages.data();
	pipe.pVertexInputState   = &vertex_input;
	pipe.pInputAssemblyState = &input_assembly;
	pipe.pRasterizationState = &raster;
	pipe.pColorBlendState    = &blend;
	pipe.pMultisampleState   = &multisample;
	pipe.pViewportState      = &viewport;
	pipe.pDepthStencilState  = &depth_stencil;
	pipe.pDynamicState       = &dynamic;

	// We need to specify the pipeline layout and the render pass description up front as well.
	pipe.renderPass = renderPass;
	pipe.layout     = m_PipelineLayout;
    	
	VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipe, nullptr, &m_GraphicsPipeline));

	// Pipeline is baked, we can delete the shader modules now.
	vkDestroyShaderModule(device, shader_stages[0].module, nullptr);
	vkDestroyShaderModule(device, shader_stages[1].module, nullptr);
    }

	void Renderer::InitBuffers()
	{
		VkDevice device = GetVulkanInfo()->Device;
		
		glm::vec3 vertexData[3] ={
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3( 0.0f,  0.5f, 0.0f),
		glm::vec3( 0.5f, -0.5f, 0.0f)
		};
		
		uint32_t indices[3] = {0,1,2};
		
		m_VertexBuffer.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		CreateOrResizeBuffer(m_VertexBuffer,sizeof(vertexData));
		
		m_IndexBuffer.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		CreateOrResizeBuffer(m_IndexBuffer,sizeof(indices));

		glm::vec3* vbMemory;
		VK_CHECK(vkMapMemory(device,m_VertexBuffer.Memory, 0,sizeof(vertexData),0,(void**)&vbMemory));
		memcpy(vbMemory,vertexData,sizeof(vertexData));
		
		uint32_t* ibMemory;
		VK_CHECK(vkMapMemory(device,m_IndexBuffer.Memory, 0,sizeof(indices),0,(void**)&ibMemory));
		memcpy(ibMemory,indices,sizeof(indices));
		
		VkMappedMemoryRange range[2] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = m_VertexBuffer.Memory;
		range[0].size = VK_WHOLE_SIZE;
		range[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[1].memory = m_IndexBuffer.Memory;
		range[1].size = VK_WHOLE_SIZE;

		VK_CHECK(vkFlushMappedMemoryRanges(device,2,range));
		vkUnmapMemory(device,m_VertexBuffer.Memory);
		vkUnmapMemory(device,m_IndexBuffer.Memory);
		
	}

    VkShaderModule Renderer::LoadShader(const std::filesystem::path& path)
    {
    	std::ifstream stream(path, std::ios::binary);

    	if(!stream)
    		return nullptr;

    	stream.seekg(0,std::ios_base::end);
    	std::streampos size = stream.tellg();
    	stream.seekg(0,std::ios_base::beg);

    	std::vector<char>  buffer(size);
    	if(!stream.read(buffer.data(),size))
    		return nullptr;

    	stream.close();
    	
    	VkShaderModuleCreateInfo shaderModuleCI{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    	shaderModuleCI.pCode = (uint32_t*)buffer.data();
    	shaderModuleCI.codeSize = buffer.size();
    	VkDevice device = GetVulkanInfo()->Device;
    	VkShaderModule result = nullptr;
    	
    	VK_CHECK(vkCreateShaderModule(device, &shaderModuleCI,nullptr, &result));
    	return result;
    }

	void Renderer::CreateOrResizeBuffer(Buffer& buffer, size_t newSize)
    {
    	VkDevice device = GetVulkanInfo()->Device;
    	
    	if (buffer.Handle != VK_NULL_HANDLE)
    		vkDestroyBuffer(device, buffer.Handle, nullptr);
    	if (buffer.Handle != VK_NULL_HANDLE)
    		vkFreeMemory(device, buffer.Memory, nullptr);

    	VkBufferCreateInfo bufferCI = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    	bufferCI.size = newSize;
    	bufferCI.usage = buffer.usage;
    	bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    	VK_CHECK(vkCreateBuffer(device, &bufferCI, nullptr, &buffer.Handle));

    	VkMemoryRequirements req;
    	vkGetBufferMemoryRequirements(device, buffer.Handle, &req);
    	//bd->BufferMemoryAlignment = (bd->BufferMemoryAlignment > req.alignment) ? bd->BufferMemoryAlignment : req.alignment;
    	VkMemoryAllocateInfo alloc_info = {};
    	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    	alloc_info.allocationSize = req.size;
    	alloc_info.memoryTypeIndex = ImGui_ImplVulkan_MemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
    	VK_CHECK(vkAllocateMemory(device, &alloc_info,nullptr, &buffer.Memory));

    	VK_CHECK(vkBindBufferMemory(device, buffer.Handle, buffer.Memory, 0));
    	buffer.Size = req.size;
    }

}
