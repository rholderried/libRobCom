/*******************************************************************************
 * USBInterface.cpp
 *
 * Member function definitions of the USBInterface class.
 *
 *******************************************************************************
 * Date of creation: 19.08.2019
 *******************************************************************************
 * Author: Roman Holderried
 *******************************************************************************
 * History:
 *
 *******************************************************************************
 * Known Bugs:
 * TODO The timeout for a control transfer (libusb) does not work right now.
 *******************************************************************************
 ******************************************************************************/

#include "USBInterface.h"
#include "Helpers.h"
#include "libusb.h"
#include <iostream>
#include <stdio.h>

using namespace std;

/***************************************************************************//**
* \brief USB Interface Constructor.
*
* Initializes libusb as the underlying low level library, which is interacting
* with the USB device driver.
*
* For future versions:
* TODO Add hotplug capabilities
*******************************************************************************/
USBInterface::USBInterface()
{
	m_devh = nullptr;

	libusb_error error_code = LIBUSB_SUCCESS;

	// Initialize libusb
	error_code = static_cast<libusb_error>(libusb_init(nullptr));

	cout << "State of libusb init: " << error_code << endl;

	if (error_code != LIBUSB_SUCCESS)
		throw("Error at libusb initialization!");

	// Check if libusb is able to carry out hotplug events
	m_hotplugCapability = libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG);

	if (m_hotplugCapability)
		cout << "libusb is capable of notifying hotplug events." << endl;
}

int USBInterface::InitializeFunctionality(uint16_t pid, uint16_t vid, int interface)
{
	libusb_device **devs; 					// List of all USB Devices
	libusb_device *dev = nullptr; 			// Single Device
	ssize_t dev_cnt = 0; 					// Number of discovered devices
	int i = 0; 								// Counter
	struct libusb_device_descriptor desc; 	// Device descriptor structure
	libusb_error error_code = LIBUSB_SUCCESS;

	dev_cnt = libusb_get_device_list(nullptr, &devs);

	// Devices are connected to the host machine
	if (dev_cnt != 0)
	{
		// Iterate over the list of devices which have been found
		while ((dev = devs[i++]) != nullptr)
		{
			// Get device descriptor
			error_code  = static_cast<libusb_error>(libusb_get_device_descriptor(dev, &desc));
			// Continue with the list if libusb can't get the device descriptor
			if (error_code != LIBUSB_SUCCESS)
				continue;

			// Is the current device the one we're looking for?
			if (desc.idVendor != vid || desc.idProduct != pid)
				continue;
			else
				break;
		}
	}

	// Unable to find any matching devices
	if (dev != nullptr)
	{
		// Open device, save device handle
		error_code = static_cast<libusb_error>(libusb_open(dev, &m_devh));

		// Try to claim the interface if device open succeeded
		if (error_code == LIBUSB_SUCCESS)
		{
			error_code = static_cast<libusb_error>(libusb_claim_interface(m_devh, interface));

			// Close the device if the interface claim didn't succeed
			if(error_code)
				libusb_close(m_devh);
			else
			{
				this->m_libusbInitialized = true;

				// Start an Event handler
				this->m_eventHandlerRuns = true;
				this->m_eventHandlerPtr = new ThreadWrapper(bind(USBInterface::EventHandlerThread,this));
			}
		}
	}
	else
		error_code = LIBUSB_ERROR_NO_DEVICE;

	// Callback invocation
	if(!error_code)
	{
		if(m_onConnectionCB != nullptr)
			m_onConnectionCB(DEVICE_CONNECTED);
		else
			cout << "No onConnectionCB defined!" << endl;
	}

	// Free device list
	libusb_free_device_list(devs,1);

	// Return the error code
	return static_cast<int>(error_code);
}

int USBInterface::DeinitializeFunctionality(void)
{
	int errorCode = 0;

	// Close the connection
	if(m_libusbInitialized)
	{
		cout << "Disconnecting the device" << endl;
		StopAllOutstandingTransfers();
		DeregisterAllTransfers();

		libusb_release_interface(m_devh, 0);
		this->m_eventHandlerRuns = false; // This flag has to be cleared in order to end the EventHandlerThread
		this->m_libusbInitialized = false;
		libusb_close(m_devh); // This operation causes an Event to be fired in the libusb.
		// Delete Eventhandlerthread (Which is only existent when the connection has been established)
		delete(this->m_eventHandlerPtr);
	}
	else
	{
		errorCode = 1; // TODO: Define Error on libusb deinit
	}

	return errorCode;
}

