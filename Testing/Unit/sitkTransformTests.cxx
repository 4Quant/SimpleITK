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

#include "sitkTransform.h"
#include "sitkAffineTransform.h"
#include "sitkTranslationTransform.h"
#include "sitkEuler3DTransform.h"
#include "sitkAdditionalProcedures.h"
#include "sitkResampleImageFilter.h"
#include "sitkHashImageFilter.h"
#include "SimpleITKTestHarness.h"

#include "itkMath.h"

namespace sitk = itk::simple;

namespace
{
std::vector<double> v2(double v1, double v2)
{
  std::vector<double> temp(2);
  temp[0]=v1;temp[1]=v2;
  return temp;
}

std::vector<double> v3(double v1, double v2, double v3)
{
  std::vector<double> temp(3);
  temp[0]=v1;temp[1]=v2;temp[2]=v3;
  return temp;
}
}

TEST(TransformTest, Construction) {


  sitk::Transform tx0( 2, sitk::sitkIdentity);
  std::cout << tx0.ToString() << std::endl;
  sitk::Transform tx1( 3, sitk::sitkIdentity);
  std::cout << tx1.ToString() << std::endl;

  sitk::Transform tx2( 2, sitk::sitkTranslation);
  std::cout << tx2.ToString() << std::endl;
  sitk::Transform tx3( 3, sitk::sitkTranslation);
  std::cout << tx3.ToString() << std::endl;

  sitk::Transform tx4( 2, sitk::sitkScale);
  std::cout << tx4.ToString() << std::endl;
  sitk::Transform tx5( 3, sitk::sitkScale);
  std::cout << tx5.ToString() << std::endl;

  sitk::Transform tx6( 2, sitk::sitkScaleLogarithmic);
  std::cout << tx6.ToString() << std::endl;
  sitk::Transform tx7( 3, sitk::sitkScaleLogarithmic);
  std::cout << tx7.ToString() << std::endl;

  sitk::Transform tx_0( 2, sitk::sitkEuler);
  std::cout << tx_0.ToString() << std::endl;
  sitk::Transform tx_1( 3, sitk::sitkEuler);
  std::cout << tx_1.ToString() << std::endl;

  sitk::Transform tx_2( 2, sitk::sitkSimilarity);
  std::cout << tx_2.ToString() << std::endl;
  sitk::Transform tx_3( 3, sitk::sitkSimilarity);
  std::cout << tx_3.ToString() << std::endl;

  EXPECT_ANY_THROW( sitk::Transform tx8( 2, sitk::sitkQuaternionRigid) );
  sitk::Transform tx9( 3, sitk::sitkQuaternionRigid);
  std::cout << tx9.ToString() << std::endl;

  EXPECT_ANY_THROW( sitk::Transform tx10( 2, sitk::sitkVersor) );
  sitk::Transform tx11( 3, sitk::sitkVersor);
  std::cout << tx11.ToString() << std::endl;

  EXPECT_ANY_THROW( sitk::Transform tx12( 2, sitk::sitkVersorRigid) );
  sitk::Transform tx13( 3, sitk::sitkVersorRigid);
  std::cout << tx13.ToString() << std::endl;

  sitk::Transform tx14( 2, sitk::sitkAffine);
  std::cout << tx14.ToString() << std::endl;
  sitk::Transform tx15( 3, sitk::sitkAffine);
  std::cout << tx15.ToString() << std::endl;

  sitk::Transform tx16( 2, sitk::sitkAffine);
  std::cout << tx16.ToString() << std::endl;
  sitk::Transform tx17( 3, sitk::sitkAffine);
  std::cout << tx17.ToString() << std::endl;


  // default constructable
  sitk::Transform tx18;
  std::cout << tx18.ToString() << std::endl;

  // displacement fields
  sitk::Image displacement = sitk::Image( 100, 100, sitk::sitkVectorFloat64 );

  sitk::Transform tx19( displacement );
  std::cout <<  tx19.ToString() << std::endl;
  EXPECT_EQ( displacement.GetSize()[0], 0u );
  EXPECT_EQ( displacement.GetSize()[1], 0u );

  displacement = sitk::Image( 100,100, 100, sitk::sitkVectorFloat64 );
  sitk::Transform tx20( displacement );
  std::cout << tx20.ToString() << std::endl;
  EXPECT_EQ( displacement.GetSize()[0], 0u );
  EXPECT_EQ( displacement.GetSize()[1], 0u );

  ASSERT_THROW( sitk::Transform( 3, sitk::sitkDisplacementField ), sitk::GenericException );
  ASSERT_THROW( sitk::Transform( 2, sitk::sitkDisplacementField ), sitk::GenericException );


  sitk::Image bsplineReference = sitk::Image(10,10, sitk::sitkUInt8);
  sitk::Transform tx21( bsplineReference, sitk::sitkBSplineTransform );

  bsplineReference = sitk::Image(10,10, 10, sitk::sitkUInt8);
  sitk::Transform tx22( bsplineReference, sitk::sitkBSplineTransform );

  ASSERT_THROW( sitk::Transform( 3, sitk::sitkBSplineTransform ), sitk::GenericException );
  ASSERT_THROW( sitk::Transform( 2, sitk::sitkBSplineTransform ), sitk::GenericException );

}

