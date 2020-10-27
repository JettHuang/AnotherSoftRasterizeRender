// \brief
//		demo scene
//

#pragma once

#include <iostream>
#include <vector>
#include "SR_Headers.h"


// abstract base
class FDemoScene
{
public:
	virtual void Init() {}
	virtual void DrawScene(FSR_Context& ctx, const glm::mat4x4& InViewMat,float InDeltaSeconds) = 0;
};


// simple cubes
class FDemoScene_Cubes : public FDemoScene
{
public:
	virtual void Init() override;
	virtual void DrawScene(FSR_Context& ctx, const glm::mat4x4& InViewMat, float InDeltaSeconds) override;

protected:
	void InitializeSceneObjects(std::vector<glm::mat4>& objects);

	std::shared_ptr<FSR_VertexShader> _vs;
	std::shared_ptr<FSR_PixelShader> _ps;

	std::vector<glm::mat4> _objects;
};