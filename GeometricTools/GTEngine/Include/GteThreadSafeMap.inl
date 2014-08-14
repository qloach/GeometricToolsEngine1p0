// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Key, typename Value>
ThreadSafeMap<Key,Value>::~ThreadSafeMap()
{
}
//----------------------------------------------------------------------------
template <typename Key, typename Value>
ThreadSafeMap<Key,Value>::ThreadSafeMap()
{
}
//----------------------------------------------------------------------------
template <typename Key, typename Value>
bool ThreadSafeMap<Key,Value>::HasElements() const
{
    bool hasElements;
    mMutex.lock();
    {
        hasElements = (mMap.size() > 0);
    }
    mMutex.unlock();
    return hasElements;
}
//----------------------------------------------------------------------------
template <typename Key, typename Value>
bool ThreadSafeMap<Key,Value>::Exists(Key key) const
{
    bool exists;
    mMutex.lock();
    {
        exists = (mMap.find(key) != mMap.end());
    }
    mMutex.unlock();
    return exists;
}
//----------------------------------------------------------------------------
template <typename Key, typename Value>
void ThreadSafeMap<Key,Value>::Insert(Key key, Value value)
{
    mMutex.lock();
    {
        mMap[key] = value;
    }
    mMutex.unlock();
}
//----------------------------------------------------------------------------
template <typename Key, typename Value>
bool ThreadSafeMap<Key,Value>::Remove(Key key, Value& value)
{
    bool exists;
    mMutex.lock();
    {
        auto iter = mMap.find(key);
        if (iter != mMap.end())
        {
            value = iter->second;
            mMap.erase(iter);
            exists = true;
        }
        else
        {
            exists = false;
        }
    }
    mMutex.unlock();
    return exists;
}
//----------------------------------------------------------------------------
template <typename Key, typename Value>
void ThreadSafeMap<Key,Value>::RemoveAll()
{
    mMutex.lock();
    {
        mMap.clear();
    }
    mMutex.unlock();
}
//----------------------------------------------------------------------------
template <typename Key, typename Value>
bool ThreadSafeMap<Key,Value>::Get(Key key, Value& value) const
{
    bool exists;
    mMutex.lock();
    {
        auto iter = mMap.find(key);
        if (iter != mMap.end())
        {
            value = iter->second;
            exists = true;
        }
        else
        {
            exists = false;
        }
    }
    mMutex.unlock();
    return exists;
}
//----------------------------------------------------------------------------
template <typename Key, typename Value>
void ThreadSafeMap<Key,Value>::GatherAll(std::vector<Value>& values) const
{
    mMutex.lock();
    {
        if (mMap.size() > 0)
        {
            values.resize(mMap.size());
            auto viter = values.begin();
            for (auto const& m : mMap)
            {
                *viter++ = m.second;
            }
        }
        else
        {
            values.clear();
        }
    }
    mMutex.unlock();
}
//----------------------------------------------------------------------------