TEST(TransformTest, Copy) {

  // Test the copy constructor and asignment operators

  sitk::Transform tx1( 2, sitk::sitkTranslation);
  sitk::Transform tx2( tx1 );
  sitk::Transform tx3 = sitk::Transform();

  tx1 = sitk::Transform();
  tx2 = tx1;

  // check self assignment
  tx3 = tx3;

}

TEST(TransformTest, SetGetParameters) {

  sitk::Transform tx;
  EXPECT_TRUE( tx.GetParameters().empty() );
  EXPECT_TRUE( tx.GetFixedParameters().empty() );

  tx = sitk::Transform( 3, sitk::sitkTranslation );
  EXPECT_EQ( tx.GetParameters().size(), 3u );
  EXPECT_TRUE( tx.GetFixedParameters().empty() );

  tx = sitk::Transform( 2, sitk::sitkScale );
  EXPECT_EQ( tx.GetParameters().size(), 2u );
  EXPECT_TRUE( tx.GetFixedParameters().empty() );

  tx = sitk::Transform( 3, sitk::sitkScaleLogarithmic );
  EXPECT_EQ( tx.GetParameters().size(), 3u );
  EXPECT_TRUE( tx.GetFixedParameters().empty() );

  tx = sitk::Transform( 2, sitk::sitkEuler );
  EXPECT_EQ( tx.GetParameters().size(), 3u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 2u );

  tx = sitk::Transform( 3, sitk::sitkEuler );
  EXPECT_EQ( tx.GetParameters().size(), 6u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 3u );

  tx = sitk::Transform( 2, sitk::sitkSimilarity );
  EXPECT_EQ( tx.GetParameters().size(), 4u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 2u );

  tx = sitk::Transform( 3, sitk::sitkSimilarity );
  EXPECT_EQ( tx.GetParameters().size(), 7u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 3u );

  tx = sitk::Transform( 3, sitk::sitkQuaternionRigid );
  EXPECT_EQ( tx.GetParameters().size(), 7u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 3u );

  tx = sitk::Transform( 3, sitk::sitkVersor );
  EXPECT_EQ( tx.GetParameters().size(), 3u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 3u );

  tx = sitk::Transform( 3, sitk::sitkVersorRigid );
  EXPECT_EQ( tx.GetParameters().size(), 6u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 3u );

  tx = sitk::Transform( 2, sitk::sitkAffine );
  EXPECT_EQ( tx.GetParameters().size(), 6u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 2u );

  sitk::Image displacement = sitk::Image( 10, 10, sitk::sitkVectorFloat64 );

  tx =  sitk::Transform ( displacement );
  EXPECT_EQ( tx.GetParameters().size(), 200u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 10u );

  displacement = sitk::Image( 10, 10, 10, sitk::sitkVectorFloat64 );

  tx =  sitk::Transform ( displacement );
  EXPECT_EQ( tx.GetParameters().size(), 3000u );
  EXPECT_EQ( tx.GetFixedParameters().size(), 18u );
}

