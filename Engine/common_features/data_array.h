/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PGE_DATAARRAY_H
#define PGE_DATAARRAY_H

#include <cstdlib>
#include <cstring>

/*!
 * This is a simple dynamic array implementation made especially for PGE Configuration definitions storing
 */
template<class T>
class PGE_DataArray
{
    public:
        /*!
         * \brief Constructor
         */
        PGE_DataArray() : m_data(NULL), m_total_elements(0), m_size(0), m_stored(0) {}

        /*!
         * \brief Copy Constructor
         * \param other Other object of PGE_DataArray class
         */
        PGE_DataArray(const PGE_DataArray &other)
            : m_data(NULL),
              m_size(other.m_size),
              m_total_elements(other.m_total_elements),
              m_stored(other.m_stored)
        {
            if(m_size > 0)
            {
                m_data = new T[m_size];

                for(int i = 0; i < m_size; i++)
                    m_data[i] = other.m_data[i];
            }
        }

        const PGE_DataArray &operator=(const PGE_DataArray &other)
        {
            if(this == &other) return *this;

            clear();
            m_size = other.m_size;

            if(m_size > 0)
            {
                m_data = new T[m_size];

                for(unsigned long i = 0; i < m_size; i++)
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

        bool allocateSlots(unsigned long number)
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

        void storeElement(unsigned long ItemID, T &element)
        {
            if((ItemID < 0) || (ItemID > m_total_elements))
                return;//Avoid out of range

            m_data[ItemID] = element;
            m_stored++;
        }

        /*
        T &operator[](const int &ElementID)
        {
            if((ElementID < 0) || (static_cast<unsigned long>(ElementID) > m_total_elements))
                return m_data[0]; //Avoid out of range
            else if(m_data)
                return m_data[ElementID];
            else
                return m_dummy_element;
        }
        */

        T &operator[](const unsigned long &ElementID)
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
            return ((ElementID >= 0) && (static_cast<unsigned long>(ElementID) <= m_total_elements));
        }
        */

        bool contains(unsigned long ElementID)
        {
            return (ElementID <= m_total_elements);
        }

        unsigned long size()
        {
            return m_size;
        }

        unsigned long stored()
        {
            return m_stored;
        }

        unsigned long total()
        {
            return m_total_elements;
        }

    private:
        T  m_dummy_element;
        T *m_data;
        unsigned long m_total_elements;
        unsigned long m_size;
        unsigned long m_stored;
};

#endif // PGE_DATAARRAY_H
