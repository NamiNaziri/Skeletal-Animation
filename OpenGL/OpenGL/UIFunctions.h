#pragma once
#include "SkeletalModel.h"
#include "imgui/Plugin/imfilebrowser.h"

namespace UIFunctions
{
	void DrawSkeletonTree(Skeleton& skel);
	void DrawSkeletonTreeHelper(Bone& root);
	void AddNewAnimationUI(ImGui::FileBrowser& fileDialog);
}

