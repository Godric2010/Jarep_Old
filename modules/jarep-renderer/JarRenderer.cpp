//
// Created by Sebastian Borsch on 24.10.23.
//

#include "JarRenderer.hpp"

namespace Graphics {
	JarRenderer::JarRenderer(const std::vector<const char*>& extensionNames) {
		extensions = extensionNames;
#if defined(__APPLE__) && defined(__MACH__)
		backend = std::make_shared<Metal::MetalBackend>(Metal::MetalBackend());
		shaderFileType = ".metal";
		std::cout << "Using metal renderer!" << std::endl;
#else
		backend = std::make_shared<Vulkan::VulkanBackend>(Vulkan::VulkanBackend(extensionNames));
		shaderFileType = ".spv";
		std::cout << "Using vulkan renderer!" << std::endl;
#endif
	}

	void JarRenderer::Initialize(NativeWindowHandleProvider* nativeWindowHandle) {
		surface = backend->CreateSurface(nativeWindowHandle);
		device = backend->CreateDevice(surface);

		const auto commandQueueBuilder = backend->InitCommandQueueBuilder();
		queue = commandQueueBuilder->Build(device);

		Internal::JarModelViewProjection mvp{};
//		mvp.model = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		auto bufferBuilder = backend->InitBufferBuilder()->SetUsageFlags(
				BufferUsage::UniformBuffer)->SetMemoryProperties(
				MemoryProperties::HostVisible | MemoryProperties::HostCoherent)->SetBufferData(
				&mvp, sizeof(Internal::JarModelViewProjection));
		for (int i = 0; i < surface->GetSwapchainImageAmount(); ++i) {
			uniformBuffers.push_back(bufferBuilder->Build(device));
		}

		descriptors = std::vector<std::shared_ptr<JarDescriptor>>();
		auto uboDescriptor = backend->InitDescriptorBuilder()->SetBinding(1)->SetStageFlags(
				StageFlags::VertexShader)->BuildUniformBufferDescriptor(device, uniformBuffers);
		descriptors.push_back(uboDescriptor);

		auto image = backend->InitImageBuilder()->EnableMipMaps(true)->SetPixelFormat(
				PixelFormat::BGRA8_UNORM)->SetImagePath(
				"../../resources/uv_texture.jpg")->Build(device);
		images.push_back(image);
		auto imageDescriptor = backend->InitDescriptorBuilder()->SetBinding(2)->SetStageFlags(
				StageFlags::FragmentShader)->BuildImageBufferDescriptor(device, image);
		descriptors.push_back(imageDescriptor);

		vertexShaderModule = createShaderModule(VertexShader, "triangle_vert");
		fragmentShaderModule = createShaderModule(FragmentShader, "triangle_frag");

		ColorAttachment colorAttachment;
		colorAttachment.loadOp = LoadOp::Clear;
		colorAttachment.storeOp = StoreOp::Store;
		colorAttachment.clearColor = ClearColor(0, 0, 0, 0);
		colorAttachment.imageFormat = B8G8R8A8_UNORM;

		StencilAttachment stencilAttachment = {};
		stencilAttachment.StencilLoadOp = LoadOp::DontCare;
		stencilAttachment.StencilStoreOp = StoreOp::DontCare;
		stencilAttachment.StencilClearValue = 0;

		DepthAttachment depthStencilAttachment;
		depthStencilAttachment.Format = ImageFormat::D32_SFLOAT;
		depthStencilAttachment.DepthLoadOp = LoadOp::Clear,
		depthStencilAttachment.DepthStoreOp = StoreOp::DontCare,
		depthStencilAttachment.DepthClearValue = 1.0f;
		depthStencilAttachment.Stencil = std::make_optional(stencilAttachment);


		JarRenderPassBuilder* rpBuilder = backend->InitRenderPassBuilder();
		rpBuilder->AddColorAttachment(colorAttachment);
		rpBuilder->AddDepthStencilAttachment(depthStencilAttachment);
		renderPass = rpBuilder->Build(device, surface);
		delete rpBuilder;

		ShaderStage shaderStage{};
		shaderStage.vertexShaderModule = vertexShaderModule;
		shaderStage.fragmentShaderModule = fragmentShaderModule;
		shaderStage.mainFunctionName = "main";

		VertexInput vertexInput{};
		vertexInput.attributeDescriptions = Vertex::GetAttributeDescriptions();
		vertexInput.bindingDescriptions = Vertex::GetBindingDescriptions();

		ColorBlendAttachment colorBlendAttachment{};
		colorBlendAttachment.pixelFormat = PixelFormat::BGRA8_UNORM;
		colorBlendAttachment.sourceRgbBlendFactor = BlendFactor::One;
		colorBlendAttachment.destinationRgbBlendFactor = BlendFactor::Zero;
		colorBlendAttachment.rgbBlendOperation = BlendOperation::Add;
		colorBlendAttachment.blendingEnabled = false;
		colorBlendAttachment.sourceAlphaBlendFactor = BlendFactor::One;
		colorBlendAttachment.destinationAlphaBlendFactor = BlendFactor::Zero;
		colorBlendAttachment.alphaBlendOperation = BlendOperation::Add;
		colorBlendAttachment.colorWriteMask = ColorWriteMask::All;

		DepthStencilState depthStencilState{};
		depthStencilState.depthTestEnable = true;
		depthStencilState.depthWriteEnable = true;
		depthStencilState.depthCompareOp = DepthCompareOperation::Less;
		depthStencilState.stencilTestEnable = false;
		depthStencilState.stencilOpState = {};

