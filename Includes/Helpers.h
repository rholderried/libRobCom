/*******************************************************************************
 * Helper functions used by the RobCom library
 *
 *******************************************************************************
 * Date of creation: 06.10.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 ******************************************************************************/

#ifndef HELPERS_H_
#define HELPERS_H_

#include <thread>
#include <mutex>
#include <functional>
#include <chrono>

/*******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef enum
{
	LITTLE,
	BIG
}BYTEORDER;


/*******************************************************************************
 * Classes
 ******************************************************************************/
// Thread management
class ThreadWrapper
{
	// std::thread object
	std::thread  m_threadHandler;

public:
	//Delete the copy constructor
	ThreadWrapper(const ThreadWrapper&) = delete;

	//Delete the Assignment opeartor
	ThreadWrapper& operator=(const ThreadWrapper&) = delete;

	// Parameterized Constructor
	ThreadWrapper(std::function<void()> func);

	// Move Constructor
	ThreadWrapper(ThreadWrapper && obj);

	//Move Assignment Operator
	ThreadWrapper & operator=(ThreadWrapper && obj);

	//Destructor
	~ThreadWrapper();
};

// Helper functions
class HelperFunctions
{
public:
	// Member variables
	BYTEORDER m_byteOrder;
	// Member functions
	HelperFunctions();
	BYTEORDER CheckByteOrder();
	~HelperFunctions();

	// Templated Functions
	/***********************************************************************//**
	* \brief Write Values of arbitrary type into uint8_t buffer.
	*
	* This functions also checks the host endianess and converts the values in
	* little endian if necessary. Note, that the RobCom protocol is defined to
	* be little endian.
	***************************************************************************/
	template<typename T>
	std::size_t WriteUI8Buffer (unsigned char* buf_ptr, T firstargument)
	{
		uint8_t* argptr = reinterpret_cast<uint8_t*>(&firstargument);

		std::size_t datasize = sizeof(T);
		uint8_t i = static_cast<uint8_t>(datasize);

		while (i > 0)
		{
			// Daten werden im Little Endian Format ausgetauscht
			if(m_byteOrder == BIG)
				*buf_ptr++ = argptr[i-1];
			else
				*buf_ptr++ = *argptr++;

			i--;
		}

		return datasize;
	}

	/***********************************************************************//**
	* \brief Write Values of arbitrary type into uint8_t buffer.
	*
	* This functions also checks the host endianess and converts the values in
	* little endian if necessary. Note, that the RobCom protocol is defined to
	* be little endian.
	* This function is designed to be recursively extracting the parameter pack.
	***************************************************************************/
	template<typename T, typename ... argtype>
	std::size_t WriteUI8Buffer (unsigned char* buf_ptr, T firstargument, argtype ... args)
	{
		uint8_t* argptr = reinterpret_cast<uint8_t*>(&firstargument);

		std::size_t datasize = sizeof(T);
		uint8_t i = static_cast<uint8_t>(datasize);

		while (i > 0)
		{
			// Daten werden im Little Endian Format ausgetauscht
			if(m_byteOrder == BIG)
				*buf_ptr++ = argptr[i-1];
			else
				*buf_ptr++ = *argptr++;
			i--;
		}

		return datasize + WriteUI8Buffer(buf_ptr, args...);
	}

	/***********************************************************************//**
	* \brief Get data from uint8_t buffer
	*
	* This function converts data from an uint8_t buffer, which represents the
	* data as inputDataType, to the desired datatype, specified by
	* outputDataType.
	***************************************************************************/
	template<typename inputDataType, typename outputDataType>
	outputDataType ConvertUI8BufferData (unsigned char* bufPtr)
	{
		outputDataType retVal;
		inputDataType tmpVal;
		std::size_t byteNumber = sizeof(inputDataType);
		uint8_t tmpArr[byteNumber];
		uint8_t i = 0;

		// Convert data endianess to the host computer endianess
		while(i < byteNumber)
		{
			if(m_byteOrder == BIG)
				tmpArr[i] = bufPtr[byteNumber - i - 1];
			else
				tmpArr[i] = bufPtr[i];
			i++;
		}

		// explicitely cast the return datatype
		tmpVal = *(inputDataType*)&tmpArr[0];

		retVal = static_cast<outputDataType>(tmpVal);
		return retVal;
	}
};

/*******************************************************************************
 * Typedefs Timer class
 ******************************************************************************/
typedef std::function<void()> TimerCB;

// Timer class
class Timeout
{
public:

    // Public member variables
	TimerCB m_timerCallback = nullptr;

	// Public member functions
    Timeout();
	~Timeout();
    void SetTimeout(int after, bool async);
};

// Ringbuffer Class
template<typename T>
class Ringbuffer
{
public:
	// Public member variables
	unsigned int m_bufSize;
	unsigned int m_bufWidth;
	unsigned int m_bufSpace;
	// the message buffer

	int m_lastDataIndex;
	int m_firstDataIndex;
	bool m_dataLost;
	// This function is to be called if the last free space in the buffer has
	// been validated
	std::function<void()> m_lastDataCB;
	T* m_ringbuffer;

