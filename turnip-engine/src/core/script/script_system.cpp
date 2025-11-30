#include "script_system.hpp"
#include "audio/audio_handler.hpp"

namespace tur
{
	void ScriptSystem::initialize()
	{
		mLua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);
		initialize_logging();
		initialize_usertypes();
	}

	void ScriptSystem::initialize_input(Window& window)
	{
		mLua["Input"] = mLua.create_table();
		mLua["Input"]["get_mouse_position"] = [&]() -> glm::vec2 { return window.get_mouse_position(); };
		mLua["Input"]["get_mouse_pressed"] = [&](MouseButton mouseButton) -> bool
		{ return window.get_mouse_pressed(mouseButton); };
		mLua["Input"]["get_key_pressed"] = [&](Key keyButton) -> bool { return window.get_key_pressed(keyButton); };

		// clang-format off

		// Mouse:
		{
			mLua["Input"]["MouseButton"] = mLua.create_table_with(
				"MOUSE_UNKNOWN", 		MouseButton::MOUSE_BUTTON_UNKNOWN,
				"MOUSE_1", 				MouseButton::MOUSE_BUTTON_1,
				"MOUSE_2", 				MouseButton::MOUSE_BUTTON_2,
				"MOUSE_3", 				MouseButton::MOUSE_BUTTON_3,
				"MOUSE_4", 				MouseButton::MOUSE_BUTTON_4,
				"MOUSE_5", 				MouseButton::MOUSE_BUTTON_5,
				"MOUSE_6", 				MouseButton::MOUSE_BUTTON_6,
				"MOUSE_7", 				MouseButton::MOUSE_BUTTON_7,
				"MOUSE_8", 				MouseButton::MOUSE_BUTTON_8,
				"MOUSE_LEFT", 			MouseButton::MOUSE_BUTTON_LEFT,
				"MOUSE_RIGHT", 			MouseButton::MOUSE_BUTTON_RIGHT
			);

			mLua["Input"]["Key"] = mLua.create_table_with(
				"SPACE", 				Key::KEY_SPACE,                            
				"APOSTROPHE", 			Key::KEY_APOSTROPHE,                  
				"COMMA",  				Key::KEY_COMMA,                            
				"MINUS",  				Key::KEY_MINUS,                            
				"PERIOD", 				Key::KEY_PERIOD,                          
				"SLASH",  				Key::KEY_SLASH,                            
				"K0", 					Key::KEY_0,                                    
				"K1", 					Key::KEY_1,                                    
				"K2", 					Key::KEY_2,                                    
				"K3", 					Key::KEY_3,                                    
				"K4", 					Key::KEY_4,                                    
				"K5", 					Key::KEY_5,                                    
				"K6", 					Key::KEY_6,                                    
				"K7", 					Key::KEY_7,                                    
				"K8", 					Key::KEY_8,                                    
				"K9", 					Key::KEY_9,                                    
				"SEMICOLON", 			Key::KEY_SEMICOLON,                    
				"EQUAL", 				Key::KEY_EQUAL,                            
				"A",					Key::KEY_A,                                    
				"B",					Key::KEY_B,                                    
				"C",					Key::KEY_C,                                    
				"D",					Key::KEY_D,                                    
				"E",					Key::KEY_E,                                    
				"F",					Key::KEY_F,                                    
				"G",					Key::KEY_G,                                    
				"H",					Key::KEY_H,                                    
				"I",					Key::KEY_I,                                    
				"J",					Key::KEY_J,                                    
				"K",					Key::KEY_K,                                    
				"L",					Key::KEY_L,                                    
				"M",					Key::KEY_M,                                    
				"N",					Key::KEY_N,                                    
				"O",					Key::KEY_O,                                    
				"P",					Key::KEY_P,                                    
				"Q",					Key::KEY_Q,                                    
				"R",					Key::KEY_R,                                    
				"S",					Key::KEY_S,                                    
				"T",					Key::KEY_T,                                    
				"U",					Key::KEY_U,                                    
				"V",					Key::KEY_V,                                    
				"W",					Key::KEY_W,                                    
				"X",					Key::KEY_X,                                    
				"Y",					Key::KEY_Y,                                    
				"Z",					Key::KEY_Z,                                    
				"LEFT_BRACKET", 		Key::KEY_LEFT_BRACKET,              
				"BACKSLASH", 			Key::KEY_BACKSLASH,                    
				"RIGHT_BRACKET", 		Key::KEY_RIGHT_BRACKET,            
				"GRAVE_ACCENT", 		Key::KEY_GRAVE_ACCENT,                                     
				"ESCAPE", 				Key::KEY_ESCAPE,                          
				"ENTER", 				Key::KEY_ENTER,                            
				"TAB", 					Key::KEY_TAB,                                
				"BACKSPACE", 			Key::KEY_BACKSPACE,                    
				"INSERT", 				Key::KEY_INSERT,                          
				"DELETE", 				Key::KEY_DELETE,                          
				"RIGHT", 				Key::KEY_RIGHT,                            
				"LEFT", 				Key::KEY_LEFT,                              
				"DOWN", 				Key::KEY_DOWN,                              
				"UP", 					Key::KEY_UP,                                  
				"PAGE_UP", 				Key::KEY_PAGE_UP,                        
				"PAGE_DOWN", 			Key::KEY_PAGE_DOWN,                    
				"HOME", 				Key::KEY_HOME,                              
				"END", 					Key::KEY_END,                                
				"CAPS_LOCK", 			Key::KEY_CAPS_LOCK,                    
				"SCROLL_LOCK", 			Key::KEY_SCROLL_LOCK,                
				"NUM_LOCK", 			Key::KEY_NUM_LOCK,                      
				"PRINT_SCREEN", 		Key::KEY_PRINT_SCREEN,              
				"PAUSE", 				Key::KEY_PAUSE,                            
				"F1", 					Key::KEY_F1,                                  
				"F2", 					Key::KEY_F2,                                  
				"F3", 					Key::KEY_F3,                                  
				"F4", 					Key::KEY_F4,                                  
				"F5", 					Key::KEY_F5,                                  
				"F6", 					Key::KEY_F6,                                  
				"F7", 					Key::KEY_F7,                                  
				"F8", 					Key::KEY_F8,                                  
				"F9", 					Key::KEY_F9,                                  
				"F10", 					Key::KEY_F10,                                
				"F11", 					Key::KEY_F11,                                
				"F12", 					Key::KEY_F12,                                
				"F13", 					Key::KEY_F13,                                
				"F14", 					Key::KEY_F14,                                
				"F15", 					Key::KEY_F15,                                
				"F16", 					Key::KEY_F16,                                
				"F17", 					Key::KEY_F17,                                
				"F18", 					Key::KEY_F18,                                
				"F19", 					Key::KEY_F19,                                
				"F20", 					Key::KEY_F20,                                
				"F21", 					Key::KEY_F21,                                
				"F22", 					Key::KEY_F22,                                
				"F23", 					Key::KEY_F23,                                
				"F24", 					Key::KEY_F24,                                
				"KP_0", 				Key::KEY_KP_0,                              
				"KP_1", 				Key::KEY_KP_1,                              
				"KP_2", 				Key::KEY_KP_2,                              
				"KP_3", 				Key::KEY_KP_3,                              
				"KP_4", 				Key::KEY_KP_4,                              
				"KP_5", 				Key::KEY_KP_5,                              
				"KP_6", 				Key::KEY_KP_6,                              
				"KP_7", 				Key::KEY_KP_7,                              
				"KP_8", 				Key::KEY_KP_8,                              
				"KP_9", 				Key::KEY_KP_9,                              
				"KP_DECIMAL", 			Key::KEY_KP_DECIMAL,                  
				"KP_DIVIDE", 			Key::KEY_KP_DIVIDE,                    
				"KP_MULTIPLY", 			Key::KEY_KP_MULTIPLY,                
				"KP_SUBTRACT", 			Key::KEY_KP_SUBTRACT,                
				"KP_ADD", 				Key::KEY_KP_ADD,                          
				"KP_ENTER", 			Key::KEY_KP_ENTER,                      
				"KP_EQUAL", 			Key::KEY_KP_EQUAL,                      
				"LEFT_SHIFT", 			Key::KEY_LEFT_SHIFT,                  
				"LEFT_CONTROL", 		Key::KEY_LEFT_CONTROL,              
				"LEFT_ALT", 			Key::KEY_LEFT_ALT,                      
				"LEFT_SUPER", 			Key::KEY_LEFT_SUPER,                  
				"RIGHT_SHIFT", 			Key::KEY_RIGHT_SHIFT,                
				"RIGHT_CONTROL", 		Key::KEY_RIGHT_CONTROL,            
				"RIGHT_ALT", 			Key::KEY_RIGHT_ALT,                    
				"RIGHT_SUPER", 			Key::KEY_RIGHT_SUPER,                
				"MENU", 				Key::KEY_MENU                                                  	
			);
		}
		// clang-format on
	}

	void ScriptSystem::initialize_audio(NON_OWNING AudioHandler* audioHandler)
	{
		rAudioHandler = audioHandler;
	}

	void ScriptSystem::initialize_logging()
	{
		// clang-format off
			mLua["Log"] = mLua.create_table();
			mLua["Log"]["critical"] 	= [](const std::string& message) { TUR_LOG_CRITICAL(message); };
			mLua["Log"]["error"] 		= [](const std::string& message) { TUR_LOG_ERROR(message); };
			mLua["Log"]["warn"] 		= [](const std::string& message) { TUR_LOG_WARN(message); };
			mLua["Log"]["debug"] 		= [](const std::string& message) { TUR_LOG_DEBUG(message); };
			mLua["Log"]["trace"] 		= [](const std::string& message) { TUR_LOG_TRACE(message); };
			mLua["Log"]["info"] 		= [](const std::string& message) { TUR_LOG_INFO(message); };
		// clang-format on
	}

	void ScriptSystem::initialize_usertypes()
	{
		// clang-format off

			// UUID:
			sol::usertype<UUID> uuidType = mLua.new_usertype<UUID>("uuid", sol::constructors<UUID(), UUID(u64)>());
			uuidType["value"] = &UUID::uuid;
			uuidType["uuid"] = &UUID::uuid;

			// vec2:
			{
				sol::usertype<glm::vec2> vec2Type = mLua.new_usertype<glm::vec2>(
					"vec2", 
					sol::constructors<glm::vec2()>(),

					sol::meta_function::addition, 
					[](const glm::vec2& lhs, const glm::vec2& rhs) -> glm::vec2 {
						return { lhs.x + rhs.x, lhs.y + rhs.y };
					},

					sol::meta_function::subtraction, 
					[](const glm::vec2& lhs, const glm::vec2& rhs) -> glm::vec2 {
						return { lhs.x - rhs.x, lhs.y - rhs.y };
					}				
				);

				vec2Type["x"] = &glm::vec2::x;
				vec2Type["y"] = &glm::vec2::y;
			}

			// vec3
			{
				sol::usertype<glm::vec3> vec3Type = mLua.new_usertype<glm::vec3>(
					"vec3", 
					sol::constructors<glm::vec3()>(),

					sol::meta_function::addition, 
					[](const glm::vec3& lhs, const glm::vec3& rhs) -> glm::vec3 {
						return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z - rhs.z };
					},

					sol::meta_function::subtraction, 
					[](const glm::vec3& lhs, const glm::vec3& rhs) -> glm::vec3 {
						return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
					}				
				);
				
				vec3Type["x"] = &glm::vec3::x;
				vec3Type["y"] = &glm::vec3::y;
				vec3Type["z"] = &glm::vec3::z;
			}

			// vec4
			{
				sol::usertype<glm::vec4> vec4Type = mLua.new_usertype<glm::vec4>(
					"vec4", 
					sol::constructors<glm::vec4()>(),

					sol::meta_function::addition, 
					[](const glm::vec4& lhs, const glm::vec4& rhs) -> glm::vec4 {
						return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
					},

					sol::meta_function::subtraction, 
					[](const glm::vec4& lhs, const glm::vec4& rhs) -> glm::vec4 {
						return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
					}				
				);
				
				vec4Type["x"] = &glm::vec4::x;
				vec4Type["y"] = &glm::vec4::y;
				vec4Type["z"] = &glm::vec4::z;
				vec4Type["w"] = &glm::vec4::w;
			}
			
			// transform:
			{
				sol::usertype<Transform> transformType = mLua.new_usertype<Transform>(
					"transform", 
					sol::constructors<
						Transform(const glm::vec3&, const glm::vec3&, const glm::vec3&), 
						Transform()
					>()
				);
				
				transformType["position"] = &Transform::position;
				transformType["rotation"] = &Transform::rotation;
				transformType["scale"] = &Transform::scale;
			}
		
			// audio:
			{
				sol::usertype<AudioSourceComponent> audioSourceType = mLua.new_usertype<AudioSourceComponent>("audio_source");
				audioSourceType["handle"] = &AudioSourceComponent::assetHandle;

				audioSourceType["play"] = [&](AudioSourceComponent* component)
				{
					asset_handle assetHandle = component->assetHandle;
					rAudioHandler->play(assetHandle);
				};

				audioSourceType["set_volume"] = [&](AudioSourceComponent* component, float volume)
				{
					asset_handle assetHandle = component->assetHandle;
					rAudioHandler->set_volume(assetHandle, volume);
				};
			}

		// clang-format on
	}
}