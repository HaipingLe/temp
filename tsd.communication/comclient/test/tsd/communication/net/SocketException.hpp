// SocketException class

#ifndef SocketException_class
#define SocketException_class

#include <string>
#include <sstream>
#include <cstring>
#include <errno.h>

class SocketException: public std::exception {
public:
	/** Constructor (C strings).
	 *  @param message C-style string error message.
	 *                 The string contents are copied upon construction.
	 *                 Hence, responsibility for deleting the \c char* lies
	 *                 with the caller.
	 */
	explicit SocketException(const char* message) :
			msg(message), m_error(0) {
	}

	/** Constructor (C strings).
	 *  @param message C-style string error message.
	 *                 The string contents are copied upon construction.
	 *                 Hence, responsibility for deleting the \c char* lies
	 *                 with the caller.
	 *  @param error the error code.
	 */
	explicit SocketException(const char* message, int error) :
			msg(message), m_error(error) {

		buildMessage();
	}

	/** Constructor (C++ STL strings).
	 *  @param message The error message.
	 */
	explicit SocketException(const std::string& message) :
			msg(message), m_error(0) {
	}

	/** Constructor (C++ STL strings).
	 *  @param message The error message.
	 *  @param error the error code.
	 */
	explicit SocketException(const std::string& message, int error) :
			msg(message), m_error(error) {

		buildMessage();
	}

	/** Destructor.
	 * Virtual to allow for subclassing.
	 */
	virtual ~SocketException() throw () {
	}

	/** Returns a pointer to the (constant) error description.
	 *  @return A pointer to a \c const \c char*. The underlying memory
	 *          is in posession of the \c Exception object. Callers \a must
	 *          not attempt to free the memory.
	 */
	virtual const char* what() const throw () {
		return msg.c_str();
	}

	/**
	 * Retrieves the error code. 0 indicates the the error code was not set.
	 *
	 * @return the error code.
	 */
	int error() const {
		return m_error;
	}

private:

	std::string msg;
	int m_error;

	void buildMessage() {
		if (m_error != 0) {
			std::ostringstream message;
			message << " (" << m_error << ": " << strerror(m_error) << ")";

			msg += message.str();
		}
	}
};

#endif
