//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2021 The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB.
// see http://omniorb.sourceforge.net/ for more about omniORB.
// The YAT library is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// The YAT library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// See COPYING file for license details
//
// Contact:
//      Stephane Poirier
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */

namespace yat
{

// ============================================================================
// Buffer::clear
// ============================================================================
template <typename T>
YAT_INLINE void Buffer<T>::clear ()
{
  ::memset(this->base_, 0,  this->capacity_ * sizeof(T));
}

// ============================================================================
// Buffer::elem_size
// ============================================================================
template <typename T>
YAT_INLINE std::size_t Buffer<T>::elem_size () const
{
  return sizeof(T);
}

// ============================================================================
// Buffer::length
// ============================================================================
template <typename T>
YAT_INLINE std::size_t Buffer<T>::length () const
{
  return this->length_;
}

// ============================================================================
// Buffer::force_length
// ============================================================================
template <typename T>
YAT_INLINE void Buffer<T>::force_length (std::size_t _new_length)
{
  if (_new_length > this->capacity_)
    this->length_ = this->capacity_;
  else
    this->length_ = _new_length;
}

// ============================================================================
// Buffer::append
// ============================================================================
template <typename T>
YAT_INLINE void Buffer<T>::append (const T& _val)
{
  if( this->length_ < this->capacity_ )
  {
    this->base_[this->length_] = _val;
    this->length_++;
  }
  else
  {
    THROW_YAT_ERROR("ERROR", "overflow error", "Buffer<T>::append");
  }
}

// ============================================================================
// Buffer::capacity
// ============================================================================
template <typename T>
YAT_INLINE std::size_t Buffer<T>::capacity () const
{
  return this->capacity_;
}

// ============================================================================
// Buffer::empty
// ============================================================================
template <typename T>
YAT_INLINE bool Buffer<T>::empty () const
{
  return this->length_ == 0;
}

// ============================================================================
// Buffer::base
// ============================================================================
template <typename T>
YAT_INLINE T * Buffer<T>::base () const
{
  return this->base_;
}

// ============================================================================
// Buffer::operator[]
// ============================================================================
template <typename T>
YAT_INLINE T& Buffer<T>::operator[] (std::size_t _indx)
{
  /* !! no bound error check !!*/
  return this->base_[_indx];
}

// ============================================================================
// Buffer::operator[] const
// ============================================================================
template <typename T>
YAT_INLINE const T& Buffer<T>::operator[] (std::size_t _indx) const
{
  /* !! no bound error check !!*/
  return this->base_[_indx];
}

// ============================================================================
// Buffer::size
// ============================================================================
template <typename T>
YAT_INLINE std::size_t Buffer<T>::size () const
{
  return this->length_ * sizeof(T);
}

// ============================================================================
// Buffer::fill
// ============================================================================
template <typename T>
YAT_INLINE void Buffer<T>::fill (const T& _val)
{
  *this = _val;
}

// ============================================================================
// Buffer::operator=
// ============================================================================
template <typename T>
YAT_INLINE Buffer<T>& Buffer<T>::operator= (const Buffer<T>& src)
{
  if (&src == this)
    return *this;

  if (this->capacity_ < src.length_)
    this->capacity(src.length_);

  std::memcpy(this->base_, src.base_, src.length_ * sizeof(T));

  this->length_ = src.length_;

  return *this;
}

// ============================================================================
// Buffer::operator=
// ============================================================================
template <typename T>
YAT_INLINE Buffer<T>& Buffer<T>::operator= (const T* _src)
{
  if (_src == this->base_)
    return *this;

  ::memcpy(this->base_, _src, this->capacity_ * sizeof(T));

  this->length_ = this->capacity_;

  return *this;
}

// ============================================================================
// Buffer::operator=
// ============================================================================
template <typename T>
YAT_INLINE Buffer<T>& Buffer<T>::operator= (const T& _val)
{
  for (std::size_t i = 0; i < this->capacity_; i++)
     *(this->base_ + i) = _val;

  this->length_ = this->capacity_;

  return *this;
}

// ======================================================================
// ImageBuffer::width
// ======================================================================
template <typename T>
YAT_INLINE std::size_t ImageBuffer<T>::width () const
{
  return this->width_;
}

// ======================================================================
// ImageBuffer::height
// ======================================================================
template <typename T> YAT_INLINE std::size_t ImageBuffer<T>::height () const
{
  return this->height_;
}

// ======================================================================
// ImageBuffer::set_dimensions
// ======================================================================
template <typename T>
YAT_INLINE void ImageBuffer<T>::set_dimensions (std::size_t _width, std::size_t _height)
{
  this->capacity(_width * _height);
  this->force_length(_width * _height);
  this->width_  = _width;
  this->height_ = _height;
}

// ======================================================================
// ImageBuffer::operator=
// ======================================================================
template <typename T>
YAT_INLINE ImageBuffer<T> & ImageBuffer<T>::operator= (const ImageBuffer<T> &_src)
{
  if (&_src == this)
    return *this;

  this->Buffer<T>::operator =(_src);

  this->width_  = _src.width_;
  this->height_ = _src.height_;

  return *this;
}

// ======================================================================
// ImageBuffer::operator=
// ======================================================================
template <typename T>
YAT_INLINE ImageBuffer<T> & ImageBuffer<T>::operator= (const T *base)
{
  Buffer<T>::operator=(base);

  return *this;
}

// ======================================================================
// ImageBuffer::operator=
// ======================================================================
template <typename T>
YAT_INLINE ImageBuffer<T> & ImageBuffer<T>::operator= (const T &val)
{
  this->fill(val);

  return *this;
}

} // namespace