// Destructor
USBInterface::~USBInterface()
{
	// Stop the usblib and the event handler thread
	DeinitializeFunctionality();
	// Return from libusb
	libusb_exit(nullptr);
	cout << "USB Interface deleted!" << endl;
}


// Event handler function (Which has to be executed in another thread)
void USBInterface::EventHandlerThread(void)
{
	while(this->m_eventHandlerRuns)
	{
		// libusb_handle_events returns only when an event has occured
		libusb_handle_events(nullptr);
	}
}

/***************************************************************************//**
* \brief Control transfer registration.
*
* Prior to sending data via endpoint 0 (control endpoint), a transfer definition
* has to be created. This module takes all needed information about the transfer
* as parameters and relates it to a transfer index. It therefore preserves the
* information for repeated usages hence eliminates the need for creating a new
* transfer every time.
*******************************************************************************/
void USBInterface::RegisterCtrlTransfer (uint8_t * bufferPtr, uint8_t bmRequestType, uint8_t bRequest,
                                         uint16_t timeout, uint8_t bufferSize, uint8_t transferIdx,
										 CommandCallback transferCB)
{
    std::size_t bSize = bufferSize;
    void* bufPtr = static_cast<void*>(bufferPtr);

    // According to the libusb manual, the buffer has to be word-aligned.
    align(2,bufferSize-1,bufPtr,bSize);

    // place the transfer data at the array position specified by transferIdx
    m_transfer_array[transferIdx].transfer = libusb_alloc_transfer(0);
    m_transfer_array[transferIdx].transferCB = transferCB;
    m_transfer_array[transferIdx].transferType = CONTROL;
    m_transfer_array[transferIdx].objPtr = this;
    m_transfer_array[transferIdx].transferIdx = transferIdx;

    // define the transfer behaviour via bmRequestType and bRequest
    libusb_fill_control_setup(static_cast<uint8_t*>(bufPtr), bmRequestType, bRequest, 0x0000, 0x0000, 0x0000);
    // fill the transfer structure
    libusb_fill_control_transfer(m_transfer_array[transferIdx].transfer, m_devh, static_cast<uint8_t*>(bufPtr),
                                 TransferCallback, static_cast<void*>(&m_transfer_array[transferIdx]), timeout);

    // save the array index of the transfer
    m_transfer_array_indizes |= (1 << transferIdx);
}

/***************************************************************************//**
* \brief Bulk transfer registration.
*
* Prior to sending data via a bulk endpoint, a transfer definition
* has to be created. This module takes all needed information about the transfer
* as parameters and relates it to a transfer index. It therefore preserves the
* information for repeated usages hence eliminates the need for creating a new
* transfer every time.
*******************************************************************************/
void USBInterface::RegisterBulkTransfer (uint8_t * bufferPtr, uint8_t endpointAddress, uint16_t timeout,
		   	   	   	   	   	   	   	   	 uint32_t bufferSize, uint8_t transferIdx, CommandCallback transferCB)
{
	// Transfer in Array anlegen
	m_transfer_array[transferIdx].transfer = libusb_alloc_transfer(0);
	m_transfer_array[transferIdx].transferCB = transferCB;
	m_transfer_array[transferIdx].transferType = (endpointAddress & (1 << 7)) ? BULK_IN : BULK_OUT;
	m_transfer_array[transferIdx].objPtr = this;
	m_transfer_array[transferIdx].transferIdx = transferIdx;

	// Bulk Transfer erstellen
	libusb_fill_bulk_transfer(m_transfer_array[transferIdx].transfer, m_devh, endpointAddress, bufferPtr,
							  bufferSize, TransferCallback, static_cast<void*>(&m_transfer_array[transferIdx]), timeout);

	// Add Index
	m_transfer_array_indizes |= (1 << transferIdx);
}

// Edit the Setup field of a control transfer
void USBInterface::EditCtrlTransfer(uint8_t* bufPtr, uint16_t wIndex, uint16_t wValue, uint16_t wLength, uint8_t transferIdx)
{
	uint8_t* trans_buf;

	// Get the Setup section of the transfer
	libusb_control_setup* setupPtr = libusb_control_transfer_get_setup(m_transfer_array[transferIdx].transfer);

	// Renew the control setup using new data
	libusb_fill_control_setup(m_transfer_array[transferIdx].transfer->buffer, setupPtr->bmRequestType, setupPtr->bRequest, wValue, wIndex, wLength);
	// Adjust length of the transfer (TODO: Why is this neccessary?)
	m_transfer_array[transferIdx].transfer->length = wLength + 8;

	// Wenn kein valider buf_ptr �bergeben wurde, raus
	if (bufPtr == nullptr || wLength == 0)
		return;

	// Pointer auf Datensektion des Transferbuffers erhalten
	trans_buf = libusb_control_transfer_get_data(m_transfer_array[transferIdx].transfer);

	// Daten in den Transferbuffer umkopieren
	while (wLength > 0)
	{
		*trans_buf++ = *bufPtr++;
		wLength--;
	}
}

