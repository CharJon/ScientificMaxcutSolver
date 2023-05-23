#ifndef SMS_SCIP_EXCEPTION_HPP
#define SMS_SCIP_EXCEPTION_HPP

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the examples to                     */
/*                         An introduction to SCIP                           */
/*                                                                           */
/*    Copyright (C) 2007 Cornelius Schwarz                                   */
/*                                                                           */
/*                  2007 University of Bayreuth                              */
/*                                                                           */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file scip_exception.hpp
 * @brief exception handling for SCIP
 * @author Cornelius Schwarz
 */

#ifndef SCIP_EXCEPTION
#define SCIP_EXCEPTION

#include <exception>
#include <string>
#include <iostream>

#include <scip/scip.h>
#include <scip/misc.h>

// unfortunately SCIP has no method to get the string of an error code, you can just print it to a file
// so we add such a method here, this has to be updated when SCIP Messages changes
// currently supporting SCIP-1.00
#define SCIP_MSG_MAX 100 ///< maximal number of characters in an error messages

/**
 * @brief translates a SCIP_RETCODE into an error string
 *
 * @param[in] retcode SCIP_RETCODE you want to translate
 * @param[in] buffersize size of buffer
 * @param[out] buffer_str buffer to character array to store translated message, this must be at least of size SCIP_MSG_MAX
 * @return buffer_str or NULL, if retcode could not be translated
 */
inline char *SCIPgetErrorString(SCIP_RETCODE retcode, char *buffer_str, int buffersize) {
    // the following was copied from SCIPprintError
    switch (retcode) {
        case SCIP_OKAY:
            (void) SCIPsnprintf(buffer_str, buffersize, "normal termination");
            return buffer_str;
        case SCIP_ERROR:
            (void) SCIPsnprintf(buffer_str, buffersize, "unspecified error");
            return buffer_str;
        case SCIP_NOMEMORY:
            (void) SCIPsnprintf(buffer_str, buffersize, "insufficient memory error");
            return buffer_str;
        case SCIP_READERROR:
            (void) SCIPsnprintf(buffer_str, buffersize, "file read error");
            return buffer_str;
        case SCIP_WRITEERROR:
            (void) SCIPsnprintf(buffer_str, buffersize, "file write error");
            return buffer_str;
        case SCIP_BRANCHERROR:
            (void) SCIPsnprintf(buffer_str, buffersize, "branch error");
            return buffer_str;
        case SCIP_NOFILE:
            (void) SCIPsnprintf(buffer_str, buffersize, "file not found error");
            return buffer_str;
        case SCIP_FILECREATEERROR:
            (void) SCIPsnprintf(buffer_str, buffersize, "cannot create file");
            return buffer_str;
        case SCIP_LPERROR:
            (void) SCIPsnprintf(buffer_str, buffersize, "error in LP solver");
            return buffer_str;
        case SCIP_NOPROBLEM:
            (void) SCIPsnprintf(buffer_str, buffersize, "no problem exists");
            return buffer_str;
        case SCIP_INVALIDCALL:
            (void) SCIPsnprintf(buffer_str, buffersize, "method cannot be called at this time in solution process");
            return buffer_str;
        case SCIP_INVALIDDATA:
            (void) SCIPsnprintf(buffer_str, buffersize, "method cannot be called with this type of data");
            return buffer_str;
        case SCIP_INVALIDRESULT:
            (void) SCIPsnprintf(buffer_str, buffersize, "method returned an invalid result code");
            return buffer_str;
        case SCIP_PLUGINNOTFOUND:
            (void) SCIPsnprintf(buffer_str, buffersize, "a required plugin was not found");
            return buffer_str;
        case SCIP_PARAMETERUNKNOWN:
            (void) SCIPsnprintf(buffer_str, buffersize, "the parameter with the given name was not found");
            return buffer_str;
        case SCIP_PARAMETERWRONGTYPE:
            (void) SCIPsnprintf(buffer_str, buffersize, "the parameter is not of the expected type");
            return buffer_str;
        case SCIP_PARAMETERWRONGVAL:
            (void) SCIPsnprintf(buffer_str, buffersize, "the value is invalid for the given parameter");
            return buffer_str;
        case SCIP_KEYALREADYEXISTING:
            (void) SCIPsnprintf(buffer_str, buffersize, "the given key is already existing in table");
            return buffer_str;
        case SCIP_MAXDEPTHLEVEL:
            (void) SCIPsnprintf(buffer_str, buffersize, "maximal branching depth level exceeded");
            return buffer_str;
        case SCIP_NOTIMPLEMENTED:
            (void) SCIPsnprintf(buffer_str, buffersize, "function not implemented");
            return buffer_str;
    }
    return nullptr;
}


/** @brief exception handling class for SCIP
 *
 * this class enables you to handle the return code of SCIP functions in a C++ way
 */
class SCIPException : public std::exception {
private:
    char _msg[SCIP_MSG_MAX]{}; ///< error message
    SCIP_RETCODE _retcode;   ///< SCIP error code

public:

    /** @brief constructs a SCIPEexception from an error code
     *
     * this constructs a new SCIPException from given error code
     * @param[in] retcode SCIP error code
     */
    explicit SCIPException(SCIP_RETCODE retcode) : _retcode(retcode) {
        if (SCIPgetErrorString(retcode, _msg, SCIP_MSG_MAX) == nullptr)
            (void) SCIPsnprintf(_msg, SCIP_MSG_MAX, "unknown SCIP retcode %d", retcode);
    }


    /** @brief returns the error message
     *
     * @return error message
     *
     * this overrides the corresponding method of std::exception in order to allow you to catch all of your exceptions as std::exception
     */
    const char *what() const noexcept override { return _msg; }


    /** @brief get method for @p _retcode
     *
     * @return stored SCIP_RETCODE
     */
    SCIP_RETCODE get_retcode() const { return _retcode; }

    /** destructor */
    ~SCIPException() override = default;
};


/** @brief macro to call scip function with exception handling
 *
 * this macro calls a SCIP function and throws an instance of SCIPException if anything went wrong
 *
 */
#define SCIP_CALL_EXC(x)            \
   {                        \
      SCIP_RETCODE retcode;            \
      if( (retcode = (x)) != SCIP_OKAY)        \
      {                        \
     throw SCIPException(retcode);        \
      }                        \
   }


#define SCIP_CALL_EXC_NO_THROW(x)            \
   {                        \
      SCIP_RETCODE retcode;            \
      if( (retcode = (x)) != SCIP_OKAY)        \
      {                        \
     std::cerr << SCIPException(retcode).what() << std::endl; \
      exit(retcode - 1);                                       \
      }                        \
   }

#endif

#endif //SMS_SCIP_EXCEPTION_HPP
