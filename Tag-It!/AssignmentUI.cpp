#include "AssignmentUI.h"
#include "MainGame.h"
#include "Game.h"

Selectable AssignmentUI::m_selectable = Selectable("UI and Elements");
Degrees angle = 0;
Degrees rot[1] = { angle };

AssignmentUI::~AssignmentUI() {
}

void AssignmentUI::CreateTab() {

	bool temp = false;

	if (ImGui::BeginTabItem(m_selectable.GetName().c_str(), m_selectable.GetSelected())) {
		temp = true;
		ImGui::EndTabItem();
	}

	if (temp) {
		CreateEditor();
	}
}

void AssignmentUI::CreateEditor() {

	if (m_selectable.GetSelected()) {

		if (ImGui::TreeNode("Challenge 2")) { //dropdown for ui elements

			if (ImGui::Button("Animation 1", ImVec2(100.f, 20.f))) { //button to change animations
				AssignmentUI::Anim(1);
			}
			if (ImGui::Button("Animation 2", ImVec2(100.f, 20.f))) { //button to chagne animation
				AssignmentUI::Anim(0);
			}

			auto& tempTran = ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer());
			float position[] = { tempTran.GetPosition().x, tempTran.GetPosition().y, tempTran.GetPosition().z };

			if (ImGui::DragFloat3("Position", position, 0.1f, -200.f, 200.f))
				tempTran.SetPosition(vec3(position[0], position[1], position[2]));


			auto& cam = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera());
			if (ImGui::SliderAngle("Camera Angle", rot, -360.f, 360.f))
				cam.SetRotationAngleZ(rot[0]);

			ImGui::TreePop();
		}


	}


}

void AssignmentUI::Anim(int n) {
	auto& animController = ECS::GetComponent<AnimationController>(EntityIdentifier::MainPlayer());
	animController.SetActiveAnim(n);
}

