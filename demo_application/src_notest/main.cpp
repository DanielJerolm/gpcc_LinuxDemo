// GPCC Linux Demo
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.
//
// Copyright (C) 2024 Daniel Jerolm

#include <gpcc/cli/CLI.hpp>
#include <gpcc/cli/CLIColors.hpp>
#include <gpcc/cli/Command.hpp>
#include <gpcc/cli/exceptions.hpp>
#include <gpcc/cli/StdIO_to_ITerminal.hpp>
#include <gpcc/execution/async/DeferredWorkQueue.hpp>
#include <gpcc/execution/async/DeferredWorkPackage.hpp>
#include <gpcc/log/backends/Backend_CLI.hpp>
#include <gpcc/log/backends/Backend_CLILogHistory.hpp>
#include <gpcc/log/cli/commands.hpp>
#include <gpcc/log/Logger.hpp>
#include <gpcc/log/logfacilities/ThreadedLogFacility.hpp>
#include <gpcc/raii/scope_guard.hpp>
#include <gpcc/string/tools.hpp>
#include <gpcc/time/TimeSpan.hpp>
#include "src_notest/cood_demo/COOD_Demo.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdint>


// The address of this is taken as owner-address for our workpackages.
uint8_t const wpOwner = 0U;


static void CLICMD_Exit(std::string const & restOfLine,
                        gpcc::cli::CLI & cli,
                        gpcc::execution::async::DeferredWorkQueue* pDWQ)
{
  (void)cli;

  if (!restOfLine.empty())
    throw gpcc::cli::UserEnteredInvalidArgsError();

  pDWQ->RequestTermination();
}

static void DelayedPrint(gpcc::cli::CLI* pCLI)
{
  pCLI->WriteLine("Delayed print");
}

static void CLICMD_DelayedPrint(std::string const & restOfLine,
                                gpcc::cli::CLI & cli,
                                gpcc::execution::async::DeferredWorkQueue* pDWQ)
{
  int32_t delay;
  try
  {
    delay = gpcc::string::AnyNumberToI32(restOfLine);
  }
  catch (std::exception const &)
  {
    std::throw_with_nested(gpcc::cli::UserEnteredInvalidArgsError());
  }

  using namespace gpcc::execution::async;
  pDWQ->Add(DeferredWorkPackage::CreateDynamic(&wpOwner, 0,
                                               std::bind(&DelayedPrint, &cli),
                                               gpcc::time::TimeSpan::sec(delay)));
}

static void CLICMD_CLIStyleDemo(std::string const & restOfLine, gpcc::cli::CLI & cli)
{
  if (!restOfLine.empty())
    throw gpcc::cli::UserEnteredInvalidArgsError();

  cli.WriteLine("StyleDemo\n"\
                "CLI_STD................" CLI_STD                "Abcdefg" CLI_STD "\n"\
                "CLI_BLACK.............." CLI_BLACK              "Abcdefg" CLI_STD "\n"\
                "CLI_RED................" CLI_RED                "Abcdefg" CLI_STD "\n"\
                "CLI_GREEN.............." CLI_GREEN              "Abcdefg" CLI_STD "\n"\
                "CLI_BROWN.............." CLI_BROWN              "Abcdefg" CLI_STD "\n"\
                "CLI_BLUE..............." CLI_BLUE               "Abcdefg" CLI_STD "\n"\
                "CLI_MAGENTA............" CLI_MAGENTA            "Abcdefg" CLI_STD "\n"\
                "CLI_CYAN..............." CLI_CYAN               "Abcdefg" CLI_STD "\n"\
                "CLI_GREY..............." CLI_GREY               "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_DARK_GREY....." CLI_BOLD_DARK_GREY     "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_LIGHT_RED....." CLI_BOLD_LIGHT_RED     "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_LIGHT_GREEN..." CLI_BOLD_LIGHT_GREEN   "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_YELLOW........" CLI_BOLD_YELLOW        "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_LIGHT_BLUE...." CLI_BOLD_LIGHT_BLUE    "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_LIGHT_MAGENTA." CLI_BOLD_LIGHT_MAGENTA "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_LIGHT_CYAN...." CLI_BOLD_LIGHT_CYAN    "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_WHITE........." CLI_BOLD_WHITE         "Abcdefg" CLI_STD "\n"\
                "CLI_BOLD_STD..........." CLI_BOLD_STD           "Abcdefg");

  cli.WriteLine(CLI_BOLD_LIGHT_RED "[ERROR] Demo Message");
  cli.WriteLine(CLI_BOLD_YELLOW "[WARN ] Demo Message");
  cli.WriteLine(CLI_GREY "[DEBUG] Demo Message");
  cli.WriteLine(CLI_STD "[INFO ] Demo Message");
}

