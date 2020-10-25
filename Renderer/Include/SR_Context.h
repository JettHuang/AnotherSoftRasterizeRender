// \brief
//	renderer context.
//

#pragma once

#include "SR_Common.h"
#include "SR_Buffer2D.h"
#include "SR_Shader.h"
#include "SR_Material.h"
#include "SR_Performance.h"


#define MSAA_SAMPLES		4

#define MSAA_SAMPLE_0		0
#define MSAA_SAMPLE_1		1
#define MSAA_SAMPLE_2		2
#define MSAA_SAMPLE_3		3


// render context
class FSR_Context
{
public:
	FSR_Context();
	virtual ~FSR_Context();

	// set render target
	void SetRenderTarget(uint32_t w, uint32_t h, uint32_t nCount, bool InbEnableMSAA = false);
	// clear render target
	void ClearRenderTarget(const glm::vec4& InColor);
	// set cull face mode
	void SetCullFaceMode(EFrontFace InMode);
	
	// set viewport
	void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
	
	// set model-view matrix
	void SetModelViewMatrix(const glm::mat4x4& InModelView);
	// set projection matrix
	void SetProjectionMatrix(const glm::mat4x4& InProj);

	// set material
	void SetMaterial(const std::shared_ptr<FSR_Material>& InMaterial) { _material = InMaterial; }
	//set pipeline
	void SetShader(const std::shared_ptr<FSR_VertexShader>& InVs, const std::shared_ptr<FSR_PixelShader>& InPs) 
	{
		_vs = InVs;
		_ps = InPs;
	}

	// get frame-buffer
	void ResolveMSAABuffer();
	std::shared_ptr<FSR_Buffer2D> GetDepthBuffer() const;
	std::shared_ptr<FSR_Buffer2D> GetColorBuffer(uint32_t InIndex) const;

	// utilities for raster
	glm::vec3 NdcToScreenPostion(const glm::vec3& ndc) const;
	const FSR_Rectangle& ViewportRectangle() const { return _viewport_rect; }
	bool DepthTestAndOverride(uint32_t cx, uint32_t cy, float InDepth) const;
	bool DepthTestAndOverrideMSAA(uint32_t cx, uint32_t cy, float InDepth, int32_t InSampleIndex) const;
	void OutputAndMergeColor(int32_t cx, int32_t cy, FSRPixelShaderOutput& InPixelOutput) const;
	void OutputAndMergeColorMSAA(int32_t cx, int32_t cy, FSRPixelShaderOutput& InPixelOutput, int32_t InSamplesBitMask) const;
protected:
	void UpdateMVP();
public:
	FSR_Rectangle	_viewport_rect;
	
	glm::mat4x4 _modelview;
	glm::mat4x4 _modelview_inv;
	glm::mat3x3	_modelview_inv_t;
	glm::mat4x4 _projection;
	glm::mat4x4 _projection_inv;
	glm::mat4x4 _mvp;
	glm::mat4x4 _mvp_inv;

	EFrontFace	_front_face;
	
	std::shared_ptr<FSR_DepthBuffer>	_rt_depth;
	std::shared_ptr<FSR_Texture2D>		_rt_colors[MAX_MRT_COUNT];

	bool		_bEnableMSAA;
	int32_t		_MSAASamplesNum;
	std::shared_ptr<FSR_DepthBuffer>	_rt_depth_msaa;
	std::shared_ptr<FSR_Texture2D>		_rt_colors_msaa[MAX_MRT_COUNT];

	std::shared_ptr<FSR_Material>		_material;
	std::shared_ptr<FSR_VertexShader>	_vs;
	std::shared_ptr<FSR_PixelShader>	_ps;

	std::shared_ptr<FSR_Performance>	_stats;
};
