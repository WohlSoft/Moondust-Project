#ifndef HISTORY_MANAGER_H
#define HISTORY_MANAGER_H

#include <QList>
#include <QSharedPointer>

template<class T>
class HistoryManagerElementNEW
{
    friend class HistoryManagerNEW;
    T*      m_scene;
public:
    HistoryManagerElementNEW(T* scene):
        m_scene(scene)
    {}
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual QString actionDesc()  = 0;
};

template<class T>
class HistoryManagerNEW
{
    friend class T;
    T*      m_scene;
    int     m_historyIndex;
    int     m_maxHistoryLength;
    QList<QSharedPointer<HistoryManagerElementNEW<T> > > m_operationList;
public:
    HistoryManagerNEW(T* scene):
        m_scene(scene),
        m_historyIndex(0),
        m_maxHistoryLength(300)
    {}

    HistoryManagerNEW(const HistoryManagerNEW &hm):
        m_scene(hm.m_scene),
        m_historyIndex(hm.m_historyIndex),
        m_maxHistoryLength(hm.m_maxHistoryLength),
        m_operationList(hm.m_operationList)
    {}

    ~HistoryManagerNEW();

    virtual void historyBack()
    {
        if(!canUndo())
            return;
        m_historyIndex--;
        QSharedPointer<HistoryManagerElementNEW<T>> lastOperation = m_operationList[m_historyIndex];
        lastOperation->undo();
    }

    virtual void historyForward()
    {
        if(!canRedo())
            return;
        QSharedPointer<HistoryManagerElementNEW<T>> lastOperation = m_operationList[m_historyIndex];
        lastOperation->redo();
        m_historyIndex++;
    }

    virtual int  getHistroyIndex() { return m_historyIndex;}
    virtual bool canUndo() { return m_historyIndex > 0; }
    virtual bool canRedo() { return m_historyIndex < m_operationList.size(); }

    virtual void addHistory(HistoryManagerElementNEW<T>* element)
    {
        Q_ASSERT(element && "Null history element!");
        //add cleanup redo elements
        updateHistoryBuffer();
        element->m_scene = m_scene;
        m_operationList.push_back(QSharedPointer<HistoryManagerElementNEW<T>(element));
        m_historyIndex++;
    }

    void setMaxHistoryLength(int maxlen)
    {
        if(maxlen < 25)
            return;
        m_maxHistoryLength = maxlen;
    }

protected:
    virtual void updateHistoryBuffer()
    {
        if(canRedo())
        {
            int lastSize = m_operationList.size();
            for(int i = m_historyIndex; i < lastSize; i++)
            {
                m_operationList.pop_back();
            }
        }
        while(m_operationList.size() >= m_maxHistoryLength)
        {
            m_operationList.pop_front();
            m_historyIndex--;
        }
    }
};

#endif // HISTORY_MANAGER_H
