/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/
#ifndef __sitkProcessObject_h
#define __sitkProcessObject_h

#include "sitkCommon.h"
#include "sitkNonCopyable.h"
#include "sitkTemplateFunctions.h"
#include "sitkEvent.h"
#include "sitkImage.h"

#include <iostream>
#include <list>

namespace itk {

#ifndef SWIG
  class ProcessObject;
  class Command;
  class EventObject;
#endif

  namespace simple {

  class Command;


  /** \class ProcessObject
   * \brief Base class for SimpleITK classes based on ProcessObject
   *
   */
  class SITKCommon_EXPORT ProcessObject:
      protected NonCopyable
  {
    public:
      typedef ProcessObject Self;

      /**
       * Default Constructor that takes no arguments and initializes
       * default parameters
       */
      ProcessObject();

      /**
       * Default Destructor
       */
      virtual ~ProcessObject();

      // Print ourselves out
      virtual std::string ToString() const = 0;

      /** return user readable name for the filter */
      virtual std::string GetName() const = 0;

      /** Turn debugging output on/off.
       *
       * Enabling debugging prints additional information to stdout
       * about the execution of the internal filters.
       * @{
       */
      virtual void DebugOn();
      virtual void DebugOff();
      /**@}*/

      /** Get the value of the debug flag.
       * @{
       */
      virtual bool GetDebug() const;
      virtual void SetDebug(bool debugFlag);
      /**@}*/

      /** Turn default debugging output value on/off.
       *
       * This is the initial values used for new classes and
       * procedural methods.
       */
      static void GlobalDefaultDebugOn();
      static void GlobalDefaultDebugOff();
      /**@}*/

      /** Get the value of the default debug flag.  */
      static bool GetGlobalDefaultDebug();
      static void SetGlobalDefaultDebug(bool debugFlag);
      /**@}*/

      /** Manage warnings produced by ITK.
       *
       * Enabled by default, this parameter may enable printing
       * of warnings indicating unstable state or parameters during
       * execution. It is a global value set for all ITK filters and
       * processes.
       * @{
       */
      static void GlobalWarningDisplayOn();
      static void GlobalWarningDisplayOff();
      static void SetGlobalWarningDisplay(bool flag);
      static bool GetGlobalWarningDisplay();
      /**@}*/

      /** Set the number of threads that all new process objects are
       *  initialized with.
       * @{
       */
      static void SetGlobalDefaultNumberOfThreads(unsigned int n);
      static unsigned int GetGlobalDefaultNumberOfThreads();
      /**@}*/

      /** The number of threads used when executing a filter if the
       * filter is multi-threaded
       * @{
       */
      virtual void SetNumberOfThreads(unsigned int n);
      virtual unsigned int GetNumberOfThreads() const;
      /**@}*/

      /** \brief Add a Command Object to observer the event.
       *
       * The Command object's Execute method will be invoked when the
       * internal ITK Object has the event. These events only occur
       * during this ProcessObject's Execute method when the ITK
       * filter is running. The command occurs in the same thread as
       * this objects Execute methods was called in.
       *
       * An internal reference is made between the Command and this
       * ProcessObject which enable automatic removal of the command
       * when deleted. This enables both object to exist as stack
       * based object and be automatically cleaned up.
       *
       * Unless specified otherwise, it's safe to get any value during
       * execution. "Measurements" will have valid values only after
       * the Execute method has returned. "Active Measurements" will
       * have valid values during events, and access the underlying
       * ITK object.
       *
       * Deleting a command this object has during a command call-back
       * will produce undefined behavior.
       *
       * For more information see the page \ref CommandPage.
       *
       * \note The return value is reserved for latter usage.
       */
      virtual int AddCommand(itk::simple::EventEnum event, itk::simple::Command &cmd);

      /** \brief Remove all registered commands.
       *
       * Calling when this object is invoking anther command will
       * produce undefined behavior.
       */
      virtual void RemoveAllCommands();

      /** \brief Query of this object has any registered commands for event. */
      virtual bool HasCommand( itk::simple::EventEnum event ) const;


      /** \brief An Active Measurement of the progress of execution.
       *
       * Get the execution progress of the current process object. The
       * progress is a floating number in [0,1] with 0 meaning no progress and 1
       * meaning the filter has completed execution (or aborted).
       *
       * This is an Active Measurement so it can be accessed during
       * Events during the execution.
       */
      virtual float GetProgress( ) const;

      /** Sets an abort flag on the active process.
       *
       * Requests the current active process to abort. Additional,
       * progress or iteration event may occur. If aborted then, an
       * AbortEvent should occur. The Progress should be set to 1.0
       * after aborting.
       *
       * The expected behavior is that not exception should be throw
       * out of this processes Execute method. Additionally, the
       * results returned are valid but undefined content. The
       * content may be only partially updated, uninitialized or the a
       * of size zero.
       *
       * If there is no active process the method has no effect.
       */
      virtual void Abort();

    protected:

      #ifndef SWIG

      struct EventCommand
      {
        EventCommand(EventEnum e, Command *c)
          : m_Event(e), m_Command(c), m_ITKTag(std::numeric_limits<unsigned long>::max())
          {}
        EventEnum     m_Event;
        Command *     m_Command;

        // set to max if currently not registered
        unsigned long m_ITKTag;

        inline bool operator==(const EventCommand &o)
          { return m_Command == o.m_Command; }
        inline bool operator<(const EventCommand &o)
          { return m_Command < o.m_Command; }
      };

      // method called before filter update to set parameters and
      // connect commands.
      virtual void PreUpdate( itk::ProcessObject *p );

      // overidable method to add a command.
      virtual unsigned long AddITKObserver(const itk::EventObject &, itk::Command *);
      virtual void RemoveITKObserver( EventCommand &e );

      // returns the current active process, if no active process then
      // an exception is throw.
      virtual itk::ProcessObject *GetActiveProcess( );

      // overidable callback when the active process has completed
      virtual void OnActiveProcessDelete( );

      friend class itk::simple::Command;
      // method call by command when it's deleted, maintains internal
      // references between command and process objects.
      virtual void onCommandDelete(const itk::simple::Command *cmd) throw();
      #endif


      template< class TImageType >
      static typename TImageType::ConstPointer CastImageToITK( const Image &img )
      {
        typename TImageType::ConstPointer itkImage =
          dynamic_cast < const TImageType* > ( img.GetITKBase() );

        if ( itkImage.IsNull() )
          {
          sitkExceptionMacro( "Unexpected template dispatch error!" );
          }
        return itkImage;
      }

      /**
       * Output operator to os with conversion to a printable type.
       *
       * That is char types are presumed to be numbers, and converted
       * to int.
       * @{
       */
      template <typename T>
      static std::ostream & ToStringHelper(std::ostream &os, const T &v)
      {
        os << v;
        return os;
      }
      static std::ostream & ToStringHelper(std::ostream &os, const char &v);
      static std::ostream & ToStringHelper(std::ostream &os, const signed char &v);
      static std::ostream & ToStringHelper(std::ostream &os, const unsigned char &v);
      /**@}*/

    private:

      // Add command to active process object, the EventCommand's
      // ITKTag must be unset as max or else an exception is
      // thrown. The EventCommand's ITKTag is updated to the command
      // registered to ITK's ProcessObject. If there is not an active
      // process object then max is returned, and no other action
      // occurs.
      unsigned long AddObserverToActiveProcessObject( EventCommand &e );

      void RemoveObserverFromActiveProcessObject( EventCommand &e );

      bool m_Debug;
      unsigned int m_NumberOfThreads;

      std::list<EventCommand> m_Commands;

      itk::ProcessObject *m_ActiveProcess;

      //
      float m_ProgressMeasurement;
    };


  }
}
#endif
