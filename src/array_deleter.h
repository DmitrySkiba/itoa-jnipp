/*
 * Copyright (C) 2011 Dmitry Skiba
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ARRAYDELETER_INCLUDED_
#define _ARRAYDELETER_INCLUDED_

///////////////////////////////////////////////////////////////////// array_deleter

/** As auto_ptr<> can't deal with arrays one has to come up 
 * with utility like this, which only purpose is to delete
 * array in destructor.
 */
template <class T>
class array_deleter {
public:
    explicit array_deleter(T* array):
        m_array(array)
    {
    }
    ~array_deleter() {
        delete[] m_array;
    }

    /** Detaches from the array by nullyfing stored array pointer.
     * Handy when you want the array to escape the scope.
     */
    void detach() {
        m_array=0;
    }
private:
    array_deleter(const array_deleter&);
    array_deleter& operator=(const array_deleter&);
private:
    T* m_array;
};

/////////////////////////////////////////////////////////////////////

#endif // _ARRAYDELETER_INCLUDED_
