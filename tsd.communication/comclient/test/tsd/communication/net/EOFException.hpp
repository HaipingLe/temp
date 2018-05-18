// EOFException class

#ifndef EOFException_class
#define EOFException_class

#include <string>

class EOFException: public std::exception {
public:
	/** Constructor (C strings).
	 *  @param message C-style string error message.
	 *                 The string contents are copied upon construction.
	 *                 Hence, responsibility for deleting the \c char* lies
	 *                 with the caller.
	 */
	explicit EOFException(const char* message) :
			msg(message) {
	}

	/** Constructor (C++ STL strings).
	 *  @param message The error message.
	 */
	explicit EOFException(const std::string& message) :
			msg(message) {
	}

	/** Destructor.
	 * Virtual to allow for subclassing.
	 */
	virtual ~EOFException() throw () {
	}

	/** Returns a pointer to the (constant) error description.
	 *  @return A pointer to a \c const \c char*. The underlying memory
	 *          is in posession of the \c Exception object. Callers \a must
	 *          not attempt to free the memory.
	 */
	virtual const char* what() const throw () {
		return msg.c_str();
	}

private:

	std::string msg;
};

#endif
