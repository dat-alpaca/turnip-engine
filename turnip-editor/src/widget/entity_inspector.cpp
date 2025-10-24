#include "entity_inspector.hpp"

#include "core/script/script_compiler.hpp"
#include "event/events.hpp"
#include "utils/gui_utils.hpp"

void EntityInspector::initialize(NON_OWNING tur::TurnipEngine* engine, NON_OWNING tur::Scene* scene,
								 SceneData* sceneData)
{
	r_Engine = engine;
	r_Scene = scene;
	r_SceneData = sceneData;

	OnEntityInspectorInitialize initEvent;
	callback(initEvent);
}
void EntityInspector::on_render_gui()
{
	if (!isOpen)
		return;

	ImGui::Begin("Entity Inspector", &isOpen);

	if (r_SceneData->viewerSelectedEntity.get_handle() == entt::null)
		return ImGui::End();

	render_components(r_SceneData->viewerSelectedEntity);

	ImGui::Separator();

	render_component_add_list(r_SceneData->viewerSelectedEntity);

	ImGui::End();
}

void EntityInspector::on_event(Event& event)
{
	Subscriber subscriber(event);
	subscriber.subscribe<SceneEditorResized>(
		[&](SceneEditorResized& resizeEvent) -> bool
		{
			m_EditorArea.width = (float)resizeEvent.width;
			m_EditorArea.height = (float)resizeEvent.height;

			return false;
		});

	subscriber.subscribe<SceneEditorMoved>(
		[&](SceneEditorMoved& moveEvent) -> bool
		{
			m_EditorArea.x = (float)moveEvent.x;
			m_EditorArea.y = (float)moveEvent.y;

			return false;
		});
}

void EntityInspector::render_components(Entity selectedEntity)
{
	if (selectedEntity.has_component<TransformComponent>())
		render_transform_component(selectedEntity);

	if (selectedEntity.has_component<QuadTexture2D>())
		render_texture_component(selectedEntity);

	if (selectedEntity.has_component<InstancedQuadComponent>())
		render_instance_component(selectedEntity);

	if (selectedEntity.has_component<EntityScriptsComponent>())
	{
		if (!selectedEntity.get_component<EntityScriptsComponent>().scriptComponents.empty())
			render_script_component(selectedEntity);
	}
}
void EntityInspector::render_transform_component(Entity selectedEntity)
{
	auto& transform = selectedEntity.get_component<TransformComponent>().transform;

	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::DragFloat3("##position", &transform.position[0]);
		ImGui::PopItemWidth();

		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::DragFloat3("##rotation", &transform.rotation[0]);
		ImGui::PopItemWidth();

		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::DragFloat3("##scale", &transform.scale[0]);
		ImGui::PopItemWidth();
	}
}
void EntityInspector::render_texture_component(Entity selectedEntity)
{
	auto* textureComponent = &selectedEntity.get_component<QuadTexture2D>();

	auto* graphicsDevice = &r_Engine->graphicsDevice;
	auto* assetLibrary = &r_Engine->assetLibrary;

	auto entityID = selectedEntity.get_handle();
	auto* scene = r_Scene;

	if (ImGui::CollapsingHeader("Texture"))
	{
		if (textureComponent->textureHandle == invalid_handle)
		{
			if (ImGui::Button("Choose Texture..."))
			{
				auto filepaths = open_file_dialog("Open texture", {"All Images (*.png, *.jpg)", "*.png", "*.jpg"});

				if (filepaths.empty())
					return;

				auto& filepath = std::filesystem::path(filepaths[0]);

				r_Engine->workerPool.submit<AssetInformation>(
					[assetLibrary, filepath]() { return load_texture_asset(assetLibrary, filepath); },
					[textureComponent, this, assetLibrary, selectedEntity](AssetInformation information)
					{
						tur::UUID& textureUUID = assetLibrary->textures.get(information.assetHandle).uuid;

						SceneQuadTextureLoaded sceneQuadLoadEvent(selectedEntity.get_handle(), textureUUID);
						callback(sceneQuadLoadEvent);

						if (!information.is_valid())
							return;

						OnNewTextureLoad textureLoadEvent(information.assetHandle);
						callback(textureLoadEvent);
					});
			}
			return;
		}
		else
		{
			if (r_Engine->guiSystem->texture_button(textureComponent->textureHandle, {50.0f, 50.0f}))
			{
				auto filepaths = open_file_dialog("Open texture", {"All Images (*.png, *.jpg)", "*.png", "*.jpg"});

				if (filepaths.empty())
					return;

				auto& filepath = std::filesystem::path(filepaths[0]);

				r_Engine->workerPool.submit<AssetInformation>(
					[assetLibrary, filepath]() { return load_texture_asset(assetLibrary, filepath); },
					[textureComponent, this, assetLibrary, selectedEntity](AssetInformation information)
					{
						tur::UUID& textureUUID = assetLibrary->textures.get(information.assetHandle).uuid;

						SceneQuadTextureLoaded sceneQuadLoadEvent(selectedEntity.get_handle(), textureUUID);
						callback(sceneQuadLoadEvent);

						if (!information.is_valid())
							return;

						OnNewTextureLoad textureLoadEvent(information.assetHandle);
						callback(textureLoadEvent);
					});
			}

			if (ImGui::Button("Remove texture"))
			{
				SceneQuadTextureUnloaded quadTextureUnloadEvent(selectedEntity.get_handle());
				callback(quadTextureUnloadEvent);
			}
		}
	}
}
void EntityInspector::render_instance_component(Entity selectedEntity)
{
	if (ImGui::CollapsingHeader("Instanced Quad"))
	{
		// TODO: implement
	}
}
void EntityInspector::render_script_component(Entity selectedEntity)
{
	auto& scripts = selectedEntity.get_component<EntityScriptsComponent>().scriptComponents;

	for (const auto& script : scripts)
	{
		if (ImGui::CollapsingHeader(script.className.c_str()))
		{
			std::string buttonName = script.className + "##ExplorerButton";
			if (ImGui::Button(buttonName.c_str()))
				open_in_explorer(script.filepath);
		}
	}
}

