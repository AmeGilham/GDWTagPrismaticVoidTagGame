#ifndef __UITAB_H__
#define __UITAB_H__

#include <imgui/imgui.h>
#include <vector>

#include "BackEnd.h"
#include "Shader.h"

class AssignmentUI abstract {
public:

	~AssignmentUI();

	//imgui editor
	static void CreateTab();
	static void CreateEditor();
	static void Anim(int n);

private:

	static Selectable m_selectable;


};

#endif // !__UITAB_H__
