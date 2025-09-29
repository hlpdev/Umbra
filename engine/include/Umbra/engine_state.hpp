#pragma once

#include "Umbra/builtins.hpp"
#include "Umbra/config.hpp"
#include "Umbra/services.hpp"
#include "Umbra/types.hpp"
#include "Umbra/vfs.hpp"

#include <memory>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>

namespace umbra {
  struct EngineState {

    Config config;

    std::shared_ptr<sol::state> lua_state;

    std::shared_ptr<Ogre::Root> ogre_root;
    Ogre::SceneManager* ogre_scene_manager;
    Ogre::Camera* ogre_camera;
    Ogre::SceneNode* ogre_camera_node;
    Ogre::Viewport* ogre_viewport;

    std::shared_ptr<TypeRegistry> type_registry;
    std::shared_ptr<BuiltinRegistry> builtin_registry;
    std::shared_ptr<ServiceRegistry> service_registry;

    std::shared_ptr<VFS> vfs;
  };
}