TEST(TransformTest, CopyOnWrite) {

  sitk::Transform tx1 = sitk::Transform( 2, sitk::sitkAffine );
  sitk::Transform tx2 = tx1;
  sitk::Transform tx3 = tx1;

  std::vector<double> params = tx1.GetParameters();
  params[1] = 0.2;

  tx2.SetParameters( params );

  ASSERT_EQ( tx1.GetParameters().size(), 6u );
  EXPECT_EQ( tx1.GetParameters()[1], 0.0 );

  ASSERT_EQ( tx2.GetParameters().size(), 6u );
  EXPECT_EQ( tx2.GetParameters()[1], 0.2 );

  ASSERT_EQ( tx3.GetParameters().size(), 6u );
  EXPECT_EQ( tx3.GetParameters()[1], 0.0 );

  params[1] = 0.3;
  tx3.SetParameters( params );

  ASSERT_EQ( tx1.GetParameters().size(), 6u );
  EXPECT_EQ( tx1.GetParameters()[1], 0.0 );

  ASSERT_EQ( tx2.GetParameters().size(), 6u );
  EXPECT_EQ( tx2.GetParameters()[1], 0.2 );

  ASSERT_EQ( tx3.GetParameters().size(), 6u );
  EXPECT_EQ( tx3.GetParameters()[1], 0.3 );

  tx1 = tx2;

  ASSERT_EQ( tx1.GetParameters().size(), 6u );
  EXPECT_EQ( tx1.GetParameters()[1], 0.2 );

  ASSERT_EQ( tx2.GetParameters().size(), 6u );
  EXPECT_EQ( tx2.GetParameters()[1], 0.2 );

  ASSERT_EQ( tx3.GetParameters().size(), 6u );
  EXPECT_EQ( tx3.GetParameters()[1], 0.3 );


  params[1] = 0.4;
  tx1.SetParameters( params );

  ASSERT_EQ( tx1.GetParameters().size(), 6u );
  EXPECT_EQ( tx1.GetParameters()[1], 0.4 );

  ASSERT_EQ( tx2.GetParameters().size(), 6u );
  EXPECT_EQ( tx2.GetParameters()[1], 0.2 );

  ASSERT_EQ( tx3.GetParameters().size(), 6u );
  EXPECT_EQ( tx3.GetParameters()[1], 0.3 );

}



TEST(TransformTest, AddTransform) {

  sitk::Transform tx1 = sitk::Transform( 2, sitk::sitkAffine );
  tx1.AddTransform( sitk::Transform( 2, sitk::sitkAffine ) );
  // check we can't add miss match dimension
  ASSERT_ANY_THROW( tx1.AddTransform( sitk::Transform( 3, sitk::sitkAffine ) ) );


  sitk::Transform tx2 = tx1;
  tx1.AddTransform( sitk::Transform( 2, sitk::sitkIdentity ) );


  sitk::Transform tx3( 3, sitk::sitkComposite );
  tx1 = tx3;
  tx3.AddTransform( sitk::Transform( 3, sitk::sitkAffine ) );

}

TEST(TransformTest, ReadTransformResample) {

  const char *txFiles[] = {
    "Input/xforms/affine_i_3.txt",
    "Input/xforms/composite_i_3.txt",
    "Input/xforms/i_3.txt",
    "Input/xforms/scale_i_3.txt",
    "Input/xforms/translation_i_3.txt",
    "Input/xforms/quaternion_rigid_i_3.txt",
    "Input/xforms/scale_logarithmic_i_3.txt",
    "Input/xforms/versor_i_3.txt",
    "" // end with zero length string
  };

  sitk::Transform tx;

  sitk::Image img = sitk::ReadImage( dataFinder.GetFile("Input/RA-Short.nrrd" ) );

  const char **ptxFiles = txFiles;
  while( strlen( *ptxFiles ) != 0 )
    {
    std::string fname = dataFinder.GetFile( *ptxFiles );
    std::cout << "Reading: " << *ptxFiles << std::endl;

    EXPECT_NO_THROW( tx = sitk::ReadTransform( fname ) );

    sitk::Image out = sitk::Resample( img, tx, sitk::sitkNearestNeighbor );

    EXPECT_EQ( "126ea8c3ef5573ca1e4e0deece920c2c4a4f38b5", sitk::Hash( out ) ) << "Resampling with identity matrix:" << tx.ToString();

    ++ptxFiles;
    }


}

TEST(TransformTest, TransformPoint) {
  sitk::Transform tx2 = sitk::Transform( 2, sitk::sitkIdentity );
  sitk::Transform tx3 = sitk::Transform( 3, sitk::sitkIdentity );

  std::vector<double> ipt;
  ipt.push_back( 1.1 );
  ipt.push_back( 2.22 );

  std::vector<double> opt;

  opt = tx2.TransformPoint( ipt );
  ASSERT_EQ( opt.size(), 2u );
  EXPECT_EQ( opt[0], 1.1 );
  EXPECT_EQ( opt[1], 2.22 );

  EXPECT_ANY_THROW( tx3.TransformPoint( ipt ) );

  ipt.push_back( 3.333 );

  EXPECT_ANY_THROW( opt = tx2.TransformPoint( ipt ) );
  ASSERT_EQ( opt.size(), 2u );
  EXPECT_EQ( opt[0], 1.1 );
  EXPECT_EQ( opt[1], 2.22 );

  opt = tx3.TransformPoint( ipt );
  ASSERT_EQ( opt.size(), 3u );
  EXPECT_EQ( opt[0], 1.1 );
  EXPECT_EQ( opt[1], 2.22 );
  EXPECT_EQ( opt[2], 3.333 );

}


