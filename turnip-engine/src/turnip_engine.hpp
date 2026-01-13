#pragma once
#include "client/agents/event_consumer.hpp"
#include "client/agents/event_emitter.hpp"
#include "client/agents/physics_script_agent.hpp"
#include "client/agents/asset_binder_system.hpp"

#include "client/project/project.hpp"

#include "client/system/camera_system.hpp"
#include "client/system/imm_quad_system.hpp"
#include "client/system/physics_system.hpp"
#include "client/system/scene_signal_system.hpp"
#include "client/system/scenegraph_system.hpp"
#include "client/system/script_system.hpp"
#include "client/system/tilemap_system.hpp"

#include "client/asset_serialization.hpp"
#include "client/scene_serialization.hpp"
#include "client/scene_view.hpp"

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