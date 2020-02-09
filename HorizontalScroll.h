#pragma once
#include "Camera.h"

class HorizontalScroll
{
public:
	HorizontalScroll();

	//Makes sure that the camera is following the focus
	void Update();

	//Getters
	Camera* GetCam() const;
	Transform* GetFocus() const;
	float GetOffset() const;

	//Setters
	void SetCam(Camera* cam);
	void SetFocus(Transform* focus);
	void SetOffest(float offset);

private:
	Camera* m_cam = nullptr;
	Transform* m_focus = nullptr;

	float m_offset = 5.f;
};

inline void to_json(nlohmann::json& j, const HorizontalScroll& horiScroll)
{
	j["Offset"] = horiScroll.GetOffset();
}
inline void from_json(const nlohmann::json& j, HorizontalScroll& horiScroll)
{
	horiScroll.SetOffest(j["Offset"]);
}