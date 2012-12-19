// Con_meIOSingleReadDigital.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <conio.h>

#include <memory.h>

extern "C"
{
#include "medriver.h"
}

#include "TestConsoleGeneric.h"

#ifdef WIN64

#pragma comment (lib, "meIDSmain64.lib")

#else

#pragma comment (lib, "meIDSmain.lib")

#endif

int main(int argc, _TCHAR* argv[])
{
	// Call generic console function in parent directory to
	// open system and list the available devices and subdevices.

	int i_number_of_devices = TestConsoleMEIDSProlog();

	if(i_number_of_devices <= 0)
	{
		return(-1);
	}

	// ---------------------------------------------------------------------

	// Count the DI and DIO sub-devices in the system

	int i_number_di_dio_subdevices = 0;

	int i_me_error;

	for(int index_device = 0; index_device < i_number_of_devices; index_device++)
	{
		int i_vendor_id;
		int i_device_id;
		int i_serial_no;
		int i_bus_type;
		int i_bus_no;
		int i_dev_no;
		int i_func_no;
		int i_plugged;

		i_me_error = meQueryInfoDevice(	index_device,				// Device index
										&i_vendor_id,				// Vendor ID returned here - not required
										&i_device_id,				// Device ID returned here - not required
										&i_serial_no,				// Serial number returned here - not required
										&i_bus_type,				// Bus type returned here - not required
										&i_bus_no,					// Bus number returned here - not required
										&i_dev_no,					// Bus device number returned here - not required
										&i_func_no,					// Bus function number returned here - not required
										&i_plugged				);	// Plugged status returned hee - not required

		if(i_plugged != ME_PLUGGED_IN)
		{
			continue;
		}

		int index_subdevice = 0;

		int i_number_of_subdevices;

		i_me_error = meQueryNumberSubdevices(	index_device,				// Device index
												&i_number_of_subdevices);	// Number of subdevices returned here

		while(true)
		{
			i_me_error = meQuerySubdeviceByType(index_device,					// Device
												index_subdevice,				// Begin search at this sub-device
												ME_TYPE_DI,						// Type of sub-device to search for
												ME_SUBTYPE_ANY,					// Sub-type of sub-device to search for
												&index_subdevice	);			// sub-device index returned here

			if(i_me_error != ME_ERRNO_SUCCESS)
			{
				break;
			}

			++i_number_di_dio_subdevices;

			if(++index_subdevice >= i_number_of_subdevices)
			{
				break;
			}
		}

		index_subdevice = 0;

		while(true)
		{
			i_me_error = meQuerySubdeviceByType(index_device,					// Device
												index_subdevice,				// Begin search at this sub-device
												ME_TYPE_DIO,					// Type of sub-device to search for
												ME_SUBTYPE_ANY,					// Sub-type of sub-device to search for
												&index_subdevice	);			// sub-device index returned here

			if(i_me_error != ME_ERRNO_SUCCESS)
			{
				break;
			}

			++i_number_di_dio_subdevices;

			if(++index_subdevice >= i_number_of_subdevices)
			{
				break;
			}
		}
	}

	if(i_number_di_dio_subdevices == 0)
	{
		printf("****    No DI or DIO sub-devices found in system    ****\n\n");

		printf("Press any key to terminate\n");

		_getch();

		meClose(0);

		return -1;
	}

	// Make a list of the DI and DIO sub-devices

	struct SDigitalInSubddevice
	{
		int	m_iDeviceIndex;

		int m_iSubdeviceIndex;

		int m_iType;

		int m_iWidth;
	};

	SDigitalInSubddevice* arrSubdevices = new SDigitalInSubddevice[i_number_di_dio_subdevices];

	i_number_di_dio_subdevices = 0;

	for(int index_device = 0; index_device < i_number_of_devices; index_device++)
	{
		int i_vendor_id;
		int i_device_id;
		int i_serial_no;
		int i_bus_type;
		int i_bus_no;
		int i_dev_no;
		int i_func_no;
		int i_plugged;

		i_me_error = meQueryInfoDevice(	index_device,				// Device index
										&i_vendor_id,				// Vendor ID returned here - not required
										&i_device_id,				// Device ID returned here - not required
										&i_serial_no,				// Serial number returned here - not required
										&i_bus_type,				// Bus type returned here - not required
										&i_bus_no,					// Bus number returned here - not required
										&i_dev_no,					// Bus device number returned here - not required
										&i_func_no,					// Bus function number returned here - not required
										&i_plugged				);	// Plugged status returned hee - not required

		if(i_plugged != ME_PLUGGED_IN)
		{
			continue;
		}

		int index_subdevice = 0;

		int i_number_of_subdevices;

		i_me_error = meQueryNumberSubdevices(	index_device,				// Device index
												&i_number_of_subdevices);	// Number of subdevices returned here

		while(true)
		{
			i_me_error = meQuerySubdeviceByType(index_device,					// Device
												index_subdevice,				// Begin search at this sub-device
												ME_TYPE_DI,						// Type of sub-device to search for
												ME_SUBTYPE_ANY,					// Sub-type of sub-device to search for
												&index_subdevice	);			// sub-device index returned here

			if(i_me_error != ME_ERRNO_SUCCESS)
			{
				break;
			}

			arrSubdevices[i_number_di_dio_subdevices].m_iDeviceIndex = index_device;

			arrSubdevices[i_number_di_dio_subdevices].m_iSubdeviceIndex = index_subdevice;

			arrSubdevices[i_number_di_dio_subdevices].m_iType = ME_TYPE_DI;


			i_me_error = meQueryNumberChannels(	index_device,										// Device
												index_subdevice,									// Sub-device
												&arrSubdevices[i_number_di_dio_subdevices].m_iWidth	);	// Number of channels returned here

			++i_number_di_dio_subdevices;

			if(++index_subdevice >= i_number_of_subdevices)
			{
				break;
			}
		}

		index_subdevice = 0;

		while(true)
		{
			i_me_error = meQuerySubdeviceByType(index_device,					// Device
												index_subdevice,				// Begin search at this sub-device
												ME_TYPE_DIO,					// Type of sub-device to search for
												ME_SUBTYPE_ANY,					// Sub-type of sub-device to search for
												&index_subdevice	);			// sub-device index returned here

			if(i_me_error != ME_ERRNO_SUCCESS)
			{
				break;
			}

			arrSubdevices[i_number_di_dio_subdevices].m_iDeviceIndex = index_device;

			arrSubdevices[i_number_di_dio_subdevices].m_iSubdeviceIndex = index_subdevice;

			arrSubdevices[i_number_di_dio_subdevices].m_iType = ME_TYPE_DIO;

			i_me_error = meQueryNumberChannels(	index_device,										// Device
												index_subdevice,									// Sub-device
												&arrSubdevices[i_number_di_dio_subdevices].m_iWidth	);	// Number of channels returned here

			++i_number_di_dio_subdevices;

			if(++index_subdevice >= i_number_of_subdevices)
			{
				break;
			}
		}
	}

	printf("%d DI or DIO sub-devices found:\n\n", i_number_di_dio_subdevices);

	for(int index_subdevice = 0; index_subdevice < i_number_di_dio_subdevices; index_subdevice++)
	{
		printf("%2d.   Device: %2d  Sub-device: %2d  Type: %s  Width: %2d Bits\n",
				index_subdevice + 1,
				arrSubdevices[index_subdevice].m_iDeviceIndex,
				arrSubdevices[index_subdevice].m_iSubdeviceIndex,
				( (arrSubdevices[index_subdevice].m_iType == ME_TYPE_DIO) ? "DIO" : " DI"),
				arrSubdevices[index_subdevice].m_iWidth										);
	}

	printf("\n\nPress any key to continue\n\n");

	getch();

	char buffer[512];

	while(true)
	{
		int index_subdevice;

		if(i_number_di_dio_subdevices > 1)
		{
			printf("Choose Subdevice:\n\n");

			for(index_subdevice = 0; index_subdevice < i_number_di_dio_subdevices; index_subdevice++)
			{
				printf("    %2d.   Device: %2d  Sub-device: %2d  Type: %s  Width: %2d Bits\n",
						index_subdevice + 1,
						arrSubdevices[index_subdevice].m_iDeviceIndex,
						arrSubdevices[index_subdevice].m_iSubdeviceIndex,
						( (arrSubdevices[index_subdevice].m_iType == ME_TYPE_DIO) ? "DIO" : " DI"),
						arrSubdevices[index_subdevice].m_iWidth										);
			}

			printf("\nYour choice (1 - %2d) (RETURN to terminate): ", i_number_di_dio_subdevices);

			gets(&buffer[0]);

			printf("\n\n");

			if(buffer[0] == 0)
			{
				break;
			}

			if( (sscanf(&buffer[0], "%d", &index_subdevice) != 1 ) )
			{
				continue;
			}

			if( (index_subdevice < 1)||(index_subdevice > i_number_di_dio_subdevices) )
			{
				continue;
			}

			--index_subdevice;
		}
		else
		{
			index_subdevice = 0;
		}

		int i_config_flags;

		int i_read_flags;

		switch(arrSubdevices[index_subdevice].m_iWidth)
		{
		case 1:

			i_config_flags = ME_IO_SINGLE_CONFIG_DIO_BIT;

			i_read_flags = ME_IO_SINGLE_TYPE_DIO_BIT;

			break;

		case 8:

			i_config_flags = ME_IO_SINGLE_CONFIG_DIO_BYTE;

			i_read_flags = ME_IO_SINGLE_TYPE_DIO_BYTE;

			break;

		case 16:

			i_config_flags = ME_IO_SINGLE_CONFIG_DIO_WORD;

			i_read_flags = ME_IO_SINGLE_TYPE_DIO_WORD;

			break;

		case 32:

			i_config_flags = ME_IO_SINGLE_CONFIG_DIO_DWORD;

			i_read_flags = ME_IO_SINGLE_TYPE_DIO_DWORD;

			break;
		}

		printf("Press any key to read data from Device: %d Subdevice: %d\n", arrSubdevices[index_subdevice].m_iDeviceIndex, arrSubdevices[index_subdevice].m_iSubdeviceIndex);

		getch();

		printf("\n\n");

		if(arrSubdevices[index_subdevice].m_iType == ME_TYPE_DIO)
		{
			i_me_error = meIOSingleConfig(	arrSubdevices[index_subdevice].m_iDeviceIndex,		// Device index
											arrSubdevices[index_subdevice].m_iSubdeviceIndex,	// Subdevcie index
											0,													// Channel
											ME_SINGLE_CONFIG_DIO_INPUT,							// Single config
											ME_VALUE_NOT_USED,									// Reference
											ME_TRIG_CHAN_DEFAULT,								// Trigger channel
											ME_TRIG_TYPE_NONE,									// Trigger type
											ME_VALUE_NOT_USED,									// Trigger edge
											i_config_flags									);	// Flags

			if(i_me_error != ME_ERRNO_SUCCESS)
			{
				printf("****    meIOSingleConfig - Error: %d    ****\n\n", i_me_error);

				printf("Press any key to terminate\n");

				_getch();

				meClose(0);

				return -1;
			}
		}

		meIOSingle_t io_single[1];

		io_single[0].iDevice = arrSubdevices[index_subdevice].m_iDeviceIndex;

		io_single[0].iSubdevice = arrSubdevices[index_subdevice].m_iSubdeviceIndex;
		io_single[0].iChannel	= 0;
		io_single[0].iDir		= ME_DIR_INPUT;
		io_single[0].iValue		= 0;
		io_single[0].iTimeOut	= 0;							// No timeout - not required for software output
		io_single[0].iFlags		= i_read_flags;

		i_me_error = meIOSingle(&io_single[0],				// Output list
								1,							// Number of elements in the above list
								ME_IO_SINGLE_NO_FLAGS	);	// Flags

		if(i_me_error != ME_ERRNO_SUCCESS)
		{
			printf("****    meIOSingle - Error: %d    ****\n\n", i_me_error);

			printf("Press any key to terminate\n");

			_getch();

			meClose(0);

			return -1;
		}

		char sz_hex_value[16];

		switch(arrSubdevices[index_subdevice].m_iWidth)
		{
		case 1:

			sprintf(&sz_hex_value[0], "%d", (io_single[0].iValue & 0x00000001) );

			break;

		case 8:

			sprintf(&sz_hex_value[0], "0x%02X", (unsigned char)(io_single[0].iValue & 0x000000FF) );

			break;

		case 16:

			sprintf(&sz_hex_value[0], "0x%04X", (unsigned short)(io_single[0].iValue & 0x0000FFFF) );

			break;

		case 32:

			sprintf(&sz_hex_value[0], "0x%08X", io_single[0].iValue);

			break;
		}

		printf("Read %s from Device: %d Subdevice: %d - press any key to continue\n", &sz_hex_value[0], arrSubdevices[index_subdevice].m_iDeviceIndex, arrSubdevices[index_subdevice].m_iSubdeviceIndex);

		_getch();
	}

	delete []arrSubdevices;

	// ---------------------------------------------------------------------

	meClose(0);

	printf("Program completed successfully - Press any key to terminate\n\n");

	_getch();

	return 0;
}

