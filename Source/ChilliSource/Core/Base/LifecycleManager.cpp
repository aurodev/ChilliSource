//
//  The MIT License (MIT)
//
//  Copyright (c) 2016 Tag Games Limited
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#include <ChilliSource/Core/Base/LifecycleManager.h>

#include <ChilliSource/Core/Base/Application.h>

namespace ChilliSource
{
    //------------------------------------------------------------------------------
    LifecycleManager::LifecycleManager(Application* application) noexcept
        : m_application(application), m_targetLifecycleState(LifecycleState::k_initialised)
    {
        m_mainThread = std::thread(std::bind(&LifecycleManager::ProcessMainThread, this));
        
        std::unique_lock<std::mutex> lock(m_initialisedMutex);
        while (!m_initialised)
        {
            m_initialisedCondition.wait(lock);
        }
    }
    
    //------------------------------------------------------------------------------
    void LifecycleManager::Resume() noexcept
    {
        CS_ASSERT(m_targetLifecycleState == LifecycleState::k_initialised, "");
        m_targetLifecycleState = LifecycleState::k_resumed;
        
        m_activeCondition.notify_one();
    }
    
    //------------------------------------------------------------------------------
    void LifecycleManager::Foreground() noexcept
    {
        CS_ASSERT(m_targetLifecycleState == LifecycleState::k_resumed, "");
        m_targetLifecycleState = LifecycleState::k_foregrounded;
        
        m_activeCondition.notify_one();
    }
    
    //------------------------------------------------------------------------------
    void LifecycleManager::Background() noexcept
    {
        CS_ASSERT(m_targetLifecycleState == LifecycleState::k_foregrounded, "");
        m_targetLifecycleState = LifecycleState::k_resumed;
        
        m_activeCondition.notify_one();
    }
    
    //------------------------------------------------------------------------------
    void LifecycleManager::Suspend() noexcept
    {
        CS_ASSERT(m_targetLifecycleState == LifecycleState::k_resumed, "");
        m_targetLifecycleState = LifecycleState::k_initialised;
        
        m_activeCondition.notify_one();
    }
    
    //------------------------------------------------------------------------------
    void LifecycleManager::Render() noexcept
    {
        CS_ASSERT(m_targetLifecycleState == LifecycleState::k_resumed || m_targetLifecycleState == LifecycleState::k_foregrounded, "");
        
        m_application->Render();
    }

    //------------------------------------------------------------------------------
    void LifecycleManager::ProcessMainThread() noexcept
    {
        while (true)
        {
            BlockIfInactive();
            
            LifecycleState targetLifecycleState = m_targetLifecycleState;
            auto shouldInit = (m_currentLifecycleState == LifecycleState::k_uninitialised && targetLifecycleState != LifecycleState::k_uninitialised);
            auto shouldResume = ((m_currentLifecycleState == LifecycleState::k_uninitialised || m_currentLifecycleState == LifecycleState::k_initialised) &&
                                 (targetLifecycleState == LifecycleState::k_resumed || targetLifecycleState == LifecycleState::k_foregrounded));
            auto shouldForeground = (m_currentLifecycleState != LifecycleState::k_foregrounded && targetLifecycleState == LifecycleState::k_foregrounded);
            auto shouldUpdate = (m_targetLifecycleState == LifecycleState::k_resumed || m_targetLifecycleState == LifecycleState::k_foregrounded);
            auto shouldBackground = (m_currentLifecycleState == LifecycleState::k_foregrounded && targetLifecycleState != LifecycleState::k_foregrounded);
            auto shouldSuspend = ((m_currentLifecycleState == LifecycleState::k_foregrounded || m_currentLifecycleState == LifecycleState::k_resumed) &&
                                  (targetLifecycleState == LifecycleState::k_initialised || targetLifecycleState == LifecycleState::k_uninitialised));
            auto shouldDestroy = (m_currentLifecycleState != LifecycleState::k_uninitialised && targetLifecycleState == LifecycleState::k_uninitialised);
            m_currentLifecycleState = targetLifecycleState;
            
            if (shouldInit)
            {
                m_initTime = std::chrono::system_clock::now();
                
                m_application->Init();
                
                std::unique_lock<std::mutex> lock(m_initialisedMutex);
                m_initialised = true;
                lock.unlock();
                
                m_initialisedCondition.notify_all();
            }
            
            if (shouldResume)
            {
                m_lastUpdateTime = std::chrono::system_clock::now();
                
                m_application->Resume();
            }
            
            if (shouldForeground)
            {
                m_application->Foreground();
            }
            
            if (shouldUpdate)
            {
                auto timeNow = std::chrono::system_clock::now();
                std::chrono::duration<float> deltaTime = timeNow - m_lastUpdateTime;
                std::chrono::duration<double> runningTime = timeNow - m_initTime;
                
                m_lastUpdateTime = timeNow;
                
                m_application->Update(deltaTime.count(), TimeIntervalSecs(runningTime.count()));
            }
            
            if (shouldBackground)
            {
                m_application->Background();
            }
            
            if (shouldSuspend)
            {
                m_application->Suspend();
            }
            
            if (shouldDestroy)
            {
                m_application->Destroy();
                return;
            }
        }
    }
    
    //------------------------------------------------------------------------------
    void LifecycleManager::BlockIfInactive() noexcept
    {
        std::unique_lock<std::mutex> lock(m_mainThreadMutex);
        while (m_targetLifecycleState == m_currentLifecycleState && (m_currentLifecycleState == LifecycleState::k_uninitialised || m_currentLifecycleState == LifecycleState::k_initialised))
        {
            m_activeCondition.wait(lock);
        }
    }
    
    //------------------------------------------------------------------------------
    LifecycleManager::~LifecycleManager() noexcept
    {
        CS_ASSERT(m_targetLifecycleState == LifecycleState::k_initialised, "");
        m_targetLifecycleState = LifecycleState::k_uninitialised;
        
        m_activeCondition.notify_one();
        
        m_mainThread.join();
    }
}