// Transfer submission
int USBInterface::SubmitTransfer(uint8_t transferIdx)
{
	int errorCode = 0;
	// Check if transfer is already running
	if (!(m_transfer_running & (1 << transferIdx)))
	{
		errorCode = libusb_submit_transfer(m_transfer_array[transferIdx].transfer);
		m_transfer_running |= (1 << transferIdx);
	}
	else
		errorCode = -99;

	return errorCode;
}

// Transfer submission using a specified buffer
int USBInterface::SubmitTransfer(uint8_t transferIdx, uint8_t *bufferPtr)
{
	int errorCode = 0;
	// Reallocation of the buffer pointer is only allowed for bulk and interrupt
	// transfers
	if (m_transfer_array[transferIdx].transferType >= BULK_IN &&
		m_transfer_array[transferIdx].transferType <= INTERRUPT_OUT)
	{
		if(bufferPtr != nullptr)
		{
			m_transfer_array[transferIdx].transfer->buffer = bufferPtr;
			// Submit transfer
			errorCode = SubmitTransfer(transferIdx);
		}
		else
			errorCode = -99;
	}
	else
		errorCode = -99;

	return errorCode;
}

void USBInterface::DeregisterAllTransfers (void)
{
    int i = 0;

    // Solange noch Transfers am Laufen sind, darf kein Versuch unternommen werden,
    // die Transfer-Struktur zu l�schen. TODO: Abbruch von bestehenden Transfers.
    // while (m_transfers_outstanding);
    cout << "Free all transfers... ";
    while (i < MAX_NUM_TRANSFERS)
    {
        if (m_transfer_array_indizes & (1 << i))
        {
            libusb_free_transfer(m_transfer_array[i].transfer);

            // maybe necessary for debug purposes
            m_transfer_array_indizes &= ~(1 << i);
        }

        i++;
    }
    cout << "Done!"<< endl;
}

void USBInterface::StopAllOutstandingTransfers(void)
{
	int i = 0;

	// Iterate over all transfers in the array to subsequently stop all not yet
	// finished ones
	cout << "Cancelling all outstanding transfers... ";
	while (i < MAX_NUM_TRANSFERS)
	{
		if (m_transfer_running & (1 << i))
			libusb_cancel_transfer(m_transfer_array[i].transfer);

		i++;
	}
	// Wait until all transfers had been finished
	while (m_transfer_running)
		;

	cout << "Done!"<< endl;
}

// This central low level callback is directly called by the event handler
void USBInterface::TransferCallback(libusb_transfer * transfer)
{
    uint8_t * BufferPtr;
    void * CtrlSetupPtr;

    // The static function does not know the object to which it belongs. We
    // therefore added the transfer struct as user data.
    TRANSFER * userdata = (static_cast<TRANSFER*>(transfer->user_data));
    USBInterface* obj = static_cast<USBInterface*>(userdata->objPtr);

    // Transfer has finished
    obj->m_transfer_running &= (~(1 << userdata->transferIdx));

    // If the device has not been disconnected...
    if(transfer->status != LIBUSB_TRANSFER_NO_DEVICE)
    {
		// Is it a control transfer? -> Get the setup pointer
		if(static_cast<libusb_transfer_type>(transfer->type) == LIBUSB_TRANSFER_TYPE_CONTROL)
		{
			CtrlSetupPtr = reinterpret_cast<void *>(transfer->buffer);
			BufferPtr = libusb_control_transfer_get_data(transfer);
		}
		// otherwise just the data pointer is assigned
		else
		{
			CtrlSetupPtr = nullptr;
			BufferPtr = transfer->buffer;
		}

		// Call the callback function
		userdata->transferCB(CtrlSetupPtr, BufferPtr, transfer->status);
    }
    else
    {
    	// Figure out the next steps
    	if (obj->m_onConnectionCB)
    		obj->m_onConnectionCB(DEVICE_DISCONNECTED);
    }
}
