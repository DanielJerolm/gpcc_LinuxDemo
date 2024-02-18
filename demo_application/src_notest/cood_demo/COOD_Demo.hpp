// GPCC Linux Demo
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Copyright (C) 2024 Daniel Jerolm

#ifndef COOD_DEMO_HPP_201904221043
#define COOD_DEMO_HPP_201904221043

#include <gpcc/cood/cli/CLIAdapterForCANOpen.hpp>
#include <gpcc/cood/ObjectDictionary.hpp>
#include <gpcc/cood/ObjectRECORD.hpp>
#include <gpcc/osal/Mutex.hpp>
#include <cstdint>

namespace gpcc {
namespace cli  {
  class CLI;
}
}

/**
 * \ingroup COOD_DEMO
 * \brief CANopen object dictionary demo.
 *
 * This class provides a demonstration of GPCC's CANopen object dictionary.\n
 * It will instantiate an object dictionary, some objects offering access to demo data, and
 * a CLI adapter for accessing the object dictionary via CLI.
 *
 * - - -
 *
 * __Thread safety:__\n
 * Thread-safe.
 */
class COOD_Demo final
{
  public:
    COOD_Demo(void) = delete;
    COOD_Demo(gpcc::cli::CLI & _cli);
    COOD_Demo(COOD_Demo const &) = delete;
    COOD_Demo(COOD_Demo &&) = delete;
    ~COOD_Demo(void);

    COOD_Demo& operator=(COOD_Demo const &) = delete;
    COOD_Demo& operator=(COOD_Demo &&) = delete;

  private:
    /// Object Dictionary.
    gpcc::cood::ObjectDictionary od;

    /// CLI Adapter for object dictionary access.
    gpcc::cood::CLIAdapterForCANOpen cliAdapter;

    /// Mutex protecting the data.
    gpcc::osal::Mutex mutex;

    /// Test data 1
    /** @ref mutex required. */
    uint32_t testData1;

    /// Test data 2
    /** VISIBLE_STRING, up to 32 chars. Actual length is 33 chars to ensure that a null-terminator is
        always present.\n
        @ref mutex required. */
    char testData2[33];

    /// Test data 3
    /** @ref mutex required. */
    uint8_t testData3[12];

    /// Test data 4
    /** @ref mutex required. */
    struct TestData4
    {
      uint32_t field1 = 0;
      uint32_t field2 = 0;
      bool field3 = false;
      bool field4 = false;
      uint8_t field5[17] = {0};
      uint16_t field6 = 0xABCDU;
    } testData4;

    /// Test data 5
    /** @ref mutex required. */
    uint8_t testData5[4];

    /// Description of @ref TestData4 structure
    static gpcc::cood::ObjectRECORD::SubIdxDescr const testData4Descr[6];
};

#endif // COOD_DEMO_HPP_201904221043