static void CLICMD_LogDemo(std::string const & restOfLine,
                           gpcc::cli::CLI & cli,
                           gpcc::log::Logger* pLogger)
{
  (void)cli;

  if (!restOfLine.empty())
    throw gpcc::cli::UserEnteredInvalidArgsError();

  pLogger->Log(gpcc::log::LogType::Debug, "LogDemo (Debug)");
  pLogger->Log(gpcc::log::LogType::Info, "LogDemo (Info)");
  pLogger->Log(gpcc::log::LogType::Warning, "LogDemo (Warning)");
  pLogger->Log(gpcc::log::LogType::Error, "LogDemo (Error)");
  pLogger->Log(gpcc::log::LogType::Fatal, "LogDemo (Fatal)");
}

static void CLICMD_LongRunningCmd(std::string const & restOfLine, gpcc::cli::CLI & cli)
{
  bool testTermination = false;
  if (!restOfLine.empty())
  {
    if (restOfLine == "TT")
      testTermination = true;
    else
      throw gpcc::cli::UserEnteredInvalidArgsError();
  }

  if (testTermination)
    cli.WriteLine("Long running (10s) command started (with CLI::TestTermination())");
  else
    cli.WriteLine("Long running (10s) command started (without CLI::TestTermination())");

  for (uint8_t i = 1U; i <= 10U; i++)
  {
    cli.WriteLine(std::to_string(i) + " of 10");

    gpcc::osal::Thread::Sleep_ms(1000U);

    if (testTermination)
      cli.TestTermination();
  }

  cli.WriteLine("Long running command finished!");
}

