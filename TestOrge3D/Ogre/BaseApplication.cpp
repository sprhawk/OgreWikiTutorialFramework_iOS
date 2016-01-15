/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
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

#include "BaseApplication.h"
#include <iostream>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#include <macUtils.h>
#endif



//---------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
    : mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mTrayMgr(0),
    mCameraMan(0),
    mDetailsPanel(0),
    mCursorWasVisible(false),
    mShutDown(false),
//    mInputManager(0),
//    mMouse(0),
//    mKeyboard(0),
    mOverlaySystem(0)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    m_ResourcePath = Ogre::macBundlePath() + "/Contents/Resources/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    m_ResourcePath = Ogre::macBundlePath() + "/";
#else
    m_ResourcePath = "";
#endif
}

//---------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
    if (mTrayMgr) delete mTrayMgr;
    if (mCameraMan) delete mCameraMan;
    if (mOverlaySystem) delete mOverlaySystem;

    // Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

//---------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
    // Show the configuration dialog and initialise the system.
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg.
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise.
        // Here we choose to let the system create a default rendering window by passing 'true'.
        mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);

    // Initialize the OverlaySystem (changed for Ogre 1.9)
//    mOverlaySystem = new Ogre::OverlaySystem();
//    mSceneMgr->addRenderQueueListener(mOverlaySystem);
}
//---------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0,0,80));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0,0,-300));
    mCamera->setNearClipDistance(1);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // Create a default camera controller
}
//---------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
    
}
//---------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
}
//---------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//---------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            // OS X does not set the working directory relative to the app.
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location.
            if (!Ogre::StringUtil::startsWith(archName, "/", false)) // only adjust relative directories
                archName = Ogre::String(Ogre::macBundlePath() + "/" + archName);
#endif

            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}
//---------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{
}
//---------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//---------------------------------------------------------------------------
void BaseApplication::go(void)
{
#ifdef _DEBUG
    #ifndef OGRE_STATIC_LIB
        mResourcesCfg = m_ResourcePath + "resources_d.cfg";
        mPluginsCfg = m_ResourcePath + "plugins_d.cfg";
    #else
        mResourcesCfg = "resources_d.cfg";
        mPluginsCfg = "plugins_d.cfg";
    #endif
#else
    #ifndef OGRE_STATIC_LIB
        mResourcesCfg = m_ResourcePath + "resources.cfg";
        mPluginsCfg = m_ResourcePath + "plugins.cfg";
    #else
        mResourcesCfg = m_ResourcePath + "resources.cfg";
        #if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
            mPluginsCfg = m_ResourcePath + "plugins.cfg";
        #endif
    #endif
#endif

    if (!setup())
        return;
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
    mRoot->startRendering();
    // Clean up
    destroyScene();
#endif

}

