#include "view.hpp"
#include "engine/turnip_engine.hpp"
#include "event/event.hpp"

namespace tur
{
	void View::set_engine(TurnipEngine* engine)
	{
		this->engine = engine;
	}
	void View::set_handle(view_handle viewHandle)
	{
		viewHandle = viewHandle;
	}
	void View::set_event_callback(const EventCallback& eventCallback)
	{
		callback = eventCallback;
	}
}

namespace tur
{
	view_handle ViewHandler::add(tur_unique<View> view)
	{
		mViews.push_back(std::move(view));

		u32 viewHandle = static_cast<view_handle>(mViews.size() - 1);
		mViews[viewHandle]->set_handle(viewHandle);
		mViews[viewHandle]->on_view_added();

		return viewHandle;
	}
	void ViewHandler::remove(view_handle handle)
	{
		auto viewHandlerIterator = mViews.begin() + handle;
		viewHandlerIterator->get()->on_view_removed();

		mViews.erase(viewHandlerIterator);
	}
}