static void protected_main(void)
{
  // Note usage of smart pointers and ON_SCOPE_EXIT(). When leaving this function (either due to an exception or
  // when the program terminates normally), everything is stopped and destroyed properly in opposite order.

  // ============================================
  // Instantiate application's components
  // ============================================
  auto spDWQ = std::make_unique<gpcc::execution::async::DeferredWorkQueue>();
  ON_SCOPE_EXIT(removeOurWorkpackagesFromDWQ) { spDWQ->Remove(&wpOwner); };

  auto spCLIAdapter = std::make_unique<gpcc::cli::StdIO_to_ITerminal>();

  auto spCLI = std::make_unique<gpcc::cli::CLI>(*spCLIAdapter, 120U, 10U, "CLI", nullptr);

  auto spLogFacility = std::make_unique<gpcc::log::ThreadedLogFacility>("Logsys", 1000U);

  auto spBackendCLI = std::make_unique<gpcc::log::Backend_CLI>(*spCLI);
  spLogFacility->Register(*spBackendCLI);
  ON_SCOPE_EXIT(unregisterBackendCLI) { spLogFacility->Unregister(*spBackendCLI); };

  auto spBackendLogHistory = std::make_unique<gpcc::log::Backend_CLILogHistory>(spCLI.get(), 6U, 64U * 1024U);
  spLogFacility->Register(*spBackendLogHistory);
  ON_SCOPE_EXIT(unregisterBackendCLILogHistory) { spLogFacility->Unregister(*spBackendLogHistory); };

  auto spMainLogger = std::make_unique<gpcc::log::Logger>("Main");
  spLogFacility->Register(*spMainLogger);
  ON_SCOPE_EXIT(unregisterMainLogger) { spLogFacility->Unregister(*spMainLogger); };

  // ============================================
  // Start application's components
  // ============================================
  std::cout << std::endl;
  spCLI->Start(gpcc::osal::Thread::SchedPolicy::Other, 0, gpcc::osal::Thread::GetDefaultStackSize());
  ON_SCOPE_EXIT(stopCLI) { spCLI->Stop(); };

  spLogFacility->Start(gpcc::osal::Thread::SchedPolicy::Other, 0U, gpcc::osal::Thread::GetDefaultStackSize());
  ON_SCOPE_EXIT(stopLogFacility) { spLogFacility->Stop(); };

  // ============================================
  // Add core CLI commands
  // ============================================
  ON_SCOPE_EXIT(unreg_core_CLICMDs)
  {
    spCLI->RemoveCommand("Exit");
    spCLI->RemoveCommand("logsys");
  };

  spCLI->AddCommand(gpcc::cli::Command::Create("Exit", "\n"
                                               "Terminates this application",
                                               std::bind(&CLICMD_Exit,
                                                         std::placeholders::_1, std::placeholders::_2, spDWQ.get())));

  spCLI->AddCommand(gpcc::cli::Command::Create("logsys", "\n"
                                               "Interactive log system configuration.",
                                               std::bind(&gpcc::log::CLI_Cmd_LogCtrl,
                                               std::placeholders::_1,
                                               std::placeholders::_2,
                                               static_cast<gpcc::log::ILogFacilityCtrl*>(spLogFacility.get()))));


  // ============================================
  // Add/instantiate demo components/CLI commands
  // ============================================
  ON_SCOPE_EXIT(unreg_demo_CLICMDs)
  {
    spCLI->RemoveCommand("DelayedPrint");
    spCLI->RemoveCommand("CLIStyleDemo");
    spCLI->RemoveCommand("LogDemo");
    spCLI->RemoveCommand("LongRunningCommand");
  };

  spCLI->AddCommand(gpcc::cli::Command::Create("DelayedPrint", " <delay>\n"
                                               "Prints a text to the CLI after <delay> seconds.",
                                               std::bind(&CLICMD_DelayedPrint,
                                                         std::placeholders::_1, std::placeholders::_2, spDWQ.get())));

  spCLI->AddCommand(gpcc::cli::Command::Create("CLIStyleDemo", "\n"
                                               "Demonstrates font styles when printing via CLI.",
                                               std::bind(&CLICMD_CLIStyleDemo,
                                                         std::placeholders::_1, std::placeholders::_2)));

  spCLI->AddCommand(gpcc::cli::Command::Create("LogDemo", "\n"
                                               "Logs some demo messages to the 'main'-logger.",
                                               std::bind(&CLICMD_LogDemo,
                                                         std::placeholders::_1, std::placeholders::_2, spMainLogger.get())));

  spCLI->AddCommand(gpcc::cli::Command::Create("LongRunningCommand", " [TT]\n"\
                                               "Counts from 1 to 10 in 10 seconds.\n"\
                                               "If TT is passed as parameter, then CLI::TestTermination() will be invoked periodically.",
                                               std::bind(&CLICMD_LongRunningCmd, std::placeholders::_1, std::placeholders::_2)));

   auto spCOODDemo = std::make_unique<COOD_Demo>(*spCLI);

  // ============================================
  // Say hello and enter work queue
  // ============================================
  spCLI->WriteLine("CLI test\n"\
                   "================================================================================\n" \
                   "Log into terminal and type 'Exit' to terminate.");

  spDWQ->Work();

  spCLI->WriteLine("Exit in process, please wait...");
}

/**
 * \brief Program's main()-function. Execution starts here.
 *
 * - - -
 *
 * __Thread safety:__\n
 * This is not thread-safe.
 *
 * __Exception safety:__\n
 * No-throw guarantee.
 *
 * __Thread cancellation safety:__\n
 * Deferred cancellation is not allowed.
 *
 * - - -
 *
 * \param argc
 * Number of arguments passed to the program.\n
 * Allowed range: 0..N
 *
 * \param argv
 * Pointer to an array of pointers to null-terminated strings containing the name of the program and the arguments
 * passed to the program.\n
 * The array contains `argc + 1` pointers. The last pointer (`argv[argc]`) is `nullptr`.\n
 * The first item in the array points to a string containing the name of the program or an empty string.
 *
 * \return
 * Always `EXIT_SUCCESS`.
 */
int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  try
  {
    protected_main();
  }
  catch (std::exception & e)
  {
    gpcc::osal::Panic("protected_main() threw: ", e);
  }
  catch (...)
  {
    gpcc::osal::Panic("protected_main() threw an unknown exception");
  }

  return EXIT_SUCCESS;
}