TEST(TransformTest,AffineTransform)
{
  // test AffineTransform

  const std::vector<double> zeros(3,0.0);
  const std::vector<double> trans2d = v2(2.2,2.2);
  const std::vector<double> trans3d = v3(3.3,3.3,3.3);

  const std::vector<double> center2d(2, 10);
  const std::vector<double> center3d(3, 20);

  const std::vector<double> scale2d = v2(1,2);
  const std::vector<double> scale3d = v3(1,1.2,1.3);

  std::auto_ptr<sitk::AffineTransform> tx;

  // 2d
  EXPECT_NO_THROW( tx.reset( new sitk::AffineTransform(2) ) );
  EXPECT_EQ( tx->GetParameters().size(), 6u );
  EXPECT_EQ( tx->GetFixedParameters().size(), 2u );

  EXPECT_EQ( tx->SetTranslation( trans2d ).GetTranslation(), trans2d );
  EXPECT_EQ( tx->SetCenter( center2d ).GetCenter(), center2d );

  tx.reset( new sitk::AffineTransform(2) );
  tx->Scale( v2(1,2));
  EXPECT_EQ( tx->TransformPoint( v2(0,0) ), v2(0,0));
  EXPECT_EQ( tx->TransformPoint( v2(1,1) ), v2(1,2));
  tx->Scale( 2 );
  EXPECT_EQ( tx->TransformPoint( v2(0,0) ), v2(0,0));
  EXPECT_EQ( tx->TransformPoint( v2(1,1) ), v2(2,4));

  tx.reset( new sitk::AffineTransform(2) );
  tx->Shear(0,1, 2.0);
  EXPECT_EQ( tx->TransformPoint( v2(0,0) ), v2(0,0));
  EXPECT_EQ( tx->TransformPoint( v2(1,2) ), v2(5,2));

  tx.reset( new sitk::AffineTransform(2) );
  tx->Translate(v2(10.0,-10.0));
  EXPECT_EQ( tx->TransformPoint( v2(0,0) ), v2(10.0,-10.0));
  EXPECT_EQ( tx->TransformPoint( v2(1,2) ), v2(11.0,-8.0));

  tx.reset( new sitk::AffineTransform(2) );
  tx->Rotate(0,1,itk::Math::pi_over_2);
  EXPECT_EQ( tx->TransformPoint( v2(0,0) ), v2(0,0));
  EXPECT_EQ( tx->TransformPoint( v2(1,2) ), v2(2,-1));

  // 3d
  EXPECT_NO_THROW( tx.reset( new sitk::AffineTransform(3) ) );
  EXPECT_EQ( tx->GetParameters().size(), 12u );
  EXPECT_EQ( tx->GetFixedParameters().size(), 3u );

  EXPECT_EQ( tx->SetTranslation( trans3d ).GetTranslation(), trans3d );
  EXPECT_EQ( tx->SetCenter( center3d ).GetCenter(), center3d );

  // exceptions
  EXPECT_THROW( tx.reset( new sitk::AffineTransform(1) ), sitk::GenericException );
  EXPECT_THROW( tx.reset( new sitk::AffineTransform(4) ), sitk::GenericException );

}

TEST(TransformTest,Euler2DTransform)
{
}


