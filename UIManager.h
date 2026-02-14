//---------------------------------------------------------------------------------------------------------------------
//
// File Name:   UIManager.h
// Author(s):   Jacob Lon (jacob.lon)
// Project:     N/A
// Course:      GAM200
//
// Copyright © 2021 DigiPen (USA) Corporation.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

//---------------------------------------------------------------------------------------------------------------------
// Include Files:
//---------------------------------------------------------------------------------------------------------------------

#include "IEngineSystem.h"
#include "Observer.h"
#include "UIWindow.h"

//---------------------------------------------------------------------------------------------------------------------
// Forward References:
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
// Public Consts:
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
// Classes:
//---------------------------------------------------------------------------------------------------------------------

class UIManager : public IEngineSystem, public Observer
{
    public:

        // Get an instance of this system.
        static UIManager *getInstance();

        // initializes the system
        void initialize() override;

        // doesn't need to do anything?
        void update() override;

        // renders all the active UI elements
        void render() override;

        // shuts down the system
        void shutdown() override;

        // Adds a window to the UI manager
        virtual void addWindow(std::string name, UIWindow* window);

        // Gets a window from the UI manager
        virtual UIWindow* getWindow(std::string name);

        // clears the entire list
        virtual void clear();

        // returns if null object
        bool isNull() const override { return false; };

        void controllerPopUp(int controllerType);

    private:

        // When observer learns an event happens, it will use a list of reactions to respond
        void onNotify(Entity* entity, EVENT event) override;

        //! Static instance to the UI manager
        static UIManager *instance_;

        std::map<std::string, UIWindow*> windows_;  //!< A map of all windows

        // Cleanup struct for singleton
        struct Cleanup{ ~Cleanup(){delete instance_;}};
        static Cleanup cleanup;

        float controlelrPopUpTime = 0;

        UIWindow * conWindow;
};

class UIManagerNull : public UIManager
{
    public:

        static UIManagerNull nullObj;   //!< The null object

        // Initializes the UI manager
        void initialize() override {};

        // Updates the UI manager
        void update() override {};

        // renders all the active UI elements
        void render() override {};

        // shuts down the system
        void shutdown() override {};

        // Used for the entity stuff
        void onNotify(Entity* entity, EVENT event) override {};

        // Adds a window to the UI manager
        void addWindow(std::string name, UIWindow* window) override {};

        // Gets a window from the UI manager
        UIWindow* getWindow(std::string name) {return &UIWindowNull::nullObj;}

        // clears the entire list
        void clear() {};

        // returns if null object
        bool isNull() const override { return true; };

};

//---------------------------------------------------------------------------------------------------------------------