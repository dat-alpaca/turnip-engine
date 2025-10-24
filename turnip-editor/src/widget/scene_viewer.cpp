#include "scene_viewer.hpp"
#include "event/events.hpp"

void SceneViewer::initialize(NON_OWNING Scene* scene, SceneData* sceneData)
{
	r_Scene = scene;
	r_SceneData = sceneData;
}

void SceneViewer::on_render_gui()
{
	if (!isOpen)
		return;

	ImGui::Begin("Scene Viewer", &isOpen);

	auto& registry = r_Scene->get_registry();
	for (auto& [entity, sceneGraphComp] : registry.view<SceneGraphComponent>().each())
	{
		if (sceneGraphComp.parent == entt::null)
			render_scene_graph_node(Entity(entity, r_Scene));
	}

	ImGui::Dummy(ImVec2(0, ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeight()));
	if (ImGui::Button("Add Entity", ImVec2(-FLT_MIN, 0)))
		add_empty_entity();

	ImGui::End();
}

void SceneViewer::render_scene_graph_node(Entity entity)
{
	auto& registry = r_Scene->get_registry();

	if (!registry.valid(entity.get_handle()))
		return;

	SceneGraphComponent& entitySceneGraph = registry.get<SceneGraphComponent>(entity);
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (r_SceneData->viewerSelectedEntity.get_handle() == entity.get_handle())
		flags |= ImGuiTreeNodeFlags_Selected;

	NameComponent& entityName = registry.get<NameComponent>(entity);

	if (!entitySceneGraph.children.empty())
	{
		if (ImGui::TreeNodeEx((void*)(u64)entity.get_handle(), flags, "%s", entityName.name.c_str()))
		{
			for (auto child : entitySceneGraph.children)
				render_scene_graph_node(Entity(child, r_Scene));

			ImGui::TreePop();
		}
	}

	if (m_RenamingEntity != entt::null)
	{
		ImGui::SetNextItemWidth(150.0f);

		std::copy(entityName.name.begin(), entityName.name.end(), m_RenameBuffer);

		if (ImGui::InputText("##Editing", m_RenameBuffer, IM_ARRAYSIZE(m_RenameBuffer),
							 ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll))
		{
			entityName.name = std::string(m_RenameBuffer);
			m_RenamingEntity = entt::null;
		}
	}

	else
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(u64)entity.get_handle(), flags, "%s", entityName.name.c_str());

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("SceneViewerPopup");
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			r_SceneData->viewerSelectedEntity = entity;

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			m_RenamingEntity = entity;

		if (ImGui::BeginPopup("SceneViewerPopup"))
			render_scene_viewer_popup(entity);
	}

	// TODO: move entities
	// TODO: entity hierarchy
	// TODO: use projectEdited
}

void SceneViewer::render_scene_viewer_popup(Entity entity)
{
	using namespace std::filesystem;

	const auto& entityExtension = r_Scene->get_registry().get<NameComponent>(entity).name + ".ins";

	if (ImGui::Button("Export as instance"))
	{
		auto instanceFilepath =
			path(save_file_dialog("Export instance file", entityExtension, {"Instance files (*.ins)", ".ins"}));
		std::string instanceFilepathName = instanceFilepath.filename().string();

		serialize_entity(instanceFilepath, r_Scene, entity);
	}

	ImGui::EndPopup();
}

void SceneViewer::add_empty_entity()
{
	r_SceneData->viewerSelectedEntity = r_Scene->add_entity();
	callback(OnProjectEdited());
}