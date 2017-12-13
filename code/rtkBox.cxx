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

#include "rtkBox.h"

namespace rtk
{

Box
::Box():
  m_BoxMin(0.),
  m_BoxMax(0.)
{
  m_Direction.SetIdentity();
}

bool
Box
::IsInside(const PointType& point) const
{
  RotationMatrixType dirt;
  dirt = m_Direction.GetTranspose();
  PointType t   = dirt * point;
  PointType min = dirt * m_BoxMin;
  PointType max = dirt * m_BoxMax;
  if(t[0] < min[0] ||
     t[0] > max[0] ||
     t[1] < min[1] ||
     t[1] > max[1] ||
     t[2] < min[2] ||
     t[2] > max[2])
    return false;
 return ApplyClippingPlanes(point);
}

bool
Box
::IsIntersectedByRay(const PointType & rayOrigin,
                     const VectorType & rayDirection,
                     double & near,
                     double & far) const
{
  // Apply transform
  RotationMatrixType dirt;
  dirt = m_Direction.GetTranspose();
  PointType org = dirt * rayOrigin;
  VectorType dir = dirt * rayDirection;
  PointType min = dirt * m_BoxMin;
  PointType max = dirt * m_BoxMax;
  for(unsigned int i=0; i<Dimension; i++)
    if(min[i]>max[i])
      std::swap( min[i], max[i] );

  // To account for m_Direction, everything (ray source and direction + boxmin/boxmax)
  // is rotated with its inverse, m_DirectionT. Then, the box is aligned with the
  // axes of the coordinate system and the algorithm at this hyperlink used:
  // http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
  // Note that the variables at this page have been renamed:
  // BI <-> min
  // Bh <-> max
  // Ro <-> org
  // Rd <-> dir
  // Tnear <-> near
  // Tfar <-> far
  near = itk::NumericTraits< ScalarType >::NonpositiveMin();
  far = itk::NumericTraits< ScalarType >::max();
  ScalarType T1, T2, invRayDir;
  for(unsigned int i=0; i<Dimension; i++)
    {
    if(dir[i] == itk::NumericTraits< ScalarType >::ZeroValue() &&
       (org[i]<min[i] || org[i]>max[i]) )
      return false;

    invRayDir = 1/dir[i];
    T1 = (min[i] - org[i]) * invRayDir;
    T2 = (max[i] - org[i]) * invRayDir;
    if(T1>T2) std::swap( T1, T2 );
    if(T1>near) near = T1;
    if(T2<far) far = T2;
    if(near>far) return false;
    if(far<0) return false;
    }

  return ApplyClippingPlanes(rayOrigin, rayDirection, near, far);
}

void
Box
::Rescale(const VectorType &r)
{
  Superclass::Rescale(r);
  for(unsigned int i=0; i<Dimension; i++)
    {
    m_BoxMin[i] *= r[i];
    m_BoxMax[i] *= r[i];
    }
}

void
Box
::Translate(const VectorType &t)
{
  Superclass::Translate(t);
  m_BoxMin += t;
  m_BoxMax += t;
}

void
Box
::Rotate(const RotationMatrixType &r)
{
  Superclass::Rotate(r);
  m_Direction = m_Direction * r;
  m_BoxMin = m_Direction * m_BoxMin;
  m_BoxMax = m_Direction * m_BoxMax;
}

itk::LightObject::Pointer
Box
::InternalClone() const
{
  LightObject::Pointer loPtr = Superclass::InternalClone();
  Self::Pointer clone = dynamic_cast<Self *>(loPtr.GetPointer());

  clone->SetBoxMin(this->GetBoxMin());
  clone->SetBoxMax(this->GetBoxMax());
  clone->SetDirection(this->GetDirection());

  return loPtr;
}

void
Box
::SetBoxFromImage( const ImageBaseType *img, bool bWithExternalHalfPixelBorder )
{
  if(Dimension != img->GetImageDimension())
    itkGenericExceptionMacro(<< "Box and image dimensions must agree");

  // Box corner 1
  m_BoxMin = img->GetOrigin().GetVectorFromOrigin();
  if(bWithExternalHalfPixelBorder)
    m_BoxMin -= img->GetDirection() * img->GetSpacing() * 0.5;

  // Box corner 2
  for(unsigned int i=0; i<Dimension; i++)
    if(bWithExternalHalfPixelBorder)
      m_BoxMax[i] = img->GetSpacing()[i] * img->GetLargestPossibleRegion().GetSize()[i];
    else
      m_BoxMax[i] = img->GetSpacing()[i] * (img->GetLargestPossibleRegion().GetSize()[i]-1);
  m_BoxMax = m_BoxMin + img->GetDirection() * m_BoxMax;

  SetDirection( img->GetDirection() );
}

} // end namespace rtk