bool BaseApplication::initializeRTShaderSystem()
{
    if (Ogre::RTShader::ShaderGenerator::initialize())
    {
        mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        
        mShaderGenerator->addSceneManager(mSceneMgr);
        
        // Setup core libraries and shader cache path.
        Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
        Ogre::StringVector::iterator itGroup = groupVector.begin();
        Ogre::StringVector::iterator itGroupEnd = groupVector.end();
        Ogre::String shaderCoreLibsPath;
        Ogre::String shaderCachePath;
        
        for (; itGroup != itGroupEnd; ++itGroup)
        {
            Ogre::ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
            Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
            Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
            bool coreLibsFound = false;
            
            // Try to find the location of the core shader lib functions and use it
            // as shader cache path as well - this will reduce the number of generated files
            // when running from different directories.
            for (; it != itEnd; ++it)
            {
                if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
                {
                    shaderCoreLibsPath = (*it)->archive->getName() + "/";
                    shaderCachePath = shaderCoreLibsPath;
                    coreLibsFound = true;
                    break;
                }
            }
            // Core libs path found in the current group.
            if (coreLibsFound)
                break;
        }
        
        // Core shader libs not found -> shader generating will fail.
        if (shaderCoreLibsPath.empty())
            return false;
        
        // Create and register the material manager listener.
        mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);
        Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
        
        Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
        baseWhite->setLightingEnabled(false);
        mShaderGenerator->createShaderBasedTechnique(
                                                     "BaseWhite",
                                                     Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                                                     Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
        mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
                                           "BaseWhite");
        baseWhite->getTechnique(0)->getPass(0)->setVertexProgram(
                                                                 baseWhite->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
        baseWhite->getTechnique(0)->getPass(0)->setFragmentProgram(
                                                                   baseWhite->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
        
        // creates shaders for base material BaseWhiteNoLighting using the RTSS
        mShaderGenerator->createShaderBasedTechnique(
                                                     "BaseWhiteNoLighting",
                                                     Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                                                     Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
        mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
                                           "BaseWhiteNoLighting");
        Ogre::MaterialPtr baseWhiteNoLighting = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
        baseWhiteNoLighting->getTechnique(0)->getPass(0)->setVertexProgram(
                                                                           baseWhiteNoLighting->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
        baseWhiteNoLighting->getTechnique(0)->getPass(0)->setFragmentProgram(
                                                                             baseWhiteNoLighting->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
    }
    
    return true;
}

void BaseApplication::terminateRTShaderSystem()
{
    mShaderGenerator->removeSceneManager(mSceneMgr);
    
    // Restore default scheme.
    Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
    
    // Unregister the material manager listener.
    if (mMaterialMgrListener != NULL)
    {
        Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
        delete mMaterialMgrListener;
        mMaterialMgrListener = NULL;
    }
    
    // Finalize RTShader system.
    if (mShaderGenerator != NULL)
    {
        Ogre::RTShader::ShaderGenerator::destroy();
        mShaderGenerator = NULL;
    }
}

//---------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
    mRoot = new Ogre::Root(mPluginsCfg);

#   ifdef OGRE_STATIC_LIB
    m_StaticPluginLoader.load();
#   endif
    
    setupResources();

    Ogre::RenderSystemList rsList = mRoot->getAvailableRenderers();
    for (unsigned int i = 0; i < rsList.size(); i++)
    {
        Ogre::String s = rsList[i]->getName();
        std::cout << "render system:" << s << std::endl;
    }

    Ogre::RenderSystem *renderer = *rsList.begin();
    mRoot->setRenderSystem(renderer);
    
    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    initializeRTShaderSystem();
    
    // Create the scene
    createScene();

    createFrameListener();

    return true;
};
//---------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{

    return true;
}

//---------------------------------------------------------------------------
// Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
//    unsigned int width, height, depth;
//    int left, top;
//    rw->getMetrics(width, height, depth, left, top);

//    const OIS::MouseState &ms = mMouse->getMouseState();
//    ms.width = width;
//    ms.height = height;
}
//---------------------------------------------------------------------------
// Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
#   ifdef OGRE_STATIC_LIB
    m_StaticPluginLoader.unload();
#   endif
    
    // Only close for window that created OIS (the main window in these demos)
    if(rw == mWindow)
    {

    }
}
//---------------------------------------------------------------------------

ShaderGeneratorTechniqueResolverListener::ShaderGeneratorTechniqueResolverListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator)
{
    mShaderGenerator = pShaderGenerator;
}

Ogre::Technique* ShaderGeneratorTechniqueResolverListener::handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend)
{
    Ogre::Technique* generatedTech = NULL;
    
    // Case this is the default shader generator scheme.
    if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
    {
        bool techniqueCreated;
        
        // Create shader generated technique for this material.
        techniqueCreated = mShaderGenerator->createShaderBasedTechnique(
                                                                        originalMaterial->getName(),
                                                                        Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                                                                        schemeName);
        
        // Case technique registration succeeded.
        if (techniqueCreated)
        {
            // Force creating the shaders for the generated technique.
            mShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());
            
            // Grab the generated technique.
            Ogre::Material::TechniqueIterator itTech = originalMaterial->getTechniqueIterator();
            
            while (itTech.hasMoreElements())
            {
                Ogre::Technique* curTech = itTech.getNext();
                
                if (curTech->getSchemeName() == schemeName)
                {
                    generatedTech = curTech;
                    break;
                }
            }
        }
    }
    
    return generatedTech;
}
