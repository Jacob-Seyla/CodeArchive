//---------------------------------------------------------------------------------------------------------------------
//
// File Name:   Subject.h
// Author(s):   Jacob Lon (jacob.lon)
// Project:     Lost in Transit
// Course:      GAM200
//
// Copyright © 2021 DigiPen (USA) Corporation.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

//---------------------------------------------------------------------------------------------------------------------
// Include Files:
//---------------------------------------------------------------------------------------------------------------------

#include <functional>
#include <list>

//---------------------------------------------------------------------------------------------------------------------
// Forward References:
//---------------------------------------------------------------------------------------------------------------------

//! reference to the class that acts as a reciever
class Observer;

/**
 * Object for the lambda functions and their respective time
 */
struct task
{
    int time;
    std::function<void(void)> func;
    bool operator>(task const &rhs) const { return (this->time > rhs.time); }
};

//---------------------------------------------------------------------------------------------------------------------
// Public Consts:
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
// Classes:
//---------------------------------------------------------------------------------------------------------------------

/**
 * System that sends messages to any systems that would recieve a message.
 * Contains list of Observers
 */
class Subject : public IEngineSystem
{
    public:

        Subject() = default;

        ~Subject() = default;

        // retrieves reference to the systems instance
        static Subject* getInstance();

        // trace log call, win events reset
        void initialize() override;

        void update() override;

        //does nothing
        void render() override;

        // does nothing
        void shutdown() override;

        //! returns if current object is null or not null
        bool isNull() const override { return false; };

        // adds an observer system to the list
        virtual void addObserver(const std::string& observername, Observer* observer);

        // removes the observer from the list
        virtual void removeObserver(const std::string& observername);

        // notifies all observers of an event
        virtual void notify(enum  EVENT event, Entity* entity = &EntityNull::nullObj);

        // sets whether an event has happened, might not be needed(TEMP)
        virtual void setEvent(enum  EVENT name, bool flag);

        // reads if evennt has happened
        virtual bool readEvent(enum  EVENT name);

        // resets all the events/makes all events enabled
        virtual void resetEvents();

        // creates a new task object of function and time that will be added to one of the lists
        virtual void addtask(const std::function<void(void)> &job, const std::string &type, int dofor, const std::string &unit = "cycle");

        // Loads a file detailing what events are active and what arent
        virtual void loadEvents(const std::string &filename = "Default");

        // translates a string to its EVENT enum value
        virtual EVENT translateString(const std::string& name);

    private:

        //! reference to the instance of the system
        static Subject* instance_;

        //! map of pointers to observer systems
        std::map<std::string, Observer*> observers_;

        //! map of event names and boolean indicating if the event is enabled
        std::map<EVENT, bool> eventnames_;

        //! list of tasks that will be called in update
        std::list<task> tasklistupdate_;

        //! list of tasks that will be called in render
        std::list<task> tasklistrender_;

};

class SubjectNull : public Subject
{
    public:

        static SubjectNull nullObj;

        SubjectNull() = default;

        ~SubjectNull() = default;

        void initialize() override {};

        void update() override {};

        void render() override {};

        void shutdown() override {};

        //! returns if the current object is null or not null
        bool isNull() const override { return true; };

        void addObserver(const std::string& observername, Observer* observer) override {};

        void removeObserver(const std::string& observername) override {};

        void loadEvents(const std::string &filename = "Default") override {};


        EVENT translateString(const std::string& name) override { return EVENT::INVALID; };

    protected:

        virtual void notify(enum  EVENT event, Entity* entity = &EntityNull::nullObj) override {};

};

//---------------------------------------------------------------------------------------------------------------------