//---------------------------------------------------------------------------------------------------------------------
//
// File Name:   UIManager.cpp
// Author(s):   Jacob Lon (jacob.lon)
// Project:     N/A
// Course:      GAM200
//
// Copyright © 2021 DigiPen (USA) Corporation.
//
//---------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "UIManager.h"
#include "Subject.h"
#include "UIElement.h"
#include "Button.h"
#include "TraceLog.h"

//---------------------------------------------------------------------------------------------------------------------
// Private Consts:
//---------------------------------------------------------------------------------------------------------------------

UIManager *UIManager::instance_;
UIManagerNull UIManagerNull::nullObj;

static GraphicsSystem *graphics;    // Graphics system

static Camera camera;   // Camera object

//---------------------------------------------------------------------------------------------------------------------
// Public Functions:
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
// Private Functions:
//---------------------------------------------------------------------------------------------------------------------

/**
 * Gets the current instance of the UI manager.
 *
 * @return
 * Returns the existing instance or constructs it if it doesn't exist yet.
 */
UIManager *UIManager::getInstance()
{
    // Checks if the instance has not been created yet
    if(!instance_)
    {
        // Creates a new instance
        instance_ = new UIManager();
        instance_->setName("UIManager");
        // add to event handler
        GET_SYSTEM(Subject)->addObserver("UIManager", instance_);
    }

    // Returns the instance
    return instance_;
}

// Initializes the UI manager
void UIManager::initialize()
{
    // Stores the world shader into the ID variable
    graphics = GET_SYSTEM(GraphicsSystem);

    // Set camera vars
    GET_SYSTEM(TraceLog)->TraceLogMessage("UIManager", __func__);


    float scal = (float) Engine::activeEngine->getWidth() / (float) 2160;
    conWindow = new UIWindow(glm::vec3(0, 0, -0.1f),glm::vec3(126.0f * scal * 2 , 64.0f* scal * 2, 1.0f) );
}

/**
 * Updates the UI manager.
 */
void UIManager::update()
{
    controlelrPopUpTime -= Engine::activeEngine->getAveragedDT();
    // Loops over all existing windows
    for(auto& [key, window] : windows_)
    {
        // Updates the window
        window->update();
    }
    float scal = (float) Engine::activeEngine->getWidth() / (float) 2160;
    conWindow->setPosition(glm::vec3(126.0f * scal * 2, 64.0f * 2, -0.1f));
    conWindow->setScale(glm::vec3(126.0f * scal * 2 , 64.0f* scal * 2 , 1.0f));
}

/**
 * Renders all windows in the UI manager.
 */
void UIManager::render()
{
    // Uses the world shader for rendering
    graphics->setContext("UIShader", camera);

    // Creates a new render properties struct
    Util::RenderProperties rProp = graphics->getRenderProp();

    if(controlelrPopUpTime > Util::GetTimeMillis() ){
        conWindow->render(rProp);
    }
    // Loops over all windows
    for(auto& [key, window] : windows_)
    {
        // Sets no texture
        rProp.texture = {};

        // Renders the current window
        window->render(rProp);
    }


}

/**
 * Shuts down the UI manager.
 */
void UIManager::shutdown()
{
    // Clears all active windows
    clear();
}

/**
 * Clears the UI manager.
 */
void UIManager::clear()
{
    // Loops over every window
    for(auto& [key, window] : windows_)
    {
        // Deletes the window and sets it to the null object
        delete window;
        window = &UIWindowNull::nullObj;
    }

    // Clears the windows map
    windows_.clear();
}

/**
 * Used for the event system, not sure how this works, this is Jacob's stuff.
 *
 * @param entity
 * An entity.
 *
 * @param event
 * An event.
 */
void UIManager::onNotify(Entity* entity, EVENT event)
{
    switch (event)
    {

        case EVENT::INVALID:
            break;
        case EVENT::CREATE:
            break;
        case EVENT::WIN:
            break;
        case EVENT::WINFIRSTOCCURRANCE:
            break;
        case EVENT::RESETWIN:
            break;
        case EVENT::ENABLEPACKAGE:
            break;
        case EVENT::DISABLEPACKAGE:
            break;
    }
}

/**
 * Adds a window to the UI manager.
 *
 * @param name
 * The name of the window.
 *
 * @param window
 * A pointer to the window to store.
 */
void UIManager::addWindow(std::string name, UIWindow *window)
{
    windows_.emplace(name, window);
}

/**
 * Gets a specific window from the UI manager.
 *
 * @param name
 * The name of the window to find.
 *
 * @return
 * Returns the requested window.
 */
UIWindow *UIManager::getWindow(std::string name)
{
    return windows_.at(name);
}

void UIManager::controllerPopUp(int controllerType) {
    if(controllerType == 0){
        conWindow->setTexture("ConnectedKeyboard");
    }else if(controllerType == 1){

        conWindow->setTexture("ConnectedDual");
    }else if(controllerType == 2){

        conWindow->setTexture("ConnectedWireless");
    }

    controlelrPopUpTime = Util::GetTimeMillis() + 5000;



}
