#include "view.hpp"
#include "engine/turnip_engine.hpp"

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
}

namespace tur
{
	view_handle ViewSystem::add(tur_unique<View> view)
	{
		mViews.push_back(std::move(view));

		u32 viewHandle = static_cast<view_handle>(mViews.size() - 1);
		mViews[viewHandle]->set_handle(viewHandle);
		mViews[viewHandle]->on_view_added();

		return viewHandle;
	}
	void ViewSystem::remove(view_handle handle)
	{
		auto viewSystemIterator = mViews.begin() + handle;
		viewSystemIterator->get()->on_view_removed();

		mViews.erase(viewSystemIterator);
	}
	tur_unique<View>& ViewSystem::get(view_handle handle)
	{
		return mViews[handle];
	}
}