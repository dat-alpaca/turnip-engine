#pragma once
#include <vector>

#include "event/event.hpp"
#include "utils/time/time.hpp"

namespace tur
{
	using view_handle = handle_type;

	class View
	{
	public:
		virtual ~View() = default;

	public:
		void set_engine(struct TurnipEngine* engine);
		void set_handle(view_handle viewHandle);

	public:
		virtual void on_engine_startup(){};
		virtual void on_engine_shutdown(){};

		virtual void on_view_added(){};
		virtual void on_view_removed(){};

		virtual void on_render(){};
		virtual void on_render_gui(){};

		virtual void on_fixed_update(){};
		virtual void on_update(const Time& time){};
		virtual void on_post_update(const Time& time){};

		virtual void on_event(Event& event){};

	protected:
		NON_OWNING struct TurnipEngine* engine = nullptr;
		view_handle viewHandle = invalid_handle;
	};
}
namespace tur
{
	class ViewHandler
	{
	public:
		view_handle add(tur_unique<View> view);
		void remove(view_handle handle);
		tur_unique<View>& get(view_handle handle);

	public:
		std::vector<tur_unique<View>>& get_views() { return mViews; }

	private:
		std::vector<tur_unique<View>> mViews;
	};
}