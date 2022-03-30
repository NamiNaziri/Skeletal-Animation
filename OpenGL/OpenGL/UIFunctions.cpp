#include "UIFunctions.h"

#include "imgui/imgui.h"
#include "imgui/Plugin/imfilebrowser.h"

void UIFunctions::DrawSkeletonTree(Skeleton& skel)
{

    }

void UIFunctions::DrawSkeletonTreeHelper(Bone& root)
{
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool node_open = false;
    ImGuiTreeNodeFlags node_flags = base_flags;

    bool isLeaf = false;
	
    if (root.GetChildren().empty())
    {
        isLeaf = true;
        //create leaf
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
        node_open = ImGui::TreeNodeEx(root.GetName().c_str(), node_flags);
    }
    else
    {
        isLeaf = false;
        // create node
		node_open = ImGui::TreeNodeEx(root.GetName().c_str(), node_flags);

    }

    if (node_open)
    {
       
        for (int i = 0; i < root.GetChildren().size(); i++)
        {
            DrawSkeletonTreeHelper(*(root.GetChildren()[i]));
        }
    	if(!isLeaf)
    	{
            ImGui::TreePop(); //todo
    	}
        
    }

	
}

void UIFunctions::AddNewAnimationUI(ImGui::FileBrowser& fileDialog)
{

	
    if (ImGui::Begin("dummy window"))
    {
        // open file dialog when user clicks this button
        if (ImGui::Button("open file dialog"))
            fileDialog.Open();
    }
    ImGui::End();

    fileDialog.Display();

    if (fileDialog.HasSelected())
    {
        std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
        fileDialog.ClearSelected();
    }
}
