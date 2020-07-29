// \brief
//		main entry.
//

#include <iostream>
#include <vector>
#include <gtc/matrix_transform.hpp>
#include "SR_Headers.h"


#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

// output ppm
void OuputPPM(const std::shared_ptr<FSR_Buffer2D>& InBuffer2D)
{
	int32_t image_width, image_height;

	if (!InBuffer2D || !InBuffer2D->IsValid())
	{
		std::cerr << "OutputPPM failed. buffer2d is null" << std::endl;
		return;
	}

	image_width = InBuffer2D->Width();
	image_height = InBuffer2D->Height();
	std::cerr << "photo size: " << image_width << ", " << image_height << std::endl;
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	for (int32_t j = image_height - 1; j >= 0; --j)
	{
		for (int32_t i = 0; i < image_width; ++i)
		{
			float R, G, B, A;
			InBuffer2D->Read(i, j, R, G, B, A);
			
			// Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
			if (R != R) R = 0.0f;
			if (G != G) G = 0.0f;
			if (B != B) B = 0.0f;

			// Write the translated [0,255] value of each color component.
			std::cout << static_cast<int>(256 * glm::clamp(R, 0.0f, 0.999f)) << ' '
					  << static_cast<int>(256 * glm::clamp(G, 0.0f, 0.999f)) << ' '
					  << static_cast<int>(256 * glm::clamp(B, 0.0f, 0.999f)) << '\n';
		}
	} // end j
}

// test simple triangle
void Example_SingleTriangle()
{
	FSR_Context ctx;
	std::shared_ptr<FSR_VertexShader> vs = std::make_shared<FSR_SimpleVertexShader>();
	std::shared_ptr<FSR_PixelShader> ps = std::make_shared<FSR_SimplePixelShader>();

	ctx.SetRenderTarget(600, 600, 1);
	ctx.SetViewport(0, 0, 600, 600);
	ctx.SetCullFaceMode(EFrontFace::FACE_CW);
	ctx.SetShader(vs, ps);
	ctx.ClearRenderTarget(glm::vec4(0, 0, 0, 0));

	FSRVertex v0, v1, v2;

	v0._vertex = glm::vec3(-0.5, -0.5, 1.0);
	v0._attributes._members[0] = glm::vec3(1.0, 0.0, 0.0);
	v0._attributes._count = 1;

	v1._vertex = glm::vec3(-0.5, 0.5, 1.0);
	v1._attributes._members[0] = glm::vec3(0.0, 1.0, 0.0);
	v1._attributes._count = 1;

	v2._vertex = glm::vec3(0.5, 0.5, 1.0);
	v2._attributes._members[0] = glm::vec3(0.0, 0.0, 1.0);
	v2._attributes._count = 1;

	FSR_Renderer::DrawTriangle(ctx, v0, v1, v2);

	OuputPPM(ctx.GetColorBuffer(0));
}

// test multi-cubes
// https://github.com/NotCamelCase/RasterizationInOneWeekend/blob/master/Go3D.h
void InitializeSceneObjects(std::vector<glm::mat4>& objects)
{
	// Construct a scene of few cubes randomly positioned

	const glm::mat4 identity(1.f);

	glm::mat4 M0 = glm::translate(identity, glm::vec3(0, 0, 2.f));
	M0 = glm::rotate(M0, glm::radians(45.f), glm::vec3(0, 1, 0));

	glm::mat4 M1 = glm::translate(identity, glm::vec3(-3.75f, 0, 0));
	M1 = glm::rotate(M1, glm::radians(30.f), glm::vec3(1, 0, 0));

	glm::mat4 M2 = glm::translate(identity, glm::vec3(3.75f, 0, 0));
	M2 = glm::rotate(M2, glm::radians(60.f), glm::vec3(0, 1, 0));

	glm::mat4 M3 = glm::translate(identity, glm::vec3(0, 0, -2.f));
	M3 = glm::rotate(M3, glm::radians(90.f), glm::vec3(0, 0, 1));

	// Change the order of cubes being rendered, see how it changes with and without depth test
	objects.push_back(M0);
	objects.push_back(M1);
	objects.push_back(M2);
	objects.push_back(M3);
}

void Example_Multi_Cubes()
{
	const uint32_t kWidth = 1280u;
	const uint32_t kHeight = 720u;

	FSR_Context ctx;
	std::shared_ptr<FSR_VertexShader> vs = std::make_shared<FSR_SimpleVertexShader>();
	std::shared_ptr<FSR_PixelShader> ps = std::make_shared<FSR_SimplePixelShader>();

	ctx.SetRenderTarget(kWidth, kHeight, 1);
	ctx.SetViewport(0, 0, kWidth, kHeight);
	ctx.SetCullFaceMode(EFrontFace::FACE_CCW);
	ctx.SetShader(vs, ps);
	ctx.ClearRenderTarget(glm::vec4(0, 0, 0, 0));
	// setup camera
	// Build view & projection matrices (right-handed sysem)
	const float nearPlane = 0.1f;
	const float farPlane = 100.f;
	const glm::vec3 eye(0, 3.75, 6.5);
	const glm::vec3 lookat(0, 0, 0);
	const glm::vec3 up(0, 1, 0);

	const glm::mat4 view = glm::lookAt(eye, lookat, up);
	const glm::mat4 proj = glm::perspective(glm::radians(60.f), static_cast<float>(kWidth) / static_cast<float>(kHeight), nearPlane, farPlane);
	ctx.SetProjectionMatrix(proj);

	// objects
	glm::vec3 vertices[] =
	{
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{  1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, -1.0f },
	};
	// Use per-face colors
	glm::vec3 colors[] =
	{
		glm::vec3(0, 0, 1),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 0, 1),
		glm::vec3(1, 1, 0)
	};
	uint32_t indices[] =
	{
		// 6 faces of cube * 2 triangles per-face * 3 vertices per-triangle = 36 indices
		1,3,0,
		7,5,4,
		4,1,0,
		5,2,1,
		2,7,3,
		0,7,4,
		1,2,3,
		7,6,5,
		4,5,1,
		5,6,2,
		2,6,7, 
		0,3,7
	};

	// Let's draw multiple objects
	std::vector<glm::mat4> objects;
	InitializeSceneObjects(objects);

	// Loop over objects in the scene
	FSRVertex v0, v1, v2;
	v0._attributes._count = 1;
	v1._attributes._count = 1;
	v2._attributes._count = 1;
	for (size_t n = 0; n < objects.size(); n++)
	{
		glm::mat4x4 modelview = view * objects[n];
		ctx.SetModelViewMatrix(modelview);

		// Loop over triangles in a given object and rasterize them one by one
		for (uint32_t idx = 0; idx < ARR_SIZE(indices) / 3; idx++)
		{
			const glm::vec3& color = colors[indices[idx * 3] % 6];

			v0._vertex = vertices[indices[idx * 3]];
			v0._attributes._members[0] = color;
			v1._vertex = vertices[indices[idx * 3 + 1]];
			v1._attributes._members[0] = color;
			v2._vertex = vertices[indices[idx * 3 + 2]];
			v2._attributes._members[0] = color;

			FSR_Renderer::DrawTriangle(ctx, v0, v1, v2);
		} // end for idx
	} // end for n

	OuputPPM(ctx.GetColorBuffer(0));
}

int main()
{
	//Example_SingleTriangle();
	Example_Multi_Cubes();
}