TEST(TransformTest,Euler3DTransform)
{
  // test Euler3DTransform

  const std::vector<double> center(3,1.1);
  const std::vector<double> zeros(3,0.0);
  const std::vector<double> trans(3, 2.2);

  std::auto_ptr<sitk::Euler3DTransform> tx(new sitk::Euler3DTransform());
  EXPECT_EQ( tx->GetParameters().size(), 6u );
  EXPECT_EQ( tx->GetFixedParameters().size(), 3u );


  tx.reset( new sitk::Euler3DTransform(center));
  EXPECT_EQ( tx->GetParameters().size(), 6u );
  EXPECT_EQ( tx->GetFixedParameters().size(), 3u );
  EXPECT_EQ( tx->GetFixedParameters()[0], 1.1 );
  EXPECT_EQ( tx->GetFixedParameters()[1], 1.1 );
  EXPECT_EQ( tx->GetFixedParameters()[2], 1.1 );
  EXPECT_EQ( tx->GetCenter(), center );


  tx.reset( new sitk::Euler3DTransform(center, 1.0, 2.0, 3.0));
  EXPECT_EQ( tx->GetParameters().size(), 6u );
  EXPECT_EQ( tx->GetFixedParameters().size(), 3u );
  EXPECT_EQ( tx->GetFixedParameters()[0], 1.1 );
  EXPECT_EQ( tx->GetFixedParameters()[1], 1.1 );
  EXPECT_EQ( tx->GetFixedParameters()[2], 1.1 );
  EXPECT_EQ( tx->GetParameters()[0], 1.0 );
  EXPECT_EQ( tx->GetParameters()[1], 2.0 );
  EXPECT_EQ( tx->GetParameters()[2], 3.0 );
  EXPECT_EQ( tx->GetCenter(), center );

  // copy constructor
  sitk::Euler3DTransform tx1( *(tx.get()) );
  EXPECT_EQ( tx1.GetFixedParameters()[0], 1.1 );
  EXPECT_EQ( tx1.GetFixedParameters()[1], 1.1 );
  EXPECT_EQ( tx1.GetFixedParameters()[2], 1.1 );
  EXPECT_EQ( tx1.GetParameters()[0], 1.0 );
  EXPECT_EQ( tx1.GetParameters()[1], 2.0 );
  EXPECT_EQ( tx1.GetParameters()[2], 3.0 );
  EXPECT_EQ( tx1.GetCenter(), center );

  sitk::Euler3DTransform tx2;

  // assignment operator
  tx1 = tx2;
  EXPECT_EQ( tx1.GetCenter(), zeros );

  // copy on write
  tx1.SetFixedParameters(center);
  EXPECT_EQ( tx1.GetFixedParameters()[0], 1.1 );
  EXPECT_EQ( tx1.GetFixedParameters()[1], 1.1 );
  EXPECT_EQ( tx1.GetFixedParameters()[2], 1.1 );
  EXPECT_EQ( tx2.GetFixedParameters()[0], 0.0 );
  EXPECT_EQ( tx2.GetFixedParameters()[1], 0.0 );
  EXPECT_EQ( tx2.GetFixedParameters()[2], 0.0 );
  EXPECT_EQ( tx1.GetCenter(), center );
  EXPECT_EQ( tx2.GetCenter(), zeros );

  tx1 = tx2;
  EXPECT_EQ( tx1.GetCenter(), zeros );
  EXPECT_EQ( tx2.GetCenter(), zeros );
  tx1.SetCenter(center);
  EXPECT_EQ( tx1.GetCenter(), center );
  EXPECT_EQ( tx2.GetCenter(), zeros );

  tx1 = tx2;
  EXPECT_EQ( tx1.GetTranslation(), zeros );
  EXPECT_EQ( tx2.GetTranslation(), zeros );
  tx1.SetTranslation(trans);
  EXPECT_EQ( tx1.GetTranslation(), trans );
  EXPECT_EQ( tx2.GetTranslation(), zeros );

  tx1 = tx2;
  EXPECT_EQ( tx1.GetAngleX(), 0.0 );
  EXPECT_EQ( tx2.GetAngleX(), 0.0 );
  tx1.SetRotation(.1,.2,.3);
  EXPECT_EQ( tx1.GetAngleX(), 0.1 );
  EXPECT_EQ( tx2.GetAngleX(), 0.0 );

  sitk::Transform tx3( *tx );
  tx.reset();

  EXPECT_EQ( tx3.GetParameters().size(), 6u );
  EXPECT_EQ( tx3.GetFixedParameters().size(), 3u );
  EXPECT_EQ( tx3.GetFixedParameters()[0], 1.1 );
  EXPECT_EQ( tx3.GetFixedParameters()[1], 1.1 );
  EXPECT_EQ( tx3.GetFixedParameters()[2], 1.1 );
  EXPECT_EQ( tx3.GetParameters()[0], 1.0 );
  EXPECT_EQ( tx3.GetParameters()[1], 2.0 );
  EXPECT_EQ( tx3.GetParameters()[2], 3.0 );

  tx.reset( new sitk::Euler3DTransform());

  // test member setters/getters
  EXPECT_EQ(tx->GetCenter(), zeros);
  tx->SetCenter(center);
  EXPECT_EQ(tx->GetCenter(), center);

  EXPECT_EQ(tx->GetAngleX(), 0.0);
  EXPECT_EQ(tx->GetAngleY(), 0.0);
  EXPECT_EQ(tx->GetAngleZ(), 0.0);
  tx->SetRotation(1.0,2.0,3.0);
  EXPECT_EQ(tx->GetAngleX(), 1.0);
  EXPECT_EQ(tx->GetAngleY(), 2.0);
  EXPECT_EQ(tx->GetAngleZ(), 3.0);

  EXPECT_EQ(tx->GetTranslation(), zeros);
  tx->SetTranslation(trans);
  EXPECT_EQ(tx->GetTranslation(),trans);

  EXPECT_FALSE(tx->GetComputeZYX());
  tx->SetComputeZYX(true);
  EXPECT_TRUE(tx->GetComputeZYX());
  tx->ComputeZYXOff();
  EXPECT_FALSE(tx->GetComputeZYX());
  tx->ComputeZYXOn();
  EXPECT_TRUE(tx->GetComputeZYX());

}


