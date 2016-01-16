/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
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

#include "TutorialApplication.h"

//---------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//---------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//---------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
    // Create your scene here :)
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));
    mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
    
    mSceneMgr->createLight("Light")->setPosition(75,75,75);
    
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    Ogre::Entity* ogreEntity = mSceneMgr->createEntity("ogrehead.mesh");
    Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    ogreNode->attachObject(ogreEntity);
    
    
    Ogre::Light* light = mSceneMgr->createLight("MainLight");
    light->setPosition(0, 0, 0);
    
    mCamera->setPosition(0, 0, 500);
    
    Ogre::Entity* ogreEntity2 = mSceneMgr->createEntity("ogrehead.mesh");
    Ogre::SceneNode* ogreNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(84, 48, 0));
    ogreNode2->attachObject(ogreEntity2);
}


//---------------------------------------------------------------------------
