/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#  include <OIS/OISEvents.h>
#  include <OIS/OISInputManager.h>
#  include <OIS/OISKeyboard.h>
#  include <OIS/OISMouse.h>

#  include <OGRE/SdkTrays.h>
#  include <OGRE/SdkCameraMan.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#  include <OgreRTShaderSystem.h>
#  include <SdkTrays.h>
#  include <SdkCameraMan.h>
#else
#  include <OISEvents.h>
#  include <OISInputManager.h>
#  include <OISKeyboard.h>
#  include <OISMouse.h>

#  include <SdkTrays.h>
#  include <SdkCameraMan.h>
#endif

#ifdef OGRE_STATIC_LIB
#  if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#    define OGRE_STATIC_GLES2
#  else
#    define OGRE_STATIC_GL
#    if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#      define OGRE_STATIC_Direct3D9
//   D3D10 will only work on vista, so be careful about statically linking
#      if OGRE_USE_D3D10
#        define OGRE_STATIC_Direct3D10
#      endif
#    endif
#    define OGRE_STATIC_CgProgramManager
#  endif
#  define OGRE_STATIC_BSPSceneManager
#  define OGRE_STATIC_ParticleFX
#  ifdef OGRE_USE_PCZ
#    define OGRE_STATIC_PCZSceneManager
#    define OGRE_STATIC_OctreeZone
#  else
#    define OGRE_STATIC_OctreeSceneManager
#  endif
#  include "OgreStaticPluginLoader.h"
#endif

/** This class demonstrates basic usage of the RTShader system.
 It sub class the material manager listener class and when a target scheme callback
 is invoked with the shader generator scheme it tries to create an equivalent shader
 based technique based on the default technique of the given material.
 */
class ShaderGeneratorTechniqueResolverListener : public Ogre::MaterialManager::Listener
{
public:
    
    ShaderGeneratorTechniqueResolverListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator);
    
    /** This is the hook point where shader based technique will be created.
     It will be called whenever the material manager won't find appropriate technique
     that satisfy the target scheme name. If the scheme name is out target RT Shader System
     scheme name we will try to create shader generated technique for it.
     */
    virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend);
protected:
    Ogre::RTShader::ShaderGenerator* mShaderGenerator;			// The shader generator instance.
};

//---------------------------------------------------------------------------

class BaseApplication : public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
, public OIS::KeyListener, public OIS::MouseListener
#endif
{
public:
    BaseApplication(void);
    virtual ~BaseApplication(void);

    virtual void go(void);
    virtual void updateDraw(Ogre::Real differenceInSeconds)
    {
        mRoot->renderOneFrame((Ogre::Real)differenceInSeconds);
    };
protected:
    virtual bool setup();
    virtual bool configure(void);
    virtual void chooseSceneManager(void);
    virtual void createCamera(void);
    virtual void createFrameListener(void);
    virtual void createScene(void) = 0; // Override me!
    virtual void destroyScene(void);
    virtual void createViewports(void);
    virtual void setupResources(void);
    virtual void createResourceListener(void);
    virtual void loadResources(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    virtual bool initializeRTShaderSystem();
    virtual void terminateRTShaderSystem();
#else
    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
#endif

    // Adjust mouse clipping area
    virtual void windowResized(Ogre::RenderWindow* rw);
    // Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(Ogre::RenderWindow* rw);

    Ogre::Root*                 mRoot;
    Ogre::Camera*               mCamera;
    Ogre::SceneManager*         mSceneMgr;
    Ogre::RenderWindow*         mWindow;
    Ogre::String                mResourcesCfg;
    Ogre::String                mPluginsCfg;

    Ogre::OverlaySystem*        mOverlaySystem;

    // OgreBites
    OgreBites::InputContext     mInputContext;
    OgreBites::SdkTrayManager*	mTrayMgr;
    OgreBites::SdkCameraMan*    mCameraMan;     	// Basic camera controller
    OgreBites::ParamsPanel*     mDetailsPanel;   	// Sample details panel
    bool                        mCursorWasVisible;	// Was cursor visible before dialog appeared?
    bool                        mShutDown;
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
    //OIS Input devices
    OIS::InputManager*          mInputManager;
    OIS::Mouse*                 mMouse;
    OIS::Keyboard*              mKeyboard;
#else
    Ogre::RTShader::ShaderGenerator* mShaderGenerator; // The Shader generator instance.
    ShaderGeneratorTechniqueResolverListener* mMaterialMgrListener; // Shader generator material manager listener.
#endif

    // Added for Mac compatibility
    Ogre::String                 m_ResourcePath;

#ifdef OGRE_STATIC_LIB
    Ogre::StaticPluginLoader m_StaticPluginLoader;
#endif
};

//---------------------------------------------------------------------------

#endif // #ifndef __BaseApplication_h_

//---------------------------------------------------------------------------