TEST(TransformTest,Similarity2DTransform)
{
}


TEST(TransformTest,Similarity3DTransform)
{
}


TEST(TransformTest,TranslationTransform)
{
  // test TranslationTransform

  const std::vector<double> zeros(3,0.0);
  const std::vector<double> trans2d(2, 2.2);
  const std::vector<double> trans3d(3, 3.3);

  std::auto_ptr<sitk::TranslationTransform> tx;

  EXPECT_NO_THROW( tx.reset( new sitk::TranslationTransform(2) ) );
  EXPECT_EQ( tx->GetParameters().size(), 2u );
  EXPECT_EQ( tx->GetParameters()[0], 0.0 );
  EXPECT_EQ( tx->GetParameters()[1], 0.0 );
  EXPECT_EQ( tx->GetFixedParameters().size(), 0u );
  EXPECT_EQ( tx->GetOffset(), std::vector<double>(2,0.0));

  EXPECT_NO_THROW( tx.reset( new sitk::TranslationTransform(3) ) );
  EXPECT_EQ( tx->GetParameters().size(), 3u );
  EXPECT_EQ( tx->GetParameters()[0], 0.0 );
  EXPECT_EQ( tx->GetParameters()[1], 0.0 );
  EXPECT_EQ( tx->GetParameters()[2], 0.0 );
  EXPECT_EQ( tx->GetFixedParameters().size(), 0u );
  EXPECT_EQ( tx->GetOffset(), std::vector<double>(3,0.0));

  EXPECT_NO_THROW( tx.reset( new sitk::TranslationTransform(2, trans2d) ) );
  EXPECT_EQ( tx->GetParameters().size(), 2u );
  EXPECT_EQ( tx->GetParameters()[0], 2.2 );
  EXPECT_EQ( tx->GetParameters()[1], 2.2 );
  EXPECT_EQ( tx->GetFixedParameters().size(), 0u );
  EXPECT_EQ( tx->GetOffset(), trans2d);

  // copy constructor
  sitk::TranslationTransform tx1(*(tx.get()));
  EXPECT_EQ( tx1.GetParameters().size(), 2u );
  EXPECT_EQ( tx1.GetParameters()[0], 2.2 );
  EXPECT_EQ( tx1.GetParameters()[1], 2.2 );
  EXPECT_EQ( tx1.GetFixedParameters().size(), 0u );;
  EXPECT_EQ( tx->GetOffset(), trans2d);

  // shallow assignment
  sitk::TranslationTransform tx2(3);
  tx1 = tx2;
  EXPECT_EQ( tx1.GetDimension(), 3u );
  EXPECT_EQ( tx->GetDimension(), 2u );
  EXPECT_EQ( tx1.GetOffset(), std::vector<double>(3,0.0));
  EXPECT_EQ( tx->GetOffset(), trans2d);

  // copy on write
  tx1.SetParameters(std::vector<double>(3, 9.9));
  EXPECT_EQ( tx2.GetOffset(), std::vector<double>(3,0.0) );

  tx2.SetOffset( trans3d );
  EXPECT_EQ(tx2.GetOffset(), trans3d);

  EXPECT_THROW( tx.reset( new sitk::TranslationTransform(3, trans2d) ), sitk::GenericException );
  EXPECT_THROW( tx1.SetOffset(trans2d), sitk::GenericException );

}


TEST(TransformTest,VersorTransform)
{
}