	// Public member functions
	Ringbuffer(unsigned int bufSize, unsigned int bufWidth)
	{
		m_bufSize = bufSize;
		m_bufWidth = bufWidth;
		m_bufSpace = bufSize;
		m_lastDataIndex = -1;
		m_firstDataIndex = -1;
		m_dataLost = false;
		m_bufferBusy = false;
		// Instantiate new Ringbuffer
		m_ringbuffer = new T [bufSize * bufWidth];
		// This callback has to be initialized explicitly
		m_lastDataCB = nullptr;
	}
	~Ringbuffer()
	{
		delete m_ringbuffer;
	}

	// Put one element into the ringbuffer
	void PutElement(T* element, bool lock = true)
	{
		unsigned int i = 0;
		int index;

		if(lock)
			AquireBufferAccess();

		index = ValidateNextDataIndex();// * m_bufWidth;

		// Copy elements into the buffer
		while(i < m_bufWidth)
		{
			m_ringbuffer[index + i] = element[i];
			i++;
		}

		if(lock)
			ReleaseBuffer();
	}

	// Get element at a specific buffer index
	bool GetElement(T** bufPtr, bool lock = true)
	{
		int firstDataIndex = 0;
		bool retVal = false;

		if(lock)
			AquireBufferAccess();

		// temporarily save the last data index
		firstDataIndex = m_firstDataIndex;

		if(InvalidateFirstDataIndex())
		{
			// Get the Element from the last Buffer index
			*bufPtr = &m_ringbuffer[firstDataIndex * m_bufWidth];
			retVal = true;
		}

		if(lock)
			ReleaseBuffer();

		return retVal;
	}

	// If the buffer is to be written not via PutElement, this function has
	// to be called first in order to gurantee data contigency
	int PrepareBufferWriteAtIndex(void)
	{
		int index;

		// Get Access to the buffer
		AquireBufferAccess();

		index = ValidateNextDataIndex();

		return index;
	}

	// In order to use this ringbuffer, this function has to be called after
	// the buffer write
	void ConcludeBufferWrite(void)
	{
		ReleaseBuffer();
	}

	// Delete the newest data in the buffer
	bool DeleteLastData(bool lock = true)
	{
		bool retVal = false;

		if(lock)
			AquireBufferAccess();

		// if there is still no valid data stored in the buffer
		if(m_firstDataIndex >= 0)
		{
			// If there is valid data in the buffer
			if(m_bufSpace < m_bufSize)
			{
				// Decrease the last data index (handle underflows)
				if(static_cast<unsigned int>(--m_lastDataIndex) > (m_bufSize - 1))
					m_lastDataIndex = m_bufSize - 1;

				m_bufSpace++;

				retVal = true;
			}
		}

		if(lock)
			ReleaseBuffer();

		return retVal;
	}

private:
	// Private class members
	// This Mutex prevents simultaneous read/writes
	bool m_bufferBusy;
	std::mutex m_bufferBusyRWMutex;

	// Private class member funcitons
	// Increase the Index of the first valid value about 1 and compute the
	// new buffer space
	int ValidateNextDataIndex(void)
	{
		// Set the first Data Index while handling index overflows
		if(static_cast<unsigned int>(++m_lastDataIndex) > m_bufSize - 1)
			m_lastDataIndex = 0;

		// if bufSpace is already 0, then the last data gets lost on a write and
		// is therefore invalidated by this function if the overwrite flag is
		// set
		if (m_bufSpace == 0)
		{
			m_dataLost = true;
			// Invalidate first valid buffer entry
			if(static_cast<unsigned int>(++m_firstDataIndex) > m_bufSize - 1)
				m_firstDataIndex = 0;
		}
		else
		{
			m_dataLost = false;
			// If this is the first data which is to be written into the buffer:
			// Validate the lastDataIndex
			if(m_firstDataIndex < 0)
				m_firstDataIndex++;

			m_bufSpace--;
			// In case that this was the last free data space
			if (!m_bufSpace && m_lastDataCB)
				m_lastDataCB();
		}

		// return the actual data buffer index
		return m_lastDataIndex * m_bufWidth;
	}

	// Invalidate the last data index
	bool InvalidateFirstDataIndex(void)
	{
		// if there is still no valid data stored in the buffer
		if(m_firstDataIndex < 0)
			return false;

		// If there is valid data in the buffer
		if(m_bufSpace < m_bufSize)
		{
			// Increase the first data index
			if(static_cast<unsigned int>(++m_firstDataIndex) > (m_bufSize - 1))
				m_firstDataIndex = 0;

			m_bufSpace++;
		}
		else
			return false;

		return true;
	}

	// Get Access to the buffer
	void AquireBufferAccess(void)
	{
		std::lock_guard<std::mutex> lock(m_bufferBusyRWMutex);

		// Wait until the buffer operation has finished
		while(m_bufferBusy);

		m_bufferBusy = true;
	}

	// Grant other threads access to the buffer
	void ReleaseBuffer(void)
	{
		m_bufferBusy = false;
	}
};


#endif /* HELPERS_H_ */
