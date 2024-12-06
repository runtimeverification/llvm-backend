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

    # Use LLD as the linker
    # This is necessary as the default linker used by CMake on macOS is
    # ld64, which currently has some incompatibilities with Homebrew and XCode15.
    # See: https://github.com/orgs/Homebrew/discussions/4794#discussioncomment-7044468
    # Adding this flag avoid the following errors:
    # ld: warning: duplicate -rpath ... ignored
    # ld: warning: ignoring duplicate libraries ...
    add_link_options("-fuse-ld=lld")

  endif() # USE_NIX
endif() # APPLE
