#include "project_view.hpp"
#include "views/main/main_view.hpp"

void ProjectView::on_engine_startup()
{
	engine->viewSystem.views.reserve(2);
	show_window(&engine->window);
}

void ProjectView::on_update()
{
	if (!m_IsOpen)
		turnip_engine_shutdown(*engine);
}
void ProjectView::on_render_gui()
{
	ImGui::SetNextWindowViewport(0);

	if (ImGui::Begin("Project Editor", &m_IsOpen, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::Button("Create New Project"))
		{
			auto projectFolderPath = std::filesystem::path(open_folder_dialog("Open Project Folder"));
			std::string projectName = projectFolderPath.filename().string();

			ProjectOptions options;
			{
				options.projectName = projectName;
				options.projectFolder = projectFolderPath;
				options.domainFilepath = engine->configData.scriptingInfo.mainDomainPath;
			}

			auto projectWrap = create_empty_project(options);
			if (!projectWrap.has_value())
				return ImGui::End();

			engine_add_view(*engine, tur::make_unique<MainView>(projectWrap.value()));
			engine_remove_view(*engine, viewHandle);
		}

		if (ImGui::Button("Open Project..."))
		{
			std::vector<std::string> filters = {fmt::format("Project Files ({})", TUR_ENGINE_FILE_EXTENSION),
												"*" + std::string(TUR_ENGINE_FILE_EXTENSION)};

			auto projectFilepaths = open_file_dialog("Open Project Folder", filters);

			if (projectFilepaths.empty())
				return ImGui::End();

			auto projectWrap = read_project_file(projectFilepaths[0]);
			if (!projectWrap.has_value())
				return ImGui::End();

			engine_add_view(*engine, tur::make_unique<MainView>(projectWrap.value()));
			engine_remove_view(*engine, viewHandle);
		}

		ImGui::End();
	}
}