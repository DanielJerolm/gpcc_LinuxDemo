// GPCC Linux Demo
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Copyright (C) 2024 Daniel Jerolm

#include "COOD_Demo.hpp"
#include <gpcc/cood/ObjectARRAY.hpp>
#include <gpcc/cood/ObjectVAR.hpp>
#include <gpcc/raii/scope_guard.hpp>
#include <memory>
#include <cstddef>

using namespace gpcc::cood;

#ifndef __DOXYGEN__
ObjectRECORD::SubIdxDescr const COOD_Demo::testData4Descr[6] =
{
  // name,                  type,                     attributes,                  nElements, byteOffset,                                bitOffset
  { "Field 1",              DataType::unsigned32,     Object::attr_ACCESS_RW,      1,         offsetof(COOD_Demo::TestData4, field1),    0},
  { "Field 2",              DataType::unsigned32,     Object::attr_ACCESS_RD,      1,         offsetof(COOD_Demo::TestData4, field2),    0},
  { "Field 3",              DataType::boolean,        Object::attr_ACCESS_RW,      1,         offsetof(COOD_Demo::TestData4, field3),    0},
  { "Field 4",              DataType::boolean,        Object::attr_ACCESS_RW,      1,         offsetof(COOD_Demo::TestData4, field4),    0},
  { "Field 5",              DataType::visible_string, Object::attr_ACCESS_RW,      16,        offsetof(COOD_Demo::TestData4, field5),    0},
  { "Field 6",              DataType::unsigned16,     Object::attr_ACCESS_RDCONST, 1,         offsetof(COOD_Demo::TestData4, field6),    0}
};
#endif

/**
 * \brief Constructor.
 *
 * - - -
 *
 * __Exception safety:__\n
 * Strong guarantee.
 *
 * __Thread cancellation safety:__\n
 * Strong guarantee.
 *
 * - - -
 *
 * \param _cli
 * Reference to the CLI that shall be used.
 */
COOD_Demo::COOD_Demo(gpcc::cli::CLI & _cli)
: od()
, cliAdapter(od, _cli, "OD")
, mutex()
, testData1()
, testData2()
, testData3()
, testData4()
, testData5()
{
  ON_SCOPE_EXIT(unregisterObjects)
  {
    od.Remove(0x1000U);
    od.Remove(0x1001U);
    od.Remove(0x1002U);
    od.Remove(0x2000U);
    od.Remove(0x2001U);
  };

  std::unique_ptr<gpcc::cood::Object> spObj;

  spObj = std::make_unique<gpcc::cood::ObjectVAR>("testData1",                          // name
                                                  gpcc::cood::DataType::unsigned32,     // type
                                                  1,                                    // nElements
                                                  gpcc::cood::Object::attr_ACCESS_RW,   // attributes
                                                  &testData1,                           // pData
                                                  &mutex,                               // pMutex
                                                  nullptr);                             // IObjectNotifiable
  od.Add(spObj, 0x1000U);

  spObj = std::make_unique<gpcc::cood::ObjectVAR>("testData2",                          // name
                                                  gpcc::cood::DataType::visible_string, // type
                                                  sizeof(testData2) - 1,                // nElements
                                                  gpcc::cood::Object::attr_ACCESS_RW,   // attributes
                                                  &testData2,                           // pData
                                                  &mutex,                               // pMutex
                                                  nullptr);                             // IObjectNotifiable
  od.Add(spObj, 0x1001U);

  spObj = std::make_unique<gpcc::cood::ObjectARRAY>("testData3",                          // name
                                                    gpcc::cood::Object::attr_ACCESS_RD,   // attributes SI0
                                                    sizeof(testData3),                    // SI0
                                                    sizeof(testData3),                    // min. SI0
                                                    sizeof(testData3),                    // max. SI0
                                                    gpcc::cood::DataType::unsigned8,      // type
                                                    gpcc::cood::Object::attr_ACCESS_RW,   // attributes SI1..n
                                                    testData3,                            // pData
                                                    &mutex,                               // pMutex
                                                    nullptr);                             // IObjectNotifiable
  od.Add(spObj, 0x1002U);

  spObj = std::make_unique<gpcc::cood::ObjectRECORD>("testData4",                          // name
                                                     sizeof(testData4Descr) / sizeof(ObjectRECORD::SubIdxDescr), // SI0
                                                     &testData4,                           // pStruct
                                                     sizeof(testData4),                    // sizeof(*pStruct)
                                                     &mutex,                               // pMutex
                                                     testData4Descr,                       // descripton of *pStruct
                                                     nullptr);                             // IObjectNotifiable
  od.Add(spObj, 0x2000U);

  spObj = std::make_unique<gpcc::cood::ObjectVAR>("testData5",                          // name
                                                  gpcc::cood::DataType::octet_string,   // type
                                                  sizeof(testData5),                    // nElements
                                                  gpcc::cood::Object::attr_ACCESS_RW,   // attributes
                                                  &testData5,                           // pData
                                                  &mutex,                               // pMutex
                                                  nullptr);                             // IObjectNotifiable
  od.Add(spObj, 0x2001U);

  ON_SCOPE_EXIT_DISMISS(unregisterObjects);
}

/**
 * \brief Destructor.
 *
 * - - -
 *
 * __Exception safety:__\n
 * No-throw guarantee.
 *
 * __Thread cancellation safety:__\n
 * Deferred cancellation is not allowed.
 *
 */
COOD_Demo::~COOD_Demo(void)
{
  od.Remove(0x1000U);
  od.Remove(0x1001U);
  od.Remove(0x1002U);
  od.Remove(0x2000U);
  od.Remove(0x2001U);
}
