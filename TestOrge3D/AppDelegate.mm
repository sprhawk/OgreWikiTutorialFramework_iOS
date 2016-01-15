//
//  AppDelegate.m
//  TestOrge3D
//
//  Created by YANG HONGBO on 2016-1-13.
//  Copyright © 2016年 Yang.me. All rights reserved.
//

#import "AppDelegate.h"
#include <OGRE/Ogre.h>
#include <OGRE/OgrePlatform.h>

#include "TutorialApplication.h"

@interface AppDelegate ()
{
    CADisplayLink *mDisplayLink;
    NSDate* mDate;
    NSTimeInterval mLastFrameTime;
    
    TutorialApplication app;
}

- (void)go;
- (void)renderOneFrame:(id)sender;

@property (nonatomic) NSTimeInterval mLastFrameTime;

@end

@implementation AppDelegate
@dynamic mLastFrameTime;

- (NSTimeInterval)mLastFrameTime
{
    return mLastFrameTime;
}

- (void)setLastFrameTime:(NSTimeInterval)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        mLastFrameTime = frameInterval;
    }
}

- (void)go {
    
    @autoreleasepool {
        try {
            app.go();
            
            Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();
            
            // Clear event times
            Ogre::Root::getSingleton().clearEventTimes();
        } catch( Ogre::Exception& e ) {
            std::cerr << "An exception has occurred: " <<
            e.getFullDescription().c_str() << std::endl;
        }
    }
}

- (void)renderOneFrame:(id)sender
{
//    [sb.mGestureView becomeFirstResponder];
    
    // NSTimeInterval is a simple typedef for double
    NSTimeInterval currentFrameTime = -[mDate timeIntervalSinceNow];
    NSTimeInterval differenceInSeconds = currentFrameTime - mLastFrameTime;
    mLastFrameTime = currentFrameTime;
    
    dispatch_async(dispatch_get_main_queue(), ^(void)
                   {
                       app.updateDraw((Ogre::Real)differenceInSeconds);
                   });
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    mLastFrameTime = 2;
    mDisplayLink = nil;
    
    [self go];
    
    return YES;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
    // Reset event times and reallocate the date and displaylink objects
    Ogre::Root::getSingleton().clearEventTimes();
    mDate = [[NSDate alloc] init];
    mLastFrameTime = 2; // Reset the timer
    
    mDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderOneFrame:)];
    [mDisplayLink setFrameInterval:mLastFrameTime];
    [mDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    Ogre::Root::getSingleton().saveConfig();
    
    mDate = nil;
    
    [mDisplayLink invalidate];
    mDisplayLink = nil;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
