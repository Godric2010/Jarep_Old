#include <iostream>
#include "core.hpp"

int main(int argc, char* argv[]) {

	const std::vector<Graphics::Vertex> vertices = {
			//back face
			{{0.5f,  -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.1f, 0.0f}},
			{{-0.5f, 0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f}, {0.1f, 0.1f}},
			{{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.1f}},
			//front face
			{{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {0.1f, 0.0f}},
			{{0.5f,  -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {0.2f, 0.0f}},
			{{0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 1.0f}, {0.2f, 0.1f}},
			{{-0.5f, 0.5f,  0.5f},  {1.0f, 1.0f, 1.0f}, {0.1f, 0.1f}},
			//top face
			{{0.5f, 0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f}, {0.2f, 0.0f}},
			{{-0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f}, {0.3f, 0.0f}},
			{{-0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 1.0f}, {0.3f, 0.1f}},
			{{0.5f, 0.5f,  0.5f},  {1.0f, 1.0f, 1.0f}, {0.2f, 0.1f}},
			//bottom face
			{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.3f, 0.0f}},
			{{0.5f,  -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.4f, 0.0f}},
			{{0.5f,  -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {0.4f, 0.1f}},
			{{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {0.3f, 0.1f}},
			//left face
			{{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {0.4f, 0.0f}},
			{{-0.5f, 0.5f,  0.5f},  {1.0f, 1.0f, 1.0f}, {0.5f, 0.0f}},
			{{-0.5f, 0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f}, {0.5f, 0.1f}},
			{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.4f, 0.1f}},
			//right face
			{{0.5f,  -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.5f, 0.0f}},
			{{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f}, {0.6f, 0.0f}},
			{{0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 1.0f}, {0.6f, 0.1f}},
			{{0.5f,  -0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}, {0.5f, 0.1f}}

	};

	const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0,// back
			4, 5, 6, 6, 7, 4,// front
			8, 9, 10, 10, 11, 8,// top
			12, 13, 14, 14, 15, 12,// bottom
			16, 17, 18, 18, 19, 16,// left
			20, 21, 22, 22, 23, 20// right
	};

	Mesh meshA = Mesh(vertices, indices);

	auto core = Core::CoreManager();
	core.Initialize();
	core.getRenderer()->AddMesh(meshA);
	core.Run();

	core.Shutdown();
	return 0;
}
