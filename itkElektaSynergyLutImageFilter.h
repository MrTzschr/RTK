#ifndef __itkElektaSynergyLutImageFilter_h
#define __itkElektaSynergyLutImageFilter_h

#include "itkLutImageFilter.h"
#include <itkNumericTraits.h>

namespace itk
{

/** \class ElektaSynergyLutImageFilter
 * \brief Lookup table for Elekta Synergy data.
 *
 * The lookup table converts the raw values to the logarithm of the value divided by the max
 *
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT ElektaSynergyLutImageFilter: public LutImageFilter<TInputImage, TOutputImage>
{

public:
  /** Standard class typedefs. */
  typedef ElektaSynergyLutImageFilter Self;
  typedef LutImageFilter<TInputImage, TOutputImage> Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  typedef typename TInputImage::PixelType InputImagePixelType;
  typedef typename TOutputImage::PixelType OutputImagePixelType;
  typedef typename Superclass::FunctorType::LutType LutType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(ElektaSynergyLutImageFilter, LutImageFilter);

protected:
  ElektaSynergyLutImageFilter();
  virtual ~ElektaSynergyLutImageFilter() {}

private:
  ElektaSynergyLutImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&);        //purposely not implemented
};

} // end namespace itk

template <class TInputImage, class TOutputImage>
itk::ElektaSynergyLutImageFilter<TInputImage, TOutputImage>::SynergyLutImageFilter()
{
  // Create the lut
  typename LutType::Pointer lut = LutType::New();
  typename LutType::SizeType size;
  size[0] = NumericTraits<InputImagePixelType>::max()-NumericTraits<InputImagePixelType>::min()+1;
  lut->SetRegions( size );
  lut->Allocate();

  // Iterate and set lut
  OutputImagePixelType logRef = log(OutputImagePixelType(size[0]));
  itk::ImageRegionIteratorWithIndex<LutType>  it( lut, lut->GetBufferedRegion() );
  it.GoToBegin();
  while( !it.IsAtEnd() ) {
    it.Set( log( OutputImagePixelType(it.GetIndex()[0]+1) ) - logRef );
    ++it;
  }

  // Set the lut to member and functor
  this->SetLut(lut);
}

#endif
