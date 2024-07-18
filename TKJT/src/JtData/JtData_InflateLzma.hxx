#pragma once
// JT format reading and visualization tools
// Copyright (C) 2024 Jürgen Riegel
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License, or any later
// version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// Copy of the GNU General Public License is in LICENSE.txt and  
// on <http://www.gnu.org/licenses/>.


#include <JtData_Reader.hxx>

#include <lzma.h>

//! Class performing unpacking of data using zlib.
class JtData_InflateLzma : public JtData_Reader
{
public:
  //! Constructor.
  Standard_EXPORT JtData_InflateLzma(JtData_Reader& theReader, const Standard_Size theLength);

  //! Destructor.
  Standard_EXPORT virtual ~JtData_InflateLzma();

  //! Buffered inflate raw bytes.
  Standard_EXPORT Standard_Boolean ReadBytes (void* theBuffer, Standard_Size theLength) Standard_OVERRIDE;

  //! Skip some bytes.
  Standard_EXPORT Standard_Boolean SkipBytes (Standard_Size theLength) Standard_OVERRIDE;

  //! Get absolute reading position in the inflated data.
  Standard_EXPORT Standard_Size GetPosition() const Standard_OVERRIDE;

protected:
  Standard_Size read (uint8_t* theBuffer, Standard_Size theLength);

protected:
  static const Standard_Size CHUNK = 32768;

  JtData_Reader* myReader;
  lzma_stream    myZStream;

  const uint8_t*    myInBuffer;
  Standard_Size  myInputRest;

  uint8_t           myOutBuffer[CHUNK];
  const uint8_t*    myOutBufPos;
  Standard_Size  myOutBufRest;
};


