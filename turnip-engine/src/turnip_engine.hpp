#pragma once
#include "client/project/project.hpp"

#include "client/views/camera_view.hpp"
#include "client/views/cubemap_view.hpp"
#include "client/views/imm_quad_view.hpp"
#include "client/views/mesh_view.hpp"
#include "client/views/physics_view.hpp"
#include "client/views/scene_signal_view.hpp"
#include "client/views/scenegraph_view.hpp"
#include "client/views/script_view.hpp"
#include "client/views/tilemap_view.hpp"
#include "client/views/physics_script_view.hpp"
#include "client/views/asset_binder_view.hpp"

#include "client/asset_serialization.hpp"
#include "client/scene_serialization.hpp"

#include "core/assets/audio/audio_asset.hpp"
#include "core/assets/audio/audio_loader.hpp"
#include "core/assets/texture/texture_asset.hpp"
#include "core/assets/texture/texture_loader.hpp"
#include "core/assets/texture/texture_options.hpp"
#include "core/assets/asset_library.hpp"
#include "core/assets/asset.hpp"

#include "core/audio/audio_handler.hpp"

#include "core/data-structures/free_list.hpp"
#include "core/data-structures/range.hpp"
#include "core/data-structures/ring_buffer.hpp"

#include "core/engine/turnip_engine.hpp"

#include "core/event/events.hpp"

#include "core/physics/physics_handler.hpp"
#include "core/physics/physics_types.hpp"

#include "core/scene/scene.hpp"
#include "core/scene/scene_holder.hpp"
#include "core/scene/components.hpp"
#include "core/scene/entity.hpp"

#include "core/script/script_handler.hpp"

#include "core/view/view.hpp"

#include "core/windowing/window.hpp"
#include "core/windowing/windowing.hpp"

#include "core/worker/worker_pool.hpp"

#include "common.hpp"