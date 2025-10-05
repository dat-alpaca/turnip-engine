#include "scene_editor.hpp"
#include "event/scene_editor_events.hpp"
#include "utils/gui_utils.hpp"

using namespace tur;

void SceneEditor::initialize(NON_OWNING GraphicsDevice* graphicsDevice, NON_OWNING GUISystem* guiSystem,
							 NON_OWNING SceneData* sceneData)
{
	r_GraphicsDevice = graphicsDevice;
	r_SceneData = sceneData;
	r_GUISystem = guiSystem;
}

void SceneEditor::on_render_gui()
{
	if (!isOpen)
		return;

	ImGui::Begin("Scene Editor", &isOpen);

	ImVec2 dimensions = ImGui::GetWindowSize();
	if (m_LatestSize.x != dimensions.x || m_LatestSize.y != dimensions.y && dimensions.x > 0 && dimensions.y > 0)
	{
		m_LatestSize = dimensions;

		SceneEditorResized editorResize(static_cast<u32>(dimensions.x), static_cast<u32>(dimensions.y));
		callback(editorResize);
	}

	ImVec2 position = ImGui::GetItemRectMin();
	if (m_LatestPosition.x != position.x || m_LatestPosition.y != position.y)
	{
		m_LatestPosition = position;

		SceneEditorMoved editorMoved((u32)m_LatestPosition.x, (u32)m_LatestPosition.y);
		callback(editorMoved);
	}

	r_GUISystem->texture(r_SceneData->sceneTexture, m_LatestSize, {0.0f, 1.0f}, {1.0f, 0.0f});

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		auto size = get_mouse_pixel_position(m_LatestSize.x, m_LatestSize.y);

		SceneEditorClicked editorClicked(size.x, size.y);
		callback(editorClicked);
	}

	if (r_SceneData->viewerSelectedEntity.is_valid())
	{
		if (!r_SceneData->viewerSelectedEntity.has_component<TransformComponent>())
			return ImGui::End();

		auto& transform = r_SceneData->viewerSelectedEntity.get_component<TransformComponent>().transform;

		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

		ImGuizmo::SetRect(m_LatestPosition.x, m_LatestPosition.y, m_LatestSize.x, m_LatestSize.y);

		ImGuizmo::OPERATION operation = ImGuizmo::UNIVERSAL;
		ImGuizmo::MODE mode = ImGuizmo::LOCAL;

		bool manipulated = ImGuizmo::Manipulate(&r_SceneData->editorCamera.camera.view()[0][0],
												&r_SceneData->editorCamera.camera.projection()[0][0], operation, mode,
												&transform.raw_transform()[0][0]);

		if (manipulated)
			transform.decompose_transform();
	}

	ImGui::End();
}