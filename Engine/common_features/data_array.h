/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef PGE_DATAARRAY_H
#define PGE_DATAARRAY_H

#include <cstdlib>
#include <cstring>

/*!
 * This is a simple dynamic array implementation made especially for PGE Configuration definitions storing
 */
template<class T, typename size_type = unsigned long>
class PGE_DataArray
{
public:
    /*!
     * \brief Constructor
     */
    PGE_DataArray() = default;

    /*!
     * \brief Copy Constructor
     * \param other Other object of PGE_DataArray class
     */
    PGE_DataArray(const PGE_DataArray &other)
        : m_total_elements(other.m_total_elements),
          m_size(other.m_size),
          m_stored(other.m_stored)
    {
        if(m_size > 0)
        {
            m_data = new T[m_size];
            for(size_type i = 0; i < m_size; i++)
                m_data[i] = other.m_data[i];
        }
    }

    PGE_DataArray &operator=(const PGE_DataArray &other)
    {
        if(this == &other) return *this;

        clear();
        m_size = other.m_size;

        if(m_size > 0)
        {
            m_data = new T[m_size];

            for(size_type i = 0; i < m_size; i++)
                m_data[i] = other.m_data[i];
        }

        m_stored = other.m_stored;
        m_total_elements = other.m_total_elements;
        m_dummy_element = other.m_dummy_element;
        return *this;
    }

    /*!
     * \brief Destructor
     */
    ~PGE_DataArray()
    {
        clear();
    }

    /*!
     * \brief Removes all data and releases memory allocation
     */
    void clear()
    {
        if(m_data)
            delete[] m_data;

        m_data = NULL;
        m_size = 0;
        m_stored = 0;
        m_total_elements = 0;
    }

    bool allocateSlots(size_type number)
    {
        if(number == 0) return false;

        if(m_data == NULL)
        {
            m_data = new T[number + 1];

            if(!m_data) return false;

            m_size = number + 1;
            m_total_elements = number;
            m_data[0] = T();
        }

        return true;
    }

    void storeElement(size_type ItemID, T &element)
    {
        if((ItemID < 0) || (ItemID > m_total_elements))
            return;//Avoid out of range

        m_data[ItemID] = element;
        m_stored++;
    }

    /*
    T &operator[](const int &ElementID)
    {
        if((ElementID < 0) || (static_cast<size_type>(ElementID) > m_total_elements))
            return m_data[0]; //Avoid out of range
        else if(m_data)
            return m_data[ElementID];
        else
            return m_dummy_element;
    }
    */

    T &operator[](const size_type &ElementID)
    {
        if(ElementID > m_total_elements)
            return m_data[0]; //Avoid out of range
        else if(m_data)
            return m_data[ElementID];
        else
            return m_dummy_element;
    }

    /*
    bool contains(int ElementID)
    {
        return ((ElementID >= 0) && (static_cast<size_type>(ElementID) <= m_total_elements));
    }
    */

    bool contains(size_type ElementID)
    {
        return (ElementID <= m_total_elements);
    }

    size_type size()
    {
        return m_size;
    }

    size_type stored()
    {
        return m_stored;
    }

    size_type total()
    {
        return m_total_elements;
    }

private:
    T  m_dummy_element = T();
    T *m_data = nullptr;
    size_type m_total_elements = 0;
    size_type m_size = 0;
    size_type m_stored = 0;
};

#endif // PGE_DATAARRAY_H
