/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#ifndef LIB_PBOP_CRITICAL_SECTION
#define LIB_PBOP_CRITICAL_SECTION

namespace pbop
{

  /// <summary>
  /// Protect a section of code against concurrent access.
  /// </summary>
  class CriticalSection
  {
  private:
    struct PImpl;
    PImpl * impl_;

  public:
    CriticalSection();
    ~CriticalSection();
  private:
    CriticalSection(const CriticalSection & copy); //disable copy constructor.
    CriticalSection & operator =(const CriticalSection & other); //disable assignment operator.
  public:

    /// <summary>
    /// Enter the critical section. Blocks other threads until a Unlock() call is processed.
    /// </summary>
    void Lock();

    /// <summary>
    /// Leave the critical section.
    /// </summary>
    void Unlock();
  };

}; //namespace pbop

#endif //LIB_PBOP_CRITICAL_SECTION