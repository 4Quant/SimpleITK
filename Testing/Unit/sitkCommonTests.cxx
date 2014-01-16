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
#include <SimpleITKTestHarness.h>

#include <sitkConditional.h>
#include <sitkCommand.h>
#include <sitkCastImageFilter.h>

TEST( ConditionalTest, ConditionalTest1 ) {

  // a quick check to make sure the conditional works
  typedef itk::simple::Conditional<true, int, float>::Type IntType;
  typedef itk::simple::Conditional<false, int, float>::Type FloatType;


  EXPECT_EQ ( typeid( IntType ).name(), typeid( int ).name() );
  EXPECT_EQ ( typeid( FloatType ).name(), typeid( float ).name() );

  return;

}

TEST( ProcessObject, GlobalWarning ) {
  // Basic coverage test of setting and getting. Need separate
  // specific check for propagation of warning to ITK.

  namespace sitk = itk::simple;

  EXPECT_TRUE( sitk::ProcessObject::GetGlobalWarningDisplay() );

  sitk::ProcessObject::GlobalWarningDisplayOff();
  EXPECT_FALSE( sitk::ProcessObject::GetGlobalWarningDisplay() );

  sitk::ProcessObject::GlobalWarningDisplayOn();
  EXPECT_TRUE( sitk::ProcessObject::GetGlobalWarningDisplay() );

  sitk::ProcessObject::SetGlobalWarningDisplay(false);
  EXPECT_FALSE( sitk::ProcessObject::GetGlobalWarningDisplay() );

}


TEST( ProcessObject, Command_Register ) {
  // Test the references between Process Objects and command.
  // Try to be mean and break stuff

  namespace sitk = itk::simple;

  // Case 0a: stack,  command first
  {
  sitk::Command cmd;
  sitk::CastImageFilter po1;
  po1.AddCommand(sitk::sitkAnyEvent, cmd);
  EXPECT_TRUE(po1.HasCommand(sitk::sitkAnyEvent));
  }

  // Case 0b: stack, process first
  {
  sitk::CastImageFilter po1;
  sitk::Command cmd;
  po1.AddCommand(sitk::sitkAnyEvent, cmd);
  EXPECT_TRUE(po1.HasCommand(sitk::sitkAnyEvent));
  }

  // Case 1a: single command, command deleted first
  {
  std::auto_ptr<sitk::CastImageFilter> po1(new sitk::CastImageFilter());
  std::auto_ptr<sitk::Command> cmd(new sitk::Command());
  po1->AddCommand(sitk::sitkAnyEvent, *cmd);

  EXPECT_TRUE(po1->HasCommand(sitk::sitkAnyEvent));
  cmd.reset();
  EXPECT_FALSE(po1->HasCommand(sitk::sitkAnyEvent));
  }

  // Case 1b: single command, process deleted first
  {
  std::auto_ptr<sitk::CastImageFilter> po1( new sitk::CastImageFilter());
  std::auto_ptr<sitk::Command> cmd(new sitk::Command());
  po1->AddCommand(sitk::sitkAnyEvent, *cmd);
  po1.reset();
  }

  // Case 2a: single command, multiple processes, command deleted first
  {
  std::auto_ptr<sitk::CastImageFilter> po1(new sitk::CastImageFilter());
  std::auto_ptr<sitk::CastImageFilter> po2(new sitk::CastImageFilter());
  std::auto_ptr<sitk::CastImageFilter> po3(new sitk::CastImageFilter());

  std::auto_ptr<sitk::Command> cmd(new sitk::Command());
  po1->AddCommand(sitk::sitkAnyEvent, *cmd);
  po2->AddCommand(sitk::sitkStartEvent, *cmd);
  po3->AddCommand(sitk::sitkEndEvent, *cmd);
  cmd.reset();
  }

  // Case 2b: single command, multiple processes, processes mostly deleted first
  {
  std::auto_ptr<sitk::CastImageFilter> po1(new sitk::CastImageFilter());
  std::auto_ptr<sitk::CastImageFilter> po2(new sitk::CastImageFilter());
  std::auto_ptr<sitk::CastImageFilter> po3(new sitk::CastImageFilter());

  std::auto_ptr<sitk::Command> cmd(new sitk::Command());
  po1->AddCommand(sitk::sitkAnyEvent, *cmd);
  po2->AddCommand(sitk::sitkStartEvent, *cmd);
  po3->AddCommand(sitk::sitkEndEvent, *cmd);

  EXPECT_TRUE(po1->HasCommand(sitk::sitkAnyEvent));
  EXPECT_TRUE(po2->HasCommand(sitk::sitkStartEvent));
  EXPECT_TRUE(po3->HasCommand(sitk::sitkEndEvent));

  po1.reset();
  EXPECT_TRUE(po2->HasCommand(sitk::sitkStartEvent));
  EXPECT_TRUE(po3->HasCommand(sitk::sitkEndEvent));
  po2.reset();
  EXPECT_TRUE(po3->HasCommand(sitk::sitkEndEvent));
  cmd.reset();
  EXPECT_FALSE(po3->HasCommand(sitk::sitkEndEvent));
  }

  // Case 3a: multiple commands, command deleted most first
  {
  std::auto_ptr<sitk::CastImageFilter> po1(new sitk::CastImageFilter());
  std::auto_ptr<sitk::Command> cmd1(new sitk::Command());
  std::auto_ptr<sitk::Command> cmd2(new sitk::Command());
  std::auto_ptr<sitk::Command> cmd3(new sitk::Command());
  po1->AddCommand(sitk::sitkAnyEvent, *cmd1);
  po1->AddCommand(sitk::sitkStartEvent, *cmd2);
  po1->AddCommand(sitk::sitkEndEvent, *cmd3);

  EXPECT_TRUE(po1->HasCommand(sitk::sitkAnyEvent));
  EXPECT_TRUE(po1->HasCommand(sitk::sitkStartEvent));
  EXPECT_TRUE(po1->HasCommand(sitk::sitkEndEvent));

  cmd1.reset();
  EXPECT_FALSE(po1->HasCommand(sitk::sitkAnyEvent));
  EXPECT_TRUE(po1->HasCommand(sitk::sitkStartEvent));
  EXPECT_TRUE(po1->HasCommand(sitk::sitkEndEvent));
  cmd2.reset();
  EXPECT_FALSE(po1->HasCommand(sitk::sitkAnyEvent));
  EXPECT_FALSE(po1->HasCommand(sitk::sitkStartEvent));
  EXPECT_TRUE(po1->HasCommand(sitk::sitkEndEvent));
  po1.reset();
  }

  // Case 3b: multiple commands, command deleted first
  {
  std::auto_ptr<sitk::CastImageFilter> po1(new sitk::CastImageFilter());
  std::auto_ptr<sitk::Command> cmd1(new sitk::Command());
  std::auto_ptr<sitk::Command> cmd2(new sitk::Command());
  std::auto_ptr<sitk::Command> cmd3(new sitk::Command());
  po1->AddCommand(sitk::sitkAnyEvent, *cmd1);
  po1->AddCommand(sitk::sitkStartEvent, *cmd2);
  po1->AddCommand(sitk::sitkEndEvent, *cmd3);
  po1.reset();

  EXPECT_TRUE(po1->HasCommand(sitk::sitkAnyEvent));
  EXPECT_TRUE(po1->HasCommand(sitk::sitkStartEvent));
  EXPECT_TRUE(po1->HasCommand(sitk::sitkEndEvent));
  }


}

