if(APPLE)
  if(NOT USE_NIX)
    if(DEFINED ENV{HOMEBREW_PREFIX})
      set(BREW_PREFIX $ENV{HOMEBREW_PREFIX})
    else()
      execute_process(
        COMMAND brew --prefix
        OUTPUT_VARIABLE BREW_PREFIX
        ERROR_VARIABLE BREW_ERROR
        RESULT_VARIABLE BREW_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE)

      if(NOT BREW_RESULT EQUAL 0)
        message(WARNING "Error running brew --prefix; you may need to manually configure package search paths.")
        message(WARNING "  : ${BREW_ERROR}")
      endif() # BREW_RESULT
    endif() # ENV{HOMEBREW_PREFIX}

    message(STATUS "Looking for Homebrew dependencies in ${BREW_PREFIX}")
    include_directories(AFTER SYSTEM "${BREW_PREFIX}/include")
    link_directories(AFTER "${BREW_PREFIX}/lib")
    set(ENV{PKG_CONFIG_PATH} "${BREW_PREFIX}/opt/libffi/lib/pkgconfig")
  endif() # USE_NIX
endif() # APPLE