		std::vector<std::shared_ptr<JarDescriptorLayout>> descriptorLayouts = std::vector<std::shared_ptr<JarDescriptorLayout>>();
		for (const auto& descriptor: descriptors) {
			descriptorLayouts.push_back(descriptor->GetDescriptorLayout());
		}


		JarPipelineBuilder* pipelineBuilder = backend->InitPipelineBuilder();
		pipelineBuilder->
				SetShaderStage(shaderStage)->
				SetRenderPass(renderPass)->
				SetVertexInput(vertexInput)->
				SetInputAssemblyTopology(InputAssemblyTopology::TriangleList)->
				SetMultisamplingCount(4)->
				BindDescriptorLayouts(descriptorLayouts)->
//				BindUniformBuffers(uniformBuffers, 1, StageFlags::VertexShader)->
//				BindImageBuffer(images[0], 2, StageFlags::FragmentShader)->
				SetColorBlendAttachments(colorBlendAttachment)->
				SetDepthStencilState(depthStencilState);
		pipeline = pipelineBuilder->Build(device);
		delete pipelineBuilder;

	}

	void JarRenderer::Resize(uint32_t width, uint32_t height) {
		surface->RecreateSurface(width, height);
	}

	void JarRenderer::AddRenderStep(std::unique_ptr<JarRenderStepDescriptor> renderStepBuilder) {
		std::cout << "Render step is currently unavailable!" << std::endl;
//		auto renderStep = std::make_shared<Internal::JarRenderStep>(std::move(renderStepBuilder), backend, device,
//		                                                            surface);
//		renderSteps.push_back(renderStep);
	}

	void JarRenderer::AddMesh(Mesh& mesh) {

		const size_t vertexDataSize = mesh.getVertices().size() * sizeof(Vertex);

		const auto vertexBufferBuilder = backend->InitBufferBuilder();
		vertexBufferBuilder->SetBufferData(mesh.getVertices().data(), vertexDataSize);
		vertexBufferBuilder->SetMemoryProperties(MemoryProperties::DeviceLocal);
		vertexBufferBuilder->SetUsageFlags(BufferUsage::VertexBuffer);
		std::shared_ptr<JarBuffer> vertexBuffer = vertexBufferBuilder->Build(device);

		const size_t indexBufferSize = sizeof(mesh.getIndices()[0]) * mesh.getIndices().size();
		const auto indexBufferBuilder = backend->
				InitBufferBuilder()->
				SetBufferData(mesh.getIndices().data(), indexBufferSize)->
				SetMemoryProperties(MemoryProperties::DeviceLocal)->
				SetUsageFlags(BufferUsage::IndexBuffer);
		std::shared_ptr<JarBuffer> indexBuffer = indexBufferBuilder->Build(device);
		meshes.push_back(Internal::JarMesh(mesh, vertexBuffer, indexBuffer));
	}

	void JarRenderer::Render() {

		prepareModelViewProjectionForFrame();

		const auto commandBuffer = queue->getNextCommandBuffer();
		if (!commandBuffer->StartRecording(surface, renderPass))
			return;

		commandBuffer->BindPipeline(pipeline, frameCounter);
		commandBuffer->BindDescriptors(descriptors);


		for (auto& mesh: meshes) {
			commandBuffer->BindVertexBuffer(mesh.getVertexBuffer());
			commandBuffer->BindIndexBuffer(mesh.getIndexBuffer());
			commandBuffer->DrawIndexed(mesh.getIndexLength());
		}

		commandBuffer->EndRecording();
		commandBuffer->Present(surface, device);
		frameCounter = (frameCounter + 1) % surface->GetSwapchainImageAmount();
	}

	void JarRenderer::Shutdown() {

		surface->ReleaseSwapchain();

		for (auto& image: images) {
			image->Release();
		}

		for (auto& uniformBuffer: uniformBuffers) {
			uniformBuffer->Release();
		}

		for (auto& descriptor: descriptors) {
			descriptor->Release();
		}
		pipeline->Release();
		vertexShaderModule->Release();
		fragmentShaderModule->Release();

		for (auto& mesh: meshes) {
			mesh.Destroy();
		}

		queue->Release();
		device->Release();


		std::cout << "Shutdown renderer" << std::endl;
	}

	void JarRenderer::prepareModelViewProjectionForFrame() {
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		auto surfaceExtent = surface->GetSurfaceExtent();

		Internal::JarModelViewProjection mvp{};
		mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.projection = glm::perspectiveRH_NO(glm::radians(45.0f), surfaceExtent.Width / surfaceExtent.Height, 0.1f,
		                                       100.0f);

		uniformBuffers[frameCounter]->Update(&mvp, sizeof(Internal::JarModelViewProjection));
	}

	std::shared_ptr<JarShaderModule> JarRenderer::createShaderModule(const ShaderType shaderType,
	                                                                 const std::string& shaderName) const {
		const auto shaderDir = "shaders/";
		const std::string shaderFilePath = shaderDir + shaderName + shaderFileType;
		const std::string shaderCodeString = readFile(shaderFilePath);

		const auto shaderModuleBuilder = backend->InitShaderModuleBuilder();
		shaderModuleBuilder->SetShader(shaderCodeString);
		shaderModuleBuilder->SetShaderType(shaderType);
		auto shaderModule = shaderModuleBuilder->Build(device);
		delete shaderModuleBuilder;

		return shaderModule;
	}
}
