/*=========================================================================
 *
 *  Copyright RTK Consortium
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

#ifndef rtkDrawBoxImageFilter_h
#define rtkDrawBoxImageFilter_h

#include "rtkDrawConvexObjectImageFilter.h"
#include "rtkConfiguration.h"
#include "rtkBox.h"

namespace rtk
{

/** \class DrawBoxImageFilter
 * \brief Draws in a 3D image user defined Box.
 *
 * \test rtkdrawgeometricphantomtest.cxx
 *
 * \author Marc Vila
 *
 * \ingroup InPlaceImageFilter
 */
template <class TInputImage, class TOutputImage>
class DrawBoxImageFilter :
public DrawConvexObjectImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef DrawBoxImageFilter                                    Self;
  typedef DrawConvexObjectImageFilter<TInputImage,TOutputImage> Superclass;
  typedef itk::SmartPointer<Self>                               Pointer;
  typedef itk::SmartPointer<const Self>                         ConstPointer;

  /** Convenient typedefs. */
  typedef Box::PointType          PointType;
  typedef Box::VectorType         VectorType;
  typedef Box::ScalarType         ScalarType;
  typedef Box::RotationMatrixType RotationMatrixType;
  typedef Box::ImageBaseType      ImageBaseType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DrawBoxImageFilter, DrawConvexObjectImageFilter);

  /** Get / Set the constant density of the volume */
  itkGetMacro(Density, ScalarType);
  itkSetMacro(Density, ScalarType);

  /** Get reference to vector of plane parameters. */
  itkGetConstReferenceMacro(PlaneDirections, std::vector<VectorType>);
  itkGetConstReferenceMacro(PlanePositions, std::vector<ScalarType>);

  void AddClippingPlane(const VectorType & dir, const ScalarType & pos);

  /** Set the box from an image. See rtkDrawBoxFunction::SetBoxFromImage. */
  void SetBoxFromImage(const ImageBaseType * img, bool bWithExternalHalfPixelBorder=true);

  itkGetMacro(BoxMin, VectorType);
  itkSetMacro(BoxMin, VectorType);
  itkGetMacro(BoxMax, VectorType);
  itkSetMacro(BoxMax, VectorType);
  itkGetMacro(Direction, RotationMatrixType);
  itkSetMacro(Direction, RotationMatrixType);

protected:
  DrawBoxImageFilter();
  ~DrawBoxImageFilter() {}

  void BeforeThreadedGenerateData ( ) ITK_OVERRIDE;

private:
  DrawBoxImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  ScalarType              m_Density;
  std::vector<VectorType> m_PlaneDirections;
  std::vector<ScalarType> m_PlanePositions;

  VectorType         m_BoxMin;
  VectorType         m_BoxMax;
  RotationMatrixType m_Direction;
};

} // end namespace rtk

#ifndef ITK_MANUAL_INSTANTIATION
#include "rtkDrawBoxImageFilter.hxx"
#endif

#endif