void EntityInspector::render_component_add_list(Entity selectedEntity)
{
	ImGui::Dummy(ImVec2(0, ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeight()));
	if (ImGui::Button("Add Component", ImVec2(-FLT_MIN, 0)))
		ImGui::OpenPopup("AddComponentPopup");

	if (ImGui::BeginPopup("AddComponentPopup"))
	{
		render_add_transform_component(selectedEntity);

		render_add_texture_component(selectedEntity);

		render_add_instance_component(selectedEntity);

		render_add_script_component(selectedEntity);

		ImGui::EndPopup();
	}
}
void EntityInspector::render_add_transform_component(Entity selectedEntity)
{
	if (!selectedEntity.has_component<TransformComponent>() && ImGui::MenuItem("Transform"))
	{
		selectedEntity.add_component<TransformComponent>();
		callback(OnProjectEdited());
	}
}
void EntityInspector::render_add_texture_component(Entity selectedEntity)
{
	if (!selectedEntity.has_component<QuadTexture2D>() && ImGui::MenuItem("Texture2D"))
	{
		auto& quadTexture = selectedEntity.add_component<QuadTexture2D>();
		callback(OnProjectEdited());
	}
}
void EntityInspector::render_add_instance_component(Entity selectedEntity)
{
	// TODO: rename
	if (!selectedEntity.has_component<InstancedQuadComponent>() && ImGui::MenuItem("InstancedQuad2D"))
	{
		selectedEntity.add_component<InstancedQuadComponent>();
		callback(OnProjectEdited());
	}
}
void EntityInspector::render_add_script_component(Entity selectedEntity)
{
	if (ImGui::MenuItem("Entity Script"))
	{
		using namespace std::filesystem;

		auto scriptFilepath = path(save_file_dialog("New Script", "", {"Script files (.cs)", "*.cs"}));
		if (scriptFilepath.empty())
			return;

		path correctedPath = scriptFilepath.replace_extension(".cs");

		std::string className = scriptFilepath.filename().replace_extension("").string();
		create_empty_script(correctedPath, className);

		InternalEntityScript script;
		{
			script.className = className;
			script.filepath = correctedPath;
			selectedEntity.get_component<EntityScriptsComponent>().scriptComponents.push_back(script);
		}

		callback(OnProjectEdited());
	}
}