TEST( ProcessObject, Command_Add ) {
  // Add command for events and verifies the state

  namespace sitk = itk::simple;

  sitk::CastImageFilter po1;
  sitk::Command cmd;

  // check initial state
  EXPECT_FALSE(po1.HasCommand(sitk::sitkAnyEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkAbortEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkDeleteEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkEndEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkIterationEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkProgressEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkStartEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkUserEvent));

  po1.AddCommand(sitk::sitkAnyEvent, cmd);
  EXPECT_TRUE(po1.HasCommand(sitk::sitkAnyEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkAbortEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkDeleteEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkEndEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkIterationEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkProgressEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkStartEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkUserEvent));

  po1.RemoveAllCommands();
  EXPECT_FALSE(po1.HasCommand(sitk::sitkAnyEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkAbortEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkDeleteEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkEndEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkIterationEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkProgressEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkStartEvent));
  EXPECT_FALSE(po1.HasCommand(sitk::sitkUserEvent));

  po1.AddCommand(sitk::sitkAnyEvent, cmd);
  po1.AddCommand(sitk::sitkAbortEvent, cmd);
  po1.AddCommand(sitk::sitkDeleteEvent, cmd);
  po1.AddCommand(sitk::sitkEndEvent, cmd);
  po1.AddCommand(sitk::sitkIterationEvent, cmd);
  po1.AddCommand(sitk::sitkProgressEvent, cmd);
  po1.AddCommand(sitk::sitkStartEvent, cmd);
  po1.AddCommand(sitk::sitkUserEvent, cmd);

  EXPECT_TRUE(po1.HasCommand(sitk::sitkAnyEvent));
  EXPECT_TRUE(po1.HasCommand(sitk::sitkAbortEvent));
  EXPECT_TRUE(po1.HasCommand(sitk::sitkDeleteEvent));
  EXPECT_TRUE(po1.HasCommand(sitk::sitkEndEvent));
  EXPECT_TRUE(po1.HasCommand(sitk::sitkIterationEvent));
  EXPECT_TRUE(po1.HasCommand(sitk::sitkProgressEvent));
  EXPECT_TRUE(po1.HasCommand(sitk::sitkStartEvent));
  EXPECT_TRUE(po1.HasCommand(sitk::sitkUserEvent));
}



TEST( Command, CommandTest1 ) {
  // Basic test.
  namespace sitk = itk::simple;

  sitk::Command cmd1;
  // not copy construct able
  //sitk::Command cmd2(cmd1);

  // not assignable
  //cmd1 = cmd1;

  // Does nothing
  cmd1.Execute();
}
