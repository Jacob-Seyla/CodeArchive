//---------------------------------------------------------------------------------------------------------------------
//
// File Name:   Subject.cpp
// Author(s):   Jacob Lon (jacob.lon)
// Project:     Lost in Transit
// Course:      GAM200
//
// Copyright © 2021 DigiPen (USA) Corporation.
//
//---------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Observer.h"
#include "Subject.h"
#include "TraceLog.h"
#include "json.hpp"
#include "fstream"

//---------------------------------------------------------------------------------------------------------------------
// Private Consts:
//---------------------------------------------------------------------------------------------------------------------

Subject* Subject::instance_;
SubjectNull SubjectNull::nullObj;

enum class EVENT;

//---------------------------------------------------------------------------------------------------------------------
// Public Functions:
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
// Private Functions:
//---------------------------------------------------------------------------------------------------------------------

/**
 * Retrieves the instance of the singlton system
 *
 * @return
 *   instance of the system
 */
Subject *Subject::getInstance()
{
    if (!instance_)
    {
        instance_ = new Subject;
        instance_->setName("Subject");
    }

    return instance_;
}

/**
 * Adds a system to the list of Observers
 *
 * @param observername
 *   name of the system
 *
 * @param observer
 *   pointer to the instance of the system
 */
void Subject::addObserver(const std::string &observername, Observer *observer)
{
    observers_.emplace(observername, observer);
    std::cout << observername << " added to observer list" << std::endl;
}

/**
 * Removes an Observer from list, doesnt do anything
 * with the pointer, just removes element
 *
 * @param observername
 *   name of the system to remove from list
 */
void Subject::removeObserver(const std::string &observername)
{
    observers_.erase(observername);
}

/**
 * Notifies all observers that an event happened
 * with an entity, if the event is enabled
 *
 * @param entity
 *   entity involved in the event
 *
 * @param event
 *   what happened that any system should no about (enum stored in Observer.cpp)
 */
void Subject::notify(enum class EVENT event, Entity *entity)
{
    if (eventnames_[event])
    {
        for (auto &[key, value]: observers_)
        {
            value->onNotify(entity, event);
        }
    }
}

/**
 * currently calls tracelog, initializes WIN and WINFIRSTOCCURRANCE events
 */
void Subject::initialize()
{
    GET_SYSTEM(TraceLog)->TraceLogMessage("Subject", __func__);
    loadEvents("default");
}

/**
 * calls/updates all of the tasks in update list
 */
void Subject::update()
{
    for (auto itbegin = tasklistupdate_.begin(); itbegin != tasklistupdate_.end();)
    {
        itbegin->func();
        itbegin->time--;
        if (itbegin->time <= 0)
        {
           itbegin = tasklistupdate_.erase(itbegin);
        } else
        {
            ++itbegin;
        }
    }
}

/**
 * calls/updates all of the tasks in the render list
 */
void Subject::render()
{
    for (auto itbegin = tasklistrender_.begin(); itbegin != tasklistrender_.end();)
    {
        itbegin->func();
        itbegin->time--;
        if (itbegin->time <= 0)
        {
            itbegin = tasklistrender_.erase(itbegin);
        } else
        {
            ++itbegin;
        }
    }
}

/**
 * currently calls tracelog
 */
void Subject::shutdown()
{
    for (auto itbegin = observers_.begin(); itbegin != observers_.end(); )
    {
        itbegin = observers_.erase(itbegin);
    }
    resetEvents();
    GET_SYSTEM(TraceLog)->TraceLogMessage("Subject", __func__);
}

/**
 * Enables or Disables an event; true means enabled
 *
 * @param name
 *   name of the event, from enum EVENT in observer.h
 *
 * @param flag
 *   flag for enable/disable
 */
void Subject::setEvent(enum class EVENT name, bool flag)
{
    eventnames_.insert_or_assign(name, flag);
}

/**
 * Reads whether an event is set to true or false
 *
 * @param name
 *   name from enum EVENT
 *
 * @return
 *   true fro enabled
 */
bool Subject::readEvent(enum class EVENT name)
{
    if(eventnames_.find(name) != eventnames_.end())
    {
        return eventnames_.at(name);
    }else
    {
        GET_SYSTEM(TraceLog)->TraceLogMessageError("Event not found");
    }
    return false;
}

/**
 * resets and enables all events plus clears both task lists
 */
void Subject::resetEvents()
{
    for(auto &[key, value] : eventnames_)
    {
        eventnames_.insert_or_assign(key, true);
    }
    tasklistupdate_.clear();
    tasklistrender_.clear();
}

/**
 * Creates and adds a task object to a system list
 *
 * @param job
 *   pointer to the function, usually a lambda will be passed
 *
 * @param type
 *   type of functipn code either "update" or "render"
 *
 * @param dofor
 *   how long to call the function for
 *
 * @param unit
 *   the unit the time is given in. "cycle" by default or "seconds" in which case dofor will be translated to update cycles
 */
void Subject::addtask(const std::function<void(void)> &job, const std::string &type, int dofor, const std::string &unit)
{
    struct task newtask;
    newtask.time = dofor;
    newtask.func = job;
    if (unit == "seconds")
        newtask.time *= 60;

    if (type == "update")
        tasklistupdate_.push_back(newtask);
    else if (type == "render")
        tasklistrender_.push_back(newtask);
}

void Subject::loadEvents(const std::string &filename)
{
    std::string file = "assets/" + filename + ".json";
    std::ifstream jsonfile;
    jsonfile.open(file);

    if (jsonfile)
    {
        nlohmann::json j = nlohmann::json::parse(jsonfile);

        {
            auto active = j.at("Active");
            int limit = static_cast<int>( active.size());
            std::cout << "Active" << std::endl;
            for (int i = 0; i < limit; i++)
            {
                std::string name = active.at(i);
                std::cout << name << std::endl;
                EVENT toadd = translateString(name);
                setEvent(toadd, true);
            }
            std::cout << "After Active" << std::endl;
        }

        {
            auto inactive = j.at("NotActive");
            std::cout << "NotActive" << std::endl;
            int limit = static_cast<int>(inactive.size());
            for (int i = 0; i < limit; i++)
            {
                std::string name = inactive.at(i);
                std::cout << name << std::endl;
                EVENT toadd = translateString(name);
                setEvent(toadd, false);
            }
            std::cout << "After NotActive" << std::endl;
        }

        jsonfile.close();
    } else
    {
        GET_SYSTEM(TraceLog)->TraceLogMessageError("EVENT FILE NOT OPENED");
    }

}

EVENT Subject::translateString(const std::string &name)
{
    if (name == "WIN") return EVENT::WIN;

    if (name == "DISABLEPACKAGE") return EVENT::DISABLEPACKAGE;

    if (name == "ENABLEPACKAGE") return EVENT::ENABLEPACKAGE;

    if (name == "WINFIRSTOCCURRANCE") return EVENT::WINFIRSTOCCURRANCE;

    if (name == "CREATE") return EVENT::CREATE;

    if (name == "RESETWIN") return EVENT::RESETWIN;


    return EVENT::INVALID;
}




